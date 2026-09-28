#pragma once
#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "Acoustics/DFAcousticStimulus.h"
#include "DFAcousticWorldSubsystem.generated.h"
DECLARE_MULTICAST_DELEGATE_OneParam(FTDFAcousticNativeDelegate,const FTDFAcousticStimulus&);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FTDFAcousticBlueprintDelegate,const FTDFAcousticStimulus&,Stimulus);

UCLASS()
class DROWNEDFREQUENCY_API UTDFAcousticWorldSubsystem : public UWorldSubsystem
{
    GENERATED_BODY()
public:
    FTDFAcousticNativeDelegate OnStimulusNative;
    UPROPERTY(BlueprintAssignable) FTDFAcousticBlueprintDelegate OnStimulus;
    UFUNCTION(BlueprintCallable) bool EmitStimulus(const FTDFAcousticStimulus& Stimulus);
    UFUNCTION(BlueprintPure) TArray<FTDFAcousticStimulus> GetRecentStimuli() const { return Recent; }
    static constexpr int32 RecentLimit=64;
    UFUNCTION(BlueprintCallable) void RegisterReceiver(class UTDFAcousticReceiverComponent* Receiver);
    UFUNCTION(BlueprintCallable) void UnregisterReceiver(class UTDFAcousticReceiverComponent* Receiver);
private:
    TArray<TWeakObjectPtr<class UTDFAcousticReceiverComponent>> Receivers;
    UPROPERTY(Transient) TArray<FTDFAcousticStimulus> Recent;
    int32 DispatchDepth=0;
};
