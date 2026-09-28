#pragma once
#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "DFSwitchReceiverInterface.generated.h"
UINTERFACE(BlueprintType)
class DROWNEDFREQUENCY_API UTDFSwitchReceiverInterface : public UInterface { GENERATED_BODY() };
class DROWNEDFREQUENCY_API ITDFSwitchReceiverInterface
{
    GENERATED_BODY()
public:
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Switch") void ReceiveSwitchState(AActor* Source, bool bOn);
};
