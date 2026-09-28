#pragma once
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Acoustics/DFAcousticStimulus.h"
#include "DFAcousticReceiverComponent.generated.h"
USTRUCT(BlueprintType)
struct DROWNEDFREQUENCY_API FTDFAcousticReceiverRule
{
    GENERATED_BODY()
    UPROPERTY(EditAnywhere, BlueprintReadOnly) FGameplayTag RequiredCategory;
    UPROPERTY(EditAnywhere, BlueprintReadOnly) FGameplayTagContainer RequiredReceiverKeys;
    UPROPERTY(EditAnywhere, BlueprintReadOnly) TSet<ETDFAcousticRoute> AcceptedRoutes={ETDFAcousticRoute::Direct};
    UPROPERTY(EditAnywhere, BlueprintReadOnly) ETDFAcousticFidelity MinimumFidelity=ETDFAcousticFidelity::Contaminated;
    UPROPERTY(EditAnywhere, BlueprintReadOnly) float MaxContamination=1.f;
    UPROPERTY(EditAnywhere, BlueprintReadOnly) int32 MaxAcceptedRepetitionIndex=999;
    UPROPERTY(EditAnywhere, BlueprintReadOnly) bool bConsumeOnce=true;
    UPROPERTY(EditAnywhere, BlueprintReadOnly) bool bRequirePuzzleEligible=true;
    UPROPERTY(EditAnywhere, BlueprintReadOnly) bool bAllowLedgerFallback=false;
};
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FTDFReceiverSatisfied,int32,RuleIndex);
UCLASS(ClassGroup=(Acoustics),meta=(BlueprintSpawnableComponent))
class DROWNEDFREQUENCY_API UTDFAcousticReceiverComponent : public UActorComponent
{
    GENERATED_BODY()
public:
    UPROPERTY(EditAnywhere, BlueprintReadOnly) TArray<FTDFAcousticReceiverRule> Rules;
    UPROPERTY(BlueprintAssignable) FTDFReceiverSatisfied OnRuleSatisfied;
    UPROPERTY(BlueprintReadOnly) FString LastEvaluation;
    UFUNCTION(BlueprintCallable) bool EvaluateStimulus(const FTDFAcousticStimulus& Stimulus);
    UFUNCTION(BlueprintCallable) bool TryLedgerFallback(class UTDFAcousticPuzzleLedgerComponent* Ledger,int32 Index);
    UFUNCTION(BlueprintPure) bool IsRuleSatisfied(int32 Index) const {return SatisfiedRules.Contains(Index);}
    UFUNCTION(BlueprintCallable) void ResetReceiver() {SatisfiedRules.Reset();}
protected:
    virtual void BeginPlay() override;
    virtual void EndPlay(const EEndPlayReason::Type Reason) override;
private:
    UPROPERTY(SaveGame) TSet<int32> SatisfiedRules;
};
