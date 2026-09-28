#pragma once
#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Acoustics/DFAcousticTypes.h"
#include "DFAudioAnomalyDataAsset.generated.h"

UCLASS(BlueprintType)
class DROWNEDFREQUENCY_API UTDFAudioAnomalyDataAsset : public UPrimaryDataAsset
{
    GENERATED_BODY()
public:
    UPROPERTY(EditAnywhere, BlueprintReadOnly) FPrimaryAssetId StableId;
    UPROPERTY(EditAnywhere, BlueprintReadOnly) FText DisplayName;
    UPROPERTY(EditAnywhere, BlueprintReadOnly) FGameplayTag CategoryTag;
    UPROPERTY(EditAnywhere, BlueprintReadOnly) TMap<ETDFAcousticFidelity,FTDFAcousticClipVariant> Variants;
    UPROPERTY(EditAnywhere, BlueprintReadOnly) FGameplayTagContainer ReceiverKeys;
    UPROPERTY(EditAnywhere, BlueprintReadOnly) bool bCanSatisfyReceiver=false;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(ClampMin="1")) int32 MaxMeaningfulRepetitions=1;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(ClampMin="0",ClampMax="1")) float RepetitionPenaltyPerExtraPlay=.25f;
    UPROPERTY(EditAnywhere, BlueprintReadOnly) bool bCanBeContaminated=true;
    UPROPERTY(EditAnywhere, BlueprintReadOnly) bool bCanBeOverdubbed=true;
    UPROPERTY(EditAnywhere, BlueprintReadOnly) FLinearColor DebugColor=FLinearColor(0.f,1.f,1.f,1.f);
    virtual FPrimaryAssetId GetPrimaryAssetId() const override { return StableId; }
    UFUNCTION(BlueprintPure) bool HasValidDefinition() const;
    const FTDFAcousticClipVariant* FindVariant(ETDFAcousticFidelity Fidelity) const;
#if WITH_EDITOR
    virtual EDataValidationResult IsDataValid(FDataValidationContext& Context) const override;
#endif
};
