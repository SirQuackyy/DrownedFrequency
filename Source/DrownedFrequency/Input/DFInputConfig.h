#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "DFInputConfig.generated.h"

class UInputAction;
class UInputMappingContext;

UCLASS(BlueprintType, Blueprintable)
class DROWNEDFREQUENCY_API UTDFInputConfig : public UDataAsset
{
    GENERATED_BODY()

public:
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Acoustics") TObjectPtr<UInputAction> RecordAction;
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Acoustics") TObjectPtr<UInputAction> PlaybackAction;
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Acoustics") TObjectPtr<UInputAction> ProjectAction;
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Acoustics") TObjectPtr<UInputAction> NextClipAction;
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Acoustics") TObjectPtr<UInputAction> PreviousClipAction;
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Acoustics") TObjectPtr<UInputAction> DeleteClipAction;
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Input")
    TObjectPtr<UInputAction> MoveAction;
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Input")
    TObjectPtr<UInputAction> LookAction;
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Input")
    TObjectPtr<UInputAction> JumpAction;
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Input")
    TObjectPtr<UInputAction> CrouchAction;
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Input")
    TObjectPtr<UInputAction> SprintAction;
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Input")
    TObjectPtr<UInputAction> LeanAction;
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Input")
    TObjectPtr<UInputAction> HoldBreathAction;
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Input")
    TObjectPtr<UInputAction> InteractAction;
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Input")
    TObjectPtr<UInputAction> InspectAction;
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Input")
    TObjectPtr<UInputAction> BackAction;
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Input")
    TObjectPtr<UInputAction> PauseAction;
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Input")
    TObjectPtr<UInputMappingContext> KeyboardMouseContext;
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Input")
    TObjectPtr<UInputMappingContext> GamepadContext;
    UFUNCTION(BlueprintPure, Category="Input")
    bool HasRequiredAssets() const;
    /** Digital action rows only; axis-direction rebinding needs separate rows in the later options UI. */
    UFUNCTION(BlueprintPure, Category="Input") UInputAction* FindRemappableAction(FName Name) const;
};
