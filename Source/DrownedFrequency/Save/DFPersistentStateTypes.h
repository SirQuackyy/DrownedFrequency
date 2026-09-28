#pragma once

#include "CoreMinimal.h"
#include "DFPersistentStateTypes.generated.h"
USTRUCT(BlueprintType)
struct FTDFDoorPersistentState
{
    GENERATED_BODY()
    UPROPERTY(EditAnywhere, BlueprintReadWrite, SaveGame) FGuid StableActorId;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, SaveGame) bool bIsOpen=false;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, SaveGame) bool bIsLocked=false;
};
USTRUCT(BlueprintType)
struct FTDFSwitchPersistentState
{
    GENERATED_BODY()
    UPROPERTY(EditAnywhere, BlueprintReadWrite, SaveGame) FGuid StableActorId;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, SaveGame) bool bIsOn=false;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, SaveGame) bool bHasBeenUsed=false;
};
USTRUCT(BlueprintType)
struct FTDFWorldItemPersistentState
{
    GENERATED_BODY()
    UPROPERTY(EditAnywhere, BlueprintReadWrite, SaveGame) FGuid StableActorId;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, SaveGame) bool bHasBeenPickedUp=false;
};
