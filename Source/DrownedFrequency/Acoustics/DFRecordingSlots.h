#pragma once
#include "CoreMinimal.h"
#include "Acoustics/DFRecordedSignal.h"
#include "DFRecordingSlots.generated.h"
USTRUCT(BlueprintType)
struct DROWNEDFREQUENCY_API FTDFRecorderSlot
{
    GENERATED_BODY()
    UPROPERTY(BlueprintReadOnly, SaveGame) int32 SlotIndex=INDEX_NONE;
    UPROPERTY(BlueprintReadOnly, SaveGame) bool bUnlocked=false;
    UPROPERTY(BlueprintReadOnly, SaveGame) FTDFRecordedSignal Signal;
};
USTRUCT(BlueprintType)
struct DROWNEDFREQUENCY_API FTDFRecorderSlotSet
{
    GENERATED_BODY()
    UPROPERTY(BlueprintReadOnly, SaveGame) TArray<FTDFRecorderSlot> Slots;
    void InitializeToTier(ETDFRecordingTier Tier);
    bool SetUnlockedSlotCount(int32 Count);
    bool PutSignal(int32 Index,const FTDFRecordedSignal& Signal,FTDFRecordedSignal* OutReplaced=nullptr);
    bool DeleteSignal(int32 Index,FTDFRecordedSignal* OutDeleted=nullptr);
    int32 GetUnlockedSlotCount() const;
    const FTDFRecordedSignal* GetSignal(int32 Index) const;
};
