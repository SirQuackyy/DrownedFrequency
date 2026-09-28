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
#include "Input/DFInputConfig.h"
#include "EnhancedInputSubsystems.h"
#include "Engine/LocalPlayer.h"
#include "InputKeyEventArgs.h"
#include "Player/DFPlayerCameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Camera/CameraTypes.h"
#include "Components/BoxComponent.h"
#include "Interaction/DFInteractionComponent.h"
#include "Tests/DFInteractionTestTarget.h"
#include "UI/DFInteractionPromptWidget.h"
#include "Components/TextBlock.h"
#include "UnrealClient.h"
#include "Misc/Paths.h"
#include "HAL/FileManager.h"

static UWorld* GetTDFTestWorld()
{
#if WITH_EDITOR
    if (GEditor && GEditor->PlayWorld) return GEditor->PlayWorld;
#endif
    for (const FWorldContext& Context : GEngine->GetWorldContexts())
        if (Context.WorldType == EWorldType::Game) return Context.World();
    return nullptr;
}
class FTDFInteractionFlowCommand : public IAutomationLatentCommand
{
public:
    explicit FTDFInteractionFlowCommand(FAutomationTestBase* InTest) : Test(InTest) {}
    virtual bool Update() override
    {
        auto* PC = GetTDFTestWorld() ? Cast<ATDFPlayerController>(GetTDFTestWorld()->GetFirstPlayerController()) : nullptr;
        auto* Pawn = PC ? Cast<ATDFPlayerCharacter>(PC->GetPawn()) : nullptr;
        if (!Pawn || !Pawn->Interaction) { Test->AddError(TEXT("Interaction component missing")); return true; }
        auto Key = [PC](FKey K, EInputEvent Event) { PC->InputKey(FInputKeyEventArgs::CreateSimulated(K, Event, Event == IE_Pressed ? 1.f : 0.f)); };
        if (StartTime < 0.)
        {
            StartTime = FPlatformTime::Seconds();
            if (Stage == 0)
            {
                PC->FlushPressedKeys();
                Pawn->SetActorLocation(FVector(0.f, 0.f, 92.f));
                PC->SetControlRotation(FRotator::ZeroRotator);
                Target = GetTDFTestWorld()->SpawnActor<ATDFInteractionTestTarget>(FVector(150.f, 0.f, 167.f), FRotator::ZeroRotator);
            }
            if (Stage == 1) Key(EKeys::LeftMouseButton, IE_Pressed);
            if (Stage == 2) { Key(EKeys::LeftMouseButton, IE_Released); Target->bInteractionEnabled = false; }
            if (Stage == 3) Target->bInteractionEnabled = true;
            if (Stage == 4) Key(EKeys::Gamepad_FaceButton_Left, IE_Pressed);
            if (Stage == 5) { Key(EKeys::Gamepad_FaceButton_Left, IE_Released); PC->SetControlRotation(FRotator(0.f, 180.f, 0.f)); }
            if (Stage == 6) PC->SetControlRotation(FRotator::ZeroRotator);
            if (Stage == 7)
            {
                Pawn->Interaction->RefreshFocus();
                Target->SetActorLocation(FVector(1000.f, 0.f, 167.f));
                Test->TestFalse(TEXT("Activation rechecks stale target range"), Pawn->Interaction->TryInteract());
            }
            if (Stage == 8)
            {
                Target->SetActorLocation(FVector(150.f, 0.f, 167.f));
                Wall = GetTDFTestWorld()->SpawnActor<AActor>();
                auto* Box = NewObject<UBoxComponent>(Wall.Get());
                Wall->SetRootComponent(Box);
                Box->SetBoxExtent(FVector(10.f, 60.f, 80.f));
                Box->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
                Box->SetCollisionResponseToAllChannels(ECR_Ignore);
                Box->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
                Box->RegisterComponent();
                Wall->SetActorLocation(FVector(75.f, 0.f, 167.f));
            }
            if (Stage == 9) Wall->Destroy();
            if (Stage == 10) Target->Destroy();
        }
        if (FPlatformTime::Seconds() - StartTime < .25) return false;
        if (Stage == 0 || Stage == 3 || Stage == 6 || Stage == 9) Test->TestTrue(TEXT("Aim focuses the eligible fixture"), Pawn->Interaction->GetFocusedActor() == Target.Get());
        if (Stage == 1) Test->TestEqual(TEXT("Mouse binding executes one interaction"), Target->InteractionCount, 1);
        if (Stage == 4) Test->TestEqual(TEXT("Gamepad binding executes one interaction"), Target->InteractionCount, 2);
        if (Stage == 1 || Stage == 4)
        {
            const FKey Expected = Stage == 1 ? EKeys::LeftMouseButton : EKeys::Gamepad_FaceButton_Left;
            Test->TestEqual(TEXT("Active device matches input"), PC->LastInputDevice, Stage == 1 ? ETDFInputDevice::KeyboardMouse : ETDFInputDevice::Gamepad);
            if (PC->PromptWidget)
                Test->TestEqual(TEXT("Prompt resolves effective action binding"), PC->PromptWidget->CurrentGlyph.ToString(), Expected.GetDisplayName().ToString());
        }
        if (Stage == 2 || Stage == 5 || Stage == 7 || Stage == 8 || Stage == 10) Test->TestNull(TEXT("Disabled, occluded, distant, destroyed, or off-aim fixture clears focus"), Pawn->Interaction->GetFocusedActor());
        if (Stage == 8) Test->TestFalse(TEXT("Cannot activate through a wall"), Pawn->Interaction->TryInteract());
        if (Test->TestNotNull(TEXT("Player owns prompt widget"), PC->PromptWidget.Get()))
        {
            const bool bFocused = Pawn->Interaction->GetFocusedActor() != nullptr;
            Test->TestEqual(TEXT("Prompt visibility follows focus"), PC->PromptWidget->IsVisible(), bFocused);
            if (bFocused)
            {
                auto* Label = Cast<UTextBlock>(PC->PromptWidget->GetWidgetFromName(TEXT("PromptLabel")));
                if (Test->TestNotNull(TEXT("Blueprint contains visible prompt label"), Label))
                    Test->TestEqual(TEXT("Rendered prompt comes from interactable"), Label->GetText().ToString(), Target->Prompt.ToString());
            }
        }
        StartTime = -1.;
        return ++Stage > 10;
    }
private:
    FAutomationTestBase* Test;
    int32 Stage = 0;
    double StartTime = -1.;
    TWeakObjectPtr<ATDFInteractionTestTarget> Target;
    TWeakObjectPtr<AActor> Wall;
};

