#pragma once
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Acoustics/DFRecordingSlots.h"
#include "DFRecorderComponent.generated.h"
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FTDFRecorderChanged);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FTDFSignalRecorded,int32,SlotIndex,const FTDFRecordedSignal&,Signal);
UCLASS(ClassGroup=(Acoustics),meta=(BlueprintSpawnableComponent))
class DROWNEDFREQUENCY_API UTDFRecorderComponent : public UActorComponent
{
    GENERATED_BODY()
public:
    UTDFRecorderComponent();
    UPROPERTY(EditAnywhere, BlueprintReadOnly) ETDFRecordingTier InitialTier=ETDFRecordingTier::Tier3;
    UPROPERTY(BlueprintAssignable) FTDFRecorderChanged OnSlotsChanged;
    UPROPERTY(BlueprintAssignable) FTDFSignalRecorded OnSignalRecorded;
    FSimpleMulticastDelegate OnSlotsChangedNative;
    UFUNCTION(BlueprintCallable) bool TryRecordAnomaly(class ATDFAudioAnomalyActor* Anomaly,int32 SlotIndex);
    UFUNCTION(BlueprintCallable) bool DeleteSlot(int32 SlotIndex);
    UFUNCTION(BlueprintCallable) bool UnlockTier(ETDFRecordingTier Tier);
    UFUNCTION(BlueprintPure) bool HasValidSignal(int32 SlotIndex) const { return Slots.GetSignal(SlotIndex)!=nullptr; }
    UFUNCTION(BlueprintPure) FTDFRecorderSlotSet GetSlotSnapshot() const {return Slots;}
    const FTDFRecorderSlotSet& GetSlots() const {return Slots;}
    bool RestoreSavedSlots(ETDFRecordingTier Tier,const TArray<FTDFRecordedSignal>& Signals,int32 Selected);
    bool IncrementPlayCount(int32 Index);
    UFUNCTION(BlueprintCallable) bool SelectSlot(int32 Index);
    UFUNCTION(BlueprintCallable) void SelectNextSlot();
    UPROPERTY(BlueprintReadOnly) int32 SelectedSlot=0;
protected:
    virtual void BeginPlay() override;
private:
    void NotifyChanged();
    UPROPERTY(SaveGame) FTDFRecorderSlotSet Slots;
};
