#pragma once
#include "CoreMinimal.h"
#include "DFMovementStateTypes.generated.h"

UENUM(BlueprintType)
enum class ETDFMovementModeState : uint8 { Walking, Crouching, Sprinting };

USTRUCT(BlueprintType)
struct FTDFPlayerMovementState
{
    GENERATED_BODY()
    UPROPERTY(BlueprintReadOnly) bool bWantsSprint = false;
    UPROPERTY(BlueprintReadOnly) bool bIsSprinting = false;
    UPROPERTY(BlueprintReadOnly) bool bIsCrouching = false;
    UPROPERTY(BlueprintReadOnly) float LeanAxis = 0.f;
    UPROPERTY(BlueprintReadOnly) bool bHoldingBreath = false;
    UPROPERTY(BlueprintReadOnly) ETDFMovementModeState Mode = ETDFMovementModeState::Walking;

    void Resolve(bool bCrouched, bool bGrounded, float Speed, float MinimumSprintSpeed)
    {
        bIsCrouching = bCrouched;
        bIsSprinting = bWantsSprint && !bIsCrouching && !bHoldingBreath && bGrounded && Speed > MinimumSprintSpeed;
        Mode = bIsCrouching ? ETDFMovementModeState::Crouching : bIsSprinting ? ETDFMovementModeState::Sprinting : ETDFMovementModeState::Walking;
    }
    bool operator==(const FTDFPlayerMovementState& Other) const
    {
        return bWantsSprint == Other.bWantsSprint && bIsSprinting == Other.bIsSprinting && bIsCrouching == Other.bIsCrouching
            && FMath::IsNearlyEqual(LeanAxis, Other.LeanAxis) && bHoldingBreath == Other.bHoldingBreath && Mode == Other.Mode;
    }
};
