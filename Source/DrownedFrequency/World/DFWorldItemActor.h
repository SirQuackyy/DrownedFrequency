#pragma once
#include "CoreMinimal.h"
#include "Save/DFPersistentActor.h"
#include "Save/DFPersistentStateTypes.h"
#include "Interaction/DFInteractableInterface.h"
#include "DFWorldItemActor.generated.h"
UCLASS(BlueprintType, Blueprintable)
class DROWNEDFREQUENCY_API ATDFWorldItemActor : public ATDFPersistentActor, public ITDFInteractableInterface
{
    GENERATED_BODY()
public:
    virtual void RefreshSavedPresentation() override;
    UFUNCTION(BlueprintPure, Category="Persistence") FTDFWorldItemPersistentState GetPersistentState() const;
    UFUNCTION(BlueprintCallable, Category="Persistence") bool ApplyPersistentState(const FTDFWorldItemPersistentState& State);
    ATDFWorldItemActor();
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Item") TObjectPtr<class UStaticMeshComponent> Mesh;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Item") TObjectPtr<class UTDFItemDataAsset> ItemData;
    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category="Item", SaveGame) bool bHasBeenPickedUp = false;
    virtual bool CanInteract_Implementation(const ATDFPlayerCharacter* Player) const override;
    virtual FText GetInteractionPrompt_Implementation(const ATDFPlayerCharacter* Player) const override;
    virtual void Interact_Implementation(ATDFPlayerCharacter* Player) override;
    virtual bool WantsPromptVisible_Implementation(const ATDFPlayerCharacter* Player) const override { return CanInteract_Implementation(Player); }
    virtual bool CanInspect_Implementation(const ATDFPlayerCharacter* Player) const override;
    virtual void Inspect_Implementation(ATDFPlayerCharacter* Player) override;
    UFUNCTION(BlueprintImplementableEvent, Category="Presentation") void OnPickedUp(ATDFPlayerCharacter* Player);
};
