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
#include "Save/DFCheckpointSubsystem.h"
#include "World/DFLevelStreamingSubsystem.h"
#include "World/DFObjectiveSubsystem.h"
#include "World/DFDoorActor.h"
#include "World/DFSwitchActor.h"
#include "Player/DFPlayerCharacter.h"
#include "Player/DFPlayerController.h"
#include "Acoustics/DFAudioAnomalyActor.h"
#include "Acoustics/DFRecorderComponent.h"
#include "Acoustics/DFRecorderPlaybackComponent.h"
#include "Acoustics/DFRecorderOperatorComponent.h"
#include "UI/DFSaveDebugWidget.h"
#include "Components/TextBlock.h"
#include "HAL/FileManager.h"
namespace
{
template<class T> T* ProgressTagged(UWorld* W,const TCHAR* Tag) {for(TActorIterator<T> I(W);I;++I) if(I->ActorHasTag(Tag)) return *I; return nullptr;}
class FProgressFlow:public IAutomationLatentCommand
{
    FAutomationTestBase* Test; int32 Stage=0; double Started=FPlatformTime::Seconds(); FName Slot; TArray<FGuid> PuckIds;
public:
    FProgressFlow(FAutomationTestBase* T):Test(T),Slot(*(TEXT("QA_Progress_")+FGuid::NewGuid().ToString())){}
    bool Update() override
    {
        if(FPlatformTime::Seconds()-Started>60) {Test->AddError(TEXT("Progression/streaming test timeout")); return true;}
        UWorld* W=nullptr;
#if WITH_EDITOR
        if(GEditor) W=GEditor->PlayWorld;
#endif
        if(!W) for(const auto& C:GEngine->GetWorldContexts()) if(C.WorldType==EWorldType::Game) W=C.World();
        auto* PC=W?Cast<ATDFPlayerController>(W->GetFirstPlayerController()):nullptr; auto* P=PC?Cast<ATDFPlayerCharacter>(PC->GetPawn()):nullptr; if(!P) return false;
        auto* S=W->GetGameInstance()->GetSubsystem<UTDFSaveSubsystem>(); auto* L=W->GetGameInstance()->GetSubsystem<UTDFLevelStreamingSubsystem>(); auto* O=W->GetGameInstance()->GetSubsystem<UTDFObjectiveSubsystem>(); auto* CP=W->GetGameInstance()->GetSubsystem<UTDFCheckpointSubsystem>();
        if(Stage==0) {CP->CheckpointSlot=FName(*(Slot.ToString()+TEXT("_CP"))); CP->ChapterStartSlot=FName(*(Slot.ToString()+TEXT("_Start"))); O->AutosaveSlot=FName(*(Slot.ToString()+TEXT("_Auto"))); L->RequestSectorSet({TEXT("Intro")},TEXT("ProgressionQA")); ++Stage; return false;}
        if(L->bStreamingTransitionActive || S->IsLoading()) return false;
        if(Stage==1)
        {
            Test->TestTrue(TEXT("Start authored chapter"),O->StartChapter(TEXT("QA_Archive"))); auto* Bell=ProgressTagged<ATDFAudioAnomalyActor>(W,TEXT("Save_Source")); if(!Bell) {Test->AddError(TEXT("Missing objective source")); return true;}
            P->SetActorLocation(Bell->GetActorLocation()+FVector(-180,0,-53)); PC->SetControlRotation(FRotator::ZeroRotator); Test->TestTrue(TEXT("Record advances objective"),P->Recorder->TryRecordAnomaly(Bell,0)); Test->TestTrue(TEXT("Next objective active"),O->GetObjectiveState(TEXT("QA_Relay"))==ETDFObjectiveState::Active);
            auto* Text=PC->SaveDebugWidget?Cast<UTextBlock>(PC->SaveDebugWidget->GetWidgetFromName(TEXT("ObjectiveText"))):nullptr; Test->TestTrue(TEXT("UI updated by objective delegate"),Text && Text->GetText().ToString().Contains(TEXT("Replay")));
            P->SetActorLocation(FVector(900,900,92)); FTDFRecorderPlaybackRequest R; R.SlotIndex=0; Test->TestTrue(TEXT("Replay completes relay objective"),P->RecorderPlayback->PlayRecordedSlot(R)); Test->TestTrue(TEXT("Radio objective active"),O->GetObjectiveState(TEXT("QA_Radio"))==ETDFObjectiveState::Active);
            TArray<FTDFPuckSaveData> Pucks; for(int32 I=0;I<3;++I) {FTDFPuckSaveData D; D.InstanceId=FGuid::NewGuid(); D.Transform.SetLocation(FVector(400+I*50,500,8)); Pucks.Add(D);} P->RecorderOperator->RestorePlacedPucks(Pucks); Test->TestEqual(TEXT("Reload safety keeps one puck available"),P->RecorderOperator->GetPucksAvailable(),1); for(const auto& D:P->RecorderOperator->CapturePlacedPucks()) PuckIds.Add(D.InstanceId);
            Test->TestTrue(TEXT("Progress save"),S->WriteSave(Slot,ETDFSaveReason::Debug)); FText Error; auto* Saved=S->ReadObject(Slot,Error); if(Saved) {FTDFActorSaveData Missing; Missing.StableActorId=FGuid::NewGuid(); Missing.StateName=TEXT("PropertiesV1"); Saved->Actors.Add(Missing); S->WriteObject(Slot,Saved,Error);}
            P->SetActorLocation(FVector(1200,0,92)); L->RequestSectorSet({TEXT("RadioRoom")},TEXT("ProgressionQA")); ++Stage; return false;
        }
        if(Stage==2)
        {
            FText CheckpointError; auto* ProgressCheckpoint=S->ReadObject(CP->CheckpointSlot,CheckpointError);
            Test->TestTrue(TEXT("Objective autosave retains checkpoint floor"),ProgressCheckpoint && ProgressCheckpoint->LoadedSectorIds.Contains(TEXT("Intro")));
            Test->TestTrue(TEXT("Objective autosave updates death recovery"),ProgressCheckpoint && ProgressCheckpoint->Objectives.ContainsByPredicate([](const FTDFObjectiveSaveData& R){return R.ObjectiveId==TEXT("QA_Radio") && R.State==ETDFObjectiveState::Active;}));
            auto* Switch=ProgressTagged<ATDFSwitchActor>(W,TEXT("Save_ObjectiveSwitch")); if(!Switch) {Test->AddError(TEXT("Missing objective switch")); return true;} Switch->Interact_Implementation(P); Test->TestTrue(TEXT("Interactable completes chapter"),O->IsChapterComplete()); Test->TestFalse(TEXT("Duplicate completion ignored"),O->CompleteObjective(TEXT("QA_Radio")));
            Test->AddExpectedMessage(TEXT("missing saved actor"),ELogVerbosity::Warning); Test->TestTrue(TEXT("Load objective snapshot"),S->RequestLoadSlot(Slot)); ++Stage; return false;
        }
        if(Stage==3)
        {
            Test->TestTrue(TEXT("Active objective restored"),O->GetObjectiveState(TEXT("QA_Radio"))==ETDFObjectiveState::Active); Test->TestTrue(TEXT("Completed objective restored"),O->GetObjectiveState(TEXT("QA_Record"))==ETDFObjectiveState::Completed);
            auto Restored=P->RecorderOperator->CapturePlacedPucks(); Test->TestEqual(TEXT("Deployed puck count restored"),Restored.Num(),2); for(const auto& D:Restored) Test->TestTrue(TEXT("Puck identity retained"),PuckIds.Contains(D.InstanceId));
            P->SetActorLocation(FVector(1200,0,92)); L->RequestSectorSet({TEXT("RadioRoom")},TEXT("UnloadCacheQA")); ++Stage; return false;
        }
        if(Stage==4) {L->RequestSectorSet({TEXT("Intro")},TEXT("ReloadCacheQA")); ++Stage; return false;}
        if(Stage==5)
        {
            auto* Door=ProgressTagged<ATDFDoorActor>(W,TEXT("Save_Door")); Test->TestTrue(TEXT("Unloaded sector puzzle state cached"),Door && Door->bIsOpen);
            L->RequestSectorSet({TEXT("RadioRoom")},TEXT("Rapid1")); L->RequestSectorSet({TEXT("Intro")},TEXT("Rapid2")); L->RequestSectorSet({TEXT("RadioRoom")},TEXT("Rapid3")); L->RequestSectorSet({TEXT("Intro")},TEXT("RapidFinal")); Test->TestFalse(TEXT("Rapid requests block save"),S->CanSaveNow()); ++Stage; return false;
        }
        Test->TestTrue(TEXT("Final request wins"),L->GetLoadedSectorIds().Num()==1 && L->GetLoadedSectorIds().Contains(TEXT("Intro"))); Test->TestTrue(TEXT("Save recovers after transition"),S->CanSaveNow());
        Test->AddExpectedMessage(TEXT("TDF Streaming unavailable"),ELogVerbosity::Warning); Test->TestFalse(TEXT("Unknown sector rejected"),L->RequestSectorSet({TEXT("Missing_QA_Sector")},TEXT("InvalidQA"))); Test->TestTrue(TEXT("Invalid request keeps existing floor"),L->GetLoadedSectorIds().Contains(TEXT("Intro")));
        L->RequestSectorSet({TEXT("RadioRoom")},TEXT("TimeoutQA")); L->ExpireTransitionForTest(); L->Tick(0);
        Test->TestFalse(TEXT("Timeout clears transition"),L->bStreamingTransitionActive); Test->TestFalse(TEXT("Timeout releases player"),PC->IsWorldTransitionLocked()); Test->TestTrue(TEXT("Timeout retains safe floor"),L->GetLoadedSectorIds().Contains(TEXT("Intro"))); Test->TestTrue(TEXT("Timeout releases save gate"),S->CanSaveNow()); Test->TestFalse(TEXT("Timeout explains recovery"),L->LastFailure.IsEmpty());
        for(const auto Name:{Slot,CP->CheckpointSlot,CP->ChapterStartSlot,O->AutosaveSlot}) IFileManager::Get().Delete(*S->SlotPath(Name)); return true;
    }
};
}
#if WITH_EDITOR
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FTDFProgressPIE,"TDF.Save.Progression.PIE",EAutomationTestFlags::EditorContext|EAutomationTestFlags::EngineFilter)
bool FTDFProgressPIE::RunTest(const FString&) {AutomationOpenMap(TEXT("/Game/Game/Maps/Test/M_Test_SaveStreaming"),true); ADD_LATENT_AUTOMATION_COMMAND(FStartPIECommand(false)); ADD_LATENT_AUTOMATION_COMMAND(FWaitLatentCommand(1.f)); ADD_LATENT_AUTOMATION_COMMAND(FProgressFlow(this)); ADD_LATENT_AUTOMATION_COMMAND(FEndPlayMapCommand()); return true;}
#endif
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FTDFProgressGame,"TDF.Save.Progression.Standalone",EAutomationTestFlags::ClientContext|EAutomationTestFlags::EngineFilter)
bool FTDFProgressGame::RunTest(const FString&) {AutomationOpenMap(TEXT("/Game/Game/Maps/Test/M_Test_SaveStreaming"),true); ADD_LATENT_AUTOMATION_COMMAND(FWaitLatentCommand(2.f)); ADD_LATENT_AUTOMATION_COMMAND(FProgressFlow(this)); return true;}
#endif
