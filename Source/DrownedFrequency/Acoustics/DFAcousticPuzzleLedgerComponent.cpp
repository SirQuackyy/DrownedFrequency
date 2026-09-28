#include "Acoustics/DFAcousticPuzzleLedgerComponent.h"
void UTDFAcousticPuzzleLedgerComponent::RegisterRecordedSignal(const FTDFRecordedSignal& S)
{
    if (!S.CanSatisfyReceiver()) return;
    EverHeldReceiverKeys.AppendTags(S.ReceiverKeysSnapshot); EverHeldPuzzleEligibleAnomalies.AddUnique(S.SourceAnomalyId);
}
void UTDFAcousticPuzzleLedgerComponent::RegisterDeletedOrOverwrittenSignal(const FTDFRecordedSignal& S) {RegisterRecordedSignal(S);}
bool UTDFAcousticPuzzleLedgerComponent::HasEverHeldReceiverKey(FGameplayTag Key) const {return Key.IsValid() && EverHeldReceiverKeys.HasTagExact(Key);}
bool UTDFAcousticPuzzleLedgerComponent::CanPuzzleUseFallback(FGameplayTag Key) const
{
    return SafetyRules && SafetyRules->AllowedFallbackKeys.HasTagExact(Key) && HasEverHeldReceiverKey(Key);
}
