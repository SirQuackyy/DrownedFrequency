#pragma once
#include "CoreMinimal.h"
#include "Acoustics/DFAcousticStimulus.h"
class UAudioComponent;
namespace TDFAcoustics
{
    DROWNEDFREQUENCY_API bool PresentStimulus(UAudioComponent* Audio,const FTDFAcousticStimulus& Stimulus);
}
