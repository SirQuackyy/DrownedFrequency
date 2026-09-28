#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Save/DFStableActorInterface.h"
#include "Save/DFStableActorComponent.h"
#include "Save/DFSaveableActorInterface.h"
#include "DFPersistentActor.generated.h"
UCLASS(Abstract)
class DROWNEDFREQUENCY_API ATDFPersistentActor : public AActor, public ITDFStableActorInterface, public ITDFSaveableActorInterface
{
    GENERATED_BODY()
public:
    ATDFPersistentActor();
    UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="Objectives") FName ObjectiveIdToComplete;
    UFUNCTION(BlueprintCallable) void CompleteAssignedObjective();
    virtual FTDFActorSaveData CaptureSaveState_Implementation() const override;
    virtual bool ApplySaveState_Implementation(const FTDFActorSaveData& State) override;
    virtual void RefreshSavedPresentation() {}
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Persistence") TObjectPtr<UTDFStableActorComponent> StableIdentity;
    virtual FGuid GetStableActorId_Implementation() const override { return StableIdentity->StableActorId; }
    virtual bool ShouldPersistInSave_Implementation() const override { return StableIdentity->ShouldPersistInSave(); }
    virtual void SetStableActorId_Implementation(FGuid Id) override;
    virtual void OnConstruction(const FTransform& Transform) override;
#if WITH_EDITOR
    virtual void PostDuplicate(EDuplicateMode::Type DuplicateMode) override;
    virtual void PostEditImport() override;
#endif
};
