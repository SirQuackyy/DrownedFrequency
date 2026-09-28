#pragma once
#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Tickable.h"
#include "DFCheckpointSubsystem.generated.h"
UCLASS()
class DROWNEDFREQUENCY_API UTDFCheckpointSubsystem : public UGameInstanceSubsystem,public FTickableGameObject
{
    GENERATED_BODY()
public:
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    virtual void Deinitialize() override;
    virtual void Tick(float DeltaTime) override;
    virtual bool IsTickable() const override {return bInitialized && !IsTemplate();}
    virtual TStatId GetStatId() const override {RETURN_QUICK_DECLARE_CYCLE_STAT(UTDFCheckpointSubsystem,STATGROUP_Tickables);}
    virtual UWorld* GetTickableGameObjectWorld() const override {return GetWorld();}
    UFUNCTION(BlueprintCallable) void HandlePlayerDeath();
    UPROPERTY(BlueprintReadOnly) bool bRespawning=false;
    FName ManualFallbackSlot=TEXT("Manual0");
    // Automated fixtures opt out so they never read a player's unrelated manual slot.
    bool bSearchManualHistory=true;
    UFUNCTION(BlueprintCallable) bool ActivateCheckpoint(FName CheckpointId,bool bForceRewrite=false);
    UPROPERTY(BlueprintReadOnly) FName ActiveCheckpointId;
    UPROPERTY(BlueprintReadOnly) FTransform RespawnTransform=FTransform::Identity;
    UPROPERTY(BlueprintReadOnly) TArray<FName> RequiredSectors;
    UPROPERTY(BlueprintReadOnly) bool bAllowManualSave=true;
    FName CheckpointSlot=TEXT("Checkpoint");
    FName ChapterStartSlot=TEXT("ChapterStart");
    int32 SuccessfulCheckpointWrites=0;
private:
    UFUNCTION() void LoadFinished(FName Slot);
    UFUNCTION() void LoadFailed(FName Slot,FText Message);
    bool bInitialized=false;
    bool bNeedsNextLoad=false;
    bool bRestartingMap=false;
    double RetryAt=0;
    TArray<FName> Candidates;
    int32 CandidateIndex=0;
    TWeakObjectPtr<UWorld> RestartedWorld;
};
