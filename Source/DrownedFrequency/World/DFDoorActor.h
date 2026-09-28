#pragma once

#include "CoreMinimal.h"
#include "Save/DFPersistentActor.h"
#include "Save/DFPersistentStateTypes.h"
#include "Interaction/DFInteractableInterface.h"
#include "World/DFSwitchReceiverInterface.h"
#include "DFDoorActor.generated.h"

UCLASS(BlueprintType, Blueprintable)
class DROWNEDFREQUENCY_API ATDFDoorActor : public ATDFPersistentActor, public ITDFInteractableInterface, public ITDFSwitchReceiverInterface
{
    GENERATED_BODY()
public:
    virtual void RefreshSavedPresentation() override;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Door") bool bAllowManualInteraction=true;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Acoustics") TObjectPtr<class UTDFPlayerNoiseEmitterComponent> NoiseEmitter;
    UFUNCTION(BlueprintPure, Category="Persistence") FTDFDoorPersistentState GetPersistentState() const;
    UFUNCTION(BlueprintCallable, Category="Persistence") bool ApplyPersistentState(const FTDFDoorPersistentState& State);
    ATDFDoorActor();
    virtual void ReceiveSwitchState_Implementation(AActor* Source, bool bOn) override { SetOpen(bOn); }
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Door") TObjectPtr<USceneComponent> Hinge;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Door") TObjectPtr<class UStaticMeshComponent> DoorMesh;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Door") bool bStartsOpen = false;
    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category="Door", SaveGame) bool bIsOpen = false;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Door", SaveGame) bool bIsLocked = false;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Door") FName RequiredKeyId;
    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category="Door") bool bIsAnimating = false;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Presentation") float OpenAngle = -95.f;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Presentation") float SwingSpeed = 95.f;
    virtual bool CanInteract_Implementation(const ATDFPlayerCharacter* Player) const override;
    virtual FText GetInteractionPrompt_Implementation(const ATDFPlayerCharacter* Player) const override;
    virtual void Interact_Implementation(ATDFPlayerCharacter* Player) override;
    virtual bool WantsPromptVisible_Implementation(const ATDFPlayerCharacter* Player) const override { return true; }
    UFUNCTION(BlueprintCallable, Category="Door") void SetOpen(bool bOpen);
    UFUNCTION(BlueprintCallable, Category="Door") void FinishDoorAnimation() { bIsAnimating = false; }
    UFUNCTION(BlueprintImplementableEvent, Category="Presentation") void OnDoorStateChanged(bool bOpen);
    UFUNCTION(BlueprintImplementableEvent, Category="Presentation") void OnDoorLockedFeedback();
    UFUNCTION(BlueprintImplementableEvent, Category="Presentation") void OnDoorUnlocked();
protected:
    virtual void BeginPlay() override;
};
