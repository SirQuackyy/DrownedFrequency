#include "World/DFSwitchActor.h"
#include "World/DFSwitchReceiverInterface.h"
#include "Components/StaticMeshComponent.h"
#include "Acoustics/DFPlayerNoiseEmitterComponent.h"
FTDFSwitchPersistentState ATDFSwitchActor::GetPersistentState() const
{
    FTDFSwitchPersistentState State;
    State.StableActorId=StableIdentity->StableActorId; State.bIsOn=bIsOn; State.bHasBeenUsed=bHasBeenUsed;
    return State;
}
bool ATDFSwitchActor::ApplyPersistentState(const FTDFSwitchPersistentState& State)
{
    if (!State.StableActorId.IsValid() || State.StableActorId!=StableIdentity->StableActorId) return false;
    bIsOn=State.bIsOn; bHasBeenUsed=State.bHasBeenUsed;
    // Restore presentation only: replaying receiver commands would make load order overwrite door saves.
    OnSwitchVisualChanged(bIsOn);
    return true;
}

ATDFSwitchActor::ATDFSwitchActor()
{
    RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
    SwitchMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SwitchMesh"));
    SwitchMesh->SetupAttachment(RootComponent);
    SwitchMesh->SetCollisionProfileName(TEXT("BlockAll"));
    NoiseEmitter=CreateDefaultSubobject<UTDFPlayerNoiseEmitterComponent>(TEXT("NoiseEmitter"));
}
void ATDFSwitchActor::BeginPlay() { Super::BeginPlay(); OnSwitchVisualChanged(bIsOn); }
FText ATDFSwitchActor::GetInteractionPrompt_Implementation(const ATDFPlayerCharacter* Player) const
{
    return bIsOn ? NSLOCTEXT("TDF", "SwitchOff", "Switch off") : NSLOCTEXT("TDF", "SwitchOn", "Switch on");
}
void ATDFSwitchActor::Interact_Implementation(ATDFPlayerCharacter* Player)
{
    if (!CanInteract_Implementation(Player)) return;
    bHasBeenUsed = true;
    if(NoiseEmitter) NoiseEmitter->EmitImpact(GetActorLocation(),.4f);
    bIsOn = bOneShot || !bIsOn;
    OnSwitchVisualChanged(bIsOn);
    OnSwitchChanged.Broadcast(bIsOn); CompleteAssignedObjective();
    for (AActor* Actor : LinkedActors)
        if (IsValid(Actor) && Actor->Implements<UTDFSwitchReceiverInterface>())
            ITDFSwitchReceiverInterface::Execute_ReceiveSwitchState(Actor, this, bIsOn);
}
void ATDFSwitchActor::RefreshSavedPresentation() {OnSwitchVisualChanged(bIsOn);}
