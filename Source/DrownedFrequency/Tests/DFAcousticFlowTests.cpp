#if WITH_DEV_AUTOMATION_TESTS
#include "Misc/AutomationTest.h"
#include "Tests/AutomationCommon.h"
#if WITH_EDITOR
#include "Tests/AutomationEditorCommon.h"
#include "Editor.h"
#endif
#include "Engine/Engine.h"
#include "EngineUtils.h"
#include "InputKeyEventArgs.h"
#include "Player/DFPlayerCharacter.h"
#include "Player/DFPlayerController.h"
#include "Acoustics/DFRecorderComponent.h"
#include "Acoustics/DFRecorderOperatorComponent.h"
#include "Acoustics/DFRecorderPlaybackComponent.h"
#include "Acoustics/DFAcousticPuckActor.h"
#include "Acoustics/DFAcousticReceiverActor.h"
#include "World/DFDoorActor.h"
#include "UI/DFRecorderWidget.h"
namespace
{
UWorld* AcousticTestWorld()
{
#if WITH_EDITOR
    if(GEditor && GEditor->PlayWorld) return GEditor->PlayWorld;
#endif
    for(const auto& C:GEngine->GetWorldContexts()) if(C.WorldType==EWorldType::Game) return C.World();
    return nullptr;
}
template<class T> T* Tagged(UWorld* W,const TCHAR* Tag) {for(TActorIterator<T> It(W);It;++It) if(It->ActorHasTag(Tag)) return *It; return nullptr;}
class FAcousticFlow:public IAutomationLatentCommand
{
public:
    FAcousticFlow(FAutomationTestBase* InTest,bool Pad):Test(InTest),bPad(Pad){}
    bool Update() override
    {
        auto* W=AcousticTestWorld(); auto* PC=W?Cast<ATDFPlayerController>(W->GetFirstPlayerController()):nullptr; auto* P=PC?Cast<ATDFPlayerCharacter>(PC->GetPawn()):nullptr;
        if(!P) {Test->AddError(TEXT("Acoustic route needs possessed player")); return true;}
        auto* Direct=Tagged<ATDFDoorActor>(W,TEXT("Gate_Direct")); auto* Remote=Tagged<ATDFDoorActor>(W,TEXT("Gate_Puck"));
        if(!Direct || !Remote) {Test->AddError(TEXT("Authored acoustic gates missing")); return true;}
        const FKey Record=bPad?EKeys::Gamepad_RightTrigger:EKeys::R, Play=bPad?EKeys::Gamepad_DPad_Up:EKeys::F, Erase=bPad?EKeys::Gamepad_DPad_Down:EKeys::Delete, Project=bPad?EKeys::Gamepad_RightThumbstick:EKeys::G;
        auto Key=[PC](FKey K,EInputEvent E){PC->InputKey(FInputKeyEventArgs::CreateSimulated(K,E,E==IE_Pressed?1.f:0.f));};
        if(Started<0)
        {
            Started=FPlatformTime::Seconds();
            switch(Stage)
            {
            case 0:
                PC->ResetBindingsToDefault(false); PC->FlushPressedKeys();
                P->SetActorLocation(FVector(-1550,220,92)); PC->SetControlRotation(FRotator::ZeroRotator);
                Test->TestNotNull(TEXT("Recorder HUD exists"),PC->RecorderWidget.Get());
                Direct->Interact_Implementation(P); Test->TestFalse(TEXT("Acoustic gate cannot be manually bypassed"),Direct->bIsOpen); break;
            case 1: Key(Record,IE_Pressed); break;
            case 2:
                Key(Record,IE_Released); Test->TestTrue(TEXT("Mapped hold records authored signal"),P->Recorder->HasValidSignal(0));
                P->SetActorLocation(FVector(-400,-250,92)); PC->SetControlRotation(FRotator::ZeroRotator); break;
            case 3: Key(Play,IE_Pressed); break;
            case 4: Key(Play,IE_Released); Test->TestTrue(TEXT("Local replay opens direct gate"),Direct->bIsOpen); Test->TestFalse(TEXT("Direct playback cannot open puck gate"),Remote->bIsOpen); break;
            case 5: Key(Erase,IE_Pressed); break;
            case 6:
                Key(Erase,IE_Released); Test->TestFalse(TEXT("Mapped erase clears slot"),P->Recorder->HasValidSignal(0));
                P->SetActorLocation(FVector(-1550,220,92)); PC->SetControlRotation(FRotator::ZeroRotator); break;
            case 7: Key(Record,IE_Pressed); break;
            case 8:
                Key(Record,IE_Released); Test->TestTrue(TEXT("Required clip reacquired after deletion"),P->Recorder->HasValidSignal(0));
                P->SetActorLocation(FVector(650,450,92)); PC->SetControlRotation(FRotator(-30,0,0)); break;
            case 9: Key(Project,IE_Pressed); break;
            case 10:
                Key(Project,IE_Released); Test->TestTrue(TEXT("Placed puck opens remote receiver gate"),Remote->bIsOpen);
                Test->TestEqual(TEXT("One puck deployed"),P->RecorderOperator->GetPucksAvailable(),2);
                for(TActorIterator<ATDFAcousticPuckActor> It(W);It;++It) if(It->GetOwner()==P)
                {
                    Test->TestTrue(TEXT("Remote origin separate from player"),!It->Playback->LastPlayback.WorldLocation.Equals(P->GetActorLocation(),100));
                    P->SetActorLocation(It->GetActorLocation()+FVector(-50,0,90));
                    Test->TestTrue(TEXT("Puck recovered"),P->RecorderOperator->RecoverPuck(*It)); break;
                }
                Test->TestEqual(TEXT("Three pucks after recovery"),P->RecorderOperator->GetPucksAvailable(),3);
                Key(bPad?EKeys::Gamepad_DPad_Right:EKeys::RightBracket,IE_Pressed); break;
            case 11:
                Key(bPad?EKeys::Gamepad_DPad_Right:EKeys::RightBracket,IE_Released);
                Test->TestEqual(TEXT("Mapped slot selection"),P->Recorder->SelectedSlot,1);
                PC->LoadInputSettings(); return true;
            }
        }
        if(FPlatformTime::Seconds()-Started<((Stage==1 || Stage==7)?1.6:.4)) return false;
        Started=-1; ++Stage; return false;
    }
private:
    FAutomationTestBase* Test; bool bPad; int32 Stage=0; double Started=-1;
};
}
#if WITH_EDITOR
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FTDFAcousticKeyboardPIE,"TDF.Acoustic.PIE.Keyboard",EAutomationTestFlags::EditorContext|EAutomationTestFlags::EngineFilter)
bool FTDFAcousticKeyboardPIE::RunTest(const FString&)
{
    AutomationOpenMap(TEXT("/Game/Game/Maps/Test/M_Test_AcousticSpine"),true); ADD_LATENT_AUTOMATION_COMMAND(FStartPIECommand(false)); ADD_LATENT_AUTOMATION_COMMAND(FWaitLatentCommand(1.f)); ADD_LATENT_AUTOMATION_COMMAND(FAcousticFlow(this,false)); ADD_LATENT_AUTOMATION_COMMAND(FEndPlayMapCommand()); return true;
}
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FTDFAcousticControllerPIE,"TDF.Acoustic.PIE.Controller",EAutomationTestFlags::EditorContext|EAutomationTestFlags::EngineFilter)
bool FTDFAcousticControllerPIE::RunTest(const FString&)
{
    AutomationOpenMap(TEXT("/Game/Game/Maps/Test/M_Test_AcousticSpine"),true); ADD_LATENT_AUTOMATION_COMMAND(FStartPIECommand(false)); ADD_LATENT_AUTOMATION_COMMAND(FWaitLatentCommand(1.f)); ADD_LATENT_AUTOMATION_COMMAND(FAcousticFlow(this,true)); ADD_LATENT_AUTOMATION_COMMAND(FEndPlayMapCommand()); return true;
}
#endif
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FTDFAcousticKeyboardGame,"TDF.Acoustic.Standalone.Keyboard",EAutomationTestFlags::ClientContext|EAutomationTestFlags::EngineFilter)
bool FTDFAcousticKeyboardGame::RunTest(const FString&)
{
    AutomationOpenMap(TEXT("/Game/Game/Maps/Test/M_Test_AcousticSpine"),true); ADD_LATENT_AUTOMATION_COMMAND(FWaitLatentCommand(2.f)); ADD_LATENT_AUTOMATION_COMMAND(FAcousticFlow(this,false)); return true;
}
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FTDFAcousticControllerGame,"TDF.Acoustic.Standalone.Controller",EAutomationTestFlags::ClientContext|EAutomationTestFlags::EngineFilter)
bool FTDFAcousticControllerGame::RunTest(const FString&)
{
    AutomationOpenMap(TEXT("/Game/Game/Maps/Test/M_Test_AcousticSpine"),true); ADD_LATENT_AUTOMATION_COMMAND(FWaitLatentCommand(2.f)); ADD_LATENT_AUTOMATION_COMMAND(FAcousticFlow(this,true)); return true;
}
#endif
