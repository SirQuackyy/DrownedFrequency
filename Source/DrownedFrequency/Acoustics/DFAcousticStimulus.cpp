#include "Acoustics/DFAcousticStimulus.h"
bool FTDFAcousticStimulus::IsValid() const
{
    return StimulusGuid.IsValid() && !WorldLocation.ContainsNaN() && TDFAcoustics::IsCategory(CategoryTag) && uint8(Fidelity)<=2 && uint8(Route)<=4
        && FMath::IsFinite(Radius) && Radius>0 && Radius<=100000 && FMath::IsFinite(Loudness) && Loudness>=0 && Loudness<=4
        && FMath::IsFinite(Contamination) && Contamination>=0 && Contamination<=1 && RepetitionIndex>=0 && FMath::IsFinite(Credibility) && Credibility>=0 && Credibility<=1
        && (!bPuzzleEligible || (SourceAnomalyId.IsValid() && !ReceiverKeys.IsEmpty()));
}
void FTDFAcousticStimulus::ClampSafeRanges()
{
    Radius=FMath::IsFinite(Radius)?FMath::Clamp(Radius,1.f,100000.f):1.f;
    Loudness=FMath::IsFinite(Loudness)?FMath::Clamp(Loudness,0.f,4.f):0.f;
    Contamination=FMath::IsFinite(Contamination)?FMath::Clamp(Contamination,0.f,1.f):1.f;
    RepetitionIndex=FMath::Max(0,RepetitionIndex);
}
FTDFAcousticStimulus FTDFAcousticStimulus::FromSignal(const FTDFRecordedSignal& S,const FTDFAcousticClipVariant& V,FVector L,AActor* A,ETDFAcousticRoute R)
{
    FTDFAcousticStimulus Out;
    Out.StimulusGuid=FGuid::NewGuid(); Out.SignalInstanceGuid=S.InstanceGuid; Out.SourceAnomalyId=S.SourceAnomalyId; Out.CategoryTag=S.CategoryTag;
    Out.ReceiverKeys=S.ReceiverKeysSnapshot; Out.Fidelity=S.Fidelity; Out.Contamination=S.Contamination; Out.RepetitionIndex=S.PlayCount;
    Out.bPuzzleEligible=S.CanSatisfyReceiver(); Out.WorldLocation=L; Out.InstigatorActor=A; Out.Route=R; Out.Loudness=V.Loudness; Out.Radius=V.EffectiveRadius;
    Out.RouteHistory.Add(FName(*StaticEnum<ETDFAcousticRoute>()->GetNameStringByValue(int64(R))));
    return Out;
}
