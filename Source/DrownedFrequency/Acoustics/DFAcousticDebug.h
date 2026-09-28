#pragma once
#include "CoreMinimal.h"
#include "Acoustics/DFAcousticStimulus.h"
DECLARE_LOG_CATEGORY_EXTERN(LogAcoustics,Log,All);
DECLARE_LOG_CATEGORY_EXTERN(LogAcousticReceiver,Log,All);
DECLARE_LOG_CATEGORY_EXTERN(LogRecorder,Log,All);
namespace TDFAcousticDebug
{
    FString Describe(const FTDFAcousticStimulus& S);
    FTDFAcousticStimulus MakeTest(FGameplayTag Category,float Radius,FVector Location);
    void Draw(UWorld* World,const FTDFAcousticStimulus& S);
    int32 ValidateAssets();
}
