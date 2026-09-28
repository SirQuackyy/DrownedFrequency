#pragma once
#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "DFDifficultyProfile.generated.h"
/** Respawn contract used by saves. Full difficulty tuning/UI belongs to the later difficulty phase. */
UCLASS(BlueprintType)
class DROWNEDFREQUENCY_API UTDFDifficultyProfile : public UPrimaryDataAsset
{
    GENERATED_BODY()
public:
    UPROPERTY(EditAnywhere,BlueprintReadOnly) FName DifficultyId=TEXT("Standard");
    UPROPERTY(EditAnywhere,BlueprintReadOnly,meta=(ClampMin="1",ClampMax="100")) float CheckpointRespawnHealth=100.f;
    virtual FPrimaryAssetId GetPrimaryAssetId() const override {return FPrimaryAssetId(TEXT("TDFDifficulty"),DifficultyId);}
};
