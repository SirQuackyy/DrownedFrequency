#include "Acoustics/DFRecorderComponent.h"
#include "Acoustics/DFAudioAnomalyActor.h"
#include "Acoustics/DFAcousticPuzzleLedgerComponent.h"
#include "GameFramework/Actor.h"
UTDFRecorderComponent::UTDFRecorderComponent() {Slots.InitializeToTier(ETDFRecordingTier::Tier3);}
void UTDFRecorderComponent::BeginPlay() {Super::BeginPlay(); Slots.InitializeToTier(InitialTier==ETDFRecordingTier::Invalid?ETDFRecordingTier::Tier3:InitialTier); NotifyChanged();}
void UTDFRecorderComponent::NotifyChanged() {OnSlotsChanged.Broadcast(); OnSlotsChangedNative.Broadcast();}
bool UTDFRecorderComponent::TryRecordAnomaly(ATDFAudioAnomalyActor* A,int32 I)
{
    if (!IsValid(A) || !A->CanBeRecordedBy(GetOwner())) return false;
    const auto S=A->MakeRecordedSignal();
    auto* Ledger=GetOwner()->FindComponentByClass<UTDFAcousticPuzzleLedgerComponent>();
    const auto* Existing=Slots.GetSignal(I);
    if (!Ledger && (S.CanSatisfyReceiver() || (Existing && Existing->CanSatisfyReceiver()))) return false;
    FTDFRecordedSignal Old; if(!Slots.PutSignal(I,S,&Old)) return false;
    if (Ledger) {Ledger->RegisterDeletedOrOverwrittenSignal(Old); Ledger->RegisterRecordedSignal(S);}
    OnSignalRecorded.Broadcast(I,S); NotifyChanged(); A->CompleteAssignedObjective(); return true;
}
bool UTDFRecorderComponent::DeleteSlot(int32 I)
{
    const auto* S=Slots.GetSignal(I); if(!S) return false;
    auto* Ledger=GetOwner()?GetOwner()->FindComponentByClass<UTDFAcousticPuzzleLedgerComponent>():nullptr;
    if (S->CanSatisfyReceiver() && !Ledger) return false;
    FTDFRecordedSignal Old; if(!Slots.DeleteSignal(I,&Old)) return false;
    if(Ledger) Ledger->RegisterDeletedOrOverwrittenSignal(Old);
    NotifyChanged(); return true;
}
bool UTDFRecorderComponent::UnlockTier(ETDFRecordingTier Tier)
{
    const int32 Count=int32(Tier), Current=Slots.GetUnlockedSlotCount();
    if (Count!=Current+1 || !Slots.SetUnlockedSlotCount(Count)) return false;
    NotifyChanged(); return true;
}
bool UTDFRecorderComponent::IncrementPlayCount(int32 I)
{
    const auto* S=Slots.GetSignal(I); if(!S || S->PlayCount==MAX_int32) return false;
    auto Copy=*S; ++Copy.PlayCount; Slots.PutSignal(I,Copy); NotifyChanged(); return true;
}
bool UTDFRecorderComponent::SelectSlot(int32 I)
{
    if(I<0 || I>=Slots.GetUnlockedSlotCount()) return false;
    if(SelectedSlot!=I) {SelectedSlot=I; NotifyChanged();} return true;
}
void UTDFRecorderComponent::SelectNextSlot() {SelectSlot((SelectedSlot+1)%FMath::Max(1,Slots.GetUnlockedSlotCount()));}
bool UTDFRecorderComponent::RestoreSavedSlots(ETDFRecordingTier Tier,const TArray<FTDFRecordedSignal>& Signals,int32 Selected)
{
    if(int32(Tier)<3 || int32(Tier)>5 || Signals.Num()!=5 || Selected<0 || Selected>=int32(Tier)) return false;
    FTDFRecorderSlotSet Restored; Restored.InitializeToTier(Tier);
    for(int32 I=0;I<Signals.Num();++I) if(Signals[I].InstanceGuid.IsValid() && !Restored.PutSignal(I,Signals[I])) return false;
    Slots=MoveTemp(Restored); SelectedSlot=Selected; NotifyChanged(); return true;
}
