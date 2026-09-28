#pragma once
#include "CoreMinimal.h"
#include "Acoustics/DFRecordedSignal.h"
#include "DFAcousticSaveTypes.generated.h"
USTRUCT(BlueprintType)
struct DROWNEDFREQUENCY_API FTDFAcousticSaveData
{
    GENERATED_BODY()
    UPROPERTY(SaveGame) int32 Version=1;
    UPROPERTY(SaveGame) ETDFRecordingTier UnlockTier=ETDFRecordingTier::Tier3;
    UPROPERTY(SaveGame) TArray<FTDFRecordedSignal> SlotSignals;
    UPROPERTY(SaveGame) int32 SelectedSlot=0;
    UPROPERTY(SaveGame) FGameplayTagContainer EverHeldReceiverKeys;
    UPROPERTY(SaveGame) TArray<FPrimaryAssetId> EverHeldPuzzleEligibleAnomalies;
    bool IsValid() const;
    static FTDFAcousticSaveData Capture(const class UTDFRecorderComponent* Recorder,const class UTDFAcousticPuzzleLedgerComponent* Ledger);
    bool Restore(class UTDFRecorderComponent* Recorder,class UTDFAcousticPuzzleLedgerComponent* Ledger) const;
};
