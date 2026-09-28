#include "Save/DFCheckpointSubsystem.h"
#include "Save/DFSaveSubsystem.h"
#include "World/DFObjectiveSubsystem.h"
#include "Player/DFPlayerCharacter.h"
#include "Player/DFPlayerController.h"
#include "Player/DFHealthComponent.h"
#include "Engine/GameInstance.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
void UTDFCheckpointSubsystem::Initialize(FSubsystemCollectionBase& C)
{
    C.InitializeDependency<UTDFSaveSubsystem>(); Super::Initialize(C); bInitialized=true;
    auto* S=GetGameInstance()->GetSubsystem<UTDFSaveSubsystem>(); S->OnLoadFinished.AddDynamic(this,&ThisClass::LoadFinished); S->OnLoadFailed.AddDynamic(this,&ThisClass::LoadFailed);
}
void UTDFCheckpointSubsystem::Deinitialize()
{
    bInitialized=false; if(auto* S=GetGameInstance()->GetSubsystem<UTDFSaveSubsystem>()) {S->OnLoadFinished.RemoveDynamic(this,&ThisClass::LoadFinished); S->OnLoadFailed.RemoveDynamic(this,&ThisClass::LoadFailed);} Super::Deinitialize();
}
void UTDFCheckpointSubsystem::HandlePlayerDeath()
{
    if(bRespawning || !GetWorld()) return; bRespawning=true; bNeedsNextLoad=true; CandidateIndex=0; RetryAt=FPlatformTime::Seconds()+.35;
    Candidates.Reset(); Candidates.Add(CheckpointSlot); Candidates.AddUnique(ManualFallbackSlot);
    if(bSearchManualHistory) {const FName Latest=UTDFSaveSubsystem::FindNewestSlot(ETDFSaveReason::Manual); if(!Latest.IsNone()) Candidates.AddUnique(Latest);} Candidates.AddUnique(ChapterStartSlot);
    if(auto* PC=Cast<ATDFPlayerController>(GetWorld()->GetFirstPlayerController())) PC->SetWorldTransitionLock(TEXT("Death"),true);
}
void UTDFCheckpointSubsystem::LoadFinished(FName)
{
    if(!bRespawning) return;
    if(auto* W=GetWorld()) if(auto* PC=Cast<ATDFPlayerController>(W->GetFirstPlayerController()))
    {
        if(auto* P=Cast<ATDFPlayerCharacter>(PC->GetPawn())) P->Health->RestoreHealth(P->Health->GetRespawnHealth());
        PC->SetWorldTransitionLock(TEXT("Death"),false);
    }
    auto* S=GetGameInstance()->GetSubsystem<UTDFSaveSubsystem>(); S->bCombatLocked=false; S->bCutsceneLocked=false; bRespawning=false; bNeedsNextLoad=false; bRestartingMap=false; RestartedWorld.Reset();
}
void UTDFCheckpointSubsystem::LoadFailed(FName,FText) {if(bRespawning) {bNeedsNextLoad=true; RetryAt=FPlatformTime::Seconds();}}
void UTDFCheckpointSubsystem::Tick(float)
{
    if(!bRespawning) return;
    if(bRestartingMap) {if(RestartedWorld.Get()!=GetWorld() && GetWorld() && GetWorld()->GetFirstPlayerController()) LoadFinished(NAME_None); return;}
    if(!bNeedsNextLoad || FPlatformTime::Seconds()<RetryAt) return;
    auto* S=GetGameInstance()->GetSubsystem<UTDFSaveSubsystem>(); if(S->IsLoading() || S->bStreamingTransitionActive) return;
    bNeedsNextLoad=false;
    if(Candidates.IsValidIndex(CandidateIndex)) {if(!S->RequestLoadSlot(Candidates[CandidateIndex++])) bNeedsNextLoad=true; return;}
    // No usable checkpoint/manual/chapter file: restart the authored chapter map safely.
    auto* W=GetWorld(); if(!W) {bNeedsNextLoad=true; return;}
    FString Map=W->GetOutermost()->GetName(); if(!W->StreamingLevelsPrefix.IsEmpty()) Map=Map.Replace(*W->StreamingLevelsPrefix,TEXT(""));
    ActiveCheckpointId=NAME_None; S->CurrentCheckpointId=NAME_None; S->CurrentChapterId=NAME_None; S->ObjectiveStates.Reset(); S->CachedActorStates.Reset(); S->LoadedSectorIds.Reset(); RestartedWorld=W; bRestartingMap=true;
    RequiredSectors.Reset(); RespawnTransform=FTransform::Identity; bAllowManualSave=true;
    GetGameInstance()->GetSubsystem<UTDFObjectiveSubsystem>()->Restore(NAME_None,{});
    UGameplayStatics::OpenLevel(W,FName(*Map));
}
