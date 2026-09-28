#pragma once
#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Tickable.h"
#include "Save/DFSaveGame.h"
#include "DFObjectiveSubsystem.generated.h"
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FTDFObjectiveChanged,FName,ObjectiveId,ETDFObjectiveState,State);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FTDFObjectiveEvent,FName,ObjectiveId);
UCLASS()
class DROWNEDFREQUENCY_API UTDFObjectiveSubsystem : public UGameInstanceSubsystem,public FTickableGameObject
{
    GENERATED_BODY()
public:
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    virtual void Deinitialize() override;
    virtual void Tick(float DeltaTime) override;
    virtual bool IsTickable() const override {return bInitialized && !IsTemplate();}
    virtual TStatId GetStatId() const override {RETURN_QUICK_DECLARE_CYCLE_STAT(UTDFObjectiveSubsystem,STATGROUP_Tickables);}
    virtual UWorld* GetTickableGameObjectWorld() const override {return GetWorld();}
    UFUNCTION(BlueprintCallable) bool StartChapter(FName ChapterId);
    UFUNCTION(BlueprintCallable) bool CompleteObjective(FName ObjectiveId);
    UFUNCTION(BlueprintCallable) bool SetObjectiveTarget(FName ObjectiveId,FGuid TargetId);
    UFUNCTION(BlueprintPure) ETDFObjectiveState GetObjectiveState(FName ObjectiveId) const;
    UFUNCTION(BlueprintPure) FText GetActiveObjectiveText() const;
    UFUNCTION(BlueprintPure) bool IsChapterComplete() const;
    UPROPERTY(BlueprintReadOnly) FName CurrentChapterId;
    UPROPERTY(BlueprintAssignable) FTDFObjectiveChanged OnObjectiveChanged;
    UPROPERTY(BlueprintAssignable) FTDFObjectiveEvent OnObjectiveCompleted;
    UPROPERTY(BlueprintAssignable) FTDFObjectiveEvent OnChapterCompleted;
    FName AutosaveSlot=TEXT("Autosave");
    TArray<FTDFObjectiveSaveData> Capture() const;
    void Restore(FName ChapterId,const TArray<FTDFObjectiveSaveData>& Saved);
    static class UTDFObjectiveDataAsset* ResolveObjective(FName Id);
    static class UTDFChapterDataAsset* ResolveChapter(FName Id);
private:
    void SyncSaveState();
    UPROPERTY(Transient) TMap<FName,FTDFObjectiveSaveData> States;
    bool bInitialized=false;
    bool bAutosavePending=false;
    bool bChapterCompletionAnnounced=false;
};
