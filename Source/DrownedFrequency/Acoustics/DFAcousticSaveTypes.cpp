#include "Acoustics/DFAcousticSaveTypes.h"
#include "Acoustics/DFRecorderComponent.h"
#include "Acoustics/DFAcousticPuzzleLedgerComponent.h"
bool FTDFAcousticSaveData::IsValid() const
{
    const int32 Count=int32(UnlockTier); if(Version!=1 || Count<3 || Count>5 || SlotSignals.Num()!=5 || SelectedSlot<0 || SelectedSlot>=Count) return false;
    for(int32 I=0;I<SlotSignals.Num();++I) if(SlotSignals[I].InstanceGuid.IsValid() && (I>=Count || !SlotSignals[I].IsValid())) return false;
    return true;
}
FTDFAcousticSaveData FTDFAcousticSaveData::Capture(const UTDFRecorderComponent* R,const UTDFAcousticPuzzleLedgerComponent* L)
{
    FTDFAcousticSaveData Out; Out.SlotSignals.SetNum(5);
    if(R) {Out.UnlockTier=ETDFRecordingTier(R->GetSlots().GetUnlockedSlotCount()); Out.SelectedSlot=R->SelectedSlot; for(const auto& S:R->GetSlots().Slots) Out.SlotSignals[S.SlotIndex]=S.Signal;}
    if(L) {Out.EverHeldReceiverKeys=L->GetEverHeldKeys(); Out.EverHeldPuzzleEligibleAnomalies=L->GetEverHeldAnomalies();} return Out;
}
bool FTDFAcousticSaveData::Restore(UTDFRecorderComponent* R,UTDFAcousticPuzzleLedgerComponent* L) const
{
    if(!R || !L || !IsValid()) return false;
    // Restore history before slot delegates notify presentation or puzzle listeners.
    L->RestoreHistory(EverHeldReceiverKeys,EverHeldPuzzleEligibleAnomalies);
    for(const auto& S:SlotSignals) L->RegisterRecordedSignal(S);
    return R->RestoreSavedSlots(UnlockTier,SlotSignals,SelectedSlot);
}
