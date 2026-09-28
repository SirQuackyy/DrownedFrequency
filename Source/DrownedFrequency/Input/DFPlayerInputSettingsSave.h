#pragma once
#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "InputCoreTypes.h"
#include "DFPlayerInputSettingsSave.generated.h"
UCLASS()
class DROWNEDFREQUENCY_API UTDFPlayerInputSettingsSave : public USaveGame
{
    GENERATED_BODY()
public:
    UPROPERTY(SaveGame) TMap<FName,FKey> KeyboardOverrides;
    UPROPERTY(SaveGame) TMap<FName,FKey> GamepadOverrides;
};
