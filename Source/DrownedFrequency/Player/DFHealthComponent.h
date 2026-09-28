#pragma once
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "DFHealthComponent.generated.h"
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FTDFPlayerDeath);
UCLASS(ClassGroup=(Player),meta=(BlueprintSpawnableComponent))
class DROWNEDFREQUENCY_API UTDFHealthComponent : public UActorComponent
{
    GENERATED_BODY()
public:
    UPROPERTY(BlueprintReadOnly) float Health=100.f;
    UPROPERTY(BlueprintReadOnly) FName DifficultyId=TEXT("Standard");
    UPROPERTY(BlueprintAssignable) FTDFPlayerDeath OnDeath;
    UFUNCTION(BlueprintCallable) void ApplyDamage(float Amount);
    void RestoreHealth(float Amount) {Health=FMath::IsFinite(Amount)?FMath::Clamp(Amount,1.f,100.f):100.f;}
    UFUNCTION(BlueprintPure) bool IsDead() const {return Health<=0.f;}
    float GetRespawnHealth() const;
};
