#include "Acoustics/DFAcousticPresentation.h"
#include "Acoustics/DFRecorderPlaybackComponent.h"
#include "Acoustics/DFAudioAnomalyDataAsset.h"
#include "Components/AudioComponent.h"
#include "Sound/SoundAttenuation.h"
bool TDFAcoustics::PresentStimulus(UAudioComponent* Audio,const FTDFAcousticStimulus& S)
{
    auto* Asset=UTDFRecorderPlaybackComponent::Resolve(S.SourceAnomalyId); const auto* V=Asset?Asset->FindVariant(S.Fidelity):nullptr;
    if(!Audio || !S.IsValid() || !V) return false;
    FSoundAttenuationSettings Att; Att.bSpatialize=true; Att.bAttenuate=true; Att.AttenuationShapeExtents=FVector(80); Att.FalloffDistance=FMath::Max(1.f,S.Radius-80);
    Att.bEnableOcclusion=true; Att.OcclusionVolumeAttenuation=.5f; Att.OcclusionLowPassFilterFrequency=1800;
    Audio->Stop(); Audio->SetWorldLocation(S.WorldLocation); Audio->AdjustAttenuation(Att); Audio->SetSound(V->Sound); Audio->SetVolumeMultiplier(S.Loudness); Audio->Play(); return true;
}
