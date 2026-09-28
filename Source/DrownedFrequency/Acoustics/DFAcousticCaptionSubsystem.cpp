#include "Acoustics/DFAcousticCaptionSubsystem.h"
#include "Acoustics/DFRecorderPlaybackComponent.h"
#include "Acoustics/DFAudioAnomalyDataAsset.h"
#include "Acoustics/DFPlayerNoiseEmitterComponent.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/Pawn.h"
void UTDFAcousticCaptionSubsystem::Initialize(FSubsystemCollectionBase& Collection) {Super::Initialize(Collection); bInitialized=true; CaptionWorld=GetWorld();}
void UTDFAcousticCaptionSubsystem::Deinitialize() {bInitialized=false; Clear(); Super::Deinitialize();}
void UTDFAcousticCaptionSubsystem::Clear() {Pending.Reset(); Active.Reset(); Clock=0.f; OnCaptionsChanged.Broadcast();}
void UTDFAcousticCaptionSubsystem::Tick(float DeltaTime)
{
    auto* World=GetWorld(); if(CaptionWorld.Get()!=World) {Clear(); CaptionWorld=World;}
    if(World && !World->IsPaused()) Advance(DeltaTime);
}
void UTDFAcousticCaptionSubsystem::Advance(float DeltaTime)
{
    if(!FMath::IsFinite(DeltaTime) || DeltaTime<0) return;
    Clock+=DeltaTime; bool Changed=Active.RemoveAll([this](const auto& C){return C.EndAt<=Clock;})>0;
    for(int32 I=0;I<Pending.Num();)
    {
        if(Pending[I].StartAt>Clock) {++I; continue;}
        auto C=Pending[I]; Pending.RemoveAt(I);
        if(C.EndAt>Clock) {if(Active.Num()>=4) Active.RemoveAt(0); Active.Add(C); Changed=true;}
    }
    for(auto& C:Active) C.TimeRemaining=FMath::Max(0.f,C.EndAt-Clock);
    if(Changed) OnCaptionsChanged.Broadcast();
}
void UTDFAcousticCaptionSubsystem::PushVariant(const FTDFAcousticClipVariant& V,FGameplayTag Category,ETDFAcousticRoute Route,FVector Location,bool bLocal)
{
    if(!V.IsValid() || !TDFAcoustics::IsCategory(Category)) return;
    for(const auto& Line:V.Captions)
    {
        if(Line.Text.IsEmpty()) continue;
        FTDFActiveAcousticCaption C; C.Text=Line.Text; C.CategoryTag=Category; C.Route=Route; C.SourceLocation=Location; C.bLocalPreview=bLocal;
        C.StartAt=Clock+Line.StartTime; C.EndAt=C.StartAt+FMath::Min(Line.Duration,20.f);
        if(Pending.Num()>=32) Pending.RemoveAt(0); Pending.Add(C);
    }
    Advance(0);
}
void UTDFAcousticCaptionSubsystem::PushCaptionsForSignal(const FTDFRecordedSignal& S,ETDFAcousticRoute Route,bool bLocal)
{
    if(!S.IsValid()) return;
    auto* Asset=UTDFRecorderPlaybackComponent::Resolve(S.SourceAnomalyId); const auto* V=Asset?Asset->FindVariant(S.Fidelity):nullptr;
    if(V) PushVariant(*V,S.CategoryTag,Route,FVector::ZeroVector,bLocal);
}
void UTDFAcousticCaptionSubsystem::PushCaptionsForStimulus(const FTDFAcousticStimulus& S)
{
    if(!S.IsValid() || S.Loudness<=0) return;
    if(auto* World=GetWorld()) if(auto* PC=World->GetFirstPlayerController()) if(APawn* Pawn=PC->GetPawn()) if(FVector::DistSquared(Pawn->GetActorLocation(),S.WorldLocation)>FMath::Square(S.Radius)) return;
    auto* Asset=UTDFRecorderPlaybackComponent::Resolve(S.SourceAnomalyId); const auto* V=Asset?Asset->FindVariant(S.Fidelity):nullptr;
    if(!V && S.InstigatorActor.IsValid()) if(const auto* Noise=S.InstigatorActor->FindComponentByClass<UTDFPlayerNoiseEmitterComponent>()) if(Noise->Tuning) V=Noise->Tuning->Categories.Find(S.CategoryTag);
    if(V) PushVariant(*V,S.CategoryTag,S.Route,S.WorldLocation,false);
}
