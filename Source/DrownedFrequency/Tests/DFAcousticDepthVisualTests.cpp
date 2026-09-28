#if WITH_DEV_AUTOMATION_TESTS
#include "Misc/AutomationTest.h"
#include "Tests/AutomationCommon.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "Engine/GameViewportClient.h"
#include "EngineUtils.h"
#include "Misc/Paths.h"
#include "Misc/App.h"
#include "HAL/FileManager.h"
#include "AudioMixerBlueprintLibrary.h"
#include "Player/DFPlayerController.h"
#include "Player/DFPlayerCharacter.h"
#include "Acoustics/DFAudioAnomalyActor.h"
#include "Acoustics/DFRecorderComponent.h"
#include "Acoustics/DFRecorderPlaybackComponent.h"
#include "UI/DFRecorderWidget.h"
#include "Components/TextBlock.h"
#include "Acoustics/DFAcousticSpeakerActor.h"
#include "Acoustics/DFAcousticPipeNode.h"
class FTDFAcousticDepthCaptures:public IAutomationLatentCommand
{
public:
    explicit FTDFAcousticDepthCaptures(FAutomationTestBase* T):Test(T){}
    bool Update() override
    {
        UWorld* W=nullptr; for(const auto& C:GEngine->GetWorldContexts()) if(C.WorldType==EWorldType::Game) W=C.World();
        auto* PC=W?Cast<ATDFPlayerController>(W->GetFirstPlayerController()):nullptr; auto* P=PC?Cast<ATDFPlayerCharacter>(PC->GetPawn()):nullptr;
        if(!P) {Test->AddError(TEXT("Capture requires game world")); return true;}
        if(Started<0)
        {
            Started=FPlatformTime::Seconds();
            if(Stage==0)
            {
                auto* Controls=PC->RecorderWidget?Cast<UTextBlock>(PC->RecorderWidget->GetWidgetFromName(TEXT("RecorderControls"))):nullptr;
                Test->TestTrue(TEXT("Initial recorder glyphs resolved without interaction"),Controls && !Controls->GetText().ToString().Contains(TEXT("Unbound")));
                P->SetActorLocation(FVector(-1560,-420,92)); PC->SetControlRotation(FRotator(-3,24,0));
                PreviousUnfocusedVolume=FApp::GetUnfocusedVolumeMultiplier(); FApp::SetUnfocusedVolumeMultiplier(1.f);
                Test->TestNotNull(TEXT("Real audio device active"),W->GetAudioDeviceRaw());
                UAudioMixerBlueprintLibrary::StartRecordingOutput(W,35.f);
            }
            if(Stage==2)
            {
                P->SetActorLocation(FVector(-1550,220,92)); PC->SetControlRotation(FRotator(0,0,0));
                for(TActorIterator<ATDFAudioAnomalyActor> It(W);It;++It) if(It->ActorHasTag(TEXT("Source_ServiceBell")))
                {
                    Test->TestTrue(TEXT("Capture records real source"),P->Recorder->TryRecordAnomaly(*It,0));
                    FTDFRecorderPlaybackRequest Request; Request.SlotIndex=0;
                    Test->TestTrue(TEXT("Audible local playback requested"),P->RecorderPlayback->PlayRecordedSlot(Request)); break;
                }
            }
            if(Stage==3) for(TActorIterator<ATDFAcousticPipeNode> I(W);I;++I) if(I->bIsInput) I->Interact_Implementation(P);
            if(Stage==4) {for(TActorIterator<ATDFAcousticSpeakerActor> I(W);I;++I) I->BroadcastRecordedSignal(P->Recorder,0); P->SetActorLocation(FVector(2670,80,92)); PC->SetControlRotation(FRotator(-4,32,0));}
        }
        if(FPlatformTime::Seconds()-Started<(Stage==0?16.:4.)) return false;
        if(Stage<6 && Stage%2==0) FScreenshotRequest::RequestScreenshot(FPaths::ConvertRelativePathToFull(FPaths::ProjectSavedDir()/TEXT("Screenshots")/Names[Stage/2]),true,false);
        if(Stage==5) UAudioMixerBlueprintLibrary::StopRecordingOutput(W,EAudioRecordingExportType::WavFile,TEXT("Phase5InEngineMix"),FPaths::ConvertRelativePathToFull(FPaths::ProjectSavedDir()/TEXT("AudioQA")));
        if(Stage==6)
        {
            for(const auto* Name:Names) Test->TestTrue(TEXT("Rendered capture saved"),IFileManager::Get().FileExists(*(FPaths::ProjectSavedDir()/TEXT("Screenshots")/Name)));
            Test->TestTrue(TEXT("Actual engine output exported"),IFileManager::Get().FileSize(*(FPaths::ProjectSavedDir()/TEXT("AudioQA/Phase5InEngineMix.wav")))>48000);
            FApp::SetUnfocusedVolumeMultiplier(PreviousUnfocusedVolume);
            return true;
        }
        Started=-1; ++Stage; return false;
    }
private:
    FAutomationTestBase* Test; int32 Stage=0; double Started=-1;
    float PreviousUnfocusedVolume=0.f;
    const TCHAR* Names[3]={TEXT("Phase5Entrance.png"),TEXT("Phase5Recorder.png"),TEXT("Phase5Corner.png")};
};
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FTDFAcousticDepthVisualTest,"TDF.Visual.Phase5Captures",EAutomationTestFlags::ClientContext|EAutomationTestFlags::EngineFilter)
bool FTDFAcousticDepthVisualTest::RunTest(const FString&) {ADD_LATENT_AUTOMATION_COMMAND(FTDFAcousticDepthCaptures(this)); return true;}
#endif
