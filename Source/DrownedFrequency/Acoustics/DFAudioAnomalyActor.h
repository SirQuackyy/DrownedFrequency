#pragma once
#include "CoreMinimal.h"
#include "Save/DFPersistentActor.h"
#include "Acoustics/DFRecordedSignal.h"
#include "DFAudioAnomalyActor.generated.h"

UCLASS(Blueprintable)
class DROWNEDFREQUENCY_API ATDFAudioAnomalyActor : public ATDFPersistentActor
{
    GENERATED_BODY()
public:
    ATDFAudioAnomalyActor();
    UPROPERTY(EditAnywhere, BlueprintReadOnly) TObjectPtr<class UTDFAudioAnomalyDataAsset> AnomalyData;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly) TObjectPtr<USceneComponent> StimulusOrigin;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly) TObjectPtr<class UStaticMeshComponent> SourceMesh;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly) TObjectPtr<class UAudioComponent> SourceAudio;
    UPROPERTY(EditAnywhere, BlueprintReadOnly) bool bAutoEmitOnBeginPlay=false;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(ClampMin="0")) float RepeatInterval=0.f;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(ClampMin="1")) float InteractionRecordRange=250.f;
    UFUNCTION(BlueprintPure) bool CanBeRecordedBy(AActor* RecorderOwner) const;
    UFUNCTION(BlueprintPure) FTDFRecordedSignal MakeRecordedSignal(ETDFAcousticFidelity Fidelity=ETDFAcousticFidelity::Clean) const;
    UFUNCTION(BlueprintCallable) bool EmitWorldStimulus();
protected:
    virtual void BeginPlay() override;
    virtual void EndPlay(const EEndPlayReason::Type Reason) override;
private:
    FTimerHandle RepeatTimer;
};
