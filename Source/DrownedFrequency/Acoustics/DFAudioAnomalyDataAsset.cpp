#include "Acoustics/DFAudioAnomalyDataAsset.h"
#include "Misc/DataValidation.h"

bool UTDFAudioAnomalyDataAsset::HasValidDefinition() const
{
    if (!StableId.IsValid() || StableId.PrimaryAssetType!=FPrimaryAssetType(TEXT("TDFAnomaly")) || !TDFAcoustics::IsCategory(CategoryTag) || !Variants.Contains(ETDFAcousticFidelity::Clean)) return false;
    if (bCanSatisfyReceiver && ReceiverKeys.IsEmpty()) return false;
    if (MaxMeaningfulRepetitions<1 || !FMath::IsFinite(RepetitionPenaltyPerExtraPlay) || RepetitionPenaltyPerExtraPlay<0 || RepetitionPenaltyPerExtraPlay>1) return false;
    for (const auto& Pair:Variants) if (Pair.Key!=Pair.Value.Fidelity || !Pair.Value.IsValid()) return false;
    return true;
}
const FTDFAcousticClipVariant* UTDFAudioAnomalyDataAsset::FindVariant(ETDFAcousticFidelity Fidelity) const
{
    const auto* V=Variants.Find(Fidelity);
    return HasValidDefinition() && V && V->IsValid() ? V : nullptr;
}
#if WITH_EDITOR
EDataValidationResult UTDFAudioAnomalyDataAsset::IsDataValid(FDataValidationContext& Context) const
{
    if (HasValidDefinition()) return EDataValidationResult::Valid;
    Context.AddError(FText::FromString(TEXT("Set a unique TDFAnomaly primary ID, one of seven categories, a Clean sound variant, matching variant fidelity, valid caption times/ranges, and receiver keys if puzzle eligible.")));
    return EDataValidationResult::Invalid;
}
#endif
