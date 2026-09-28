#include "World/DFSectorVolume.h"
#include "World/DFLevelStreamingSubsystem.h"
#include "World/DFObjectiveSubsystem.h"
#include "Save/DFSaveSubsystem.h"
#include "Player/DFPlayerCharacter.h"
#include "Components/BoxComponent.h"
#include "Engine/World.h"
#include "Engine/GameInstance.h"
ATDFSectorVolume::ATDFSectorVolume() {Trigger=CreateDefaultSubobject<UBoxComponent>(TEXT("Trigger")); SetRootComponent(Trigger); Trigger->SetBoxExtent(FVector(80,130,120)); Trigger->SetCollisionProfileName(TEXT("Trigger"));}
void ATDFSectorVolume::BeginPlay() {Super::BeginPlay(); Trigger->OnComponentBeginOverlap.AddDynamic(this,&ThisClass::Overlap);}
void ATDFSectorVolume::Overlap(UPrimitiveComponent*,AActor* Other,UPrimitiveComponent*,int32,bool,const FHitResult&)
{
    if(!Cast<ATDFPlayerCharacter>(Other)) return; auto* S=GetGameInstance()->GetSubsystem<UTDFSaveSubsystem>(); if(S->IsLoading()) return;
    auto* L=GetGameInstance()->GetSubsystem<UTDFLevelStreamingSubsystem>(); auto Target=L->GetLoadedSectorIds(); for(const auto Id:ExitSectorIds) Target.Remove(Id); for(const auto Id:EnterSectorIds) Target.AddUnique(Id); L->RequestSectorSet(Target,TEXT("SectorVolume"));
}
ATDFSaveTestBootstrap::ATDFSaveTestBootstrap() {PrimaryActorTick.bCanEverTick=true;}
void ATDFSaveTestBootstrap::BeginPlay()
{
    Super::BeginPlay();
#if WITH_DEV_AUTOMATION_TESTS
    if(GIsAutomationTesting) {SetActorTickEnabled(false); return;}
#endif
    if(GetGameInstance()->GetSubsystem<UTDFSaveSubsystem>()->IsLoading()) SetActorTickEnabled(false);
}
void ATDFSaveTestBootstrap::Tick(float)
{
    auto* L=GetGameInstance()->GetSubsystem<UTDFLevelStreamingSubsystem>();
    if(!bRequested) {bRequested=L->RequestSectorSet({TEXT("Intro")},TEXT("ChapterStart")); return;}
    if(L->bStreamingTransitionActive) return;
    GetGameInstance()->GetSubsystem<UTDFObjectiveSubsystem>()->StartChapter(TEXT("QA_Archive")); SetActorTickEnabled(false);
}
