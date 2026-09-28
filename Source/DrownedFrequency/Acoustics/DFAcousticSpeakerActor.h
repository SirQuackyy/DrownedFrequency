#pragma once
#include "CoreMinimal.h"
#include "Save/DFPersistentActor.h"
#include "Interaction/DFInteractableInterface.h"
#include "Acoustics/DFAcousticStimulus.h"
#include "DFAcousticSpeakerActor.generated.h"
UCLASS(Blueprintable)
class DROWNEDFREQUENCY_API ATDFAcousticSpeakerActor : public ATDFPersistentActor,public ITDFInteractableInterface
{
    GENERATED_BODY()
public:
    ATDFAcousticSpeakerActor();
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly) TObjectPtr<USceneComponent> SpeakerOrigin;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly) TObjectPtr<class UStaticMeshComponent> SpeakerMesh;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly) TObjectPtr<class UAudioComponent> OutputAudio;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly) TObjectPtr<class UTDFRecorderPlaybackComponent> Playback;
    UPROPERTY(EditAnywhere, BlueprintReadOnly) float BroadcastRadiusMultiplier=1.5f;
    UPROPERTY(EditAnywhere, BlueprintReadOnly) bool bRebroadcastNearby=false;
    UPROPERTY(EditAnywhere, BlueprintReadOnly) float ListenRadius=150.f;
    UFUNCTION(BlueprintCallable) bool BroadcastRecordedSignal(class UTDFRecorderComponent* Recorder,int32 SlotIndex);
    UFUNCTION(BlueprintCallable) bool RebroadcastStimulus(const FTDFAcousticStimulus& Stimulus);
    FGuid RoutingId() const {return StableIdentity->StableActorId.IsValid()?StableIdentity->StableActorId:RuntimeRoutingId;}
    virtual bool CanInteract_Implementation(const ATDFPlayerCharacter* Player) const override {return true;}
    virtual bool WantsPromptVisible_Implementation(const ATDFPlayerCharacter* Player) const override {return true;}
    virtual FText GetInteractionPrompt_Implementation(const ATDFPlayerCharacter* Player) const override {return NSLOCTEXT("TDF","SpeakerBroadcast","Broadcast selected clip");}
    virtual FName GetInteractionId_Implementation() const override {return TEXT("SpeakerInput");}
    virtual void Interact_Implementation(ATDFPlayerCharacter* Player) override;
protected:
    virtual void BeginPlay() override;
    virtual void EndPlay(const EEndPlayReason::Type Reason) override;
private:
    void HearStimulus(const FTDFAcousticStimulus& Stimulus);
    FDelegateHandle ListenerHandle;
    FGuid RuntimeRoutingId=FGuid::NewGuid();
};
