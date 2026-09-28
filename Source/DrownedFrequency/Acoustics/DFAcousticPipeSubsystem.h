#pragma once
#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "Acoustics/DFAcousticStimulus.h"
#include "DFAcousticPipeSubsystem.generated.h"
UCLASS()
class DROWNEDFREQUENCY_API UTDFAcousticPipeSubsystem : public UWorldSubsystem
{
    GENERATED_BODY()
public:
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    virtual void Deinitialize() override;
    void RegisterNode(class ATDFAcousticPipeNode* Node);
    void UnregisterNode(class ATDFAcousticPipeNode* Node);
    UFUNCTION(BlueprintCallable) bool TryRouteThroughPipe(const FTDFAcousticStimulus& Input,FName NetworkId);
private:
    void HandleStimulus(const FTDFAcousticStimulus& Stimulus);
    TArray<TWeakObjectPtr<ATDFAcousticPipeNode>> Nodes;
    FDelegateHandle StimulusHandle;
};
