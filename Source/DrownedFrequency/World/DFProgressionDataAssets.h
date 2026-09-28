#pragma once
#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "DFProgressionDataAssets.generated.h"
UCLASS(BlueprintType)
class DROWNEDFREQUENCY_API UTDFObjectiveDataAsset : public UPrimaryDataAsset
{
    GENERATED_BODY()
public:
    UPROPERTY(EditAnywhere,BlueprintReadOnly) FName ObjectiveId;
    UPROPERTY(EditAnywhere,BlueprintReadOnly) FName ChapterId;
    UPROPERTY(EditAnywhere,BlueprintReadOnly) FText ShortText;
    UPROPERTY(EditAnywhere,BlueprintReadOnly) FText LongText;
    UPROPERTY(EditAnywhere,BlueprintReadOnly) FGuid OptionalTargetActorId;
    UPROPERTY(EditAnywhere,BlueprintReadOnly) bool bShowCompass=false;
    UPROPERTY(EditAnywhere,BlueprintReadOnly) bool bShowDistance=false;
    UPROPERTY(EditAnywhere,BlueprintReadOnly) bool bOptional=false;
    UPROPERTY(EditAnywhere,BlueprintReadOnly) TArray<FName> NextObjectiveIds;
    virtual FPrimaryAssetId GetPrimaryAssetId() const override {return FPrimaryAssetId(TEXT("TDFObjective"),ObjectiveId);}
    bool HasValidDefinition() const {return !ObjectiveId.IsNone() && !ChapterId.IsNone() && !ShortText.IsEmpty() && !LongText.IsEmpty() && !NextObjectiveIds.Contains(ObjectiveId);}
#if WITH_EDITOR
    virtual EDataValidationResult IsDataValid(FDataValidationContext& Context) const override;
#endif
};
UCLASS(BlueprintType)
class DROWNEDFREQUENCY_API UTDFChapterDataAsset : public UPrimaryDataAsset
{
    GENERATED_BODY()
public:
    UPROPERTY(EditAnywhere,BlueprintReadOnly) FName ChapterId;
    UPROPERTY(EditAnywhere,BlueprintReadOnly) FText DisplayName;
    UPROPERTY(EditAnywhere,BlueprintReadOnly) FName PersistentMapName;
    UPROPERTY(EditAnywhere,BlueprintReadOnly) FName EntryCheckpointId;
    UPROPERTY(EditAnywhere,BlueprintReadOnly) TArray<FName> ObjectiveIds;
    virtual FPrimaryAssetId GetPrimaryAssetId() const override {return FPrimaryAssetId(TEXT("TDFChapter"),ChapterId);}
    bool HasValidDefinition() const;
    static bool ValidateGraph(const TArray<UTDFChapterDataAsset*>& Chapters,const TArray<UTDFObjectiveDataAsset*>& Objectives,FText& Error);
#if WITH_EDITOR
    virtual EDataValidationResult IsDataValid(FDataValidationContext& Context) const override;
#endif
};
