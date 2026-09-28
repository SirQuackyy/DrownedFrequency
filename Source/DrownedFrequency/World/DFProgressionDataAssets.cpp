#include "World/DFProgressionDataAssets.h"
#include "Misc/PackageName.h"
#include "Misc/DataValidation.h"
bool UTDFChapterDataAsset::HasValidDefinition() const {return !ChapterId.IsNone() && !DisplayName.IsEmpty() && !EntryCheckpointId.IsNone() && !ObjectiveIds.IsEmpty() && FPackageName::DoesPackageExist(PersistentMapName.ToString());}
bool UTDFChapterDataAsset::ValidateGraph(const TArray<UTDFChapterDataAsset*>& Chapters,const TArray<UTDFObjectiveDataAsset*>& Objectives,FText& Error)
{
    const auto Fail=[&](const TCHAR* Text){Error=FText::FromString(Text); return false;};
    TMap<FName,UTDFObjectiveDataAsset*> ById; TSet<FName> ChapterIds;
    for(auto* O:Objectives) {if(!O || !O->HasValidDefinition() || ById.Contains(O->ObjectiveId)) return Fail(TEXT("Invalid or duplicate objective ID.")); ById.Add(O->ObjectiveId,O);}
    for(const auto* C:Chapters)
    {
        if(!C || !C->HasValidDefinition() || ChapterIds.Contains(C->ChapterId)) return Fail(TEXT("Invalid or duplicate chapter ID.")); ChapterIds.Add(C->ChapterId); TSet<FName> Unique;
        for(const auto Id:C->ObjectiveIds) {auto* const* O=ById.Find(Id); if(!O || (*O)->ChapterId!=C->ChapterId || Unique.Contains(Id)) return Fail(TEXT("Chapter has a missing, duplicate or foreign objective.")); Unique.Add(Id);}
        for(const auto Id:C->ObjectiveIds) for(const auto Next:ById[Id]->NextObjectiveIds) if(!Unique.Contains(Next)) return Fail(TEXT("Objective points outside its chapter graph."));
    }
    for(const auto* O:Objectives) if(!ChapterIds.Contains(O->ChapterId)) return Fail(TEXT("Objective refers to a missing chapter."));
    TSet<FName> Visiting,Visited;
    TFunction<bool(FName)> Visit=[&](FName Id){if(Visiting.Contains(Id)) return false; if(Visited.Contains(Id)) return true; Visiting.Add(Id); for(const auto Next:ById[Id]->NextObjectiveIds) if(!ById.Contains(Next) || !Visit(Next)) return false; Visiting.Remove(Id); Visited.Add(Id); return true;};
    for(const auto& Pair:ById) if(!Visit(Pair.Key)) return Fail(TEXT("Objective graph contains a cycle or missing edge."));
    return true;
}
#if WITH_EDITOR
EDataValidationResult UTDFObjectiveDataAsset::IsDataValid(FDataValidationContext& C) const {if(!HasValidDefinition()) {C.AddError(NSLOCTEXT("Objective","Invalid","Objective needs stable IDs, authored text and valid next links.")); return EDataValidationResult::Invalid;} return EDataValidationResult::Valid;}
EDataValidationResult UTDFChapterDataAsset::IsDataValid(FDataValidationContext& C) const {if(!HasValidDefinition()) {C.AddError(NSLOCTEXT("Chapter","Invalid","Chapter needs a stable ID, existing map, entry checkpoint and objective IDs.")); return EDataValidationResult::Invalid;} return EDataValidationResult::Valid;}
#endif
