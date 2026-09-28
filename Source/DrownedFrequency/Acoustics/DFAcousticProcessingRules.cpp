#include "Acoustics/DFAcousticProcessingRules.h"
ETDFAcousticFidelity UTDFAcousticProcessingRules::ComputeEffectiveFidelity(const FTDFRecordedSignal& S) const
{
    int32 Level=FMath::Clamp(int32(S.Fidelity),0,2);
    if(S.PlayCount>=FMath::Max(1,PlaysBeforeDegrade)) Level=FMath::Max(Level,1);
    if(S.Contamination>0.f || S.PlayCount>=FMath::Max(PlaysBeforeDegrade+1,PlaysBeforeContaminate)) Level=2;
    return ETDFAcousticFidelity(Level);
}
float UTDFAcousticProcessingRules::ComputeEffectiveRadiusMultiplier(const FTDFRecordedSignal& S) const
{
    const float Penalty=FMath::IsFinite(RepetitionRadiusPenalty)?FMath::Clamp(RepetitionRadiusPenalty,0.f,1.f):.15f;
    return FMath::Clamp(1.f-FMath::Max(0,S.PlayCount)*Penalty,FMath::Clamp(MinimumRadiusMultiplier,.05f,1.f),1.f);
}
float UTDFAcousticProcessingRules::ComputeCredibility(const FTDFRecordedSignal& S,int32 MeaningfulPlays,float Penalty) const
{
    return FMath::Clamp(1.f-FMath::Max(0,S.PlayCount-FMath::Max(1,MeaningfulPlays)+1)*FMath::Clamp(Penalty,0.f,1.f),0.f,1.f);
}
