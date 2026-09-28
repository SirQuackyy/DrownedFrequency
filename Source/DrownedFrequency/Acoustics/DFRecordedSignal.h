#pragma once
#include "CoreMinimal.h"
#include "Acoustics/DFAcousticTypes.h"
#include "DFRecordedSignal.generated.h"

USTRUCT(BlueprintType)
struct DROWNEDFREQUENCY_API FTDFRecordedSignal
{
    GENERATED_BODY()
    UPROPERTY(BlueprintReadOnly, SaveGame) FGuid InstanceGuid;
    UPROPERTY(BlueprintReadOnly, SaveGame) FPrimaryAssetId SourceAnomalyId;
    UPROPERTY(BlueprintReadOnly, SaveGame) FGameplayTag CategoryTag;
    UPROPERTY(BlueprintReadOnly, SaveGame) ETDFAcousticFidelity Fidelity=ETDFAcousticFidelity::Clean;
    UPROPERTY(BlueprintReadOnly, SaveGame) float Contamination=0.f;
    UPROPERTY(BlueprintReadOnly, SaveGame) int32 Generation=0;
    UPROPERTY(BlueprintReadOnly, SaveGame) int32 PlayCount=0;
    UPROPERTY(BlueprintReadOnly, SaveGame) TArray<FPrimaryAssetId> OverdubbedSourceIds;
    UPROPERTY(BlueprintReadOnly, SaveGame) FGameplayTagContainer ReceiverKeysSnapshot;
    UPROPERTY(BlueprintReadOnly, SaveGame) bool bPuzzleEligible=false;
    bool IsValid() const;
    bool CanSatisfyReceiver() const;
    FString ToDebugString() const;
};
