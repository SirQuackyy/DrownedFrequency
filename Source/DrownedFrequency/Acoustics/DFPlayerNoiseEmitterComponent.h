#pragma once
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Engine/DataAsset.h"
#include "Acoustics/DFAcousticTypes.h"
#include "DFPlayerNoiseEmitterComponent.generated.h"
UCLASS(BlueprintType)
class DROWNEDFREQUENCY_API UTDFPlayerNoiseTuning : public UDataAsset
{
    GENERATED_BODY()
public:
    UPROPERTY(EditAnywhere, BlueprintReadOnly) TMap<FGameplayTag,FTDFAcousticClipVariant> Categories;
    UPROPERTY(EditAnywhere, BlueprintReadOnly) float WalkStride=165.f;
    UPROPERTY(EditAnywhere, BlueprintReadOnly) float SprintStride=210.f;
    UPROPERTY(EditAnywhere, BlueprintReadOnly) float CrouchStride=120.f;
    UPROPERTY(EditAnywhere, BlueprintReadOnly) float CrouchNoiseMultiplier=.25f;
    UPROPERTY(EditAnywhere, BlueprintReadOnly) float SprintNoiseMultiplier=1.4f;
    UFUNCTION(BlueprintPure) bool HasValidDefinition() const;
};
UCLASS(ClassGroup=(Acoustics),meta=(BlueprintSpawnableComponent))
class DROWNEDFREQUENCY_API UTDFPlayerNoiseEmitterComponent : public UActorComponent
{
    GENERATED_BODY()
public:
    UTDFPlayerNoiseEmitterComponent();
    UPROPERTY(EditAnywhere, BlueprintReadOnly) TObjectPtr<UTDFPlayerNoiseTuning> Tuning;
    UFUNCTION(BlueprintCallable) bool EmitFootstep(FVector Location,float SurfaceLoudness=1.f);
    UFUNCTION(BlueprintCallable) bool EmitImpact(FVector Location,float Strength=1.f);
    UFUNCTION(BlueprintCallable) bool EmitMachine(FVector Location);
    UFUNCTION(BlueprintCallable) bool EmitBell(FVector Location);
    UFUNCTION(BlueprintCallable) bool EmitWater(FVector Location);
    UFUNCTION(BlueprintCallable) bool EmitStatic(FVector Location);
    UFUNCTION(BlueprintCallable) bool EmitAuthoredVoice(FVector Location,FPrimaryAssetId VoiceId);
    bool EmitCategory(FGameplayTag Category,FVector Location,float Strength=1.f);
    virtual void TickComponent(float Delta,ELevelTick TickType,FActorComponentTickFunction* TickFunction) override;
protected:
    virtual void BeginPlay() override;
private:
    bool EmitVariant(FGameplayTag Category,const FTDFAcousticClipVariant& Variant,FVector Location,float Strength,FPrimaryAssetId SourceId=FPrimaryAssetId());
    float DistanceSinceStep=0.f;
    FVector PreviousLocation=FVector::ZeroVector;
};
