#pragma once
#include "CoreMinimal.h"
#include "Save/DFPersistentActor.h"
#include "Interaction/DFInteractableInterface.h"
#include "DFAcousticPipeNode.generated.h"
UCLASS(Blueprintable)
class DROWNEDFREQUENCY_API ATDFAcousticPipeNode : public ATDFPersistentActor,public ITDFInteractableInterface
{
    GENERATED_BODY()
public:
    ATDFAcousticPipeNode();
    UPROPERTY(EditAnywhere, BlueprintReadOnly) FName NetworkId;
    UPROPERTY(EditAnywhere, BlueprintReadOnly) bool bIsInput=false;
    UPROPERTY(EditAnywhere, BlueprintReadOnly) bool bIsOutput=false;
    UPROPERTY(EditAnywhere, BlueprintReadOnly) float InputRadius=150.f;
    UPROPERTY(EditAnywhere, BlueprintReadOnly) float AttenuationMultiplier=.75f;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly) TObjectPtr<class UStaticMeshComponent> NodeMesh;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly) TObjectPtr<class UAudioComponent> OutputAudio;
    FGuid RoutingId() const {return StableIdentity->StableActorId.IsValid()?StableIdentity->StableActorId:RuntimeRoutingId;}
    virtual bool CanInteract_Implementation(const ATDFPlayerCharacter* Player) const override {return bIsInput;}
    virtual bool WantsPromptVisible_Implementation(const ATDFPlayerCharacter* Player) const override {return bIsInput;}
    virtual FText GetInteractionPrompt_Implementation(const ATDFPlayerCharacter* Player) const override {return NSLOCTEXT("TDF","PipeFeed","Feed selected clip into pipe");}
    virtual FName GetInteractionId_Implementation() const override {return TEXT("PipeInput");}
    virtual void Interact_Implementation(ATDFPlayerCharacter* Player) override;
protected:
    virtual void BeginPlay() override;
    virtual void EndPlay(const EEndPlayReason::Type Reason) override;
private:
    FGuid RuntimeRoutingId=FGuid::NewGuid();
};
