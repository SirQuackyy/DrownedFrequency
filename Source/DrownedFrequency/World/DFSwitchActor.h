#pragma once

#include "CoreMinimal.h"
#include "Save/DFPersistentActor.h"
#include "Save/DFPersistentStateTypes.h"
#include "Interaction/DFInteractableInterface.h"
#include "DFSwitchActor.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FTDFSwitchChanged, bool, bOn);
UCLASS(BlueprintType, Blueprintable)
class DROWNEDFREQUENCY_API ATDFSwitchActor : public ATDFPersistentActor, public ITDFInteractableInterface
{
    GENERATED_BODY()
public:
    virtual void RefreshSavedPresentation() override;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Acoustics") TObjectPtr<class UTDFPlayerNoiseEmitterComponent> NoiseEmitter;
    UFUNCTION(BlueprintPure, Category="Persistence") FTDFSwitchPersistentState GetPersistentState() const;
    UFUNCTION(BlueprintCallable, Category="Persistence") bool ApplyPersistentState(const FTDFSwitchPersistentState& State);
    ATDFSwitchActor();
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Switch") TObjectPtr<class UStaticMeshComponent> SwitchMesh;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Switch", SaveGame) bool bIsOn = false;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Switch") bool bOneShot = false;
    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category="Switch", SaveGame) bool bHasBeenUsed = false;
    UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category="Switch") TArray<TObjectPtr<AActor>> LinkedActors;
    UPROPERTY(BlueprintAssignable, Category="Switch") FTDFSwitchChanged OnSwitchChanged;
    virtual bool CanInteract_Implementation(const ATDFPlayerCharacter* Player) const override { return !bOneShot || !bHasBeenUsed; }
    virtual FText GetInteractionPrompt_Implementation(const ATDFPlayerCharacter* Player) const override;
    virtual void Interact_Implementation(ATDFPlayerCharacter* Player) override;
    virtual bool WantsPromptVisible_Implementation(const ATDFPlayerCharacter* Player) const override { return CanInteract_Implementation(Player); }
    UFUNCTION(BlueprintImplementableEvent, Category="Presentation") void OnSwitchVisualChanged(bool bOn);
protected:
    virtual void BeginPlay() override;
};
