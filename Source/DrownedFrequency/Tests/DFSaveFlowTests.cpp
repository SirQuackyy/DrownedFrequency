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
#include "Save/DFSaveSubsystem.h"
#include "World/DFLevelStreamingSubsystem.h"
#include "World/DFDoorActor.h"
#include "World/DFWorldItemActor.h"
#include "Player/DFPlayerCharacter.h"
#include "Player/DFPlayerController.h"
#include "Inventory/DFInventoryComponent.h"
#include "Inventory/DFItemDataAsset.h"
#include "Acoustics/DFAudioAnomalyActor.h"
#include "Acoustics/DFRecorderComponent.h"
#include "Acoustics/DFRecorderPlaybackComponent.h"
#include "HAL/FileManager.h"
namespace
{
UWorld* SaveTestWorld()
{
#if WITH_EDITOR
    if(GEditor && GEditor->PlayWorld) return GEditor->PlayWorld;
#endif
    for(const auto& C:GEngine->GetWorldContexts()) if(C.WorldType==EWorldType::Game) return C.World(); return nullptr;
}
template<class T> T* SaveTagged(UWorld* W,const TCHAR* Tag) {for(TActorIterator<T> I(W);I;++I) if(I->ActorHasTag(Tag)) return *I; return nullptr;}
class FSaveFlow:public IAutomationLatentCommand
{
public:
    FSaveFlow(FAutomationTestBase* T):Test(T),Slot(*(TEXT("QA_Flow_")+FGuid::NewGuid().ToString())){}
    bool Update() override
    {
        if(FPlatformTime::Seconds()-Start>45) {Test->AddError(TEXT("Save flow timed out")); return true;}
        auto* W=SaveTestWorld(); auto* PC=W?Cast<ATDFPlayerController>(W->GetFirstPlayerController()):nullptr; auto* P=PC?Cast<ATDFPlayerCharacter>(PC->GetPawn()):nullptr; if(!P) return false;
        auto* Saves=W->GetGameInstance()->GetSubsystem<UTDFSaveSubsystem>(); auto* Streaming=W->GetGameInstance()->GetSubsystem<UTDFLevelStreamingSubsystem>();
        if(Stage==0) {Test->TestTrue(TEXT("Request intro"),Streaming->RequestSectorSet({TEXT("Intro")},TEXT("QA"))); Test->TestFalse(TEXT("Saving blocked during transition"),Saves->CanSaveNow()); ++Stage; return false;}
        if(Streaming->bStreamingTransitionActive || Saves->IsLoading()) return false;
        auto* Door=SaveTagged<ATDFDoorActor>(W,TEXT("Save_Door"));
        if(Stage==1)
        {
            auto* Bell=SaveTagged<ATDFAudioAnomalyActor>(W,TEXT("Save_Source")); auto* Item=SaveTagged<ATDFWorldItemActor>(W,TEXT("Save_Key"));
            if(!Door || !Bell || !Item || !Item->ItemData) {Test->AddError(TEXT("Streamed fixture actors absent")); return true;}
            P->SetActorLocation(Bell->GetActorLocation()+FVector(-180,0,-53)); PC->SetControlRotation(FRotator::ZeroRotator); Test->TestTrue(TEXT("Record in streamed sector"),P->Recorder->TryRecordAnomaly(Bell,0));
            Item->Interact_Implementation(P); KeyId=Item->ItemData->KeyId; Test->TestTrue(TEXT("Key collected"),P->Inventory->HasKey(KeyId)); Door->SetOpen(true); DoorId=Door->StableIdentity->StableActorId;
            P->SetActorLocation(FVector(500,600,92)); SavedLocation=P->GetActorLocation(); Test->TestTrue(TEXT("Write world snapshot"),Saves->WriteSave(Slot,ETDFSaveReason::Debug));
            Door->SetOpen(false); P->Inventory->RestoreInventory({},{}); P->Recorder->DeleteSlot(0); P->SetActorLocation(FVector(1100,0,92));
            Test->TestTrue(TEXT("Leave intro for radio room"),Streaming->RequestSectorSet({TEXT("RadioRoom")},TEXT("QA"))); ++Stage; return false;
        }
        if(Stage==2) {Test->TestNull(TEXT("Old sector unloaded"),Door); Test->TestTrue(TEXT("Read save request"),Saves->RequestLoadSlot(Slot)); Test->TestTrue(TEXT("Input frozen while loading"),PC->IsWorldTransitionLocked()); ++Stage; return false;}
        if(Stage==3)
        {
            Test->TestTrue(TEXT("Sector restored"),Streaming->GetLoadedSectorIds().Contains(TEXT("Intro"))); Test->TestTrue(TEXT("Door restored by stable ID"),Door && Door->StableIdentity->StableActorId==DoorId && Door->bIsOpen); Test->TestTrue(TEXT("Player placed after sector"),P->GetActorLocation().Equals(SavedLocation,5));
            Test->TestTrue(TEXT("Signal restored"),P->Recorder->HasValidSignal(0)); Test->TestTrue(TEXT("Inventory restored"),P->Inventory->HasKey(KeyId)); Test->TestFalse(TEXT("Input released"),PC->IsWorldTransitionLocked());
            IFileManager::Get().Delete(*UTDFSaveSubsystem::SlotPath(Slot)); return true;
        }
        return false;
    }
private:
    FAutomationTestBase* Test; FName Slot,KeyId; FGuid DoorId; FVector SavedLocation; int32 Stage=0; double Start=FPlatformTime::Seconds();
};
}
#if WITH_EDITOR
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FTDFSavePIE,"TDF.Save.Flow.PIE",EAutomationTestFlags::EditorContext|EAutomationTestFlags::EngineFilter)
bool FTDFSavePIE::RunTest(const FString&) {AutomationOpenMap(TEXT("/Game/Game/Maps/Test/M_Test_SaveStreaming"),true); ADD_LATENT_AUTOMATION_COMMAND(FStartPIECommand(false)); ADD_LATENT_AUTOMATION_COMMAND(FWaitLatentCommand(1.f)); ADD_LATENT_AUTOMATION_COMMAND(FSaveFlow(this)); ADD_LATENT_AUTOMATION_COMMAND(FEndPlayMapCommand()); return true;}
#endif
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FTDFSaveGameFlow,"TDF.Save.Flow.Standalone",EAutomationTestFlags::ClientContext|EAutomationTestFlags::EngineFilter)
bool FTDFSaveGameFlow::RunTest(const FString&) {AutomationOpenMap(TEXT("/Game/Game/Maps/Test/M_Test_SaveStreaming"),true); ADD_LATENT_AUTOMATION_COMMAND(FWaitLatentCommand(2.f)); ADD_LATENT_AUTOMATION_COMMAND(FSaveFlow(this)); return true;}
#endif
