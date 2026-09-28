#include "Save/DFSaveSubsystem.h"
#include "Save/DFCheckpointSubsystem.h"
#include "Acoustics/DFRecorderOperatorComponent.h"
#include "Save/DFSaveableActorInterface.h"
#include "Player/DFPlayerCharacter.h"
#include "Player/DFHealthComponent.h"
#include "GameFramework/PlayerController.h"
#include "Inventory/DFInventoryComponent.h"
#include "Engine/World.h"
#include "Engine/GameInstance.h"
#include "World/DFLevelStreamingSubsystem.h"
#include "Engine/Level.h"
#include "Kismet/GameplayStatics.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "HAL/FileManager.h"
FString UTDFSaveSubsystem::SlotPath(FName Slot)
{
    const FString Name=Slot.ToString(); if(Slot.IsNone() || Name.Len()>64) return {};
    for(const auto C:Name) if(!FChar::IsAlnum(C) && C!=TEXT('_') && C!=TEXT('-')) return {};
    return FPaths::ProjectSavedDir()/TEXT("SaveGames")/(TEXT("TDF_Progress_")+Name+TEXT(".sav"));
}
bool UTDFSaveSubsystem::WriteObject(FName Slot,UTDFSaveGame* Save,FText& Error)
{
    const FString Path=SlotPath(Slot); if(Path.IsEmpty() || !Save) {Error=NSLOCTEXT("Save","BadSlot","Choose a valid save slot."); return false;}
    TArray<uint8> Bytes; if(!UGameplayStatics::SaveGameToMemory(Save,Bytes)) {Error=NSLOCTEXT("Save","SerializeFailed","Could not prepare the save."); return false;}
    IFileManager::Get().MakeDirectory(*FPaths::GetPath(Path),true); const FString Temp=Path+TEXT(".tmp");
    if(!FFileHelper::SaveArrayToFile(Bytes,*Temp)) {Error=NSLOCTEXT("Save","WriteFailed","Could not write the save. Check available disk space."); return false;}
    TArray<uint8> Verify; if(!FFileHelper::LoadFileToArray(Verify,*Temp) || Verify!=Bytes || !Cast<UTDFSaveGame>(UGameplayStatics::LoadGameFromMemory(Verify))) {Error=NSLOCTEXT("Save","VerifyFailed","The temporary save could not be verified. Your previous save is unchanged."); return false;}
    if(!IFileManager::Get().Move(*Path,*Temp,true,true,false,true)) {Error=NSLOCTEXT("Save","ReplaceFailed","Could not replace the save. The temporary recovery file was kept."); return false;}
    return true;
}
UTDFSaveGame* UTDFSaveSubsystem::ReadObject(FName Slot,FText& Error)
{
    const FString Path=SlotPath(Slot); TArray<uint8> Bytes;
    if(Path.IsEmpty() || !FFileHelper::LoadFileToArray(Bytes,*Path)) {Error=NSLOCTEXT("Save","Missing","This save could not be found or read."); return nullptr;}
    if(auto* Save=Cast<UTDFSaveGame>(UGameplayStatics::LoadGameFromMemory(Bytes))) return Save;
    Error=NSLOCTEXT("Save","Corrupt","This save is damaged or incompatible."); return nullptr;
}
bool UTDFSaveSubsystem::CanSaveNow() const
{
    if(IsLoading() || bWriting || bStreamingTransitionActive || bCutsceneLocked || bCombatLocked) return false;
    if(auto* W=GetWorld()) if(auto* PC=W->GetFirstPlayerController()) if(auto* P=Cast<ATDFPlayerCharacter>(PC->GetPawn())) if(P->Health && P->Health->IsDead()) return false;
    return true;
}
void UTDFSaveSubsystem::SnapshotRegisteredActors()
{
    for(auto I=RegisteredActors.CreateIterator();I;++I)
    {
        auto* A=I.Value().Get(); if(!A) {I.RemoveCurrent(); continue;}
        if(A->GetWorld()!=GetWorld() || !A->Implements<UTDFSaveableActorInterface>()) continue;
        auto Record=ITDFSaveableActorInterface::Execute_CaptureSaveState(A); Record.SectorId=GetGameInstance()->GetSubsystem<UTDFLevelStreamingSubsystem>()->FindSectorForLevel(A->GetLevel()); if(Record.StableActorId==I.Key()) CachedActorStates.Add(I.Key(),MoveTemp(Record));
    }
}
UTDFSaveGame* UTDFSaveSubsystem::CaptureWorld(ETDFSaveReason Reason)
{
    auto* W=GetWorld(); auto* PC=W?W->GetFirstPlayerController():nullptr; auto* P=PC?Cast<ATDFPlayerCharacter>(PC->GetPawn()):nullptr; if(!P) return nullptr;
    auto* Save=NewObject<UTDFSaveGame>(this); Save->SavedAtUtc=FDateTime::UtcNow(); Save->Reason=Reason; Save->CurrentChapterId=CurrentChapterId; Save->CurrentCheckpointId=CurrentCheckpointId;
    FString Map=W->GetOutermost()->GetName(); if(!W->StreamingLevelsPrefix.IsEmpty()) Map=Map.Replace(*W->StreamingLevelsPrefix,TEXT("")); Save->CurrentPersistentMap=FName(*Map);
    Save->LoadedSectorIds=LoadedSectorIds; Save->Objectives=ObjectiveStates;
    Save->Player.Location=P->GetActorLocation(); Save->Player.Rotation=PC->GetControlRotation(); Save->Player.ItemIds=P->Inventory->GetOwnedItemIds(); Save->Player.KeyIds=P->Inventory->GetOwnedKeyIds();
    Save->Player.Health=P->Health->Health; Save->Player.DifficultyId=P->Health->DifficultyId; Save->Settings.DifficultyId=P->Health->DifficultyId;
    Save->Player.Acoustics=FTDFAcousticSaveData::Capture(P->Recorder,P->AcousticLedger); Save->Player.PlacedPucks=P->RecorderOperator->CapturePlacedPucks();
    if(auto* CP=GetGameInstance()->GetSubsystem<UTDFCheckpointSubsystem>())
    {
        Save->CheckpointRespawnTransform=CP->RespawnTransform; Save->CheckpointRequiredSectors=CP->RequiredSectors; Save->bManualSaveAllowed=CP->bAllowManualSave;
        if((Reason==ETDFSaveReason::Checkpoint || Reason==ETDFSaveReason::ChapterStart) && !CP->ActiveCheckpointId.IsNone())
        {
            Save->Player.Location=CP->RespawnTransform.GetLocation(); Save->Player.Rotation=CP->RespawnTransform.Rotator();
            // Objective autosaves can happen after leaving the checkpoint's sector.
            for(const FName Sector:CP->RequiredSectors) Save->LoadedSectorIds.AddUnique(Sector);
        }
    }
    SnapshotRegisteredActors(); CachedActorStates.GenerateValueArray(Save->Actors); return Save;
}
bool UTDFSaveSubsystem::WriteSave(FName Slot,ETDFSaveReason Reason)
{
    if(!CanSaveNow() || (Reason==ETDFSaveReason::Manual && !GetGameInstance()->GetSubsystem<UTDFCheckpointSubsystem>()->bAllowManualSave)) {LastMessage=NSLOCTEXT("Save","Unsafe","Wait until the current transition or danger has passed before saving."); OnSaveFailed.Broadcast(Slot,LastMessage); return false;}
    TGuardValue<bool> Writing(bWriting,true); OnSaveStarted.Broadcast(Slot);
    auto* Save=CaptureWorld(Reason); if(!Save) {LastMessage=NSLOCTEXT("Save","NoPlayer","A possessed player is required to save."); OnSaveFailed.Broadcast(Slot,LastMessage); return false;}
    if(!WriteObject(Slot,Save,LastMessage)) {OnSaveFailed.Broadcast(Slot,LastMessage); return false;}
    if(Reason==ETDFSaveReason::Manual) GetGameInstance()->GetSubsystem<UTDFCheckpointSubsystem>()->ManualFallbackSlot=Slot;
    LastMessage=NSLOCTEXT("Save","Saved","Saved."); OnSaveFinished.Broadcast(Slot); return true;
}



FName UTDFSaveSubsystem::FindNewestSlot(ETDFSaveReason Reason)
{
    TArray<FString> Files; const FString Directory=FPaths::ProjectSavedDir()/TEXT("SaveGames"); IFileManager::Get().FindFiles(Files,*(Directory/TEXT("TDF_Progress_*.sav")),true,false);
    FDateTime Newest=FDateTime::MinValue(); FName Result;
    for(const auto& File:Files)
    {
        const FString Name=FPaths::GetBaseFilename(File).RightChop(13); if(Name.StartsWith(TEXT("QA_"))) continue;
        FText Error; if(auto* S=ReadObject(FName(*Name),Error); S && S->Reason==Reason && S->SavedAtUtc>Newest) {Newest=S->SavedAtUtc; Result=FName(*Name);}
    }
    return Result;
}
