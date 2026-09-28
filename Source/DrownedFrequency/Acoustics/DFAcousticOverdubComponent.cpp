#include "Acoustics/DFAcousticOverdubComponent.h"
#include "Acoustics/DFRecorderPlaybackComponent.h"
#include "Acoustics/DFAudioAnomalyDataAsset.h"
FTDFRecordedSignal UTDFAcousticOverdubComponent::ApplyContamination(const FTDFRecordedSignal& S,float Amount) const
{
    auto Out=S; auto* Asset=UTDFRecorderPlaybackComponent::Resolve(S.SourceAnomalyId);
    if(!S.IsValid() || !Asset || !Asset->bCanBeContaminated || !FMath::IsFinite(Amount)) return Out;
    Out.Contamination=FMath::Clamp(S.Contamination+Amount,0.f,1.f);
    if(Out.Contamination>0 && Asset->FindVariant(ETDFAcousticFidelity::Contaminated)) Out.Fidelity=ETDFAcousticFidelity::Contaminated;
    return Out;
}
FTDFOverdubResult UTDFAcousticOverdubComponent::TryOverdub(const FTDFRecordedSignal& Base,const FTDFRecordedSignal& Layer)
{
    FTDFOverdubResult Out;
    const auto Fail=[&](const TCHAR* Why){Out.FailureReason=FText::FromString(Why); return Out;};
    // The vertical slice never exposes this ability, even if tuning is accidentally lowered.
    if(CurrentAct<FMath::Max(5,Rules?Rules->UnlockAct:5)) return Fail(TEXT("Overdub is locked until Act V."));
    auto* A=UTDFRecorderPlaybackComponent::Resolve(Base.SourceAnomalyId); auto* B=UTDFRecorderPlaybackComponent::Resolve(Layer.SourceAnomalyId);
    if(!Base.IsValid() || !Layer.IsValid() || !A || !B || !A->bCanBeOverdubbed || !B->bCanBeOverdubbed) return Fail(TEXT("Both sources must permit overdubbing."));
    if(!A->FindVariant(ETDFAcousticFidelity::Contaminated)) return Fail(TEXT("Base source needs an authored composite/contaminated take."));
    auto& S=Out.ResultSignal; S=Base; S.InstanceGuid=FGuid::NewGuid(); S.Generation=FMath::Min(MAX_int32-1,FMath::Max(Base.Generation,Layer.Generation))+1; S.PlayCount=0;
    S.OverdubbedSourceIds.AddUnique(Base.SourceAnomalyId); S.OverdubbedSourceIds.AddUnique(Layer.SourceAnomalyId);
    for(const auto& Id:Layer.OverdubbedSourceIds) S.OverdubbedSourceIds.AddUnique(Id);
    S.OverdubbedSourceIds.Sort([](const FPrimaryAssetId& L,const FPrimaryAssetId& R){return L.ToString()<R.ToString();});
    S.ReceiverKeysSnapshot.Reset(); if(Base.CanSatisfyReceiver()) S.ReceiverKeysSnapshot.AppendTags(Base.ReceiverKeysSnapshot); if(Layer.CanSatisfyReceiver()) S.ReceiverKeysSnapshot.AppendTags(Layer.ReceiverKeysSnapshot);
    S.bPuzzleEligible=!S.ReceiverKeysSnapshot.IsEmpty(); S.Fidelity=ETDFAcousticFidelity::Contaminated;
    S.Contamination=FMath::Clamp(FMath::Max(Base.Contamination,Layer.Contamination)+(Rules?Rules->OverdubContamination:.25f),0.f,1.f);
    Out.bSuccess=S.IsValid(); return Out;
}
