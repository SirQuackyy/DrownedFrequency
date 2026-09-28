#pragma once
#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Styling/SlateColor.h"
#include "Acoustics/DFRecordedSignal.h"
#include "DFAcousticBlueprintLibrary.generated.h"
UCLASS()
class DROWNEDFREQUENCY_API UTDFAcousticBlueprintLibrary : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()
public:
    UFUNCTION(BlueprintPure) static FText GetSignalDisplayName(const FTDFRecordedSignal& Signal);
    UFUNCTION(BlueprintPure) static FSlateColor GetCategoryColor(FGameplayTag CategoryTag);
    UFUNCTION(BlueprintPure) static bool IsSignalPuzzleEligible(const FTDFRecordedSignal& Signal) {return Signal.CanSatisfyReceiver();}
    UFUNCTION(BlueprintPure) static FText GetRouteDisplayName(ETDFAcousticRoute Route);
};
