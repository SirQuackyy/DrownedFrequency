#pragma once
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "DFStableActorComponent.generated.h"
UCLASS(ClassGroup=(TDF),meta=(BlueprintSpawnableComponent))
class DROWNEDFREQUENCY_API UTDFStableActorComponent : public UActorComponent
{
    GENERATED_BODY()
public:
    UPROPERTY(EditInstanceOnly, SaveGame, BlueprintReadOnly, Category="Persistence") FGuid StableActorId;
    // Authored placement opts in. Runtime spawns are temporary unless explicitly opted in.
    UPROPERTY(EditInstanceOnly, SaveGame, BlueprintReadOnly, Category="Persistence") bool bPersistInSave=false;
    bool ShouldPersistInSave() const { return bPersistInSave; }
    void EnsurePlacedEditorId();
    void AssignNewEditorId();
protected:
    virtual void BeginPlay() override;
    virtual void EndPlay(const EEndPlayReason::Type Reason) override;
};
