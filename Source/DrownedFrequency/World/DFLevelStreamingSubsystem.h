#pragma once
#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Tickable.h"
#include "DFLevelStreamingSubsystem.generated.h"
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FTDFSectorSetReady);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FTDFSectorEvent,FName,SectorId);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FTDFSectorFailure,FText,Reason);
UCLASS()
class DROWNEDFREQUENCY_API UTDFLevelStreamingSubsystem : public UGameInstanceSubsystem,public FTickableGameObject
{
    GENERATED_BODY()
public:
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    virtual void Deinitialize() override;
    virtual void Tick(float DeltaTime) override;
    virtual bool IsTickable() const override {return bInitialized && !IsTemplate();}
    virtual TStatId GetStatId() const override {RETURN_QUICK_DECLARE_CYCLE_STAT(UTDFLevelStreamingSubsystem,STATGROUP_Tickables);}
    virtual UWorld* GetTickableGameObjectWorld() const override {return GetWorld();}
    UFUNCTION(BlueprintCallable) bool RequestSectorSet(const TArray<FName>& TargetSectorIds,FName Reason);
    UFUNCTION(BlueprintPure) TArray<FName> GetLoadedSectorIds() const {return Loaded.Array();}
    UPROPERTY(BlueprintReadOnly) bool bStreamingTransitionActive=false;
    UPROPERTY(BlueprintReadOnly) FText LastFailure;
    UPROPERTY(BlueprintAssignable) FTDFSectorSetReady OnSectorSetReady;
    UPROPERTY(BlueprintAssignable) FTDFSectorEvent OnSectorLoaded;
    UPROPERTY(BlueprintAssignable) FTDFSectorEvent OnSectorUnloaded;
    UPROPERTY(BlueprintAssignable) FTDFSectorFailure OnTransitionFailed;
    class UTDFSectorDataAsset* ResolveSector(FName Id) const;
    FName FindSectorForLevel(const class ULevel* Level) const;
    void CancelTransition();
#if WITH_DEV_AUTOMATION_TESTS
    void ExpireTransitionForTest() {Started=FPlatformTime::Seconds()-31.;}
#endif
private:
    class ULevelStreaming* FindStreaming(FName Id) const;
    void Finish(bool bSuccess,const FText& Message=FText());
    bool bInitialized=false;
    bool bUnloading=false;
    bool bWasVisibleLastTick=false;
    double Started=0;
    TSet<FName> Loaded;
    TSet<FName> Target;
    TSet<FName> Previous;
    TWeakObjectPtr<UWorld> ActiveWorld;
};
