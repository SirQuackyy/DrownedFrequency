#include "World/DFDoorActor.h"
#include "Components/StaticMeshComponent.h"
#include "Player/DFPlayerCharacter.h"
#include "Inventory/DFInventoryComponent.h"
#include "Acoustics/DFPlayerNoiseEmitterComponent.h"
FTDFDoorPersistentState ATDFDoorActor::GetPersistentState() const
{
    FTDFDoorPersistentState State;
    State.StableActorId=StableIdentity->StableActorId; State.bIsOpen=bIsOpen; State.bIsLocked=bIsLocked;
    return State;
}
bool ATDFDoorActor::ApplyPersistentState(const FTDFDoorPersistentState& State)
{
    if (!State.StableActorId.IsValid() || State.StableActorId!=StableIdentity->StableActorId) return false;
    bIsLocked=State.bIsLocked; bIsOpen=State.bIsOpen && !bIsLocked; bIsAnimating=true;
    OnDoorStateChanged(bIsOpen);
    return true;
}

ATDFDoorActor::ATDFDoorActor()
{
    PrimaryActorTick.bCanEverTick = true;
    RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
    Hinge = CreateDefaultSubobject<USceneComponent>(TEXT("Hinge"));
    Hinge->SetupAttachment(RootComponent);
    DoorMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DoorMesh"));
    DoorMesh->SetupAttachment(Hinge);
    DoorMesh->SetCollisionProfileName(TEXT("BlockAll"));
    NoiseEmitter=CreateDefaultSubobject<UTDFPlayerNoiseEmitterComponent>(TEXT("NoiseEmitter"));
}
void ATDFDoorActor::BeginPlay()
{
    Super::BeginPlay();
    bIsOpen = bStartsOpen && !bIsLocked;
    OnDoorStateChanged(bIsOpen);
}
bool ATDFDoorActor::CanInteract_Implementation(const ATDFPlayerCharacter* Player) const { return bAllowManualInteraction && !bIsAnimating; }
FText ATDFDoorActor::GetInteractionPrompt_Implementation(const ATDFPlayerCharacter* Player) const
{
    if (bIsLocked)
    {
        if (Player && Player->Inventory && Player->Inventory->HasKey(RequiredKeyId)) return NSLOCTEXT("TDF","DoorUnlock","Unlock");
        return NSLOCTEXT("TDF", "DoorLocked", "Locked");
    }
    return bIsOpen ? NSLOCTEXT("TDF", "CloseDoor", "Close") : NSLOCTEXT("TDF", "OpenDoor", "Open");
}
void ATDFDoorActor::Interact_Implementation(ATDFPlayerCharacter* Player)
{
    if (!CanInteract_Implementation(Player)) return;
    if (bIsLocked)
    {
        if (!Player || !Player->Inventory || !Player->Inventory->HasKey(RequiredKeyId)) { OnDoorLockedFeedback(); return; }
        bIsLocked=false;
        OnDoorUnlocked();
        SetOpen(true);
        return;
    }
    SetOpen(!bIsOpen);
}
void ATDFDoorActor::SetOpen(bool bOpen)
{
    if (bIsOpen == bOpen || (bOpen && bIsLocked)) return;
    bIsOpen = bOpen;
    bIsAnimating = true;
    if(NoiseEmitter) NoiseEmitter->EmitMachine(GetActorLocation());
    OnDoorStateChanged(bIsOpen);
}
void ATDFDoorActor::RefreshSavedPresentation()
{
    bIsOpen=bIsOpen && !bIsLocked; Hinge->SetRelativeRotation(FRotator(0,bIsOpen?OpenAngle:0,0)); bIsAnimating=false; SetActorTickEnabled(false); OnDoorStateChanged(bIsOpen);
}
