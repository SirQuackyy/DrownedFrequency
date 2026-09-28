#include "Acoustics/DFRecordedSignal.h"
bool FTDFRecordedSignal::IsValid() const
{
    return InstanceGuid.IsValid() && SourceAnomalyId.IsValid() && TDFAcoustics::IsCategory(CategoryTag) && uint8(Fidelity)<=2
        && FMath::IsFinite(Contamination) && Contamination>=0 && Contamination<=1 && Generation>=0 && PlayCount>=0
        && (!bPuzzleEligible || !ReceiverKeysSnapshot.IsEmpty());
}
bool FTDFRecordedSignal::CanSatisfyReceiver() const { return IsValid() && bPuzzleEligible && !ReceiverKeysSnapshot.IsEmpty(); }
FString FTDFRecordedSignal::ToDebugString() const
{
    return FString::Printf(TEXT("%s [%s] fidelity=%d contamination=%.2f plays=%d"),*SourceAnomalyId.ToString(),*CategoryTag.ToString(),int32(Fidelity),Contamination,PlayCount);
}
