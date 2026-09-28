#pragma once
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "DFRecorderOperatorComponent.generated.h"
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FTDFRecorderFeedback);
UCLASS(ClassGroup=(Acoustics),meta=(BlueprintSpawnableComponent))
class DROWNEDFREQUENCY_API UTDFRecorderOperatorComponent : public UActorComponent
{
    GENERATED_BODY()
public:
    TArray<struct FTDFPuckSaveData> CapturePlacedPucks() const;
    void RestorePlacedPucks(const TArray<struct FTDFPuckSaveData>& Pucks);
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly) TSubclassOf<class ATDFAcousticPuckActor> PuckClass;
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly) float CaptureSeconds=1.2f;
    UPROPERTY(BlueprintReadOnly) FText StatusText;
    UPROPERTY(BlueprintReadOnly) bool bRecording=false;
    UPROPERTY(BlueprintAssignable) FTDFRecorderFeedback OnFeedback;
    UFUNCTION(BlueprintCallable) void StartRecording();
    UFUNCTION(BlueprintCallable) void CancelRecording();
    UFUNCTION(BlueprintCallable) void PlaySelected();
    UFUNCTION(BlueprintCallable) void DeleteSelected();
    UFUNCTION(BlueprintCallable) void NextSlot();
    UFUNCTION(BlueprintCallable) void PreviousSlot();
    UFUNCTION(BlueprintCallable) bool PlacePuck();
    UFUNCTION(BlueprintCallable) bool RecoverPuck(ATDFAcousticPuckActor* Puck);
    UFUNCTION(BlueprintPure) int32 GetPucksAvailable() const;
    UFUNCTION(BlueprintCallable) void RestorePucksAtSafeDesk();
    void Feedback(const FString& Message);
protected:
    virtual void EndPlay(const EEndPlayReason::Type Reason) override;
private:
    bool CanOperate() const;
    void CompleteRecording();
    TWeakObjectPtr<class ATDFAudioAnomalyActor> CaptureSource;
    TArray<TWeakObjectPtr<ATDFAcousticPuckActor>> PlacedPucks;
    FTimerHandle CaptureTimer;
    int32 CaptureSlot=INDEX_NONE;
};
