#pragma once
#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "DFStableActorInterface.generated.h"
UINTERFACE(BlueprintType)
class DROWNEDFREQUENCY_API UTDFStableActorInterface : public UInterface { GENERATED_BODY() };
class DROWNEDFREQUENCY_API ITDFStableActorInterface
{
    GENERATED_BODY()
public:
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Persistence") FGuid GetStableActorId() const;
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Persistence") void SetStableActorId(FGuid Id);
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Persistence") bool ShouldPersistInSave() const;
};
