#include "World/DFLevelStreamingSubsystem.h"
#include "World/DFSectorDataAsset.h"
#include "Save/DFSaveSubsystem.h"
#include "Player/DFPlayerController.h"
#include "GameFramework/Pawn.h"
#include "Engine/AssetManager.h"
#include "Engine/World.h"
#include "Engine/GameInstance.h"
#include "Engine/LevelStreaming.h"
#include "Kismet/GameplayStatics.h"
void UTDFLevelStreamingSubsystem::Initialize(FSubsystemCollectionBase& C) {Super::Initialize(C); bInitialized=true; ActiveWorld=GetWorld();}
void UTDFLevelStreamingSubsystem::Deinitialize() {bInitialized=false; Loaded.Reset(); Target.Reset(); Previous.Reset(); Super::Deinitialize();}
UTDFSectorDataAsset* UTDFLevelStreamingSubsystem::ResolveSector(FName Id) const {return Cast<UTDFSectorDataAsset>(UAssetManager::Get().GetPrimaryAssetPath(FPrimaryAssetId(TEXT("TDFSector"),Id)).TryLoad());}
ULevelStreaming* UTDFLevelStreamingSubsystem::FindStreaming(FName Id) const {const auto* D=ResolveSector(Id); return D && GetWorld()?UGameplayStatics::GetStreamingLevel(GetWorld(),D->LevelPackageName):nullptr;}
FName UTDFLevelStreamingSubsystem::FindSectorForLevel(const ULevel* Level) const
{
    auto Ids=Loaded.Union(Target).Union(Previous); for(const auto Id:Ids) if(const auto* S=FindStreaming(Id); S && S->GetLoadedLevel()==Level) return Id; return NAME_None;
}
bool UTDFLevelStreamingSubsystem::RequestSectorSet(const TArray<FName>& Ids,FName Reason)
{
    if(!GetWorld()) return false;
    if(ActiveWorld.Get()!=GetWorld()) {Loaded.Reset(); Target.Reset(); Previous.Reset(); bStreamingTransitionActive=false; ActiveWorld=GetWorld();}
    TSet<FName> Next(Ids);
    for(const auto Id:Next) {const auto* D=ResolveSector(Id); if(!D || !D->HasValidDefinition() || !FindStreaming(Id)) {LastFailure=FText::Format(NSLOCTEXT("Sector","Unavailable","Sector {0} is not available in this map."),FText::FromName(Id)); UE_LOG(LogTemp,Warning,TEXT("TDF Streaming unavailable: id=%s definition=%d valid=%d streaming=%d levels=%d"),*Id.ToString(),D!=nullptr,D && D->HasValidDefinition(),FindStreaming(Id)!=nullptr,GetWorld()->GetStreamingLevels().Num()); OnTransitionFailed.Broadcast(LastFailure); return false;}}
    if(!bStreamingTransitionActive) {Previous=Loaded; if(auto* Saves=GetGameInstance()->GetSubsystem<UTDFSaveSubsystem>(); !Saves->IsLoading()) Saves->SnapshotRegisteredActors();} else Previous.Append(Target);
    for(const auto Id:Loaded) if(const auto* D=ResolveSector(Id); D && D->bKeepLoadedAfterVisit) Next.Add(Id);
    Target=MoveTemp(Next); bStreamingTransitionActive=true; bUnloading=false; bWasVisibleLastTick=false; Started=FPlatformTime::Seconds();
    if(auto* S=GetGameInstance()->GetSubsystem<UTDFSaveSubsystem>()) S->bStreamingTransitionActive=true;
    if(auto* PC=Cast<ATDFPlayerController>(GetWorld()->GetFirstPlayerController())) PC->SetWorldTransitionLock(TEXT("Streaming"),true);
    for(const auto Id:Target) if(auto* L=FindStreaming(Id)) {L->SetShouldBeLoaded(true); L->SetShouldBeVisible(true);}
    UE_LOG(LogTemp,Display,TEXT("TDF Streaming: requested %d sectors (%s)"),Target.Num(),*Reason.ToString()); return true;
}
void UTDFLevelStreamingSubsystem::Finish(bool Success,const FText& Message)
{
    bStreamingTransitionActive=false; bUnloading=false; bWasVisibleLastTick=false; LastFailure=Message;
    if(auto* S=GetGameInstance()->GetSubsystem<UTDFSaveSubsystem>()) {S->bStreamingTransitionActive=false; S->LoadedSectorIds=Loaded.Array(); if(Success) S->RestoreRegisteredActors(false);}
    if(auto* W=GetWorld()) if(auto* PC=Cast<ATDFPlayerController>(W->GetFirstPlayerController())) PC->SetWorldTransitionLock(TEXT("Streaming"),false);
    if(Success) OnSectorSetReady.Broadcast(); else OnTransitionFailed.Broadcast(Message);
}
void UTDFLevelStreamingSubsystem::CancelTransition()
{
    if(!bStreamingTransitionActive) return;
    // On timeout retain every visible floor; never unload the last safe room during recovery.
    TSet<FName> Retained;
    for(const auto Id:Loaded.Union(Previous).Union(Target)) if(auto* L=FindStreaming(Id))
    {
        const bool Keep=L->IsLevelVisible(); L->SetShouldBeLoaded(Keep); L->SetShouldBeVisible(Keep);
        if(Keep) Retained.Add(Id);
    }
    Loaded=MoveTemp(Retained);
    GetGameInstance()->GetSubsystem<UTDFSaveSubsystem>()->RestoreRegisteredActors(false);
    Target=Loaded; Previous=Loaded;
    Finish(false,NSLOCTEXT("Sector","Timeout","The room transition could not finish. Your current rooms remain available."));
}
void UTDFLevelStreamingSubsystem::Tick(float)
{
    if(!bStreamingTransitionActive) return;
    if(ActiveWorld.Get()!=GetWorld()) {Loaded.Reset(); Target.Reset(); Previous.Reset(); Finish(false,NSLOCTEXT("Sector","WorldChanged","The map changed during a room transition.")); return;}
    if(FPlatformTime::Seconds()-Started>30) {CancelTransition(); return;}
    if(!bUnloading)
    {
        for(const auto Id:Target) {auto* L=FindStreaming(Id); if(!L || !L->IsLevelVisible() || !L->GetLoadedLevel()) return;}
        if(!bWasVisibleLastTick) {bWasVisibleLastTick=true; return;}
        auto* Saves=GetGameInstance()->GetSubsystem<UTDFSaveSubsystem>();
        if(!Saves->IsLoading()) if(auto* PC=GetWorld()->GetFirstPlayerController()) if(APawn* P=PC->GetPawn())
        {
            FHitResult Hit; FCollisionQueryParams Q(SCENE_QUERY_STAT(SectorFloor),false,P);
            if(GetWorld()->LineTraceSingleByChannel(Hit,P->GetActorLocation(),P->GetActorLocation()-FVector(0,0,220),ECC_Visibility,Q) && Hit.GetActor())
            {
                const FName FloorSector=FindSectorForLevel(Hit.GetActor()->GetLevel());
                if(!FloorSector.IsNone() && !Target.Contains(FloorSector)) {CancelTransition(); return;}
            }
        }
        if(!Saves->IsLoading()) {Saves->RestoreRegisteredActors(false); Saves->SnapshotRegisteredActors();}
        for(const auto Id:Previous) if(!Target.Contains(Id)) if(auto* L=FindStreaming(Id)) {L->SetShouldBeVisible(false); L->SetShouldBeLoaded(false);}
        bUnloading=true; return;
    }
    for(const auto Id:Previous) if(!Target.Contains(Id)) if(auto* L=FindStreaming(Id); L && (L->IsLevelVisible() || L->IsLevelLoaded())) return;
    const auto Old=Loaded; Loaded=Target;
    for(const auto Id:Loaded) if(!Old.Contains(Id)) OnSectorLoaded.Broadcast(Id);
    for(const auto Id:Old) if(!Loaded.Contains(Id)) OnSectorUnloaded.Broadcast(Id);
    Finish(true);
}
