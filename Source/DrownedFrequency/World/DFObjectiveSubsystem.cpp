#include "World/DFObjectiveSubsystem.h"
#include "World/DFProgressionDataAssets.h"
#include "Save/DFSaveSubsystem.h"
#include "Save/DFCheckpointSubsystem.h"
#include "Save/DFCheckpointActor.h"
#include "World/DFLevelStreamingSubsystem.h"
#include "EngineUtils.h"
#include "Engine/AssetManager.h"
#include "Engine/GameInstance.h"
void UTDFObjectiveSubsystem::Initialize(FSubsystemCollectionBase& C) {C.InitializeDependency<UTDFSaveSubsystem>(); Super::Initialize(C); bInitialized=true;}
void UTDFObjectiveSubsystem::Deinitialize() {bInitialized=false; States.Reset(); Super::Deinitialize();}
UTDFObjectiveDataAsset* UTDFObjectiveSubsystem::ResolveObjective(FName Id) {return Cast<UTDFObjectiveDataAsset>(UAssetManager::Get().GetPrimaryAssetPath(FPrimaryAssetId(TEXT("TDFObjective"),Id)).TryLoad());}
UTDFChapterDataAsset* UTDFObjectiveSubsystem::ResolveChapter(FName Id) {return Cast<UTDFChapterDataAsset>(UAssetManager::Get().GetPrimaryAssetPath(FPrimaryAssetId(TEXT("TDFChapter"),Id)).TryLoad());}
ETDFObjectiveState UTDFObjectiveSubsystem::GetObjectiveState(FName Id) const {const auto* S=States.Find(Id); return S?S->State:ETDFObjectiveState::Locked;}
TArray<FTDFObjectiveSaveData> UTDFObjectiveSubsystem::Capture() const {TArray<FTDFObjectiveSaveData> Out; States.GenerateValueArray(Out); Out.Sort([](const auto& A,const auto& B){return A.ObjectiveId.LexicalLess(B.ObjectiveId);}); return Out;}
void UTDFObjectiveSubsystem::SyncSaveState() {auto* S=GetGameInstance()->GetSubsystem<UTDFSaveSubsystem>(); S->CurrentChapterId=CurrentChapterId; S->ObjectiveStates=Capture();}
void UTDFObjectiveSubsystem::Restore(FName ChapterId,const TArray<FTDFObjectiveSaveData>& Saved)
{
    CurrentChapterId=ChapterId; States.Reset(); bAutosavePending=false;
    for(const auto& R:Saved) if(!R.ObjectiveId.IsNone() && uint8(R.State)<=uint8(ETDFObjectiveState::Hidden)) States.Add(R.ObjectiveId,R);
    bChapterCompletionAnnounced=IsChapterComplete(); SyncSaveState(); for(const auto& Pair:States) OnObjectiveChanged.Broadcast(Pair.Key,Pair.Value.State);
    if(States.IsEmpty()) OnObjectiveChanged.Broadcast(NAME_None,ETDFObjectiveState::Hidden);
}
bool UTDFObjectiveSubsystem::StartChapter(FName Id)
{
    auto* C=ResolveChapter(Id); if(!C || (CurrentChapterId==Id && !States.IsEmpty())) return false;
    TArray<UTDFObjectiveDataAsset*> Definitions; for(const auto Objective:C->ObjectiveIds) Definitions.Add(ResolveObjective(Objective)); FText Error;
    if(!UTDFChapterDataAsset::ValidateGraph({C},Definitions,Error)) {UE_LOG(LogTemp,Warning,TEXT("TDF Objectives: %s"),*Error.ToString()); return false;}
    if(!GetGameInstance()->GetSubsystem<UTDFSaveSubsystem>()->CanSaveNow()) return false;
    if(!GetWorld()) return false;
    FString Map=GetWorld()->GetOutermost()->GetName(); if(!GetWorld()->StreamingLevelsPrefix.IsEmpty()) Map=Map.Replace(*GetWorld()->StreamingLevelsPrefix,TEXT(""));
    if(FName(*Map)!=C->PersistentMapName) return false;
    ATDFCheckpointActor* Entry=nullptr;
    for(TActorIterator<ATDFCheckpointActor> I(GetWorld());I;++I) if(I->CheckpointId==C->EntryCheckpointId) {if(Entry) return false; Entry=*I;}
    if(!Entry || Entry->ChapterId!=Id) return false;
    const auto Loaded=GetGameInstance()->GetSubsystem<UTDFLevelStreamingSubsystem>()->GetLoadedSectorIds(); for(const auto Sector:Entry->RequiredLoadedSectors) if(!Loaded.Contains(Sector)) return false;
    CurrentChapterId=Id; States.Reset(); bChapterCompletionAnnounced=false;
    for(int32 I=0;I<Definitions.Num();++I) {FTDFObjectiveSaveData R; R.ObjectiveId=Definitions[I]->ObjectiveId; R.TargetActorId=Definitions[I]->OptionalTargetActorId; R.State=I==0?ETDFObjectiveState::Active:ETDFObjectiveState::Locked; States.Add(R.ObjectiveId,R);}
    bChapterCompletionAnnounced=IsChapterComplete(); SyncSaveState(); for(const auto& Pair:States) OnObjectiveChanged.Broadcast(Pair.Key,Pair.Value.State);
    if(!GetGameInstance()->GetSubsystem<UTDFCheckpointSubsystem>()->ActivateCheckpoint(C->EntryCheckpointId)) bAutosavePending=true;
    return true;
}
bool UTDFObjectiveSubsystem::CompleteObjective(FName Id)
{
    auto* State=States.Find(Id); const auto* Definition=ResolveObjective(Id); if(!State || State->State!=ETDFObjectiveState::Active || !Definition) return false;
    State->State=ETDFObjectiveState::Completed;
    for(const auto Next:Definition->NextObjectiveIds) if(auto* R=States.Find(Next); R && R->State==ETDFObjectiveState::Locked) R->State=ETDFObjectiveState::Active;
    SyncSaveState(); OnObjectiveChanged.Broadcast(Id,ETDFObjectiveState::Completed); OnObjectiveCompleted.Broadcast(Id);
    for(const auto Next:Definition->NextObjectiveIds) OnObjectiveChanged.Broadcast(Next,GetObjectiveState(Next));
    if(IsChapterComplete() && !bChapterCompletionAnnounced) {bChapterCompletionAnnounced=true; OnChapterCompleted.Broadcast(CurrentChapterId);} bAutosavePending=true; return true;
}
bool UTDFObjectiveSubsystem::SetObjectiveTarget(FName Id,FGuid TargetId) {auto* S=States.Find(Id); if(!S || !TargetId.IsValid()) return false; S->TargetActorId=TargetId; SyncSaveState(); OnObjectiveChanged.Broadcast(Id,S->State); return true;}
bool UTDFObjectiveSubsystem::IsChapterComplete() const
{
    const auto* C=ResolveChapter(CurrentChapterId); if(!C) return false;
    for(const auto Id:C->ObjectiveIds) {const auto* O=ResolveObjective(Id); if(!O || (!O->bOptional && GetObjectiveState(Id)!=ETDFObjectiveState::Completed)) return false;} return true;
}
FText UTDFObjectiveSubsystem::GetActiveObjectiveText() const
{
    FString Text; for(const auto& R:Capture()) if(R.State==ETDFObjectiveState::Active) {if(!Text.IsEmpty()) Text+=TEXT("\n"); const auto* O=ResolveObjective(R.ObjectiveId); Text+=O?O->ShortText.ToString():TEXT("Objective unavailable");} return FText::FromString(Text);
}
void UTDFObjectiveSubsystem::Tick(float)
{
    if(!bAutosavePending) return; auto* S=GetGameInstance()->GetSubsystem<UTDFSaveSubsystem>();
    if(S->CanSaveNow())
    {
        bAutosavePending=false;
        const auto* CP=GetGameInstance()->GetSubsystem<UTDFCheckpointSubsystem>();
        // A safe progression boundary updates the actual death-recovery checkpoint.
        S->WriteSave(CP->ActiveCheckpointId.IsNone()?AutosaveSlot:CP->CheckpointSlot,ETDFSaveReason::Checkpoint);
    }
}
