#pragma once
#include "CoreMinimal.h"
#include "Save/DFPersistentActor.h"
#include "DFCheckpointActor.generated.h"
UCLASS(Blueprintable)
class DROWNEDFREQUENCY_API ATDFCheckpointActor : public ATDFPersistentActor
{
    GENERATED_BODY()
public:
    ATDFCheckpointActor();
    UPROPERTY(EditAnywhere,BlueprintReadOnly) FName CheckpointId;
    UPROPERTY(EditAnywhere,BlueprintReadOnly) FName ChapterId;
    UPROPERTY(EditAnywhere,BlueprintReadOnly) FTransform RespawnTransform=FTransform::Identity;
    UPROPERTY(EditAnywhere,BlueprintReadOnly) TArray<FName> RequiredLoadedSectors;
    UPROPERTY(EditAnywhere,BlueprintReadOnly) bool bAutoTrigger=true;
    UPROPERTY(EditAnywhere,BlueprintReadOnly) bool bAllowManualSaveAfter=true;
    UPROPERTY(EditAnywhere,BlueprintReadOnly) bool bChapterStart=false;
    UPROPERTY(VisibleAnywhere) TObjectPtr<class UBoxComponent> Trigger;
    UPROPERTY(VisibleAnywhere) TObjectPtr<class UArrowComponent> RespawnArrow;
protected:
    virtual void BeginPlay() override;
    virtual void EndPlay(const EEndPlayReason::Type Reason) override;
    virtual void OnConstruction(const FTransform& Transform) override;
    UFUNCTION() void Overlap(UPrimitiveComponent* Component,AActor* Other,UPrimitiveComponent* OtherComponent,int32 BodyIndex,bool bFromSweep,const FHitResult& Hit);
    void RetryOverlap();
    TWeakObjectPtr<class ATDFPlayerCharacter> PendingPlayer;
    FTimerHandle RetryTimer;
};
