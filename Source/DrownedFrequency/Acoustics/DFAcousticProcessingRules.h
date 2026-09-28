#pragma once
#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Acoustics/DFRecordedSignal.h"
#include "DFAcousticProcessingRules.generated.h"
UCLASS(BlueprintType)
class DROWNEDFREQUENCY_API UTDFAcousticProcessingRules : public UDataAsset
{
    GENERATED_BODY()
public:
    UPROPERTY(EditAnywhere, BlueprintReadOnly) int32 PlaysBeforeDegrade=2;
    UPROPERTY(EditAnywhere, BlueprintReadOnly) int32 PlaysBeforeContaminate=4;
    UPROPERTY(EditAnywhere, BlueprintReadOnly) float RepetitionRadiusPenalty=.15f;
    UPROPERTY(EditAnywhere, BlueprintReadOnly) float MinimumRadiusMultiplier=.25f;
    UFUNCTION(BlueprintPure) ETDFAcousticFidelity ComputeEffectiveFidelity(const FTDFRecordedSignal& Signal) const;
    UFUNCTION(BlueprintPure) float ComputeEffectiveRadiusMultiplier(const FTDFRecordedSignal& Signal) const;
    UFUNCTION(BlueprintPure) float ComputeCredibility(const FTDFRecordedSignal& Signal,int32 MeaningfulPlays,float Penalty) const;
};
