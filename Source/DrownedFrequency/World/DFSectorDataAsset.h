#pragma once
#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "DFSectorDataAsset.generated.h"
UCLASS(BlueprintType)
class DROWNEDFREQUENCY_API UTDFSectorDataAsset : public UPrimaryDataAsset
{
    GENERATED_BODY()
public:
    UPROPERTY(EditAnywhere,BlueprintReadOnly) FName SectorId;
    UPROPERTY(EditAnywhere,BlueprintReadOnly) FName LevelPackageName;
    UPROPERTY(EditAnywhere,BlueprintReadOnly) TArray<FName> NeighborSectorIds;
    UPROPERTY(EditAnywhere,BlueprintReadOnly) TArray<FName> RequiredForCheckpoints;
    UPROPERTY(EditAnywhere,BlueprintReadOnly) bool bKeepLoadedAfterVisit=false;
    virtual FPrimaryAssetId GetPrimaryAssetId() const override {return FPrimaryAssetId(TEXT("TDFSector"),SectorId);}
    bool HasValidDefinition() const;
#if WITH_EDITOR
    virtual EDataValidationResult IsDataValid(FDataValidationContext& Context) const override;
#endif
};
