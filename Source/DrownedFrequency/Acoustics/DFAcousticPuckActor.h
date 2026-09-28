#pragma once
#include "CoreMinimal.h"
#include "Save/DFPersistentActor.h"
#include "Interaction/DFInteractableInterface.h"
#include "DFAcousticPuckActor.generated.h"
UCLASS(Blueprintable)
class DROWNEDFREQUENCY_API ATDFAcousticPuckActor : public ATDFPersistentActor, public ITDFInteractableInterface
{
    GENERATED_BODY()
public:
    FGuid PuckInstanceId=FGuid::NewGuid();
    ATDFAcousticPuckActor();
    virtual bool CanInteract_Implementation(const ATDFPlayerCharacter* Player) const override;
    virtual bool WantsPromptVisible_Implementation(const ATDFPlayerCharacter* Player) const override {return CanInteract_Implementation(Player);}
    virtual FText GetInteractionPrompt_Implementation(const ATDFPlayerCharacter* Player) const override {return NSLOCTEXT("TDF","RecoverPuck","Recover puck");}
    virtual FName GetInteractionId_Implementation() const override {return TEXT("AcousticPuck");}
    virtual void Interact_Implementation(ATDFPlayerCharacter* Player) override;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly) TObjectPtr<USceneComponent> ProjectionOrigin;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly) TObjectPtr<class UStaticMeshComponent> PuckMesh;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly) TObjectPtr<class UTDFRecorderPlaybackComponent> Playback;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(ClampMin="0.1")) float MaxProjectionLifetime=20.f;
    UFUNCTION(BlueprintCallable) bool ProjectRecordedSignal(class UTDFRecorderComponent* Recorder,int32 SlotIndex);
protected:
    virtual void EndPlay(const EEndPlayReason::Type Reason) override;
private:
    FTimerHandle StopTimer;
};
