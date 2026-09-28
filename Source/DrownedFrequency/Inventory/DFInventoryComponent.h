#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "DFInventoryComponent.generated.h"

UCLASS(BlueprintType, Blueprintable)
class DROWNEDFREQUENCY_API UTDFInventoryComponent : public UActorComponent
{
    GENERATED_BODY()
public:
    void RestoreInventory(const TArray<FName>& Items,const TArray<FName>& Keys) {OwnedItemIds=TSet<FName>(Items); OwnedKeyIds=TSet<FName>(Keys); OnInventoryChanged.Broadcast();}
    UFUNCTION(BlueprintCallable, Category="Inventory") bool AddItem(class UTDFItemDataAsset* Item);
    UFUNCTION(BlueprintPure, Category="Inventory") bool HasItem(FName ItemId) const { return !ItemId.IsNone() && OwnedItemIds.Contains(ItemId); }
    UFUNCTION(BlueprintPure, Category="Inventory") bool HasKey(FName KeyId) const { return !KeyId.IsNone() && OwnedKeyIds.Contains(KeyId); }
    UFUNCTION(BlueprintPure, Category="Inventory") TArray<FName> GetOwnedItemIds() const { return OwnedItemIds.Array(); }
    UFUNCTION(BlueprintPure, Category="Inventory") TArray<FName> GetOwnedKeyIds() const { return OwnedKeyIds.Array(); }
    DECLARE_DYNAMIC_MULTICAST_DELEGATE(FTDFInventoryChanged);
    UPROPERTY(BlueprintAssignable, Category="Inventory") FTDFInventoryChanged OnInventoryChanged;
private:
    UPROPERTY(SaveGame) TSet<FName> OwnedItemIds;
    UPROPERTY(SaveGame) TSet<FName> OwnedKeyIds;
};
