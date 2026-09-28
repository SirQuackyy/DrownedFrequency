#if WITH_DEV_AUTOMATION_TESTS
#include "Misc/AutomationTest.h"
#include "Tests/AutomationCommon.h"
#if WITH_EDITOR
#include "Tests/AutomationEditorCommon.h"
#include "Editor.h"
#endif
#include "Engine/Engine.h"
#include "Engine/GameInstance.h"
#include "EngineUtils.h"
#include "Save/DFCheckpointSubsystem.h"
#include "Save/DFCheckpointActor.h"
#include "Save/DFSaveSubsystem.h"
#include "World/DFLevelStreamingSubsystem.h"
#include "World/DFDoorActor.h"
#include "Player/DFPlayerCharacter.h"
#include "Player/DFHealthComponent.h"
#include "Player/DFPlayerController.h"
#include "HAL/FileManager.h"
namespace
{
class FCheckpointFlow:public IAutomationLatentCommand
{
public:
    FCheckpointFlow(FAutomationTestBase* T,bool Death=false,int32 Fallback=0):Test(T),bDeath(Death),FallbackMode(Fallback){}
    bool Update() override
    {
        if(FPlatformTime::Seconds()-Start>40) {Test->AddError(TEXT("Checkpoint flow timed out")); return true;}
        UWorld* W=nullptr;
#if WITH_EDITOR
        if(GEditor) W=GEditor->PlayWorld;
#endif
        if(!W) for(const auto& C:GEngine->GetWorldContexts()) if(C.WorldType==EWorldType::Game) W=C.World();
        auto* PC=W?Cast<ATDFPlayerController>(W->GetFirstPlayerController()):nullptr; auto* P=PC?Cast<ATDFPlayerCharacter>(PC->GetPawn()):nullptr; if(!P) return false;
        auto* S=W->GetGameInstance()->GetSubsystem<UTDFSaveSubsystem>(); auto* C=W->GetGameInstance()->GetSubsystem<UTDFCheckpointSubsystem>(); auto* L=W->GetGameInstance()->GetSubsystem<UTDFLevelStreamingSubsystem>();
        if(Stage==0) {InitialWrites=C->SuccessfulCheckpointWrites; C->CheckpointSlot=FName(*(TEXT("QA_CP_")+FGuid::NewGuid().ToString())); C->ManualFallbackSlot=FName(*(C->CheckpointSlot.ToString()+TEXT("_Manual"))); C->ChapterStartSlot=FName(*(C->CheckpointSlot.ToString()+TEXT("_Start"))); C->bSearchManualHistory=false; Test->TestTrue(TEXT("Load checkpoint sector"),L->RequestSectorSet({TEXT("Intro")},TEXT("CheckpointQA"))); ++Stage; return false;}
        if(L->bStreamingTransitionActive || S->IsLoading() || C->bRespawning) return false;
        ATDFDoorActor* Door=nullptr; for(TActorIterator<ATDFDoorActor> I(W);I;++I) if(I->ActorHasTag(TEXT("Save_Door"))) Door=*I;
        if(!Door) {Test->AddError(TEXT("Checkpoint puzzle fixture missing")); return true;}
        if(Stage==1)
        {
            Door->SetOpen(true); Checkpoint=W->SpawnActor<ATDFCheckpointActor>(FVector(700,500,100),FRotator::ZeroRotator); Checkpoint->CheckpointId=C->CheckpointSlot; Checkpoint->ChapterId=TEXT("QA_Chapter"); Checkpoint->RequiredLoadedSectors={TEXT("Intro")}; Checkpoint->RespawnTransform=FTransform(FRotator(0,90,0),FVector(700,450,92)); P->SetActorLocation(FVector(700,500,92)); ++Stage; return false;
        }
        if(Stage==2)
        {
            Test->TestEqual(TEXT("Overlap wrote checkpoint"),C->SuccessfulCheckpointWrites,InitialWrites+1); Test->TestFalse(TEXT("Duplicate checkpoint suppressed"),C->ActivateCheckpoint(Checkpoint->CheckpointId)); Test->TestEqual(TEXT("No repeated write"),C->SuccessfulCheckpointWrites,InitialWrites+1);
            FText Error; auto* Save=S->ReadObject(C->CheckpointSlot,Error); Test->TestTrue(TEXT("Checkpoint ID stored"),Save && Save->CurrentCheckpointId==Checkpoint->CheckpointId); Test->TestTrue(TEXT("Respawn transform stored"),Save && Save->Player.Location.Equals(FVector(700,450,92)));
            if(FallbackMode && Save) {Save->Reason=FallbackMode==1?ETDFSaveReason::Manual:ETDFSaveReason::ChapterStart; Test->TestTrue(TEXT("Write isolated fallback"),S->WriteObject(FallbackMode==1?C->ManualFallbackSlot:C->ChapterStartSlot,Save,Error)); IFileManager::Get().Delete(*S->SlotPath(C->CheckpointSlot));}
            Door->SetOpen(false); P->SetActorLocation(FVector(1100,0,92)); if(bDeath) {P->Health->ApplyDamage(200); Test->TestTrue(TEXT("Death freezes input"),PC->IsWorldTransitionLocked());} else Test->TestTrue(TEXT("Checkpoint reload"),S->RequestLoadSlot(C->CheckpointSlot)); ++Stage; return false;
        }
        Test->TestEqual(TEXT("Respawn health"),P->Health->Health,P->Health->GetRespawnHealth()); Test->TestTrue(TEXT("Checkpoint placement restored"),P->GetActorLocation().Equals(FVector(700,450,92),5)); Test->TestTrue(TEXT("Solved puzzle stays solved"),Door->bIsOpen); Test->TestFalse(TEXT("Control restored"),PC->IsWorldTransitionLocked()); for(const FName Slot:{C->CheckpointSlot,C->ManualFallbackSlot,C->ChapterStartSlot}) IFileManager::Get().Delete(*S->SlotPath(Slot)); Checkpoint->Destroy(); return true;
    }
private:
    FAutomationTestBase* Test; bool bDeath; int32 FallbackMode=0; TWeakObjectPtr<ATDFCheckpointActor> Checkpoint; int32 Stage=0,InitialWrites=0; double Start=FPlatformTime::Seconds();
};
}
#if WITH_EDITOR
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FTDFCheckpointPIE,"TDF.Save.Checkpoint.PIE",EAutomationTestFlags::EditorContext|EAutomationTestFlags::EngineFilter)
bool FTDFCheckpointPIE::RunTest(const FString&) {AutomationOpenMap(TEXT("/Game/Game/Maps/Test/M_Test_SaveStreaming"),true); ADD_LATENT_AUTOMATION_COMMAND(FStartPIECommand(false)); ADD_LATENT_AUTOMATION_COMMAND(FWaitLatentCommand(1.f)); ADD_LATENT_AUTOMATION_COMMAND(FCheckpointFlow(this)); ADD_LATENT_AUTOMATION_COMMAND(FEndPlayMapCommand()); return true;}
#endif
#if WITH_EDITOR
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FTDFDeathPIE,"TDF.Save.Death.PIE",EAutomationTestFlags::EditorContext|EAutomationTestFlags::EngineFilter)
bool FTDFDeathPIE::RunTest(const FString&) {AutomationOpenMap(TEXT("/Game/Game/Maps/Test/M_Test_SaveStreaming"),true); ADD_LATENT_AUTOMATION_COMMAND(FStartPIECommand(false)); ADD_LATENT_AUTOMATION_COMMAND(FWaitLatentCommand(1.f)); ADD_LATENT_AUTOMATION_COMMAND(FCheckpointFlow(this,true)); ADD_LATENT_AUTOMATION_COMMAND(FEndPlayMapCommand()); return true;}
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FTDFManualFallbackPIE,"TDF.Save.Death.ManualFallback",EAutomationTestFlags::EditorContext|EAutomationTestFlags::EngineFilter)
bool FTDFManualFallbackPIE::RunTest(const FString&) {AutomationOpenMap(TEXT("/Game/Game/Maps/Test/M_Test_SaveStreaming"),true); ADD_LATENT_AUTOMATION_COMMAND(FStartPIECommand(false)); ADD_LATENT_AUTOMATION_COMMAND(FWaitLatentCommand(1.f)); ADD_LATENT_AUTOMATION_COMMAND(FCheckpointFlow(this,true,1)); ADD_LATENT_AUTOMATION_COMMAND(FEndPlayMapCommand()); return true;}
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FTDFChapterFallbackPIE,"TDF.Save.Death.ChapterFallback",EAutomationTestFlags::EditorContext|EAutomationTestFlags::EngineFilter)
bool FTDFChapterFallbackPIE::RunTest(const FString&) {AutomationOpenMap(TEXT("/Game/Game/Maps/Test/M_Test_SaveStreaming"),true); ADD_LATENT_AUTOMATION_COMMAND(FStartPIECommand(false)); ADD_LATENT_AUTOMATION_COMMAND(FWaitLatentCommand(1.f)); ADD_LATENT_AUTOMATION_COMMAND(FCheckpointFlow(this,true,2)); ADD_LATENT_AUTOMATION_COMMAND(FEndPlayMapCommand()); return true;}
#endif
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FTDFDeathGame,"TDF.Save.Death.Standalone",EAutomationTestFlags::ClientContext|EAutomationTestFlags::EngineFilter)
bool FTDFDeathGame::RunTest(const FString&) {AutomationOpenMap(TEXT("/Game/Game/Maps/Test/M_Test_SaveStreaming"),true); ADD_LATENT_AUTOMATION_COMMAND(FWaitLatentCommand(2.f)); ADD_LATENT_AUTOMATION_COMMAND(FCheckpointFlow(this,true)); return true;}
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FTDFCheckpointGame,"TDF.Save.Checkpoint.Standalone",EAutomationTestFlags::ClientContext|EAutomationTestFlags::EngineFilter)
bool FTDFCheckpointGame::RunTest(const FString&) {AutomationOpenMap(TEXT("/Game/Game/Maps/Test/M_Test_SaveStreaming"),true); ADD_LATENT_AUTOMATION_COMMAND(FWaitLatentCommand(2.f)); ADD_LATENT_AUTOMATION_COMMAND(FCheckpointFlow(this)); return true;}
#endif
