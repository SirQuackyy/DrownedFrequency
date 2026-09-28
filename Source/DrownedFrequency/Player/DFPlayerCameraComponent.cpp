#include "Player/DFPlayerCameraComponent.h"
#include "Player/DFPlayerCharacter.h"
#include "Engine/World.h"
#include "Camera/CameraTypes.h"

UTDFPlayerCameraComponent::UTDFPlayerCameraComponent()
{
    PrimaryComponentTick.bCanEverTick = true;
    PrimaryComponentTick.TickGroup = TG_PostPhysics;
}

void UTDFPlayerCameraComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
    const auto* Player = Cast<ATDFPlayerCharacter>(GetOwner());
    if (!Player) return;
    CurrentLean = FMath::FInterpTo(CurrentLean, Player->CurrentMovementState.LeanAxis, DeltaTime, LeanInterpSpeed);
    FVector Local = GetRelativeLocation();
    Local.Z = FMath::FInterpTo(Local.Z, Player->CurrentMovementState.bIsCrouching ? CrouchEyeZ : StandingEyeZ, DeltaTime, CrouchCameraInterpSpeed);
    Local.Y = 0.f;
    const FVector Start = Player->GetActorTransform().TransformPosition(Local);
    const FVector Offset = Player->GetActorRightVector() * CurrentLean * MaxLeanSideOffset;
    FHitResult Hit;
    FCollisionQueryParams Query(SCENE_QUERY_STAT(TDFLean), false, Player);
    float Fraction = 1.f;
    if (GetWorld()->SweepSingleByChannel(Hit, Start, Start + Offset, FQuat::Identity, ECC_Camera, FCollisionShape::MakeSphere(8.f), Query))
    {
        Fraction = Hit.bStartPenetrating ? 0.f : FMath::Max(0.f, Hit.Time - .05f);
    }
    Local.Y = CurrentLean * MaxLeanSideOffset * Fraction;
    CurrentLeanRoll = CurrentLean * MaxLeanRollDegrees * Fraction;
    SetRelativeLocation(Local);
    const float MovementFraction = FMath::Clamp(Player->GetVelocity().Size2D() / Player->WalkSpeed, 0.f, 1.f);
    const float TargetSway = (Player->CurrentMovementState.bIsSprinting ? SprintSwayAmplitude : WalkSwayAmplitude) * MovementFraction;
    SwayAmplitude = FMath::FInterpTo(SwayAmplitude, TargetSway, DeltaTime, 8.f);
    SwayPhase += DeltaTime * (Player->CurrentMovementState.bIsSprinting ? 9.f : 6.f);
}

void UTDFPlayerCameraComponent::GetCameraView(float DeltaTime, FMinimalViewInfo& DesiredView)
{
    Super::GetCameraView(DeltaTime, DesiredView);
    if (!bReducedMotion) DesiredView.Rotation.Roll += CurrentLeanRoll;
    const auto* Player = Cast<ATDFPlayerCharacter>(GetOwner());
    if (Player && !bReducedMotion)
    {
        const float Multiplier = Player->CurrentMovementState.bHoldingBreath ? HoldBreathSwayMultiplier : 1.f;
        DesiredView.Rotation.Pitch += FMath::Sin(GetWorld()->GetTimeSeconds() * 1.8f) * BreathingSwayDegrees * Multiplier;
        DesiredView.Location += FVector::UpVector * FMath::Sin(SwayPhase * 2.f) * SwayAmplitude * Multiplier;
        DesiredView.Location += Player->GetActorRightVector() * FMath::Cos(SwayPhase) * SwayAmplitude * .5f * Multiplier;
    }
}
