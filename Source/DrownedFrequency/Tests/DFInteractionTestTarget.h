#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interaction/DFInteractableInterface.h"
#include "DFInteractionTestTarget.generated.h"

/** A deliberately simple input-gym fixture, not a production door or switch. */
UCLASS()
class DROWNEDFREQUENCY_API ATDFInteractionTestTarget : public AActor, public ITDFInteractableInterface
{
    GENERATED_BODY()
public:
    ATDFInteractionTestTarget();
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly) TObjectPtr<class UStaticMeshComponent> TargetMesh;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Test") bool bInteractionEnabled = true;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Test") bool bShowPrompt = true;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Test") FText Prompt = NSLOCTEXT("TDF", "TestTarget", "Test station control");
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Test") int32 InteractionCount = 0;
    virtual bool CanInteract_Implementation(const ATDFPlayerCharacter* Player) const override;
    virtual FText GetInteractionPrompt_Implementation(const ATDFPlayerCharacter* Player) const override;
    virtual void Interact_Implementation(ATDFPlayerCharacter* Player) override;
    virtual bool WantsPromptVisible_Implementation(const ATDFPlayerCharacter* Player) const override;
    virtual FName GetInteractionId_Implementation() const override;
};
