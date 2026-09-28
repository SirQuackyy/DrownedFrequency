#pragma once
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Acoustics/DFAcousticStimulus.h"
#include "DFRecorderPlaybackComponent.generated.h"
USTRUCT(BlueprintType)
struct DROWNEDFREQUENCY_API FTDFRecorderPlaybackRequest
{
    GENERATED_BODY()
    UPROPERTY(EditAnywhere, BlueprintReadWrite) int32 SlotIndex=INDEX_NONE;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) bool bEmitWorldStimulus=true;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) float VolumeMultiplier=1.f;
};
UCLASS(ClassGroup=(Acoustics),meta=(BlueprintSpawnableComponent))
class DROWNEDFREQUENCY_API UTDFRecorderPlaybackComponent : public UActorComponent
{
    GENERATED_BODY()
public:
    UPROPERTY(EditAnywhere, BlueprintReadOnly) TObjectPtr<class UTDFAcousticProcessingRules> ProcessingRules;
    UPROPERTY(EditAnywhere, BlueprintReadOnly) TObjectPtr<class USoundClass> PlaybackSoundClass;
    UFUNCTION(BlueprintCallable) bool PlayRecordedSlot(const FTDFRecorderPlaybackRequest& Request);
    bool PlayFrom(class UTDFRecorderComponent* Recorder,int32 Index,FVector Location,ETDFAcousticRoute Route,bool bEmit,float Volume=1.f,float RadiusMultiplier=1.f,FGuid RoutingNodeId=FGuid());
    UFUNCTION(BlueprintCallable) void StopPlayback();
    UPROPERTY(Transient, BlueprintReadOnly) TObjectPtr<class UAudioComponent> ActiveAudio;
    UPROPERTY(BlueprintReadOnly) FTDFAcousticStimulus LastPlayback;
    UPROPERTY(BlueprintReadOnly) int32 PresentationRequests=0;
    static class UTDFAudioAnomalyDataAsset* Resolve(const FPrimaryAssetId& Id);
protected:
    virtual void EndPlay(const EEndPlayReason::Type Reason) override;
};
