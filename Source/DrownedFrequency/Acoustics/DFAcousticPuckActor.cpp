#include "Acoustics/DFAcousticPuckActor.h"
#include "Acoustics/DFRecorderPlaybackComponent.h"
#include "Components/StaticMeshComponent.h"
#include "TimerManager.h"
#include "Player/DFPlayerCharacter.h"
#include "Acoustics/DFRecorderOperatorComponent.h"
bool ATDFAcousticPuckActor::CanInteract_Implementation(const ATDFPlayerCharacter* Player) const {return Player && GetOwner()==Player;}
void ATDFAcousticPuckActor::Interact_Implementation(ATDFPlayerCharacter* Player) {if(Player && Player->RecorderOperator) Player->RecorderOperator->RecoverPuck(this);}
ATDFAcousticPuckActor::ATDFAcousticPuckActor()
{
    ProjectionOrigin=CreateDefaultSubobject<USceneComponent>(TEXT("ProjectionOrigin")); SetRootComponent(ProjectionOrigin);
    PuckMesh=CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PuckMesh")); PuckMesh->SetupAttachment(ProjectionOrigin);
    Playback=CreateDefaultSubobject<UTDFRecorderPlaybackComponent>(TEXT("Playback"));
}
bool ATDFAcousticPuckActor::ProjectRecordedSignal(UTDFRecorderComponent* Recorder,int32 SlotIndex)
{
    if(!FMath::IsFinite(MaxProjectionLifetime) || MaxProjectionLifetime<=0 || !Playback->PlayFrom(Recorder,SlotIndex,ProjectionOrigin->GetComponentLocation(),ETDFAcousticRoute::PuckProjected,true)) return false;
    GetWorldTimerManager().SetTimer(StopTimer,FTimerDelegate::CreateWeakLambda(this,[this]{Playback->StopPlayback();}),MaxProjectionLifetime,false);
    return true;
}
void ATDFAcousticPuckActor::EndPlay(const EEndPlayReason::Type Reason)
{
    if(GetWorld()) GetWorldTimerManager().ClearTimer(StopTimer); Playback->StopPlayback(); Super::EndPlay(Reason);
}
