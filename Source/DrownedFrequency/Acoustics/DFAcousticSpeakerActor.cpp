#include "Acoustics/DFAcousticSpeakerActor.h"
#include "Acoustics/DFAcousticWorldSubsystem.h"
#include "Acoustics/DFAcousticPresentation.h"
#include "Acoustics/DFRecorderPlaybackComponent.h"
#include "Acoustics/DFRecorderComponent.h"
#include "Player/DFPlayerCharacter.h"
#include "Components/StaticMeshComponent.h"
#include "Components/AudioComponent.h"
#include "Engine/World.h"
ATDFAcousticSpeakerActor::ATDFAcousticSpeakerActor()
{
    SpeakerOrigin=CreateDefaultSubobject<USceneComponent>(TEXT("SpeakerOrigin")); SetRootComponent(SpeakerOrigin);
    SpeakerMesh=CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SpeakerMesh")); SpeakerMesh->SetupAttachment(SpeakerOrigin);
    OutputAudio=CreateDefaultSubobject<UAudioComponent>(TEXT("OutputAudio")); OutputAudio->SetupAttachment(SpeakerOrigin); OutputAudio->bAutoActivate=false;
    Playback=CreateDefaultSubobject<UTDFRecorderPlaybackComponent>(TEXT("Playback"));
}
void ATDFAcousticSpeakerActor::BeginPlay()
{
    Super::BeginPlay(); ListenerHandle=GetWorld()->GetSubsystem<UTDFAcousticWorldSubsystem>()->OnStimulusNative.AddUObject(this,&ThisClass::HearStimulus);
}
void ATDFAcousticSpeakerActor::EndPlay(const EEndPlayReason::Type Reason)
{
    if(GetWorld()) if(auto* Bus=GetWorld()->GetSubsystem<UTDFAcousticWorldSubsystem>()) Bus->OnStimulusNative.Remove(ListenerHandle);
    OutputAudio->Stop(); Playback->StopPlayback(); Super::EndPlay(Reason);
}
bool ATDFAcousticSpeakerActor::BroadcastRecordedSignal(UTDFRecorderComponent* Recorder,int32 Index)
{
    return Playback->PlayFrom(Recorder,Index,SpeakerOrigin->GetComponentLocation(),ETDFAcousticRoute::Speaker,true,1.f,BroadcastRadiusMultiplier,RoutingId());
}
void ATDFAcousticSpeakerActor::Interact_Implementation(ATDFPlayerCharacter* P) {if(P) BroadcastRecordedSignal(P->Recorder,P->Recorder->SelectedSlot);}
void ATDFAcousticSpeakerActor::HearStimulus(const FTDFAcousticStimulus& S)
{
    if(bRebroadcastNearby && FMath::IsFinite(ListenRadius) && ListenRadius>0 && FVector::DistSquared(S.WorldLocation,GetActorLocation())<=FMath::Square(FMath::Min(ListenRadius,S.Radius))) RebroadcastStimulus(S);
}
bool ATDFAcousticSpeakerActor::RebroadcastStimulus(const FTDFAcousticStimulus& S)
{
    if(!GetWorld() || !S.IsValid() || S.RouteHistory.Num()>=8 || S.VisitedNodes.Contains(RoutingId()) || !FMath::IsFinite(BroadcastRadiusMultiplier) || BroadcastRadiusMultiplier<=0 || BroadcastRadiusMultiplier>4 || !UTDFRecorderPlaybackComponent::Resolve(S.SourceAnomalyId)) return false;
    auto Out=S; Out.StimulusGuid=FGuid::NewGuid(); Out.WorldLocation=SpeakerOrigin->GetComponentLocation(); Out.Route=ETDFAcousticRoute::Speaker;
    Out.Radius=FMath::Min(100000.f,S.Radius*BroadcastRadiusMultiplier); Out.VisitedNodes.Add(RoutingId()); Out.RouteHistory.Add(TEXT("Speaker"));
    auto* Bus=GetWorld()->GetSubsystem<UTDFAcousticWorldSubsystem>(); if(!Bus || !Bus->EmitStimulus(Out)) return false;
    TDFAcoustics::PresentStimulus(OutputAudio,Out); return true;
}
