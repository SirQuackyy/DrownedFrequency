#pragma once
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Engine/DataAsset.h"
#include "Acoustics/DFRecordedSignal.h"
#include "DFAcousticOverdubComponent.generated.h"
UCLASS(BlueprintType)
class DROWNEDFREQUENCY_API UTDFContaminationRules : public UDataAsset
{
    GENERATED_BODY()
public:
    UPROPERTY(EditAnywhere, BlueprintReadOnly) float OverdubContamination=.25f;
    UPROPERTY(EditAnywhere, BlueprintReadOnly) int32 UnlockAct=5;
};
USTRUCT(BlueprintType)
struct DROWNEDFREQUENCY_API FTDFOverdubResult
{
    GENERATED_BODY()
    UPROPERTY(BlueprintReadOnly) bool bSuccess=false;
    UPROPERTY(BlueprintReadOnly) FTDFRecordedSignal ResultSignal;
    UPROPERTY(BlueprintReadOnly) FText FailureReason;
};
UCLASS(ClassGroup=(Acoustics),meta=(BlueprintSpawnableComponent))
class DROWNEDFREQUENCY_API UTDFAcousticOverdubComponent : public UActorComponent
{
    GENERATED_BODY()
public:
    UPROPERTY(EditAnywhere, BlueprintReadOnly) TObjectPtr<UTDFContaminationRules> Rules;
    UPROPERTY(BlueprintReadOnly, SaveGame) int32 CurrentAct=1;
    UFUNCTION(BlueprintCallable) void SetCampaignAct(int32 Act) {CurrentAct=FMath::Clamp(Act,1,5);}
    UFUNCTION(BlueprintCallable) FTDFOverdubResult TryOverdub(const FTDFRecordedSignal& Base,const FTDFRecordedSignal& Layer);
    UFUNCTION(BlueprintCallable) FTDFRecordedSignal ApplyContamination(const FTDFRecordedSignal& Signal,float Amount) const;
};
