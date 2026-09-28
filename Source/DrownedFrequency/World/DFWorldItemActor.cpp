#include "World/DFWorldItemActor.h"
#include "Components/StaticMeshComponent.h"
#include "Inventory/DFItemDataAsset.h"
#include "Inventory/DFInventoryComponent.h"
#include "Player/DFPlayerCharacter.h"
#include "Player/DFPlayerController.h"
FTDFWorldItemPersistentState ATDFWorldItemActor::GetPersistentState() const
{
    FTDFWorldItemPersistentState State;
    State.StableActorId=StableIdentity->StableActorId; State.bHasBeenPickedUp=bHasBeenPickedUp;
    return State;
}
bool ATDFWorldItemActor::ApplyPersistentState(const FTDFWorldItemPersistentState& State)
{
    if (!State.StableActorId.IsValid() || State.StableActorId!=StableIdentity->StableActorId) return false;
    bHasBeenPickedUp=State.bHasBeenPickedUp;
    SetActorHiddenInGame(bHasBeenPickedUp);
    SetActorEnableCollision(!bHasBeenPickedUp);
    return true;
}
ATDFWorldItemActor::ATDFWorldItemActor()
{
    RootComponent=CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
    Mesh=CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
    Mesh->SetupAttachment(RootComponent);
    Mesh->SetCollisionProfileName(TEXT("BlockAll"));
    Mesh->SetSimulatePhysics(false);
}
bool ATDFWorldItemActor::CanInteract_Implementation(const ATDFPlayerCharacter* Player) const
{
    return Player && ItemData && ItemData->HasValidDefinition() && !bHasBeenPickedUp
        && (ItemData->bCanPickUp || ItemData->bCanInspect);
}
bool ATDFWorldItemActor::CanInspect_Implementation(const ATDFPlayerCharacter* Player) const
{
    return CanInteract_Implementation(Player) && ItemData->bCanInspect;
}
FText ATDFWorldItemActor::GetInteractionPrompt_Implementation(const ATDFPlayerCharacter* Player) const
{
    if (!ItemData) return FText::GetEmpty();
    return FText::Format(ItemData->bCanPickUp ? NSLOCTEXT("TDF","TakeItem","Take {0}") : NSLOCTEXT("TDF","InspectItem","Inspect {0}"), ItemData->DisplayName);
}
void ATDFWorldItemActor::Inspect_Implementation(ATDFPlayerCharacter* Player)
{
    if (!CanInspect_Implementation(Player)) return;
    if (auto* PC=Cast<ATDFPlayerController>(Player->GetController())) PC->OpenInspection(ItemData);
}
void ATDFWorldItemActor::Interact_Implementation(ATDFPlayerCharacter* Player)
{
    if (!CanInteract_Implementation(Player)) return;
    if (ItemData->bCanPickUp)
    {
        if (!Player->Inventory) return;
        // A second authored copy of an already-owned key may be collected without duplicating inventory.
        if (!Player->Inventory->AddItem(ItemData) && !Player->Inventory->HasItem(ItemData->ItemId)) return;
        bHasBeenPickedUp=true;
        SetActorHiddenInGame(true);
        SetActorEnableCollision(false);
        OnPickedUp(Player); CompleteAssignedObjective();
    }
    else Inspect_Implementation(Player);
}
void ATDFWorldItemActor::RefreshSavedPresentation() {SetActorHiddenInGame(bHasBeenPickedUp); SetActorEnableCollision(!bHasBeenPickedUp);}