class FTDFBaselineInputCommand : public IAutomationLatentCommand
{
public:
    explicit FTDFBaselineInputCommand(FAutomationTestBase* InTest) : Test(InTest) {}
    virtual bool Update() override
    {
        auto* PC = GetTDFTestWorld() ? Cast<ATDFPlayerController>(GetTDFTestWorld()->GetFirstPlayerController()) : nullptr;
        auto* Pawn = PC ? Cast<ATDFPlayerCharacter>(PC->GetPawn()) : nullptr;
        if (!PC || !Pawn) { Test->AddError(TEXT("Player missing during input test")); return true; }
        auto Key = [PC](FKey K, EInputEvent Event, float Amount) { PC->InputKey(FInputKeyEventArgs::CreateSimulated(K, Event, Amount)); };
        if (StartTime < 0.)
        {
            StartTime = FPlatformTime::Seconds();
            StartPosition = Pawn->GetActorLocation();
            StartYaw = PC->GetControlRotation().Yaw;
            if (Stage == 24 || Stage == 29)
            {
                // Start each sustained-motion scenario on the test platform.
                Pawn->SetActorLocation(FVector(0.f, 0.f, 100.f));
                Pawn->GetCharacterMovement()->StopMovementImmediately();
                PC->SetControlRotation(FRotator::ZeroRotator);
            }
            if (Stage == 0) Key(EKeys::W, IE_Pressed, 1.f);
            if (Stage == 2) Key(EKeys::LeftControl, IE_Pressed, 1.f);
            if (Stage == 3) Key(EKeys::LeftControl, IE_Released, 0.f);
            if (Stage == 6) Key(EKeys::Gamepad_FaceButton_Right, IE_Pressed, 1.f);
            if (Stage == 7) Key(EKeys::Gamepad_FaceButton_Right, IE_Released, 0.f);
            if (Stage == 8) Key(EKeys::LeftShift, IE_Pressed, 1.f);
            if (Stage == 9) Key(EKeys::W, IE_Pressed, 1.f);
            if (Stage == 10) Key(EKeys::LeftShift, IE_Released, 0.f);
            if (Stage == 11) { Key(EKeys::LeftShift, IE_Pressed, 1.f); Key(EKeys::LeftControl, IE_Pressed, 1.f); }
            if (Stage == 12) { Key(EKeys::LeftShift, IE_Released, 0.f); Key(EKeys::LeftControl, IE_Released, 0.f); Key(EKeys::W, IE_Released, 0.f); }
            if (Stage == 13) Key(EKeys::Gamepad_LeftThumbstick, IE_Pressed, 1.f);
            if (Stage == 14) Key(EKeys::Gamepad_LeftThumbstick, IE_Released, 0.f);
            if (Stage == 15) Key(EKeys::Gamepad_LeftY, IE_Axis, 0.f);
            if (Stage == 16) Key(EKeys::Q, IE_Pressed, 1.f);
            if (Stage == 17) Key(EKeys::Q, IE_Released, 0.f);
            if (Stage == 18) Key(EKeys::E, IE_Pressed, 1.f);
            if (Stage == 19) Key(EKeys::E, IE_Released, 0.f);
            if (Stage == 20) Key(EKeys::Gamepad_LeftShoulder, IE_Pressed, 1.f);
            if (Stage == 21) Key(EKeys::Gamepad_LeftShoulder, IE_Released, 0.f);
            if (Stage == 22) Key(EKeys::Gamepad_RightShoulder, IE_Pressed, 1.f);
            if (Stage == 23) Key(EKeys::Gamepad_RightShoulder, IE_Released, 0.f);
            if (Stage == 24) { Key(EKeys::W, IE_Pressed, 1.f); Key(EKeys::LeftShift, IE_Pressed, 1.f); }
            if (Stage == 25) Key(EKeys::RightMouseButton, IE_Pressed, 1.f);
            if (Stage == 26) Key(EKeys::RightMouseButton, IE_Released, 0.f);
            if (Stage == 27) Key(EKeys::X, IE_Pressed, 1.f);
            if (Stage == 28) PC->FlushPressedKeys();
            if (Stage == 29) Key(EKeys::Gamepad_LeftThumbstick, IE_Pressed, 1.f);
            if (Stage == 30) Key(EKeys::Gamepad_LeftTrigger, IE_Pressed, 1.f);
            if (Stage == 31) { Key(EKeys::Gamepad_LeftTrigger, IE_Released, 0.f); PC->FlushPressedKeys(); }
            if (Stage == 32)
            {
                Obstruction = GetTDFTestWorld()->SpawnActor<AActor>();
                auto* Box = NewObject<UBoxComponent>(Obstruction.Get());
                Obstruction->SetRootComponent(Box);
                Box->SetBoxExtent(FVector(30.f, 8.f, 40.f));
                Box->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
                Box->SetCollisionResponseToAllChannels(ECR_Ignore);
                Box->SetCollisionResponseToChannel(ECC_Camera, ECR_Block);
                Box->RegisterComponent();
                Obstruction->SetActorLocation(Pawn->PlayerCamera->GetComponentLocation() + Pawn->GetActorRightVector() * 22.f);
                Obstruction->SetActorRotation(Pawn->GetActorRotation());
                Key(EKeys::E, IE_Pressed, 1.f);
            }
            if (Stage == 33) { Key(EKeys::E, IE_Released, 0.f); if (Obstruction.IsValid()) Obstruction->Destroy(); }
        }
        if (Stage == 1) Key(EKeys::MouseX, IE_Axis, 5.f);
        if (Stage == 4) Key(EKeys::Gamepad_LeftY, IE_Axis, .8f);
        if (Stage == 5) Key(EKeys::Gamepad_RightX, IE_Axis, .7f);
        if (Stage == 13 || Stage == 14) Key(EKeys::Gamepad_LeftY, IE_Axis, 1.f);
        if (Stage == 29 || Stage == 30) Key(EKeys::Gamepad_LeftY, IE_Axis, 1.f);
        // Physical controllers continue sending centered axes after release.
        if (Stage != 4 && Stage != 13 && Stage != 14 && Stage != 29 && Stage != 30) Key(EKeys::Gamepad_LeftY, IE_Axis, 0.f);
        if (Stage != 5) Key(EKeys::Gamepad_RightX, IE_Axis, 0.f);
        if (FPlatformTime::Seconds() - StartTime < .4) return false;
        if (Stage == 0 || Stage == 4) Test->TestTrue(TEXT("Mapped movement changes position"), FVector::Dist2D(StartPosition, Pawn->GetActorLocation()) > 10.f);
        if (Stage == 1 || Stage == 5) Test->TestTrue(TEXT("Mapped look changes yaw"), FMath::Abs(FMath::FindDeltaAngleDegrees(StartYaw, PC->GetControlRotation().Yaw)) > 1.f);
        if (Stage == 1) Test->TestEqual(TEXT("Mouse movement selects KBM despite neutral gamepad events"), PC->LastInputDevice, ETDFInputDevice::KeyboardMouse);
        if (Stage == 5) Test->TestEqual(TEXT("Stick movement selects gamepad"), PC->LastInputDevice, ETDFInputDevice::Gamepad);
        if (Stage == 2 || Stage == 6) Test->TestTrue(TEXT("Mapped crouch shrinks character"), Pawn->bIsCrouched);
        if (Stage == 2 || Stage == 6) Test->TestTrue(TEXT("Crouch camera converges to eye height"), FMath::Abs(Pawn->PlayerCamera->GetRelativeLocation().Z - Pawn->PlayerCamera->CrouchEyeZ) < 2.f);
        if (Stage == 3 || Stage == 7) Test->TestFalse(TEXT("Crouch release restores standing"), Pawn->bIsCrouched);
        if (Stage == 3 || Stage == 7) Test->TestTrue(TEXT("Standing camera height restores without drift"), FMath::Abs(Pawn->PlayerCamera->GetRelativeLocation().Z - Pawn->PlayerCamera->StandingEyeZ) < 2.f);
        if (Stage == 8) Test->TestFalse(TEXT("Sprint does not activate at rest"), Pawn->CurrentMovementState.bIsSprinting);
        if (Stage == 9 || Stage == 13) Test->TestTrue(TEXT("Both devices can hold sprint while moving"), Pawn->CurrentMovementState.bIsSprinting);
        if (Stage == 10 || Stage == 14) Test->TestEqual(TEXT("Sprint release restores walk speed"), Pawn->GetCharacterMovement()->MaxWalkSpeed, Pawn->WalkSpeed);
        if (Stage == 11) Test->TestFalse(TEXT("Crouch cancels sprint immediately"), Pawn->CurrentMovementState.bIsSprinting);
        if (Stage >= 16 && Stage <= 23)
        {
            const float Offset = Pawn->PlayerCamera->GetRelativeLocation().Y;
            if (Stage == 16 || Stage == 20) Test->TestTrue(TEXT("Both devices lean camera left"), Offset < -5.f);
            if (Stage == 18 || Stage == 22) Test->TestTrue(TEXT("Both devices lean camera right"), Offset > 5.f);
            if (Stage % 2 == 1) Test->TestTrue(TEXT("Lean release returns camera to center"), FMath::Abs(Offset) < .5f);
            Test->TestTrue(TEXT("Lean never translates the capsule"), FVector::Dist2D(StartPosition, Pawn->GetActorLocation()) < 1.f);
            if (Stage == 18)
            {
                FMinimalViewInfo View;
                Pawn->PlayerCamera->GetCameraView(0.f, View);
                Test->TestTrue(TEXT("Camera view preserves lean roll despite control rotation"), View.Rotation.Roll > 4.f);
                Pawn->PlayerCamera->bReducedMotion = true;
                Pawn->PlayerCamera->GetCameraView(0.f, View);
                Test->TestTrue(TEXT("Reduced motion suppresses roll"), FMath::Abs(View.Rotation.Roll) < .01f);
                Test->TestTrue(TEXT("Reduced motion retains useful lean offset"), Pawn->PlayerCamera->GetRelativeLocation().Y > 5.f);
                Pawn->PlayerCamera->bReducedMotion = false;
            }
        }
        if (Stage == 24 || Stage == 29) Test->TestTrue(FString::Printf(TEXT("Sprint before breath stage=%d position=%s grounded=%d wants=%d breath=%d speed=%.1f"), Stage, *Pawn->GetActorLocation().ToString(), Pawn->GetCharacterMovement()->IsMovingOnGround(), Pawn->CurrentMovementState.bWantsSprint, Pawn->CurrentMovementState.bHoldingBreath, Pawn->GetVelocity().Size2D()), Pawn->CurrentMovementState.bIsSprinting);
        if (Stage == 25 || Stage == 27 || Stage == 30)
        {
            Test->TestTrue(TEXT("Breath input enters held state"), Pawn->CurrentMovementState.bHoldingBreath);
            Test->TestFalse(TEXT("Held breath cancels sprint"), Pawn->CurrentMovementState.bIsSprinting);
        }
        if (Stage == 26 || Stage == 28 || Stage == 31)
        {
            Test->TestFalse(TEXT("Breath release or focus flush clears state"), Pawn->CurrentMovementState.bHoldingBreath);
            Test->TestFalse(TEXT("Canceled sprint does not silently restart"), Pawn->CurrentMovementState.bWantsSprint);
        }
        if (Stage == 32) Test->TestTrue(TEXT("Lean collision prevents camera entering nearby wall"), Pawn->PlayerCamera->GetRelativeLocation().Y < 7.f);
        if (Stage == 0) Key(EKeys::W, IE_Released, 0.f);
        if (Stage == 4) Key(EKeys::Gamepad_LeftY, IE_Axis, 0.f);
        if (Stage == 5) Key(EKeys::Gamepad_RightX, IE_Axis, 0.f);
        StartTime = -1.;
        return ++Stage > 33;
    }
private:
    FAutomationTestBase* Test;
    int32 Stage = 0;
    double StartTime = -1.;
    FVector StartPosition;
    float StartYaw = 0.f;
    TWeakObjectPtr<AActor> Obstruction;
};

