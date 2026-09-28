#include "Save/DFCheckpointActor.h"
#include "Save/DFCheckpointSubsystem.h"
#include "Player/DFPlayerCharacter.h"
#include "Components/BoxComponent.h"
#include "Components/ArrowComponent.h"
#include "Engine/World.h"
#include "Engine/GameInstance.h"
#include "TimerManager.h"
ATDFCheckpointActor::ATDFCheckpointActor()
{
    Trigger=CreateDefaultSubobject<UBoxComponent>(TEXT("Trigger")); SetRootComponent(Trigger); Trigger->SetBoxExtent(FVector(90,120,110)); Trigger->SetCollisionProfileName(TEXT("Trigger"));
    RespawnArrow=CreateDefaultSubobject<UArrowComponent>(TEXT("RespawnArrow")); RespawnArrow->SetupAttachment(Trigger);
}
void ATDFCheckpointActor::OnConstruction(const FTransform& T) {Super::OnConstruction(T); if(RespawnArrow && !RespawnTransform.Equals(FTransform::Identity)) RespawnArrow->SetWorldTransform(RespawnTransform);}
void ATDFCheckpointActor::BeginPlay() {Super::BeginPlay(); Trigger->OnComponentBeginOverlap.AddDynamic(this,&ThisClass::Overlap);}
void ATDFCheckpointActor::Overlap(UPrimitiveComponent*,AActor* Other,UPrimitiveComponent*,int32,bool,const FHitResult&)
{
    if(!bAutoTrigger || !Cast<ATDFPlayerCharacter>(Other)) return; PendingPlayer=Cast<ATDFPlayerCharacter>(Other); RetryOverlap();
    if(PendingPlayer.IsValid()) GetWorldTimerManager().SetTimer(RetryTimer,this,&ThisClass::RetryOverlap,.25f,true);
}
void ATDFCheckpointActor::RetryOverlap()
{
    auto* C=GetGameInstance()->GetSubsystem<UTDFCheckpointSubsystem>();
    if(!PendingPlayer.IsValid() || !Trigger->IsOverlappingActor(PendingPlayer.Get()) || C->ActiveCheckpointId==CheckpointId || C->ActivateCheckpoint(CheckpointId)) {PendingPlayer.Reset(); GetWorldTimerManager().ClearTimer(RetryTimer);}
}
void ATDFCheckpointActor::EndPlay(const EEndPlayReason::Type R) {if(GetWorld()) GetWorldTimerManager().ClearTimer(RetryTimer); Super::EndPlay(R);}
