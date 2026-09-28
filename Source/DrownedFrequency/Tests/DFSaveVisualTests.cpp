#if WITH_DEV_AUTOMATION_TESTS
#include "Misc/AutomationTest.h"
#include "Tests/AutomationCommon.h"
#include "Engine/Engine.h"
#include "Engine/GameInstance.h"
#include "Engine/GameViewportClient.h"
#include "EngineUtils.h"
#include "Misc/Paths.h"
#include "Misc/App.h"
#include "HAL/FileManager.h"
#include "AudioMixerBlueprintLibrary.h"
#include "World/DFLevelStreamingSubsystem.h"
#include "World/DFObjectiveSubsystem.h"
#include "Save/DFCheckpointSubsystem.h"
#include "Save/DFSaveSubsystem.h"
#include "Player/DFPlayerController.h"
#include "Player/DFPlayerCharacter.h"
#include "Acoustics/DFAudioAnomalyActor.h"
#include "Acoustics/DFRecorderComponent.h"
#include "Acoustics/DFRecorderPlaybackComponent.h"
class FTDFSaveCaptures:public IAutomationLatentCommand
{
    FAutomationTestBase* Test; int32 Stage=0; double Next=0,Start=FPlatformTime::Seconds(); float PreviousVolume=0;
public:
    explicit FTDFSaveCaptures(FAutomationTestBase* T):Test(T){}
    bool Update() override
    {
        if(FPlatformTime::Seconds()-Start>90) {Test->AddError(TEXT("Save visual test timed out")); return true;}
        UWorld* W=nullptr; for(const auto& C:GEngine->GetWorldContexts()) if(C.WorldType==EWorldType::Game) W=C.World();
        auto* PC=W?Cast<ATDFPlayerController>(W->GetFirstPlayerController()):nullptr; auto* P=PC?Cast<ATDFPlayerCharacter>(PC->GetPawn()):nullptr; if(!P) return false;
        auto* L=W->GetGameInstance()->GetSubsystem<UTDFLevelStreamingSubsystem>(); auto* O=W->GetGameInstance()->GetSubsystem<UTDFObjectiveSubsystem>(); auto* CP=W->GetGameInstance()->GetSubsystem<UTDFCheckpointSubsystem>();
        if(Stage==0) {CP->CheckpointSlot=TEXT("QA_Visual_CP"); CP->ChapterStartSlot=TEXT("QA_Visual_Start"); O->AutosaveSlot=TEXT("QA_Visual_Auto"); L->RequestSectorSet({TEXT("Intro"),TEXT("RadioRoom")},TEXT("VisualQA")); ++Stage; return false;}
        if(L->bStreamingTransitionActive || FPlatformTime::Seconds()<Next) return false;
        if(Stage==1)
        {
            Test->TestTrue(TEXT("Visual chapter startup"),O->StartChapter(TEXT("QA_Archive"))); P->SetActorLocation(FVector(600,240,92)); PC->SetControlRotation(FRotator(-3,65,0));
            PreviousVolume=FApp::GetUnfocusedVolumeMultiplier(); FApp::SetUnfocusedVolumeMultiplier(1.f); Test->TestNotNull(TEXT("Real mixer device"),W->GetAudioDeviceRaw()); UAudioMixerBlueprintLibrary::StartRecordingOutput(W,30.f); Next=FPlatformTime::Seconds()+14;
        }
        if(Stage==2) {Capture(TEXT("Phase6Archive.png")); Next=FPlatformTime::Seconds()+2;}
        if(Stage==3)
        {
            bool Found=false;
            for(TActorIterator<ATDFAudioAnomalyActor> I(W);I;++I) if(I->ActorHasTag(TEXT("Save_Source"))) {Found=true; P->SetActorLocation(I->GetActorLocation()+FVector(-180,0,-53)); PC->SetControlRotation(FRotator::ZeroRotator); Test->TestTrue(TEXT("Visual source recorded"),P->Recorder->TryRecordAnomaly(*I,0)); FTDFRecorderPlaybackRequest R; R.SlotIndex=0; Test->TestTrue(TEXT("Real audible playback"),P->RecorderPlayback->PlayRecordedSlot(R)); break;}
            Test->TestTrue(TEXT("Authored source exists"),Found); Next=FPlatformTime::Seconds()+.65;
        }
        if(Stage==4) {Capture(TEXT("Phase6Caption.png")); Next=FPlatformTime::Seconds()+2;}
        if(Stage==5) {P->SetActorLocation(FVector(1580,330,92)); PC->SetControlRotation(FRotator(-3,42,0)); Next=FPlatformTime::Seconds()+5;}
        if(Stage==6) {Capture(TEXT("Phase6Radio.png")); Next=FPlatformTime::Seconds()+2;}
        if(Stage==7) {UAudioMixerBlueprintLibrary::StopRecordingOutput(W,EAudioRecordingExportType::WavFile,TEXT("Phase6InEngineMix"),FPaths::ConvertRelativePathToFull(FPaths::ProjectSavedDir()/TEXT("AudioQA"))); Next=FPlatformTime::Seconds()+2;}
        if(Stage==8)
        {
            for(const TCHAR* Name:{TEXT("Phase6Archive.png"),TEXT("Phase6Caption.png"),TEXT("Phase6Radio.png")}) Test->TestTrue(TEXT("Fresh rendered image"),IFileManager::Get().FileExists(*(FPaths::ProjectSavedDir()/TEXT("Screenshots")/Name)));
            Test->TestTrue(TEXT("Engine mix exported"),IFileManager::Get().FileSize(*(FPaths::ProjectSavedDir()/TEXT("AudioQA/Phase6InEngineMix.wav")))>48000); FApp::SetUnfocusedVolumeMultiplier(PreviousVolume);
            for(FName Slot:{CP->CheckpointSlot,CP->ChapterStartSlot,O->AutosaveSlot}) IFileManager::Get().Delete(*UTDFSaveSubsystem::SlotPath(Slot)); return true;
        }
        ++Stage; return false;
    }
    void Capture(const TCHAR* Name) {FScreenshotRequest::RequestScreenshot(FPaths::ConvertRelativePathToFull(FPaths::ProjectSavedDir()/TEXT("Screenshots")/Name),true,false);}
};
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FTDFSaveVisualTest,"TDF.Visual.Phase6Captures",EAutomationTestFlags::ClientContext|EAutomationTestFlags::EngineFilter)
bool FTDFSaveVisualTest::RunTest(const FString&) {AutomationOpenMap(TEXT("/Game/Game/Maps/Test/M_Test_SaveStreaming"),true); ADD_LATENT_AUTOMATION_COMMAND(FWaitLatentCommand(2.f)); ADD_LATENT_AUTOMATION_COMMAND(FTDFSaveCaptures(this)); return true;}
#endif
