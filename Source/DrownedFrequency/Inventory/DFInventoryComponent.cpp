#include "Inventory/DFInventoryComponent.h"
#include "Inventory/DFItemDataAsset.h"
bool UTDFInventoryComponent::AddItem(UTDFItemDataAsset* Item)
{
    if (!Item || !Item->HasValidDefinition() || !Item->bCanPickUp || !Item->bPersistsInInventory
        || Item->ItemType == ETDFItemType::ConsumableFuture || OwnedItemIds.Contains(Item->ItemId)) return false;
    OwnedItemIds.Add(Item->ItemId);
    if (Item->ItemType == ETDFItemType::KeyItem) OwnedKeyIds.Add(Item->KeyId);
    OnInventoryChanged.Broadcast();
    return true;
}
