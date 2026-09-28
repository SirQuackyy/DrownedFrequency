#pragma once
#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Tickable.h"
#include "Acoustics/DFAcousticStimulus.h"
#include "DFAcousticCaptionSubsystem.generated.h"
USTRUCT(BlueprintType)
struct DROWNEDFREQUENCY_API FTDFActiveAcousticCaption
{
    GENERATED_BODY()
    UPROPERTY(BlueprintReadOnly) FText Text;
    UPROPERTY(BlueprintReadOnly) float TimeRemaining=0.f;
    UPROPERTY(BlueprintReadOnly) FGameplayTag CategoryTag;
    UPROPERTY(BlueprintReadOnly) ETDFAcousticRoute Route=ETDFAcousticRoute::Direct;
    UPROPERTY(BlueprintReadOnly) FVector SourceLocation=FVector::ZeroVector;
    UPROPERTY(BlueprintReadOnly) bool bLocalPreview=false;
    UPROPERTY() float StartAt=0.f;
    UPROPERTY() float EndAt=0.f;
};
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FTDFCaptionChanged);
UCLASS()
class DROWNEDFREQUENCY_API UTDFAcousticCaptionSubsystem : public UGameInstanceSubsystem,public FTickableGameObject
{
    GENERATED_BODY()
public:
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    virtual void Deinitialize() override;
    virtual void Tick(float DeltaTime) override;
    virtual bool IsTickable() const override {return bInitialized && !IsTemplate();}
    virtual TStatId GetStatId() const override {RETURN_QUICK_DECLARE_CYCLE_STAT(UTDFAcousticCaptionSubsystem,STATGROUP_Tickables);}
    virtual UWorld* GetTickableGameObjectWorld() const override {return GetWorld();}
    UPROPERTY(BlueprintAssignable) FTDFCaptionChanged OnCaptionsChanged;
    UFUNCTION(BlueprintCallable) void PushCaptionsForSignal(const FTDFRecordedSignal& Signal,ETDFAcousticRoute Route,bool bLocalPreview=true);
    UFUNCTION(BlueprintCallable) void PushCaptionsForStimulus(const FTDFAcousticStimulus& Stimulus);
    UFUNCTION(BlueprintPure) TArray<FTDFActiveAcousticCaption> GetActiveCaptions() const {return Active;}
    void PushVariant(const FTDFAcousticClipVariant& Variant,FGameplayTag Category,ETDFAcousticRoute Route,FVector Location,bool bLocal);
    void Advance(float DeltaTime);
    void Clear();
private:
    UPROPERTY(Transient) TArray<FTDFActiveAcousticCaption> Pending;
    UPROPERTY(Transient) TArray<FTDFActiveAcousticCaption> Active;
    TWeakObjectPtr<UWorld> CaptionWorld;
    float Clock=0.f;
    bool bInitialized=false;
};
