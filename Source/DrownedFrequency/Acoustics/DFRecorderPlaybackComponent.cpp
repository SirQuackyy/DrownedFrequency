#include "Acoustics/DFRecorderPlaybackComponent.h"
#include "Acoustics/DFRecorderComponent.h"
#include "Acoustics/DFAudioAnomalyDataAsset.h"
#include "Acoustics/DFAcousticWorldSubsystem.h"
#include "Engine/AssetManager.h"
#include "Engine/World.h"
#include "Components/AudioComponent.h"
#include "Sound/SoundAttenuation.h"
#include "Acoustics/DFAcousticProcessingRules.h"
#include "Acoustics/DFAcousticCaptionSubsystem.h"
#include "Engine/GameInstance.h"
UTDFAudioAnomalyDataAsset* UTDFRecorderPlaybackComponent::Resolve(const FPrimaryAssetId& Id)
{
    if (!Id.IsValid()) return nullptr;
    auto* Asset=Cast<UTDFAudioAnomalyDataAsset>(UAssetManager::Get().GetPrimaryAssetPath(Id).TryLoad());
    return Asset && Asset->GetPrimaryAssetId()==Id && Asset->HasValidDefinition() ? Asset : nullptr;
}
bool UTDFRecorderPlaybackComponent::PlayRecordedSlot(const FTDFRecorderPlaybackRequest& R)
{
    return GetOwner() && PlayFrom(GetOwner()->FindComponentByClass<UTDFRecorderComponent>(),R.SlotIndex,GetOwner()->GetActorLocation(),ETDFAcousticRoute::Direct,R.bEmitWorldStimulus,R.VolumeMultiplier);
}
bool UTDFRecorderPlaybackComponent::PlayFrom(UTDFRecorderComponent* Recorder,int32 I,FVector Location,ETDFAcousticRoute Route,bool bEmit,float Volume,float RadiusMultiplier,FGuid RoutingNodeId)
{
    if (!Recorder || !GetOwner() || !GetWorld() || Recorder->GetWorld()!=GetWorld() || !FMath::IsFinite(Volume) || Volume<=0 || Volume>2 || !FMath::IsFinite(RadiusMultiplier) || RadiusMultiplier<=0 || RadiusMultiplier>4) return false;
    const auto* Stored=Recorder->GetSlots().GetSignal(I); if(!Stored || Stored->PlayCount==MAX_int32) return false;
    auto S=*Stored;
    if(!S.OverdubbedSourceIds.IsEmpty()) Route=ETDFAcousticRoute::Overdub;
    if(ProcessingRules) S.Fidelity=ProcessingRules->ComputeEffectiveFidelity(S);
    const auto* Asset=Resolve(S.SourceAnomalyId); const auto* Variant=Asset?Asset->FindVariant(S.Fidelity):nullptr;
    if (!Variant) {UE_LOG(LogTemp,Display,TEXT("Acoustic playback: missing authored variant for %s"),*S.SourceAnomalyId.ToString()); return false;}
    auto Event=FTDFAcousticStimulus::FromSignal(S,*Variant,Location,Recorder->GetOwner(),Route);
    if(ProcessingRules)
    {
        Event.Radius*=ProcessingRules->ComputeEffectiveRadiusMultiplier(S);
        Event.Credibility=ProcessingRules->ComputeCredibility(S,Asset->MaxMeaningfulRepetitions,Asset->RepetitionPenaltyPerExtraPlay);
    }
    auto* Bus=GetWorld()->GetSubsystem<UTDFAcousticWorldSubsystem>();
    Event.Radius=FMath::Min(100000.f,Event.Radius*RadiusMultiplier);
    if(RoutingNodeId.IsValid()) Event.VisitedNodes.AddUnique(RoutingNodeId);
    if(!Event.IsValid() || (bEmit && !Bus)) return false;
    if(!ActiveAudio)
    {
        ActiveAudio=NewObject<UAudioComponent>(GetOwner()); ActiveAudio->bAutoActivate=false; ActiveAudio->bAutoDestroy=false;
        ActiveAudio->bStopWhenOwnerDestroyed=true; GetOwner()->AddInstanceComponent(ActiveAudio); ActiveAudio->RegisterComponent();
        if(GetOwner()->GetRootComponent()) ActiveAudio->AttachToComponent(GetOwner()->GetRootComponent(),FAttachmentTransformRules::KeepWorldTransform);
    }
    ActiveAudio->Stop(); ActiveAudio->SetWorldLocation(Location); ActiveAudio->SetSound(Variant->Sound); ActiveAudio->SetVolumeMultiplier(Volume*Variant->Loudness);
    ActiveAudio->SoundClassOverride=PlaybackSoundClass;
    FSoundAttenuationSettings Att;
    Att.bAttenuate=bEmit; Att.bSpatialize=bEmit; Att.AttenuationShapeExtents=FVector(80.f); Att.FalloffDistance=FMath::Max(1.f,Event.Radius-80);
    Att.bEnableOcclusion=bEmit; Att.OcclusionVolumeAttenuation=.5f; Att.OcclusionLowPassFilterFrequency=1800;
    ActiveAudio->AdjustAttenuation(Att); ActiveAudio->bIsUISound=!bEmit;
    // Reserve the play count before delegate dispatch; nested playback sees the new value.
    if(!Recorder->IncrementPlayCount(I)) return false;
    ActiveAudio->Play(); ++PresentationRequests; LastPlayback=Event;
    if(bEmit) Bus->EmitStimulus(Event);
    else if(auto* GI=GetWorld()->GetGameInstance()) if(auto* Captions=GI->GetSubsystem<UTDFAcousticCaptionSubsystem>()) Captions->PushCaptionsForSignal(S,Route,true);
    return true;
}
void UTDFRecorderPlaybackComponent::StopPlayback() {if(ActiveAudio) ActiveAudio->Stop();}
void UTDFRecorderPlaybackComponent::EndPlay(const EEndPlayReason::Type Reason) {StopPlayback(); Super::EndPlay(Reason);}