DEFINE_LATENT_AUTOMATION_COMMAND_ONE_PARAMETER(FTDFVerifyPlayerPIE, FAutomationTestBase*, Test);
bool FTDFVerifyPlayerPIE::Update()
{
    UWorld* World = GetTDFTestWorld();
    if (!Test->TestNotNull(TEXT("PIE world exists"), World)) return true;
    auto* Controller = Cast<ATDFPlayerController>(World->GetFirstPlayerController());
    if (!Test->TestNotNull(TEXT("Project controller spawned by map GameMode"), Controller)) return true;
    Test->TestNotNull(TEXT("Project pawn possessed"), Cast<ATDFPlayerCharacter>(Controller->GetPawn()));
    Test->TestTrue(TEXT("Controller initialized both layouts"), Controller->AreInputContextsReady());
    Test->AddExpectedError(TEXT("TDF Input: missing InputConfig"), EAutomationExpectedErrorFlags::Contains, 1);
    UTDFInputConfig* SavedConfig = Controller->InputConfig;
    Controller->InputConfig = nullptr;
    Test->TestFalse(TEXT("Local input initialization fails safely with missing assets"), Controller->InitializePlayerInputContexts());
    Controller->InputConfig = SavedConfig;
    Test->TestTrue(TEXT("Repeated initialization is safe"), Controller->InitializePlayerInputContexts());
    if (Controller->InputConfig && Controller->GetLocalPlayer())
    {
        auto* Input = Controller->GetLocalPlayer()->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
        if (Test->TestNotNull(TEXT("Enhanced Input subsystem"), Input))
        {
            Test->TestTrue(TEXT("KBM mapping applied"), Input->HasMappingContext(Controller->InputConfig->KeyboardMouseContext));
            Test->TestTrue(TEXT("Gamepad mapping applied"), Input->HasMappingContext(Controller->InputConfig->GamepadContext));
        }
    }
    return true;
}

