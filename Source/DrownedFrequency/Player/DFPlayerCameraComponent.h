#pragma once

#include "CoreMinimal.h"
#include "Camera/CameraComponent.h"
#include "DFPlayerCameraComponent.generated.h"

UCLASS(BlueprintType, Blueprintable)
class DROWNEDFREQUENCY_API UTDFPlayerCameraComponent : public UCameraComponent
{
    GENERATED_BODY()
public:
    UTDFPlayerCameraComponent();
    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
    virtual void GetCameraView(float DeltaTime, FMinimalViewInfo& DesiredView) override;
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Lean") float MaxLeanRollDegrees = 7.f;
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Lean") float MaxLeanSideOffset = 12.f;
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Lean") float LeanInterpSpeed = 12.f;
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Sway") float BreathingSwayDegrees = .08f;
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Sway") float HoldBreathSwayMultiplier = .15f;
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Height") float StandingEyeZ = 75.f;
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Height") float CrouchEyeZ = 55.f;
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Height") float CrouchCameraInterpSpeed = 12.f;
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Sway") float WalkSwayAmplitude = .22f;
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Sway") float SprintSwayAmplitude = .45f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Accessibility") bool bReducedMotion = false;
private:
    float CurrentLean = 0.f;
    float CurrentLeanRoll = 0.f;
    float SwayAmplitude = 0.f;
    float SwayPhase = 0.f;
};
