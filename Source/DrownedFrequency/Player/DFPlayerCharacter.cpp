#include "Player/DFPlayerCharacter.h"
#include "Player/DFHealthComponent.h"
#include "Player/DFPlayerController.h"
#include "Player/DFPlayerCameraComponent.h"
#include "Input/DFInputConfig.h"
#include "EnhancedInputComponent.h"
#include "InputActionValue.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Interaction/DFInteractionComponent.h"
#include "Inventory/DFInventoryComponent.h"
#include "UI/DFInspectionWidget.h"
#include "Acoustics/DFRecorderComponent.h"
#include "Acoustics/DFRecorderOperatorComponent.h"
#include "Acoustics/DFRecorderPlaybackComponent.h"
#include "Acoustics/DFAcousticPuzzleLedgerComponent.h"
#include "Acoustics/DFPlayerNoiseEmitterComponent.h"

ATDFPlayerCharacter::ATDFPlayerCharacter()
{
    Health=CreateDefaultSubobject<UTDFHealthComponent>(TEXT("Health"));
    PrimaryActorTick.bCanEverTick = true;
    GetCapsuleComponent()->InitCapsuleSize(34.f, 90.f);
    BaseEyeHeight = 75.f;
    CrouchedEyeHeight = 55.f;
    bUseControllerRotationYaw = true;
    GetCharacterMovement()->bOrientRotationToMovement = false;
    GetCharacterMovement()->GetNavAgentPropertiesRef().bCanCrouch = true;
    GetCharacterMovement()->SetCrouchedHalfHeight(55.f);
    GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
    GetCharacterMovement()->MaxWalkSpeedCrouched = CrouchSpeed;
    PlayerCamera = CreateDefaultSubobject<UTDFPlayerCameraComponent>(TEXT("PlayerCamera"));
    PlayerCamera->SetupAttachment(GetCapsuleComponent());
    PlayerCamera->SetRelativeLocation(FVector(0.f, 0.f, BaseEyeHeight));
    PlayerCamera->bUsePawnControlRotation = true;
    PlayerCamera->FieldOfView = 85.f;
    Interaction = CreateDefaultSubobject<UTDFInteractionComponent>(TEXT("Interaction"));
    Inventory = CreateDefaultSubobject<UTDFInventoryComponent>(TEXT("Inventory"));
    Recorder=CreateDefaultSubobject<UTDFRecorderComponent>(TEXT("Recorder"));
    RecorderOperator=CreateDefaultSubobject<UTDFRecorderOperatorComponent>(TEXT("RecorderOperator"));
    RecorderPlayback=CreateDefaultSubobject<UTDFRecorderPlaybackComponent>(TEXT("RecorderPlayback"));
    AcousticLedger=CreateDefaultSubobject<UTDFAcousticPuzzleLedgerComponent>(TEXT("AcousticLedger"));
    NoiseEmitter=CreateDefaultSubobject<UTDFPlayerNoiseEmitterComponent>(TEXT("NoiseEmitter"));
}

void ATDFPlayerCharacter::Landed(const FHitResult& Hit)
{
    const float Strength=FMath::Clamp(FMath::Abs(GetVelocity().Z)/600.f,.15f,1.5f);
    Super::Landed(Hit);
    if(NoiseEmitter) NoiseEmitter->EmitImpact(Hit.ImpactPoint,Strength);
}

void ATDFPlayerCharacter::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);
    RefreshMovementState();
}

void ATDFPlayerCharacter::RefreshMovementState()
{
    CurrentMovementState.Resolve(bIsCrouched, GetCharacterMovement()->IsMovingOnGround(), GetVelocity().Size2D(), MinSprintSpeed);
    GetCharacterMovement()->MaxWalkSpeed = CurrentMovementState.bIsSprinting ? SprintSpeed : WalkSpeed;
    GetCharacterMovement()->MaxWalkSpeedCrouched = CrouchSpeed;
    if (!(CurrentMovementState == PublishedMovementState))
    {
        PublishedMovementState = CurrentMovementState;
        OnMovementStateChanged.Broadcast(CurrentMovementState);
    }
}

void ATDFPlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);
    auto* Input = Cast<UEnhancedInputComponent>(PlayerInputComponent);
    auto* PC = Cast<ATDFPlayerController>(GetController());
    const auto* Config = PC ? PC->InputConfig.Get() : nullptr;
    if (!Input || !Config || !Config->HasRequiredAssets())
    {
        UE_LOG(LogTemp, Error, TEXT("TDF Input: character cannot bind missing Enhanced Input component or configuration."));
        return;
    }
    Input->BindAction(Config->MoveAction, ETriggerEvent::Triggered, this, &ThisClass::HandleMove);
    Input->BindAction(Config->LookAction, ETriggerEvent::Triggered, this, &ThisClass::HandleLook);
    Input->BindAction(Config->JumpAction, ETriggerEvent::Started, this, &ThisClass::StartJump);
    Input->BindAction(Config->JumpAction, ETriggerEvent::Completed, this, &ThisClass::EndJump);
    Input->BindAction(Config->JumpAction, ETriggerEvent::Canceled, this, &ThisClass::EndJump);
    Input->BindAction(Config->CrouchAction, ETriggerEvent::Started, this, &ThisClass::StartCrouch);
    Input->BindAction(Config->CrouchAction, ETriggerEvent::Completed, this, &ThisClass::EndCrouch);
    Input->BindAction(Config->CrouchAction, ETriggerEvent::Canceled, this, &ThisClass::EndCrouch);
    Input->BindAction(Config->SprintAction, ETriggerEvent::Started, this, &ThisClass::StartSprint);
    Input->BindAction(Config->SprintAction, ETriggerEvent::Completed, this, &ThisClass::StopSprint);
    Input->BindAction(Config->SprintAction, ETriggerEvent::Canceled, this, &ThisClass::StopSprint);
    Input->BindAction(Config->LeanAction, ETriggerEvent::Triggered, this, &ThisClass::HandleLean);
    Input->BindAction(Config->LeanAction, ETriggerEvent::Completed, this, &ThisClass::StopLean);
    Input->BindAction(Config->LeanAction, ETriggerEvent::Canceled, this, &ThisClass::StopLean);
    Input->BindAction(Config->HoldBreathAction, ETriggerEvent::Started, this, &ThisClass::StartHoldBreath);
    Input->BindAction(Config->HoldBreathAction, ETriggerEvent::Completed, this, &ThisClass::StopHoldBreath);
    Input->BindAction(Config->HoldBreathAction, ETriggerEvent::Canceled, this, &ThisClass::StopHoldBreath);
    Input->BindAction(Config->InteractAction, ETriggerEvent::Started, this, &ThisClass::HandleInteract);
    Input->BindAction(Config->InspectAction, ETriggerEvent::Started, this, &ThisClass::HandleInspect);
    Input->BindAction(Config->BackAction, ETriggerEvent::Started, this, &ThisClass::HandleBack);
    Input->BindAction(Config->PauseAction, ETriggerEvent::Started, this, &ThisClass::HandleBack);
    if(Config->RecordAction)
    {
        Input->BindAction(Config->RecordAction,ETriggerEvent::Started,RecorderOperator.Get(),&UTDFRecorderOperatorComponent::StartRecording);
        Input->BindAction(Config->RecordAction,ETriggerEvent::Completed,RecorderOperator.Get(),&UTDFRecorderOperatorComponent::CancelRecording);
        Input->BindAction(Config->RecordAction,ETriggerEvent::Canceled,RecorderOperator.Get(),&UTDFRecorderOperatorComponent::CancelRecording);
    }
    if(Config->PlaybackAction) Input->BindAction(Config->PlaybackAction,ETriggerEvent::Started,RecorderOperator.Get(),&UTDFRecorderOperatorComponent::PlaySelected);
    if(Config->DeleteClipAction) Input->BindAction(Config->DeleteClipAction,ETriggerEvent::Started,RecorderOperator.Get(),&UTDFRecorderOperatorComponent::DeleteSelected);
    if(Config->NextClipAction) Input->BindAction(Config->NextClipAction,ETriggerEvent::Started,RecorderOperator.Get(),&UTDFRecorderOperatorComponent::NextSlot);
    if(Config->PreviousClipAction) Input->BindAction(Config->PreviousClipAction,ETriggerEvent::Started,RecorderOperator.Get(),&UTDFRecorderOperatorComponent::PreviousSlot);
    if(Config->ProjectAction) Input->BindAction(Config->ProjectAction,ETriggerEvent::Started,this,&ThisClass::HandleProjectPuck);
}

void ATDFPlayerCharacter::HandleMove(const FInputActionValue& Value)
{
    if (!Controller) return;
    const FVector2D Axis = Value.Get<FVector2D>();
    const FRotator Facing(0.f, Controller->GetControlRotation().Yaw, 0.f);
    AddMovementInput(FRotationMatrix(Facing).GetUnitAxis(EAxis::X), Axis.Y);
    AddMovementInput(FRotationMatrix(Facing).GetUnitAxis(EAxis::Y), Axis.X);
}