#if WITH_EDITOR
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FTDFPlayerPIETest, "TDF.Player.PIE.InputContexts",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FTDFPlayerPIETest::RunTest(const FString& Parameters)
{
    AutomationOpenMap(TEXT("/Game/Game/Maps/Test/L_TDF_InputGym"));
    ADD_LATENT_AUTOMATION_COMMAND(FStartPIECommand(false));
    ADD_LATENT_AUTOMATION_COMMAND(FWaitLatentCommand(1.0f));
    ADD_LATENT_AUTOMATION_COMMAND(FTDFVerifyPlayerPIE(this));
    ADD_LATENT_AUTOMATION_COMMAND(FTDFBaselineInputCommand(this));
    ADD_LATENT_AUTOMATION_COMMAND(FTDFInteractionFlowCommand(this));
    ADD_LATENT_AUTOMATION_COMMAND(FEndPlayMapCommand());
    return true;
}
#endif

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FTDFPlayerStandaloneTest, "TDF.Player.Standalone.InputFlow",
    EAutomationTestFlags::ClientContext | EAutomationTestFlags::EngineFilter)
bool FTDFPlayerStandaloneTest::RunTest(const FString& Parameters)
{
    ADD_LATENT_AUTOMATION_COMMAND(FWaitLatentCommand(1.f));
    ADD_LATENT_AUTOMATION_COMMAND(FTDFVerifyPlayerPIE(this));
    ADD_LATENT_AUTOMATION_COMMAND(FTDFBaselineInputCommand(this));
    ADD_LATENT_AUTOMATION_COMMAND(FTDFInteractionFlowCommand(this));
    return true;
}

