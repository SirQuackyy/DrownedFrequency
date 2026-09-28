#include "Acoustics/DFRecorderOperatorComponent.h"
#include "Acoustics/DFRecorderComponent.h"
#include "Acoustics/DFRecorderPlaybackComponent.h"
#include "Acoustics/DFAudioAnomalyActor.h"
#include "Acoustics/DFAcousticPuckActor.h"
#include "Player/DFPlayerCharacter.h"
#include "Player/DFPlayerController.h"
#include "EngineUtils.h"
#include "TimerManager.h"
bool UTDFRecorderOperatorComponent::CanOperate() const
{
    const auto* P=Cast<ATDFPlayerCharacter>(GetOwner()); const auto* PC=P?Cast<ATDFPlayerController>(P->GetController()):nullptr;
    return P && P->Recorder && PC && !PC->IsInspectingItem() && !PC->IsMoveInputIgnored();
}
void UTDFRecorderOperatorComponent::Feedback(const FString& Message) {StatusText=FText::FromString(Message); OnFeedback.Broadcast();}
void UTDFRecorderOperatorComponent::StartRecording()
{
    if(!CanOperate() || bRecording) return;
    auto* P=CastChecked<ATDFPlayerCharacter>(GetOwner()); ATDFAudioAnomalyActor* Best=nullptr; float BestDistance=MAX_flt;
    for(TActorIterator<ATDFAudioAnomalyActor> It(GetWorld());It;++It)
    {
        const float D=FVector::DistSquared(P->GetActorLocation(),It->GetActorLocation());
        if(D<BestDistance && It->CanBeRecordedBy(P)) {Best=*It; BestDistance=D;}
    }
    if(!Best) {Feedback(TEXT("No source in range. Face a sound source and move closer.")); return;}
    CaptureSource=Best; CaptureSlot=P->Recorder->SelectedSlot; bRecording=true; Feedback(TEXT("Recording... keep facing the source."));
    GetWorld()->GetTimerManager().SetTimer(CaptureTimer,this,&ThisClass::CompleteRecording,FMath::Max(.1f,CaptureSeconds),false);
}
void UTDFRecorderOperatorComponent::CancelRecording()
{
    GetWorld()->GetTimerManager().ClearTimer(CaptureTimer); CaptureSource.Reset();
    if(bRecording) {bRecording=false; Feedback(TEXT("Recording cancelled."));}
}
void UTDFRecorderOperatorComponent::CompleteRecording()
{
    auto* P=Cast<ATDFPlayerCharacter>(GetOwner());
    const bool Success=CanOperate() && P->Recorder->TryRecordAnomaly(CaptureSource.Get(),CaptureSlot);
    bRecording=false; CaptureSource.Reset(); Feedback(Success?TEXT("Signal captured. The source remains available to record again."):TEXT("Capture lost. Face the source and hold record again."));
}
void UTDFRecorderOperatorComponent::PlaySelected()
{
    if(!CanOperate()) return;
    auto* P=CastChecked<ATDFPlayerCharacter>(GetOwner()); FTDFRecorderPlaybackRequest R; R.SlotIndex=P->Recorder->SelectedSlot;
    Feedback(P->RecorderPlayback->PlayRecordedSlot(R)?TEXT("Local playback. The room can hear this."):TEXT("This slot has no playable signal."));
}
void UTDFRecorderOperatorComponent::DeleteSelected()
{
    if(!CanOperate()) return; CancelRecording();
    auto* P=CastChecked<ATDFPlayerCharacter>(GetOwner()); P->RecorderPlayback->StopPlayback();
    Feedback(P->Recorder->DeleteSlot(P->Recorder->SelectedSlot)?TEXT("Clip erased. Required sources can be recorded again."):TEXT("This slot is already empty."));
}
void UTDFRecorderOperatorComponent::NextSlot() {if(CanOperate()) {CancelRecording(); CastChecked<ATDFPlayerCharacter>(GetOwner())->Recorder->SelectNextSlot();}}
void UTDFRecorderOperatorComponent::PreviousSlot()
{
    if(!CanOperate()) return; CancelRecording(); auto* R=CastChecked<ATDFPlayerCharacter>(GetOwner())->Recorder.Get();
    R->SelectSlot((R->SelectedSlot+R->GetSlots().GetUnlockedSlotCount()-1)%R->GetSlots().GetUnlockedSlotCount());
}
int32 UTDFRecorderOperatorComponent::GetPucksAvailable() const
{
    int32 Active=0; for(const auto& P:PlacedPucks) if(P.IsValid()) ++Active; return FMath::Max(0,3-Active);
}
bool UTDFRecorderOperatorComponent::PlacePuck()
{
    if(!CanOperate() || !PuckClass) return false;
    auto* P=CastChecked<ATDFPlayerCharacter>(GetOwner());
    if(!P->Recorder->HasValidSignal(P->Recorder->SelectedSlot)) {Feedback(TEXT("Record a signal before placing a puck.")); return false;}
    if(GetPucksAvailable()==0) {Feedback(TEXT("Recover a deployed puck or recharge at a safe desk.")); return false;}
    FVector Eye; FRotator Facing; P->GetActorEyesViewPoint(Eye,Facing);
    FHitResult Hit; FCollisionQueryParams Params(SCENE_QUERY_STAT(PuckPlacement),false,P);
    FVector Position=Eye+Facing.Vector()*600;
    if(GetWorld()->LineTraceSingleByChannel(Hit,Eye,Position,ECC_Visibility,Params)) Position=Hit.ImpactPoint+Hit.ImpactNormal*15;
    if(!GetWorld()->LineTraceSingleByChannel(Hit,Position,Position-FVector(0,0,450),ECC_Visibility,Params) || Hit.ImpactNormal.Z<.6f) {Feedback(TEXT("Aim at a clear patch of floor.")); return false;}
    FActorSpawnParameters Spawn; Spawn.Owner=P; Spawn.SpawnCollisionHandlingOverride=ESpawnActorCollisionHandlingMethod::DontSpawnIfColliding;
    auto* Puck=GetWorld()->SpawnActor<ATDFAcousticPuckActor>(PuckClass,Hit.ImpactPoint+FVector(0,0,7),FRotator::ZeroRotator,Spawn);
    if(!Puck) return false;
    if(!Puck->ProjectRecordedSignal(P->Recorder,P->Recorder->SelectedSlot)) {Puck->Destroy(); Feedback(TEXT("Signal could not be projected.")); return false;}
    PlacedPucks.RemoveAll([](const auto& Item){return !Item.IsValid();}); PlacedPucks.Add(Puck); Feedback(TEXT("Puck transmitting. Walk up to it to recover it.")); return true;
}
bool UTDFRecorderOperatorComponent::RecoverPuck(ATDFAcousticPuckActor* Puck)
{
    if(!CanOperate() || !IsValid(Puck) || !PlacedPucks.Contains(Puck) || FVector::DistSquared(GetOwner()->GetActorLocation(),Puck->GetActorLocation())>FMath::Square(250.f)) return false;
    PlacedPucks.Remove(Puck); Puck->Destroy(); Feedback(TEXT("Puck recovered.")); return true;
}
void UTDFRecorderOperatorComponent::RestorePucksAtSafeDesk() {for(const auto& P:PlacedPucks) if(P.IsValid()) P->Destroy(); PlacedPucks.Reset(); Feedback(TEXT("Three pucks ready."));}
void UTDFRecorderOperatorComponent::EndPlay(const EEndPlayReason::Type Reason) {if(GetWorld()) GetWorld()->GetTimerManager().ClearTimer(CaptureTimer); Super::EndPlay(Reason);}
