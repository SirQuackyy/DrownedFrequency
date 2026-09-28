#include "Save/DFSaveSubsystem.h"
#include "Save/DFStableActorComponent.h"
#include "World/DFLevelStreamingSubsystem.h"
#include "Engine/World.h"
#include "Engine/GameInstance.h"
#include "Engine/AssetManager.h"
#include "Engine/Level.h"
#include "EngineUtils.h"
#include "HAL/IConsoleManager.h"
#include "Containers/Ticker.h"
#if !UE_BUILD_SHIPPING
namespace
{
void ValidateWorldIds(UWorld* W)
{
    TMap<FGuid,FString> Seen; int32 Invalid=0;
    for(TActorIterator<AActor> I(W);I;++I) if(const auto* C=I->FindComponentByClass<UTDFStableActorComponent>(); C && C->ShouldPersistInSave())
    {
        const FString Context=I->GetName()+TEXT(" in sector/level ")+I->GetLevel()->GetOutermost()->GetName();
        if(!C->StableActorId.IsValid() || Seen.Contains(C->StableActorId)) {++Invalid; UE_LOG(LogTemp,Error,TEXT("TDF Stable ID validation: invalid/duplicate ID %s: %s"),*C->StableActorId.ToString(),*Context);} else Seen.Add(C->StableActorId,Context);
    }
    UE_LOG(LogTemp,Display,TEXT("TDF Stable ID validation: %d actors, %d errors"),Seen.Num(),Invalid);
}
FAutoConsoleCommandWithWorldAndArgs SaveCommand(TEXT("tdf.SaveSlot"),TEXT("SaveSlot SlotName"),FConsoleCommandWithWorldAndArgsDelegate::CreateLambda([](const TArray<FString>& Args,UWorld* W){if(W && W->GetGameInstance()) W->GetGameInstance()->GetSubsystem<UTDFSaveSubsystem>()->WriteSave(Args.IsEmpty()?FName(TEXT("Manual0")):FName(*Args[0]),ETDFSaveReason::Manual);}));
FAutoConsoleCommandWithWorldAndArgs LoadCommand(TEXT("tdf.LoadSlot"),TEXT("LoadSlot SlotName"),FConsoleCommandWithWorldAndArgsDelegate::CreateLambda([](const TArray<FString>& Args,UWorld* W){if(W && W->GetGameInstance()) W->GetGameInstance()->GetSubsystem<UTDFSaveSubsystem>()->RequestLoadSlot(Args.IsEmpty()?FName(TEXT("Manual0")):FName(*Args[0]));}));
FAutoConsoleCommand VersionCommand(TEXT("tdf.DumpSaveVersion"),TEXT("DumpSaveVersion SlotName"),FConsoleCommandWithArgsDelegate::CreateLambda([](const TArray<FString>& Args){if(Args.IsEmpty()) return; FText Error; if(auto* S=UTDFSaveSubsystem::ReadObject(FName(*Args[0]),Error)) {UE_LOG(LogTemp,Display,TEXT("TDF Save version: %d, current %d, build %s"),S->SaveVersion,TDFSave::CurrentVersion,*S->BuildVersion);} else {UE_LOG(LogTemp,Display,TEXT("%s"),*Error.ToString());}}));
FAutoConsoleCommandWithWorldAndArgs IdCommand(TEXT("tdf.ValidateStableIds"),TEXT("ValidateStableIds [AllSectors]"),FConsoleCommandWithWorldAndArgsDelegate::CreateLambda([](const TArray<FString>& Args,UWorld* W)
{
    if(!W) return; if(!Args.Contains(TEXT("AllSectors")) || !W->GetGameInstance()) {ValidateWorldIds(W); return;}
    TArray<FPrimaryAssetId> Definitions; UAssetManager::Get().GetPrimaryAssetIdList(TEXT("TDFSector"),Definitions); TArray<FName> Ids; for(const auto& Id:Definitions) Ids.Add(Id.PrimaryAssetName);
    if(!W->GetGameInstance()->GetSubsystem<UTDFLevelStreamingSubsystem>()->RequestSectorSet(Ids,TEXT("ValidateStableIds"))) return;
    TWeakObjectPtr<UWorld> Weak=W; const double Expires=FPlatformTime::Seconds()+35;
    FTSTicker::GetCoreTicker().AddTicker(FTickerDelegate::CreateLambda([Weak,Expires](float)
    {
        if(!Weak.IsValid() || !Weak->GetGameInstance()) return false;
        if(Weak->GetGameInstance()->GetSubsystem<UTDFLevelStreamingSubsystem>()->bStreamingTransitionActive) return FPlatformTime::Seconds()<Expires;
        ValidateWorldIds(Weak.Get()); return false;
    }),.1f);
}));
}
#endif
