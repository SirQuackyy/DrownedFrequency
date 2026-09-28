#include "Save/DFSaveSubsystem.h"
#include "Save/DFCheckpointSubsystem.h"
#include "Acoustics/DFRecorderOperatorComponent.h"
#include "Save/DFSaveMigrationLibrary.h"
#include "Save/DFSaveableActorInterface.h"
#include "World/DFLevelStreamingSubsystem.h"
#include "World/DFObjectiveSubsystem.h"
#include "Player/DFPlayerCharacter.h"
#include "Player/DFHealthComponent.h"
#include "Player/DFPlayerController.h"
#include "Inventory/DFInventoryComponent.h"
#include "Acoustics/DFRecorderPlaybackComponent.h"
#include "Acoustics/DFAcousticCaptionSubsystem.h"
#include "Engine/GameInstance.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "Misc/PackageName.h"
void UTDFSaveSubsystem::Initialize(FSubsystemCollectionBase& C) {Super::Initialize(C); bInitialized=true;}
void UTDFSaveSubsystem::Deinitialize() {bInitialized=false; PendingLoad=nullptr; RegisteredActors.Reset(); CachedActorStates.Reset(); Super::Deinitialize();}
bool UTDFSaveSubsystem::RequestLoadSlot(FName Slot)
{
    if(IsLoading() || bWriting || bStreamingTransitionActive) {LastMessage=NSLOCTEXT("Save","Busy","Wait for the current operation to finish."); OnLoadFailed.Broadcast(Slot,LastMessage); return false;}
    auto* Save=ReadObject(Slot,LastMessage);
    if(!Save) {OnLoadFailed.Broadcast(Slot,LastMessage); return false;}
    ETDFLoadFailure Failure; if(!UTDFSaveMigrationLibrary::Upgrade(Save,Failure,LastMessage)) {OnLoadFailed.Broadcast(Slot,LastMessage); return false;}
    if(!Save || Save->SaveVersion!=TDFSave::CurrentVersion || !Save->Player.Acoustics.IsValid() || Save->Player.Location.ContainsNaN() || Save->Player.Rotation.ContainsNaN() || !FPackageName::DoesPackageExist(Save->CurrentPersistentMap.ToString()))
    {if(Save) LastMessage=NSLOCTEXT("Save","Invalid","This save uses an unsupported version or unavailable map."); OnLoadFailed.Broadcast(Slot,LastMessage); return false;}
    PendingLoad=Save; LoadingSlot=Slot; bSectorsRequested=false; bRegistrationFramePassed=false; LoadStarted=FPlatformTime::Seconds();
    if(auto* W=GetWorld())
    {
        if(auto* PC=Cast<ATDFPlayerController>(W->GetFirstPlayerController())) PC->SetWorldTransitionLock(TEXT("SaveLoad"),true);
        FString Map=W->GetOutermost()->GetName(); if(!W->StreamingLevelsPrefix.IsEmpty()) Map=Map.Replace(*W->StreamingLevelsPrefix,TEXT(""));
        if(Map!=Save->CurrentPersistentMap.ToString()) UGameplayStatics::OpenLevel(W,Save->CurrentPersistentMap);
    }
    return true;
}
void UTDFSaveSubsystem::RestoreRegisteredActors(bool WarnMissing)
{
    for(const auto& Pair:CachedActorStates)
    {
        const auto* Weak=RegisteredActors.Find(Pair.Key); auto* A=Weak?Weak->Get():nullptr;
        if(A && A->GetWorld()==GetWorld() && A->Implements<UTDFSaveableActorInterface>()) {if(!ITDFSaveableActorInterface::Execute_ApplySaveState(A,Pair.Value)) UE_LOG(LogTemp,Warning,TEXT("TDF Save: actor %s rejected state %s"),*A->GetPathName(),*Pair.Key.ToString());}
        else if(WarnMissing && (Pair.Value.SectorId.IsNone() || LoadedSectorIds.Contains(Pair.Value.SectorId))) UE_LOG(LogTemp,Warning,TEXT("TDF Save: missing saved actor %s in sector %s; continuing."),*Pair.Key.ToString(),*Pair.Value.SectorId.ToString());
    }
}
void UTDFSaveSubsystem::FinishLoad(bool Success,const FText& Message)
{
    PendingLoad=nullptr; LastMessage=Message;
    if(auto* W=GetWorld()) if(auto* PC=Cast<ATDFPlayerController>(W->GetFirstPlayerController())) PC->SetWorldTransitionLock(TEXT("SaveLoad"),false);
    if(Success) OnLoadFinished.Broadcast(LoadingSlot); else OnLoadFailed.Broadcast(LoadingSlot,Message);
}
void UTDFSaveSubsystem::Tick(float)
{
    if(!PendingLoad) return;
    if(FPlatformTime::Seconds()-LoadStarted>45) {GetGameInstance()->GetSubsystem<UTDFLevelStreamingSubsystem>()->CancelTransition(); FinishLoad(false,NSLOCTEXT("Save","Timeout","Loading timed out. The save file was not changed.")); return;}
    auto* W=GetWorld(); if(!W) return;
    auto* PC=Cast<ATDFPlayerController>(W->GetFirstPlayerController()); auto* P=PC?Cast<ATDFPlayerCharacter>(PC->GetPawn()):nullptr; if(!P) return;
    PC->SetWorldTransitionLock(TEXT("SaveLoad"),true);
    FString Map=W->GetOutermost()->GetName(); if(!W->StreamingLevelsPrefix.IsEmpty()) Map=Map.Replace(*W->StreamingLevelsPrefix,TEXT("")); if(Map!=PendingLoad->CurrentPersistentMap.ToString()) return;
    auto* Streaming=GetGameInstance()->GetSubsystem<UTDFLevelStreamingSubsystem>();
    if(!bSectorsRequested)
    {
        CachedActorStates.Reset(); for(const auto& R:PendingLoad->Actors) if(R.StableActorId.IsValid()) CachedActorStates.Add(R.StableActorId,R);
        bSectorsRequested=true; if(!Streaming->RequestSectorSet(PendingLoad->LoadedSectorIds,TEXT("SaveLoad"))) FinishLoad(false,Streaming->LastFailure); return;
    }
    if(Streaming->bStreamingTransitionActive) return;
    if(!Streaming->LastFailure.IsEmpty()) {FinishLoad(false,Streaming->LastFailure); return;}
    if(!bRegistrationFramePassed) {bRegistrationFramePassed=true; return;}
    LoadedSectorIds=Streaming->GetLoadedSectorIds(); CurrentChapterId=PendingLoad->CurrentChapterId; CurrentCheckpointId=PendingLoad->CurrentCheckpointId; ObjectiveStates=PendingLoad->Objectives;
    // Newly loaded defaults must not replace the saved state during transition snapshots.
    CachedActorStates.Reset(); for(const auto& R:PendingLoad->Actors) if(R.StableActorId.IsValid()) CachedActorStates.Add(R.StableActorId,R);
    GetGameInstance()->GetSubsystem<UTDFObjectiveSubsystem>()->Restore(CurrentChapterId,ObjectiveStates);
    RestoreRegisteredActors(true); P->RecorderPlayback->StopPlayback();
    if(!PendingLoad->Player.Acoustics.Restore(P->Recorder,P->AcousticLedger)) {FinishLoad(false,NSLOCTEXT("Save","SignalsInvalid","The saved recorder data is invalid.")); return;}
    auto* CP=GetGameInstance()->GetSubsystem<UTDFCheckpointSubsystem>(); CP->ActiveCheckpointId=PendingLoad->CurrentCheckpointId; CP->RespawnTransform=PendingLoad->CheckpointRespawnTransform; CP->RequiredSectors=PendingLoad->CheckpointRequiredSectors; CP->bAllowManualSave=PendingLoad->bManualSaveAllowed;
    P->Health->DifficultyId=PendingLoad->Player.DifficultyId; P->Health->RestoreHealth(PendingLoad->Player.Health);
    P->RecorderOperator->RestorePlacedPucks(PendingLoad->Player.PlacedPucks);
    P->Inventory->RestoreInventory(PendingLoad->Player.ItemIds,PendingLoad->Player.KeyIds);
    P->SetActorLocationAndRotation(PendingLoad->Player.Location,FRotator(0,PendingLoad->Player.Rotation.Yaw,0),false,nullptr,ETeleportType::TeleportPhysics); PC->SetControlRotation(PendingLoad->Player.Rotation);
    if(auto* Captions=GetGameInstance()->GetSubsystem<UTDFAcousticCaptionSubsystem>()) Captions->Clear();
    FinishLoad(true,NSLOCTEXT("Save","Loaded","Loaded."));
}
