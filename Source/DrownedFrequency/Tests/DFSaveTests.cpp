#if WITH_DEV_AUTOMATION_TESTS
#include "Misc/AutomationTest.h"
#include "Save/DFSaveGame.h"
#include "Kismet/GameplayStatics.h"
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FTDFSaveSchemaTest,"TDF.Save.Schema",EAutomationTestFlags::EditorContext|EAutomationTestFlags::ClientContext|EAutomationTestFlags::EngineFilter)
bool FTDFSaveSchemaTest::RunTest(const FString&)
{
    auto* S=NewObject<UTDFSaveGame>(); S->CurrentChapterId=TEXT("QA_Chapter"); S->CurrentCheckpointId=TEXT("QA_CP"); S->CurrentPersistentMap=TEXT("/Game/Game/Maps/Test/M_Test_AcousticDepth"); S->LoadedSectorIds={TEXT("QA_A"),TEXT("QA_B")}; S->SavedAtUtc=FDateTime::UtcNow(); S->Player.Location=FVector(10,20,90); S->Player.KeyIds={TEXT("QA_Key")};
    FTDFActorSaveData Actor; Actor.StableActorId=FGuid::NewGuid(); Actor.SerializedFields.Add(TEXT("IsOpen"),TEXT("True")); S->Actors.Add(Actor);
    const FString Slot=TEXT("TDF_QA_Schema_")+FGuid::NewGuid().ToString(); TestTrue(TEXT("Disk write"),UGameplayStatics::SaveGameToSlot(S,Slot,0)); auto* L=Cast<UTDFSaveGame>(UGameplayStatics::LoadGameFromSlot(Slot,0));
    if(TestNotNull(TEXT("Disk read"),L)) {TestEqual(TEXT("Current version"),L->SaveVersion,TDFSave::CurrentVersion); TestEqual(TEXT("Chapter"),L->CurrentChapterId,S->CurrentChapterId); TestEqual(TEXT("Checkpoint"),L->CurrentCheckpointId,S->CurrentCheckpointId); TestTrue(TEXT("Sectors"),L->LoadedSectorIds==S->LoadedSectorIds); TestEqual(TEXT("Timestamp"),L->SavedAtUtc,S->SavedAtUtc); TestEqual(TEXT("Actor ID"),L->Actors[0].StableActorId,Actor.StableActorId); TestTrue(TEXT("Player"),L->Player.Location==S->Player.Location);}
    UGameplayStatics::DeleteGameInSlot(Slot,0); return !HasAnyErrors();
}
#include "Save/DFSaveSubsystem.h"
#include "Engine/GameInstance.h"
#include "HAL/FileManager.h"
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FTDFSaveWriteTest,"TDF.Save.Write",EAutomationTestFlags::EditorContext|EAutomationTestFlags::ClientContext|EAutomationTestFlags::EngineFilter)
bool FTDFSaveWriteTest::RunTest(const FString&)
{
    auto* Saves=NewObject<UTDFSaveSubsystem>(NewObject<UGameInstance>()); TestTrue(TEXT("Safe baseline"),Saves->CanSaveNow()); Saves->bStreamingTransitionActive=true; TestFalse(TEXT("Streaming blocks save"),Saves->CanSaveNow()); Saves->bStreamingTransitionActive=false; Saves->bCombatLocked=true; TestFalse(TEXT("Danger blocks save"),Saves->CanSaveNow());
    auto* S=NewObject<UTDFSaveGame>(); S->Player.Location=FVector(40,20,90); FText Error; const FName Slot(*(TEXT("QA_Write_")+FGuid::NewGuid().ToString()));
    TestTrue(TEXT("Verified temp to final write"),UTDFSaveSubsystem::WriteObject(Slot,S,Error)); auto* L=UTDFSaveSubsystem::ReadObject(Slot,Error); TestTrue(TEXT("Location roundtrip"),L && L->Player.Location==S->Player.Location); TestFalse(TEXT("Temp removed after success"),IFileManager::Get().FileExists(*(UTDFSaveSubsystem::SlotPath(Slot)+TEXT(".tmp"))));
    TestFalse(TEXT("Invalid traversal slot rejected"),UTDFSaveSubsystem::WriteObject(TEXT("../InputSettings"),S,Error)); TestTrue(TEXT("Failed request preserves previous save"),UTDFSaveSubsystem::ReadObject(Slot,Error)!=nullptr);
    IFileManager::Get().Delete(*UTDFSaveSubsystem::SlotPath(Slot)); return !HasAnyErrors();
}
#include "Save/DFSaveMigrationLibrary.h"
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FTDFMigrationTest,"TDF.Save.Migrations",EAutomationTestFlags::EditorContext|EAutomationTestFlags::ClientContext|EAutomationTestFlags::EngineFilter)
bool FTDFMigrationTest::RunTest(const FString&)
{
    auto* S=NewObject<UTDFSaveGame>(); S->SaveVersion=1; S->Player.KeyIds={TEXT("RetainedKey")}; S->CurrentCheckpointId=TEXT("RetainedCheckpoint"); ETDFLoadFailure Failure; FText Message;
    TestTrue(TEXT("V1 upgrades sequentially"),UTDFSaveMigrationLibrary::Upgrade(S,Failure,Message)); TestEqual(TEXT("Latest version"),S->SaveVersion,TDFSave::CurrentVersion); TestTrue(TEXT("Existing progress preserved"),S->Player.KeyIds.Contains(TEXT("RetainedKey")) && S->CurrentCheckpointId==TEXT("RetainedCheckpoint")); TestEqual(TEXT("Old pucks safely returned"),S->Player.PlacedPucks.Num(),0);
    TestTrue(TEXT("Migration idempotent"),UTDFSaveMigrationLibrary::Upgrade(S,Failure,Message)); S->SaveVersion=TDFSave::CurrentVersion+1; TestFalse(TEXT("Newer version rejected"),UTDFSaveMigrationLibrary::Upgrade(S,Failure,Message)); TestTrue(TEXT("Specific newer-build failure"),Failure==ETDFLoadFailure::SaveFromNewerBuild); TestFalse(TEXT("User-facing reason"),Message.IsEmpty());
    return !HasAnyErrors();
}
#include "World/DFProgressionDataAssets.h"
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FTDFObjectiveGraphTest,"TDF.Save.ObjectiveGraph",EAutomationTestFlags::EditorContext|EAutomationTestFlags::ClientContext|EAutomationTestFlags::EngineFilter)
bool FTDFObjectiveGraphTest::RunTest(const FString&)
{
    auto* C=NewObject<UTDFChapterDataAsset>(); C->ChapterId=TEXT("QA"); C->DisplayName=FText::FromString(TEXT("QA")); C->PersistentMapName=TEXT("/Game/Game/Maps/Test/M_Test_SaveStreaming"); C->EntryCheckpointId=TEXT("QA_CP"); C->ObjectiveIds={TEXT("First"),TEXT("Second")};
    auto* A=NewObject<UTDFObjectiveDataAsset>(); auto* B=NewObject<UTDFObjectiveDataAsset>(); A->ObjectiveId=TEXT("First"); B->ObjectiveId=TEXT("Second"); A->ChapterId=B->ChapterId=C->ChapterId; A->ShortText=B->ShortText=A->LongText=B->LongText=FText::FromString(TEXT("Authored task")); A->NextObjectiveIds={B->ObjectiveId}; FText Error;
    TestTrue(TEXT("Valid graph"),C->ValidateGraph({C},{A,B},Error)); TestFalse(TEXT("Duplicate IDs rejected"),C->ValidateGraph({C},{A,A},Error)); B->NextObjectiveIds={A->ObjectiveId}; TestFalse(TEXT("Cycle rejected"),C->ValidateGraph({C},{A,B},Error)); B->NextObjectiveIds.Reset(); A->NextObjectiveIds={TEXT("Missing")}; TestFalse(TEXT("Missing target rejected"),C->ValidateGraph({C},{A,B},Error));
    return !HasAnyErrors();
}
#endif
