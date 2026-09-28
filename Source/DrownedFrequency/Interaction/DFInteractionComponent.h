#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "DFInteractionComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FTDFFocusChanged, AActor*, FocusedActor);

UCLASS(BlueprintType, Blueprintable)
class DROWNEDFREQUENCY_API UTDFInteractionComponent : public UActorComponent
{
    GENERATED_BODY()
public:
    UTDFInteractionComponent();
    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
    UFUNCTION(BlueprintCallable, Category="Interaction") void RefreshFocus();
    UFUNCTION(BlueprintCallable, Category="Interaction") bool TryInteract();
    UFUNCTION(BlueprintPure, Category="Interaction") AActor* GetFocusedActor() const { return FocusedActor.Get(); }
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Interaction") float InteractionDistance = 220.f;
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Interaction") float TraceRadius = 8.f;
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Interaction") TEnumAsByte<ECollisionChannel> TraceChannel = ECC_Visibility;
    UPROPERTY(BlueprintAssignable, Category="Interaction") FTDFFocusChanged OnFocusedInteractableChanged;
private:
    TWeakObjectPtr<AActor> FocusedActor;
    bool bHadFocus = false;
};
