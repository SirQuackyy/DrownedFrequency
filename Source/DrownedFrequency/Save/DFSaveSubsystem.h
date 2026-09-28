#pragma once
#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Save/DFSaveGame.h"
#include "Tickable.h"
#include "DFSaveSubsystem.generated.h"
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FTDFSaveEvent,FName,SlotName);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FTDFSaveFailure,FName,SlotName,FText,Message);
UCLASS()
class DROWNEDFREQUENCY_API UTDFSaveSubsystem : public UGameInstanceSubsystem,public FTickableGameObject
{
    GENERATED_BODY()
public:
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    virtual void Deinitialize() override;
    virtual void Tick(float DeltaTime) override;
    virtual bool IsTickable() const override {return bInitialized && !IsTemplate();}
    virtual TStatId GetStatId() const override {RETURN_QUICK_DECLARE_CYCLE_STAT(UTDFSaveSubsystem,STATGROUP_Tickables);}
    virtual UWorld* GetTickableGameObjectWorld() const override {return GetWorld();}
    UFUNCTION(BlueprintCallable) bool RequestLoadSlot(FName SlotName);
    UFUNCTION(BlueprintPure) bool IsLoading() const {return PendingLoad!=nullptr;}
    UPROPERTY(BlueprintAssignable) FTDFSaveEvent OnLoadFinished;
    UPROPERTY(BlueprintAssignable) FTDFSaveFailure OnLoadFailed;
    void RestoreRegisteredActors(bool bWarnMissing);
    bool RegisterStableActor(AActor* Actor, FGuid Id);
    void UnregisterStableActor(AActor* Actor, FGuid Id);
    UPROPERTY(BlueprintAssignable) FTDFSaveEvent OnSaveStarted;
    UPROPERTY(BlueprintAssignable) FTDFSaveEvent OnSaveFinished;
    UPROPERTY(BlueprintAssignable) FTDFSaveFailure OnSaveFailed;
    UPROPERTY(BlueprintReadOnly) FText LastMessage;
    UPROPERTY(BlueprintReadOnly) bool bWriting=false;
    bool bStreamingTransitionActive=false;
    bool bCutsceneLocked=false;
    bool bCombatLocked=false;
    UPROPERTY(BlueprintReadOnly) FName CurrentChapterId;
    UPROPERTY(BlueprintReadOnly) FName CurrentCheckpointId;
    TArray<FName> LoadedSectorIds;
    TArray<FTDFObjectiveSaveData> ObjectiveStates;
    UFUNCTION(BlueprintPure) bool CanSaveNow() const;
    UFUNCTION(BlueprintCallable) bool WriteSave(FName SlotName,ETDFSaveReason Reason);
    UTDFSaveGame* CaptureWorld(ETDFSaveReason Reason);
    static bool WriteObject(FName SlotName,UTDFSaveGame* Save,FText& Error);
    static UTDFSaveGame* ReadObject(FName SlotName,FText& Error);
    static FString SlotPath(FName SlotName);
    static FName FindNewestSlot(ETDFSaveReason Reason);
    void SnapshotRegisteredActors();
    TMap<FGuid,FTDFActorSaveData> CachedActorStates;
private:
    bool bInitialized=false;
    bool bSectorsRequested=false;
    bool bRegistrationFramePassed=false;
    double LoadStarted=0;
    FName LoadingSlot;
    UPROPERTY(Transient) TObjectPtr<UTDFSaveGame> PendingLoad;
    void FinishLoad(bool bSuccess,const FText& Message);
    TMap<FGuid,TWeakObjectPtr<AActor>> RegisteredActors;
};
