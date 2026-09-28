#if WITH_DEV_AUTOMATION_TESTS
#include "Misc/AutomationTest.h"
#include "Tests/AutomationCommon.h"
#include "Engine/Engine.h"
#include "Engine/GameInstance.h"
#include "EngineUtils.h"
#include "Save/DFSaveSubsystem.h"
#include "Save/DFCheckpointSubsystem.h"
#include "World/DFLevelStreamingSubsystem.h"
#include "World/DFObjectiveSubsystem.h"
#include "World/DFDoorActor.h"
#include "World/DFWorldItemActor.h"
#include "Inventory/DFInventoryComponent.h"
#include "Player/DFPlayerCharacter.h"
#include "Player/DFPlayerController.h"
#include "Player/DFHealthComponent.h"
#include "Acoustics/DFAudioAnomalyActor.h"
#include "Acoustics/DFRecorderComponent.h"
#include "Acoustics/DFRecorderPlaybackComponent.h"
#include "Acoustics/DFRecorderOperatorComponent.h"
#include "HAL/FileManager.h"
namespace
{
template<class T> T* RelaunchTagged(UWorld* W,const TCHAR* Tag) {for(TActorIterator<T> I(W);I;++I) if(I->ActorHasTag(Tag)) return *I; return nullptr;}
class FRelaunchFlow:public IAutomationLatentCommand
{
    FAutomationTestBase* Test; bool bRead; int32 Stage=0; double Started=FPlatformTime::Seconds();
public:
    FRelaunchFlow(FAutomationTestBase* T,bool Read):Test(T),bRead(Read){}
    bool Update() override
    {
        if(FPlatformTime::Seconds()-Started>65) {Test->AddError(TEXT("Fresh-process save test timed out")); return true;}
        UWorld* W=nullptr; for(const auto& C:GEngine->GetWorldContexts()) if(C.WorldType==EWorldType::Game) W=C.World();
        auto* PC=W?Cast<ATDFPlayerController>(W->GetFirstPlayerController()):nullptr; auto* P=PC?Cast<ATDFPlayerCharacter>(PC->GetPawn()):nullptr; if(!P) return false;
        auto* S=W->GetGameInstance()->GetSubsystem<UTDFSaveSubsystem>(); auto* L=W->GetGameInstance()->GetSubsystem<UTDFLevelStreamingSubsystem>(); auto* O=W->GetGameInstance()->GetSubsystem<UTDFObjectiveSubsystem>(); auto* CP=W->GetGameInstance()->GetSubsystem<UTDFCheckpointSubsystem>();
        if(Stage==0)
        {
            CP->CheckpointSlot=TEXT("QA_Relaunch_CP"); CP->ChapterStartSlot=TEXT("QA_Relaunch_Start"); O->AutosaveSlot=TEXT("QA_Relaunch_Auto"); CP->bSearchManualHistory=false;
            if(bRead) {Test->TestTrue(TEXT("Fresh process begins in another map"),W->GetName().Contains(TEXT("InputGym"))); if(!S->RequestLoadSlot(TEXT("QA_Relaunch"))) {Test->AddError(S->LastMessage.ToString()); return true;}}
            else L->RequestSectorSet({TEXT("Intro")},TEXT("RelaunchQA"));
            ++Stage; return false;
        }
        if(S->IsLoading() || L->bStreamingTransitionActive) return false;
        if(!bRead)
        {
            Test->TestTrue(TEXT("Start saved chapter"),O->StartChapter(TEXT("QA_Archive")));
            auto* Bell=RelaunchTagged<ATDFAudioAnomalyActor>(W,TEXT("Save_Source")); auto* Key=RelaunchTagged<ATDFWorldItemActor>(W,TEXT("Save_Key"));
            if(!Bell || !Key) {Test->AddError(TEXT("Missing relaunch fixtures")); return true;}
            P->SetActorLocation(Bell->GetActorLocation()+FVector(-180,0,-53)); PC->SetControlRotation(FRotator::ZeroRotator); Test->TestTrue(TEXT("Record persisted signal"),P->Recorder->TryRecordAnomaly(Bell,0)); Key->Interact_Implementation(P);
            P->SetActorLocation(FVector(900,900,92)); FTDFRecorderPlaybackRequest Request; Request.SlotIndex=0; Test->TestTrue(TEXT("Solve persisted puzzle"),P->RecorderPlayback->PlayRecordedSlot(Request));
            FTDFPuckSaveData D; D.InstanceId=FGuid(101,202,303,404); D.Transform.SetLocation(FVector(400,500,8)); P->RecorderOperator->RestorePlacedPucks({D});
            P->Health->RestoreHealth(73); P->SetActorLocation(FVector(500,600,92)); PC->SetControlRotation(FRotator(0,35,0));
            Test->TestTrue(TEXT("Write durable restart fixture"),S->WriteSave(TEXT("QA_Relaunch"),ETDFSaveReason::Manual)); return true;
        }
        Test->TestTrue(TEXT("Persistent map opened from saved path"),W->GetName().Contains(TEXT("SaveStreaming")));
        Test->TestTrue(TEXT("Saved sector loaded after restart"),L->GetLoadedSectorIds().Contains(TEXT("Intro")));
        Test->TestTrue(TEXT("Player placed after restart"),P->GetActorLocation().Equals(FVector(500,600,92),5));
        Test->TestEqual(TEXT("Health persisted across process"),P->Health->Health,73.f);
        Test->TestTrue(TEXT("Control rotation persisted"),FMath::IsNearlyEqual(PC->GetControlRotation().Yaw,35.f,1.f));
        Test->TestTrue(TEXT("Recorder persisted across process"),P->Recorder->HasValidSignal(0));
        Test->TestTrue(TEXT("Key inventory persisted"),!P->Inventory->GetOwnedKeyIds().IsEmpty());
        auto* Door=RelaunchTagged<ATDFDoorActor>(W,TEXT("Save_Door")); Test->TestTrue(TEXT("GUID puzzle state persisted"),Door && Door->bIsOpen);
        Test->TestTrue(TEXT("Objective state persisted"),O->GetObjectiveState(TEXT("QA_Radio"))==ETDFObjectiveState::Active);
        const auto Pucks=P->RecorderOperator->CapturePlacedPucks(); Test->TestTrue(TEXT("Puck ID persisted"),Pucks.Num()==1 && Pucks[0].InstanceId==FGuid(101,202,303,404));
        Test->TestFalse(TEXT("Controls released after cross-map load"),PC->IsWorldTransitionLocked());
        for(FName Slot:{FName(TEXT("QA_Relaunch")),CP->CheckpointSlot,CP->ChapterStartSlot,O->AutosaveSlot}) IFileManager::Get().Delete(*S->SlotPath(Slot));
        return true;
    }
};
}
// These two tests deliberately run in separate packaged processes, in this order.
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FTDFSaveRestartWrite,"TDF.Save.Relaunch.Write",EAutomationTestFlags::ClientContext|EAutomationTestFlags::EngineFilter)
bool FTDFSaveRestartWrite::RunTest(const FString&) {AutomationOpenMap(TEXT("/Game/Game/Maps/Test/M_Test_SaveStreaming"),true); ADD_LATENT_AUTOMATION_COMMAND(FWaitLatentCommand(2.f)); ADD_LATENT_AUTOMATION_COMMAND(FRelaunchFlow(this,false)); return true;}
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FTDFSaveRestartRead,"TDF.Save.Relaunch.Read",EAutomationTestFlags::ClientContext|EAutomationTestFlags::EngineFilter)
bool FTDFSaveRestartRead::RunTest(const FString&) {AutomationOpenMap(TEXT("/Game/Game/Maps/Test/L_TDF_InputGym"),true); ADD_LATENT_AUTOMATION_COMMAND(FWaitLatentCommand(2.f)); ADD_LATENT_AUTOMATION_COMMAND(FRelaunchFlow(this,true)); return true;}
#endif
