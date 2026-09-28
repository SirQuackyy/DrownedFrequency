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
#include "Player/DFPlayerCharacter.h"
#include "Player/DFPlayerController.h"
#include "Acoustics/DFAudioAnomalyActor.h"
#include "Acoustics/DFRecorderComponent.h"
#include "Acoustics/DFRecorderPlaybackComponent.h"
#include "Acoustics/DFAcousticPuckActor.h"
#include "Acoustics/DFAcousticPipeNode.h"
#include "Acoustics/DFAcousticSpeakerActor.h"
#include "Acoustics/DFAcousticReceiverActor.h"
#include "Acoustics/DFAcousticWorldSubsystem.h"
#include "Acoustics/DFAcousticCaptionSubsystem.h"
#include "World/DFDoorActor.h"
namespace
{
template<class T> T* DepthTagged(UWorld* W,const TCHAR* Tag) {for(TActorIterator<T> I(W);I;++I) if(I->ActorHasTag(Tag)) return *I; return nullptr;}
class FDepthFlow:public IAutomationLatentCommand
{
    FAutomationTestBase* Test;
public:
    FDepthFlow(FAutomationTestBase* T):Test(T){}
    bool Update() override
    {
        UWorld* W=nullptr;
#if WITH_EDITOR
        if(GEditor) W=GEditor->PlayWorld;
#endif
        if(!W) for(const auto& C:GEngine->GetWorldContexts()) if(C.WorldType==EWorldType::Game) W=C.World();
        auto* PC=W?Cast<ATDFPlayerController>(W->GetFirstPlayerController()):nullptr; auto* P=PC?Cast<ATDFPlayerCharacter>(PC->GetPawn()):nullptr;
        if(!Test->TestNotNull(TEXT("Possessed player"),P)) return true;
        auto* Bell=DepthTagged<ATDFAudioAnomalyActor>(W,TEXT("Source_ServiceBell")); auto* Pipe=DepthTagged<ATDFAcousticPipeNode>(W,TEXT("Pipe_Input")); auto* Speaker=DepthTagged<ATDFAcousticSpeakerActor>(W,TEXT("Speaker_Station")); auto* Recovery=DepthTagged<ATDFAcousticReceiverActor>(W,TEXT("Receiver_Recovery"));
        if(!Bell || !Pipe || !Speaker || !Recovery) {Test->AddError(TEXT("Acoustic depth fixtures missing")); return true;}
        auto Record=[&](){P->SetActorLocation(FVector(-1550,220,92)); PC->SetControlRotation(FRotator::ZeroRotator); return P->Recorder->TryRecordAnomaly(Bell,0);};
        Test->TestTrue(TEXT("Record authored source"),Record());
        auto* Bus=W->GetSubsystem<UTDFAcousticWorldSubsystem>(); auto* Captions=W->GetGameInstance()->GetSubsystem<UTDFAcousticCaptionSubsystem>();
        Captions->Clear(); const int32 Before=Bus->GetRecentStimuli().Num(); FTDFRecorderPlaybackRequest Request; Request.SlotIndex=0; Request.bEmitWorldStimulus=false;
        Test->TestTrue(TEXT("Private playback"),P->RecorderPlayback->PlayRecordedSlot(Request)); Test->TestEqual(TEXT("Private playback does not emit world event"),Bus->GetRecentStimuli().Num(),Before); Test->TestTrue(TEXT("Private caption visible"),Captions->GetActiveCaptions().Num()>0); Test->TestNotNull(TEXT("Caption widget instantiated"),PC->CaptionWidget.Get());
        P->SetActorLocation(FVector(-400,-250,92)); Request.bEmitWorldStimulus=true; Test->TestTrue(TEXT("World playback"),P->RecorderPlayback->PlayRecordedSlot(Request));
        auto Open=[&](const TCHAR* Tag){auto* D=DepthTagged<ATDFDoorActor>(W,Tag); return D && D->bIsOpen;}; Test->TestTrue(TEXT("Direct gate opens"),Open(TEXT("Gate_Direct")));
        Test->TestTrue(TEXT("Fresh tape for clean puck receiver"),Record()); auto* Puck=W->SpawnActor<ATDFAcousticPuckActor>(); Puck->SetOwner(P); Puck->SetActorLocation(FVector(900,490,100)); Test->TestTrue(TEXT("Puck projects"),Puck->ProjectRecordedSignal(P->Recorder,0)); Test->TestTrue(TEXT("Puck gate opens"),Open(TEXT("Gate_Puck"))); Puck->Destroy();
        P->SetActorLocation(Pipe->GetActorLocation()+FVector(-100,0,0)); Pipe->Interact_Implementation(P); Test->TestTrue(TEXT("Pipe hidden receiver gate opens"),Open(TEXT("Gate_Pipe")));
        P->SetActorLocation(Speaker->GetActorLocation()+FVector(-100,0,0)); Speaker->Interact_Implementation(P); Test->TestTrue(TEXT("Speaker receiver gate opens"),Open(TEXT("Gate_Speaker")));
        Test->TestTrue(TEXT("Delete every recording"),P->Recorder->DeleteSlot(0)); Test->TestFalse(TEXT("Recorder empty"),P->Recorder->HasValidSignal(0)); Test->TestTrue(TEXT("Recovery interaction offered"),Recovery->CanInteract_Implementation(P)); Recovery->Interact_Implementation(P); Test->TestTrue(TEXT("Explicit ledger fallback completes receiver"),Recovery->bIsSatisfied);
        return true;
    }
};
}
#if WITH_EDITOR
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FTDFDepthPIE,"TDF.Acoustic.A24.PIE",EAutomationTestFlags::EditorContext|EAutomationTestFlags::EngineFilter)
bool FTDFDepthPIE::RunTest(const FString&) {AutomationOpenMap(TEXT("/Game/Game/Maps/Test/M_Test_AcousticDepth"),true); ADD_LATENT_AUTOMATION_COMMAND(FStartPIECommand(false)); ADD_LATENT_AUTOMATION_COMMAND(FWaitLatentCommand(2.f)); ADD_LATENT_AUTOMATION_COMMAND(FDepthFlow(this)); ADD_LATENT_AUTOMATION_COMMAND(FEndPlayMapCommand()); return true;}
#endif
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FTDFDepthGame,"TDF.Acoustic.A24.Standalone",EAutomationTestFlags::ClientContext|EAutomationTestFlags::EngineFilter)
bool FTDFDepthGame::RunTest(const FString&) {AutomationOpenMap(TEXT("/Game/Game/Maps/Test/M_Test_AcousticDepth"),true); ADD_LATENT_AUTOMATION_COMMAND(FWaitLatentCommand(2.f)); ADD_LATENT_AUTOMATION_COMMAND(FDepthFlow(this)); return true;}
#endif
