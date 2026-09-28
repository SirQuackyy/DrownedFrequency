#if WITH_DEV_AUTOMATION_TESTS
#include "Misc/AutomationTest.h"
#include "Tests/AutomationCommon.h"
#if WITH_EDITOR
#include "Tests/AutomationEditorCommon.h"
#include "Editor.h"
#endif
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "Player/DFPlayerController.h"
#include "Player/DFPlayerCharacter.h"
#include "Tests/DFInteractionTestTarget.h"
#include "InputKeyEventArgs.h"
#include "Input/DFInputConfig.h"
#include "UI/DFInteractionPromptWidget.h"
#include "Interaction/DFInteractionComponent.h"

static UWorld* InteractionTestWorld()
{
#if WITH_EDITOR
    if (GEditor && GEditor->PlayWorld) return GEditor->PlayWorld;
#endif
    for (const FWorldContext& Context:GEngine->GetWorldContexts()) if (Context.WorldType==EWorldType::Game) return Context.World();
    return nullptr;
}
class FTDFRemapFlow : public IAutomationLatentCommand
{
public:
    explicit FTDFRemapFlow(FAutomationTestBase* InTest):Test(InTest){}
    bool Update() override
    {
        auto* W=InteractionTestWorld();
        auto* PC=W ? Cast<ATDFPlayerController>(W->GetFirstPlayerController()) : nullptr;
        auto* P=PC ? Cast<ATDFPlayerCharacter>(PC->GetPawn()) : nullptr;
        if (!P) { Test->AddError(TEXT("Remap test requires possessed player")); return true; }
        if (Started<0.)
        {
            Started=FPlatformTime::Seconds();
            auto Key=[PC](FKey K,EInputEvent E){ PC->InputKey(FInputKeyEventArgs::CreateSimulated(K,E,E==IE_Pressed ? 1.f : 0.f)); };
            switch (Stage)
            {
            case 0:
                PC->ResetBindingsToDefault(false); PC->FlushPressedKeys();
                P->SetActorLocation(FVector(0,0,92)); PC->SetControlRotation(FRotator::ZeroRotator);
                Target=W->SpawnActor<ATDFInteractionTestTarget>(FVector(150,0,167),FRotator::ZeroRotator);
                break;
            case 1: Test->TestTrue(TEXT("Keyboard rebind accepted"),PC->RebindAction(TEXT("Interact"),EKeys::K,ETDFInputDevice::KeyboardMouse,false)); break;
            case 2: Key(EKeys::K,IE_Pressed); break;
            case 3:
                Key(EKeys::K,IE_Released);
                Test->TestEqual(TEXT("New keyboard key executes interaction"),Target->InteractionCount,1);
                Test->TestEqual(TEXT("Prompt resolves remapped key"),PC->GetActionInputLabel(PC->InputConfig->InteractAction).ToString(),EKeys::K.GetDisplayName().ToString());
                break;
            case 4: Key(EKeys::LeftMouseButton,IE_Pressed); break;
            case 5: Key(EKeys::LeftMouseButton,IE_Released); Test->TestEqual(TEXT("Old key no longer executes"),Target->InteractionCount,1); break;
            case 6: Test->TestTrue(TEXT("Controller rebind accepted"),PC->RebindAction(TEXT("Interact"),EKeys::Gamepad_Special_Left_Touched,ETDFInputDevice::Gamepad,false)); break;
            case 7: Key(EKeys::Gamepad_Special_Left_Touched,IE_Pressed); break;
            case 8:
                Key(EKeys::Gamepad_Special_Left_Touched,IE_Released);
                Test->TestEqual(TEXT("New controller key executes"),Target->InteractionCount,2);
                Test->TestEqual(TEXT("Controller prompt resolves effective key"),PC->GetActionInputLabel(PC->InputConfig->InteractAction).ToString(),EKeys::Gamepad_Special_Left_Touched.GetDisplayName().ToString());
                break;
            case 9:
                Test->TestFalse(TEXT("Movement binding conflict rejected"),PC->RebindAction(TEXT("Interact"),EKeys::W,ETDFInputDevice::KeyboardMouse,false));
                Test->TestFalse(TEXT("Wrong-device key rejected"),PC->RebindAction(TEXT("Interact"),EKeys::K,ETDFInputDevice::Gamepad,false));
                PC->ResetBindingsToDefault(false); break;
            case 10: Key(EKeys::LeftMouseButton,IE_Pressed); break;
            case 11:
                Key(EKeys::LeftMouseButton,IE_Released);
                Test->TestEqual(TEXT("Reset restores original binding"),Target->InteractionCount,3);
                Target->Destroy(); PC->LoadInputSettings(); return true;
            }
        }
        if (FPlatformTime::Seconds()-Started<.3) return false;
        Started=-1.; ++Stage;
        return false;
    }
private:
    FAutomationTestBase* Test;
    TWeakObjectPtr<ATDFInteractionTestTarget> Target;
    int32 Stage=0;
    double Started=-1.;
};
#if WITH_EDITOR
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FTDFRemapPIETest,"TDF.World.PIE.Remapping",EAutomationTestFlags::EditorContext|EAutomationTestFlags::EngineFilter)
bool FTDFRemapPIETest::RunTest(const FString& Parameters)
{
    AutomationOpenMap(TEXT("/Game/Game/Maps/Test/L_TDF_InputGym"));
    ADD_LATENT_AUTOMATION_COMMAND(FStartPIECommand(false));
    ADD_LATENT_AUTOMATION_COMMAND(FWaitLatentCommand(1.f));
    ADD_LATENT_AUTOMATION_COMMAND(FTDFRemapFlow(this));
    ADD_LATENT_AUTOMATION_COMMAND(FEndPlayMapCommand());
    return true;
}
#endif
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FTDFRemapStandaloneTest,"TDF.World.Standalone.Remapping",EAutomationTestFlags::ClientContext|EAutomationTestFlags::EngineFilter)
bool FTDFRemapStandaloneTest::RunTest(const FString& Parameters)
{
    ADD_LATENT_AUTOMATION_COMMAND(FWaitLatentCommand(1.f));
    ADD_LATENT_AUTOMATION_COMMAND(FTDFRemapFlow(this));
    return true;
}
#endif