void ATDFPlayerCharacter::HandleLook(const FInputActionValue& Value)
{
    const FVector2D Axis = Value.Get<FVector2D>();
    const auto* PC = Cast<APlayerController>(Controller);
    const bool bStick = PC && (FMath::Abs(PC->GetInputAnalogKeyState(EKeys::Gamepad_RightX)) > .18f || FMath::Abs(PC->GetInputAnalogKeyState(EKeys::Gamepad_RightY)) > .18f);
    if (auto* TDFPC=Cast<ATDFPlayerController>(Controller); TDFPC && TDFPC->IsInspectingItem())
    {
        if (TDFPC->InspectionWidget) TDFPC->InspectionWidget->RotatePreview(Axis*(bStick ? 110.f*GetWorld()->GetDeltaSeconds() : 1.2f));
        return;
    }
    const float Scale = (bStick ? GamepadLookRate * GetWorld()->GetDeltaSeconds() : MouseLookScale)
        * (CurrentMovementState.bHoldingBreath ? HoldBreathLookMultiplier : 1.f);
    AddControllerYawInput(Axis.X * Scale);
    const float PitchScale = bStick ? GamepadLookPitchRate * GetWorld()->GetDeltaSeconds()
        * (CurrentMovementState.bHoldingBreath ? HoldBreathLookMultiplier : 1.f) : Scale;
    AddControllerPitchInput(-Axis.Y * PitchScale);
}

bool ATDFPlayerCharacter::IsGameplayInputBlocked() const { const auto* PC=Cast<ATDFPlayerController>(Controller); return PC && (PC->IsInspectingItem() || PC->IsWorldTransitionLocked()); }
void ATDFPlayerCharacter::HandleInspect() { if (auto* PC=Cast<ATDFPlayerController>(Controller)) PC->HandleInspectAction(); }
void ATDFPlayerCharacter::HandleBack() { if (auto* PC=Cast<ATDFPlayerController>(Controller)) PC->HandleBackAction(); }
void ATDFPlayerCharacter::HandleProjectPuck() {if(RecorderOperator) RecorderOperator->PlacePuck();}
void ATDFPlayerCharacter::StartJump() { if (!IsGameplayInputBlocked() && bAllowJump) Jump(); }
void ATDFPlayerCharacter::EndJump() { StopJumping(); }
void ATDFPlayerCharacter::StartCrouch() { if (IsGameplayInputBlocked()) return; StopSprint(); Crouch(); }
void ATDFPlayerCharacter::EndCrouch() { UnCrouch(); }
void ATDFPlayerCharacter::StartSprint() { if (IsGameplayInputBlocked()) return; CurrentMovementState.bWantsSprint = true; RefreshMovementState(); }
void ATDFPlayerCharacter::StopSprint() { CurrentMovementState.bWantsSprint = false; RefreshMovementState(); }
void ATDFPlayerCharacter::HandleLean(const FInputActionValue& Value) { if (IsGameplayInputBlocked()) return; CurrentMovementState.LeanAxis = FMath::Clamp(Value.Get<float>(), -1.f, 1.f); RefreshMovementState(); }
void ATDFPlayerCharacter::StopLean() { CurrentMovementState.LeanAxis = 0.f; RefreshMovementState(); }
void ATDFPlayerCharacter::StartHoldBreath() { if (IsGameplayInputBlocked()) return; CurrentMovementState.bHoldingBreath = true; CurrentMovementState.bWantsSprint = false; RefreshMovementState(); }
void ATDFPlayerCharacter::StopHoldBreath() { CurrentMovementState.bHoldingBreath = false; RefreshMovementState(); }
void ATDFPlayerCharacter::HandleInteract() { if (Interaction) Interaction->TryInteract(); }
void ATDFPlayerCharacter::ResetTransientInput()
{
    if(RecorderOperator) RecorderOperator->CancelRecording();
    CurrentMovementState.bWantsSprint = false;
    CurrentMovementState.bHoldingBreath = false;
    CurrentMovementState.LeanAxis = 0.f;
    StopJumping();
    UnCrouch();
    RefreshMovementState();
}

void ATDFPlayerCharacter::OnStartCrouch(float Adjust, float ScaledAdjust)
{
    Super::OnStartCrouch(Adjust, ScaledAdjust);
    // Preserve the previous world eye position after the capsule's instant resize.
    // The camera component alone interpolates toward the new eye height.
    PlayerCamera->SetRelativeLocation(PlayerCamera->GetRelativeLocation() + FVector(0.f, 0.f, ScaledAdjust));
}
void ATDFPlayerCharacter::OnEndCrouch(float Adjust, float ScaledAdjust)
{
    Super::OnEndCrouch(Adjust, ScaledAdjust);
    PlayerCamera->SetRelativeLocation(PlayerCamera->GetRelativeLocation() - FVector(0.f, 0.f, ScaledAdjust));
}
