#include "Save/DFCheckpointSubsystem.h"
#include "Save/DFCheckpointActor.h"
#include "Save/DFSaveSubsystem.h"
#include "World/DFLevelStreamingSubsystem.h"
#include "Engine/GameInstance.h"
#include "EngineUtils.h"
bool UTDFCheckpointSubsystem::ActivateCheckpoint(FName Id,bool Force)
{
    auto* Saves=GetGameInstance()->GetSubsystem<UTDFSaveSubsystem>(); if(Id.IsNone() || (!Force && ActiveCheckpointId==Id) || !GetWorld() || !Saves->CanSaveNow()) return false;
    ATDFCheckpointActor* Found=nullptr; for(TActorIterator<ATDFCheckpointActor> I(GetWorld());I;++I) if(I->CheckpointId==Id) {if(Found) {UE_LOG(LogTemp,Error,TEXT("TDF Checkpoint: duplicate ID %s"),*Id.ToString()); return false;} Found=*I;}
    if(!Found || Found->RespawnTransform.ContainsNaN()) return false;
    const auto Loaded=GetGameInstance()->GetSubsystem<UTDFLevelStreamingSubsystem>()->GetLoadedSectorIds(); for(const auto Sector:Found->RequiredLoadedSectors) if(!Loaded.Contains(Sector)) return false;
    const auto PreviousId=ActiveCheckpointId,PreviousChapter=Saves->CurrentChapterId; const auto PreviousTransform=RespawnTransform; const auto PreviousSectors=RequiredSectors; const bool PreviousManual=bAllowManualSave;
    ActiveCheckpointId=Id; Saves->CurrentCheckpointId=Id; Saves->CurrentChapterId=Found->ChapterId; RespawnTransform=Found->RespawnTransform; RequiredSectors=Found->RequiredLoadedSectors; bAllowManualSave=Found->bAllowManualSaveAfter;
    if(!Saves->WriteSave(CheckpointSlot,ETDFSaveReason::Checkpoint)) {ActiveCheckpointId=PreviousId; Saves->CurrentCheckpointId=PreviousId; Saves->CurrentChapterId=PreviousChapter; RespawnTransform=PreviousTransform; RequiredSectors=PreviousSectors; bAllowManualSave=PreviousManual; return false;}
    ++SuccessfulCheckpointWrites;
    if(Found->bChapterStart) Saves->WriteSave(ChapterStartSlot,ETDFSaveReason::ChapterStart);
    return true;
}
