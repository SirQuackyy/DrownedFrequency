#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DFSectorVolume.generated.h"
UCLASS(Blueprintable)
class DROWNEDFREQUENCY_API ATDFSectorVolume : public AActor
{
    GENERATED_BODY()
public:
    ATDFSectorVolume();
    UPROPERTY(EditAnywhere,BlueprintReadOnly) TArray<FName> EnterSectorIds;
    UPROPERTY(EditAnywhere,BlueprintReadOnly) TArray<FName> ExitSectorIds;
    UPROPERTY(VisibleAnywhere) TObjectPtr<class UBoxComponent> Trigger;
protected:
    virtual void BeginPlay() override;
    UFUNCTION() void Overlap(UPrimitiveComponent* Component,AActor* Other,UPrimitiveComponent* OtherComponent,int32 BodyIndex,bool bFromSweep,const FHitResult& Hit);
};
UCLASS()
class DROWNEDFREQUENCY_API ATDFSaveTestBootstrap : public AActor
{
    GENERATED_BODY()
public:
    ATDFSaveTestBootstrap();
    virtual void Tick(float DeltaTime) override;
protected:
    virtual void BeginPlay() override;
private:
    bool bRequested=false;
};
