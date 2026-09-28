#pragma once
#include "CoreMinimal.h"
#include "Acoustics/DFRecordedSignal.h"
#include "DFAcousticStimulus.generated.h"

USTRUCT(BlueprintType)
struct DROWNEDFREQUENCY_API FTDFAcousticStimulus
{
    GENERATED_BODY()
    UPROPERTY(BlueprintReadOnly) FGuid StimulusGuid;
    UPROPERTY(BlueprintReadOnly) FGuid SignalInstanceGuid;
    UPROPERTY(BlueprintReadOnly) FVector WorldLocation=FVector::ZeroVector;
    UPROPERTY(BlueprintReadOnly) TWeakObjectPtr<AActor> InstigatorActor;
    UPROPERTY(BlueprintReadOnly) FPrimaryAssetId SourceAnomalyId;
    UPROPERTY(BlueprintReadOnly) FGameplayTag CategoryTag;
    UPROPERTY(BlueprintReadOnly) FGameplayTagContainer ReceiverKeys;
    UPROPERTY(BlueprintReadOnly) ETDFAcousticFidelity Fidelity=ETDFAcousticFidelity::Clean;
    UPROPERTY(BlueprintReadOnly) ETDFAcousticRoute Route=ETDFAcousticRoute::Direct;
    UPROPERTY(BlueprintReadOnly) float Loudness=1.f;
    UPROPERTY(BlueprintReadOnly) float Radius=1200.f;
    UPROPERTY(BlueprintReadOnly) float Contamination=0.f;
    UPROPERTY(BlueprintReadOnly) int32 RepetitionIndex=0;
    UPROPERTY(BlueprintReadOnly) float Credibility=1.f;
    UPROPERTY(BlueprintReadOnly) bool bPuzzleEligible=false;
    UPROPERTY(BlueprintReadOnly) TArray<FName> RouteHistory;
    UPROPERTY(BlueprintReadOnly) TArray<FGuid> VisitedNodes;
    bool IsValid() const;
    void ClampSafeRanges();
    static FTDFAcousticStimulus FromSignal(const FTDFRecordedSignal& Signal,const FTDFAcousticClipVariant& Variant,FVector Location,AActor* Instigator,ETDFAcousticRoute Route);
};
