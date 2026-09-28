#include "Acoustics/DFRecordingSlots.h"
void FTDFRecorderSlotSet::InitializeToTier(ETDFRecordingTier Tier)
{
    Slots.SetNum(5);
    for(int32 I=0;I<5;++I) {Slots[I].SlotIndex=I; Slots[I].bUnlocked=false; Slots[I].Signal={};}
    SetUnlockedSlotCount(int32(Tier));
}
bool FTDFRecorderSlotSet::SetUnlockedSlotCount(int32 Count)
{
    if (Count<3 || Count>5 || Slots.Num()!=5 || Count<GetUnlockedSlotCount()) return false;
    for(int32 I=0;I<5;++I) Slots[I].bUnlocked=I<Count;
    return true;
}
const FTDFRecordedSignal* FTDFRecorderSlotSet::GetSignal(int32 I) const
{
    return Slots.IsValidIndex(I) && Slots[I].bUnlocked && Slots[I].Signal.IsValid() ? &Slots[I].Signal : nullptr;
}
bool FTDFRecorderSlotSet::PutSignal(int32 I,const FTDFRecordedSignal& S,FTDFRecordedSignal* Old)
{
    if (!Slots.IsValidIndex(I) || !Slots[I].bUnlocked || !S.IsValid()) return false;
    if(Old) *Old=Slots[I].Signal;
    Slots[I].Signal=S; return true;
}
bool FTDFRecorderSlotSet::DeleteSignal(int32 I,FTDFRecordedSignal* Old)
{
    if (!GetSignal(I)) return false;
    if(Old) *Old=Slots[I].Signal;
    Slots[I].Signal={}; return true;
}
int32 FTDFRecorderSlotSet::GetUnlockedSlotCount() const
{
    int32 Count=0; for(const auto& S:Slots) if(S.bUnlocked) ++Count; return Count;
}
