#pragma once
#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Save/DFSaveGame.h"
#include "DFSaveMigrationLibrary.generated.h"
UENUM(BlueprintType)
enum class ETDFLoadFailure:uint8 {None,InvalidSave,SaveFromNewerBuild};
UCLASS()
class DROWNEDFREQUENCY_API UTDFSaveMigrationLibrary : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()
public:
    static bool Upgrade(UTDFSaveGame* Save,ETDFLoadFailure& Failure,FText& Message);
};
