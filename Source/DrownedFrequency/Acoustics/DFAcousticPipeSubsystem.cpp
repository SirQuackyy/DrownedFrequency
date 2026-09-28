#include "Acoustics/DFAcousticPipeSubsystem.h"
#include "Acoustics/DFAcousticPipeNode.h"
#include "Acoustics/DFAcousticWorldSubsystem.h"
#include "Acoustics/DFAcousticPresentation.h"
#include "Acoustics/DFRecorderPlaybackComponent.h"
#include "Engine/World.h"
void UTDFAcousticPipeSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection); Collection.InitializeDependency<UTDFAcousticWorldSubsystem>();
    StimulusHandle=GetWorld()->GetSubsystem<UTDFAcousticWorldSubsystem>()->OnStimulusNative.AddUObject(this,&ThisClass::HandleStimulus);
}
void UTDFAcousticPipeSubsystem::Deinitialize()
{
    if(GetWorld()) if(auto* Bus=GetWorld()->GetSubsystem<UTDFAcousticWorldSubsystem>()) Bus->OnStimulusNative.Remove(StimulusHandle);
    Nodes.Reset(); Super::Deinitialize();
}
void UTDFAcousticPipeSubsystem::RegisterNode(ATDFAcousticPipeNode* Node) {if(IsValid(Node) && Node->GetWorld()==GetWorld()) Nodes.AddUnique(Node);}
void UTDFAcousticPipeSubsystem::UnregisterNode(ATDFAcousticPipeNode* Node) {Nodes.Remove(Node);}
void UTDFAcousticPipeSubsystem::HandleStimulus(const FTDFAcousticStimulus& S)
{
    TSet<FName> Networks;
    for(const auto& Weak:Nodes) if(const auto* N=Weak.Get(); N && N->bIsInput && !N->NetworkId.IsNone()) Networks.Add(N->NetworkId);
    for(const auto Network:Networks) TryRouteThroughPipe(S,Network);
}
bool UTDFAcousticPipeSubsystem::TryRouteThroughPipe(const FTDFAcousticStimulus& Input,FName NetworkId)
{
    if(!Input.IsValid() || NetworkId.IsNone() || Input.RouteHistory.Num()>=8 || !UTDFRecorderPlaybackComponent::Resolve(Input.SourceAnomalyId)) return false;
    Nodes.RemoveAll([](const auto& N){return !N.IsValid();});
    const auto Snapshot=Nodes; ATDFAcousticPipeNode* Entry=nullptr;
    for(const auto& Weak:Snapshot)
    {
        auto* N=Weak.Get();
        if(N && N->NetworkId==NetworkId && N->bIsInput && !Input.VisitedNodes.Contains(N->RoutingId()) && FMath::IsFinite(N->InputRadius) && N->InputRadius>0 && FVector::DistSquared(N->GetActorLocation(),Input.WorldLocation)<=FMath::Square(FMath::Min(N->InputRadius,Input.Radius))) {Entry=N; break;}
    }
    if(!Entry) return false;
    bool Routed=false;
    for(const auto& Weak:Snapshot)
    {
        auto* N=Weak.Get();
        if(!N || !N->bIsOutput || N->NetworkId!=NetworkId || N==Entry || Input.VisitedNodes.Contains(N->RoutingId()) || !FMath::IsFinite(N->AttenuationMultiplier) || N->AttenuationMultiplier<=0) continue;
        auto Out=Input; Out.StimulusGuid=FGuid::NewGuid(); Out.WorldLocation=N->GetActorLocation(); Out.Route=ETDFAcousticRoute::Pipe;
        Out.Radius*=FMath::Clamp(N->AttenuationMultiplier,.05f,1.f); Out.VisitedNodes.AddUnique(Entry->RoutingId()); Out.VisitedNodes.AddUnique(N->RoutingId());
        Out.RouteHistory.Add(FName(*(TEXT("Pipe:")+NetworkId.ToString())));
        if(GetWorld()->GetSubsystem<UTDFAcousticWorldSubsystem>()->EmitStimulus(Out)) {TDFAcoustics::PresentStimulus(N->OutputAudio,Out); Routed=true;}
    }
    return Routed;
}
