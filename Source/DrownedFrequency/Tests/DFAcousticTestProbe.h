#pragma once
#include "CoreMinimal.h"
#include "Acoustics/DFAcousticStimulus.h"
#include "DFAcousticTestProbe.generated.h"
/** Reflection delegate target used by unattended acoustic contract tests. */
UCLASS(Transient)
class UTDFAcousticTestProbe : public UObject
{
    GENERATED_BODY()
public:
    int32 Stimuli=0;
    int32 Mutations=0;
    int32 Satisfied=0;
    UFUNCTION() void ReceiveStimulus(const FTDFAcousticStimulus& Stimulus) {++Stimuli;}
    UFUNCTION() void ReceiveMutation() {++Mutations;}
    UFUNCTION() void ReceiveRule(int32 Index) {++Satisfied;}
};
