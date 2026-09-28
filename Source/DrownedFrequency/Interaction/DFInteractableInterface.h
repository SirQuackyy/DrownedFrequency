#pragma once
#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "DFInteractableInterface.generated.h"

class ATDFPlayerCharacter;

UINTERFACE(BlueprintType)
class DROWNEDFREQUENCY_API UTDFInteractableInterface : public UInterface
{
    GENERATED_BODY()
};
class DROWNEDFREQUENCY_API ITDFInteractableInterface
{
    GENERATED_BODY()
public:
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Interaction")
    bool CanInspect(const ATDFPlayerCharacter* Player) const;
    virtual bool CanInspect_Implementation(const ATDFPlayerCharacter* Player) const { return false; }
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Interaction")
    void Inspect(ATDFPlayerCharacter* Player);
    virtual void Inspect_Implementation(ATDFPlayerCharacter* Player) {}
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Interaction")
    bool CanInteract(const ATDFPlayerCharacter* Player) const;
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Interaction")
    FText GetInteractionPrompt(const ATDFPlayerCharacter* Player) const;
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Interaction")
    void Interact(ATDFPlayerCharacter* Player);
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Interaction")
    bool WantsPromptVisible(const ATDFPlayerCharacter* Player) const;
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Interaction")
    FName GetInteractionId() const;
};
