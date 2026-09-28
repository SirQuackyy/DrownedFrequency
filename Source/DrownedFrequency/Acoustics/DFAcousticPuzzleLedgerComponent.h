#pragma once
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Engine/DataAsset.h"
#include "Acoustics/DFRecordedSignal.h"
#include "DFAcousticPuzzleLedgerComponent.generated.h"

UCLASS(BlueprintType)
class DROWNEDFREQUENCY_API UTDFAcousticPuzzleSafetyRules : public UDataAsset
{
    GENERATED_BODY()
public:
    UPROPERTY(EditAnywhere, BlueprintReadOnly) FGameplayTagContainer AllowedFallbackKeys;
};
UCLASS(ClassGroup=(Acoustics),meta=(BlueprintSpawnableComponent))
class DROWNEDFREQUENCY_API UTDFAcousticPuzzleLedgerComponent : public UActorComponent
{
    GENERATED_BODY()
public:
    UPROPERTY(EditAnywhere, BlueprintReadOnly) TObjectPtr<UTDFAcousticPuzzleSafetyRules> SafetyRules;
    UFUNCTION(BlueprintCallable) void RegisterRecordedSignal(const FTDFRecordedSignal& Signal);
    UFUNCTION(BlueprintCallable) void RegisterDeletedOrOverwrittenSignal(const FTDFRecordedSignal& Signal);
    UFUNCTION(BlueprintPure) bool HasEverHeldReceiverKey(FGameplayTag Key) const;
    UFUNCTION(BlueprintPure) bool CanPuzzleUseFallback(FGameplayTag Key) const;
    void RestoreHistory(const FGameplayTagContainer& Keys,const TArray<FPrimaryAssetId>& Ids) {EverHeldReceiverKeys=Keys; EverHeldPuzzleEligibleAnomalies=Ids;}
    const FGameplayTagContainer& GetEverHeldKeys() const {return EverHeldReceiverKeys;}
    const TArray<FPrimaryAssetId>& GetEverHeldAnomalies() const {return EverHeldPuzzleEligibleAnomalies;}
private:
    UPROPERTY(SaveGame) FGameplayTagContainer EverHeldReceiverKeys;
    UPROPERTY(SaveGame) TArray<FPrimaryAssetId> EverHeldPuzzleEligibleAnomalies;
};
