#pragma once
#include "CoreMinimal.h"
#include "Save/DFPersistentActor.h"
#include "Interaction/DFInteractableInterface.h"
#include "DFAcousticReceiverActor.generated.h"
UCLASS(Blueprintable)
class DROWNEDFREQUENCY_API ATDFAcousticReceiverActor : public ATDFPersistentActor, public ITDFInteractableInterface
{
    GENERATED_BODY()
public:
    ATDFAcousticReceiverActor();
    virtual void RefreshSavedPresentation() override {OnReceiverVisualChanged(bIsSatisfied);}
    virtual bool CanInteract_Implementation(const ATDFPlayerCharacter* Player) const override;
    virtual bool WantsPromptVisible_Implementation(const ATDFPlayerCharacter* Player) const override {return CanInteract_Implementation(Player);}
    virtual FText GetInteractionPrompt_Implementation(const ATDFPlayerCharacter* Player) const override {return NSLOCTEXT("Acoustic","Recover","Recover remembered signal");}
    virtual void Interact_Implementation(ATDFPlayerCharacter* Player) override;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly) TObjectPtr<class UStaticMeshComponent> ReceiverMesh;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Presentation") TObjectPtr<class UPointLightComponent> StatusLight;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly) TObjectPtr<class UTDFAcousticReceiverComponent> Receiver;
    UPROPERTY(EditInstanceOnly, BlueprintReadOnly) TObjectPtr<AActor> LinkedActor;
    UPROPERTY(BlueprintReadOnly, SaveGame) bool bIsSatisfied=false;
    UFUNCTION(BlueprintImplementableEvent, Category="Presentation") void OnReceiverVisualChanged(bool bSatisfied);
protected:
    virtual void BeginPlay() override;
    UFUNCTION() void HandleSatisfied(int32 RuleIndex);
};
