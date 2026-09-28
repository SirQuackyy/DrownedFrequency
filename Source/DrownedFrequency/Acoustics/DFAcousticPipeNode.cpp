#include "Acoustics/DFAcousticPipeNode.h"
#include "Acoustics/DFAcousticPipeSubsystem.h"
#include "Acoustics/DFRecorderPlaybackComponent.h"
#include "Acoustics/DFRecorderComponent.h"
#include "Player/DFPlayerCharacter.h"
#include "Components/StaticMeshComponent.h"
#include "Components/AudioComponent.h"
#include "Engine/World.h"
ATDFAcousticPipeNode::ATDFAcousticPipeNode()
{
    NodeMesh=CreateDefaultSubobject<UStaticMeshComponent>(TEXT("NodeMesh")); SetRootComponent(NodeMesh);
    OutputAudio=CreateDefaultSubobject<UAudioComponent>(TEXT("OutputAudio")); OutputAudio->SetupAttachment(NodeMesh); OutputAudio->bAutoActivate=false;
}
void ATDFAcousticPipeNode::BeginPlay() {Super::BeginPlay(); GetWorld()->GetSubsystem<UTDFAcousticPipeSubsystem>()->RegisterNode(this);}
void ATDFAcousticPipeNode::EndPlay(const EEndPlayReason::Type Reason)
{
    if(GetWorld()) if(auto* Pipes=GetWorld()->GetSubsystem<UTDFAcousticPipeSubsystem>()) Pipes->UnregisterNode(this);
    OutputAudio->Stop(); Super::EndPlay(Reason);
}
void ATDFAcousticPipeNode::Interact_Implementation(ATDFPlayerCharacter* P)
{
    if(bIsInput && P && P->RecorderPlayback) P->RecorderPlayback->PlayFrom(P->Recorder,P->Recorder->SelectedSlot,GetActorLocation(),ETDFAcousticRoute::Direct,true);
}
