#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "DFItemDataAsset.generated.h"

UENUM(BlueprintType)
enum class ETDFItemType : uint8 { Inspectable, KeyItem, ConsumableFuture };
UCLASS(BlueprintType, Blueprintable)
class DROWNEDFREQUENCY_API UTDFItemDataAsset : public UPrimaryDataAsset
{
    GENERATED_BODY()
public:
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Item") FName ItemId;
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Item") FText DisplayName;
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Item", meta=(MultiLine=true)) FText ShortDescription;
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Item") ETDFItemType ItemType = ETDFItemType::Inspectable;
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Item") TSoftObjectPtr<class UStaticMesh> InspectionMesh;
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Item") TSoftObjectPtr<class UMaterialInterface> InspectionMaterialOverride;
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Item") FVector InspectionScale = FVector(1.f,.65f,.4f);
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Item") TSoftObjectPtr<class UTexture2D> Icon;
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Item") bool bCanInspect = true;
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Item") bool bCanPickUp = false;
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Item") bool bPersistsInInventory = false;
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Item") FName KeyId;
    virtual FPrimaryAssetId GetPrimaryAssetId() const override { return FPrimaryAssetId(TEXT("TDFItem"), ItemId); }
    UFUNCTION(BlueprintPure, Category="Item") bool HasValidDefinition() const
    {
        return !ItemId.IsNone() && !DisplayName.IsEmpty()
            && (ItemType != ETDFItemType::KeyItem || (!KeyId.IsNone() && bCanPickUp && bPersistsInInventory));
    }
};
