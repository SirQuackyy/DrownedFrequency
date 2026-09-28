#pragma once
#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Save/DFSaveGame.h"
#include "DFSaveableActorInterface.generated.h"
UINTERFACE(BlueprintType)
class UTDFSaveableActorInterface : public UInterface {GENERATED_BODY()};
class DROWNEDFREQUENCY_API ITDFSaveableActorInterface
{
    GENERATED_BODY()
public:
    UFUNCTION(BlueprintNativeEvent,BlueprintCallable) FTDFActorSaveData CaptureSaveState() const;
    UFUNCTION(BlueprintNativeEvent,BlueprintCallable) bool ApplySaveState(const FTDFActorSaveData& State);
};