class FTDFLightingCaptureCommand : public IAutomationLatentCommand
{
public:
    explicit FTDFLightingCaptureCommand(FAutomationTestBase* InTest) : Test(InTest) {}
    bool Update() override
    {
        UWorld* World = GetTDFTestWorld();
        auto* PC = World ? Cast<ATDFPlayerController>(World->GetFirstPlayerController()) : nullptr;
        if (!PC || !PC->GetPawn()) { Test->AddError(TEXT("Visual capture needs a player world")); return true; }
        if (StartTime < 0.)
        {
            StartTime = FPlatformTime::Seconds();
            if (Stage == 0) { PC->GetPawn()->SetActorLocation(FVector(-650, -580, 92)); PC->SetControlRotation(FRotator(-3, 32, 0)); }
            if (Stage == 2) { PC->GetPawn()->SetActorLocation(FVector(760, 0, 92)); PC->SetControlRotation(FRotator::ZeroRotator); }
        }
        if (FPlatformTime::Seconds() - StartTime < (Stage == 0 ? 20. : 5.)) return false;
        if (Stage == 0 || Stage == 2)
        {
            if (Stage == 2) Test->TestTrue(TEXT("Interaction prompt visible in rendered game"), PC->PromptWidget && PC->PromptWidget->IsVisible());
            const FString Path = FPaths::ConvertRelativePathToFull(FPaths::ProjectSavedDir() / TEXT("Screenshots") / (Stage == 0 ? TEXT("Phase2Lighting.png") : TEXT("Phase2Prompt.png")));
            FScreenshotRequest::RequestScreenshot(Path, true, false);
            Test->AddInfo(FString::Printf(TEXT("Requested rendered capture: %s"), *Path));
        }
        if (Stage == 3)
        {
            for (const TCHAR* Name : {TEXT("Phase2Lighting.png"), TEXT("Phase2Prompt.png")})
                Test->TestTrue(TEXT("Rendered capture exists"), IFileManager::Get().FileExists(*(FPaths::ProjectSavedDir() / TEXT("Screenshots") / Name)));
        }
        StartTime = -1.;
        return ++Stage > 3;
    }
private:
    FAutomationTestBase* Test;
    int32 Stage = 0;
    double StartTime = -1.;
};

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FTDFLightingCaptureTest, "TDF.Visual.FoundationCaptures",
    EAutomationTestFlags::ClientContext | EAutomationTestFlags::EngineFilter)
bool FTDFLightingCaptureTest::RunTest(const FString& Parameters)
{
    ADD_LATENT_AUTOMATION_COMMAND(FTDFLightingCaptureCommand(this));
    return true;
}
#endif
