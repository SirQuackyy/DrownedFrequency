#include "Acoustics/DFAcousticWorldSubsystem.h"
#include "Acoustics/DFAcousticReceiverComponent.h"
#include "GameFramework/Actor.h"
#include "Acoustics/DFAcousticCaptionSubsystem.h"
#include "Engine/GameInstance.h"
#include "Acoustics/DFAcousticDebug.h"
#include "Save/DFSaveSubsystem.h"
void UTDFAcousticWorldSubsystem::RegisterReceiver(UTDFAcousticReceiverComponent* R) {if(IsValid(R) && R->GetWorld()==GetWorld()) Receivers.AddUnique(R);}
void UTDFAcousticWorldSubsystem::UnregisterReceiver(UTDFAcousticReceiverComponent* R) {Receivers.Remove(R);}
bool UTDFAcousticWorldSubsystem::EmitStimulus(const FTDFAcousticStimulus& S)
{
    if (!S.IsValid() || DispatchDepth>=8) return false;
    if(auto* GI=GetWorld()->GetGameInstance()) if(auto* Saves=GI->GetSubsystem<UTDFSaveSubsystem>(); Saves && (Saves->IsLoading() || Saves->bStreamingTransitionActive)) return false;
    // Take a copy: a nested listener emission may reallocate the recent buffer.
    const FTDFAcousticStimulus Copy=S;
    if (Recent.Num()>=RecentLimit) Recent.RemoveAt(0);
    Recent.Add(Copy); TDFAcousticDebug::Draw(GetWorld(),Copy);
    TGuardValue<int32> Depth(DispatchDepth,DispatchDepth+1);
    if(auto* GI=GetWorld()->GetGameInstance()) if(auto* Captions=GI->GetSubsystem<UTDFAcousticCaptionSubsystem>()) Captions->PushCaptionsForStimulus(Copy);
    OnStimulusNative.Broadcast(Copy); OnStimulus.Broadcast(Copy);
    Receivers.RemoveAll([](const auto& R){return !R.IsValid();});
    const auto Snapshot=Receivers;
    for(const auto& Weak:Snapshot)
    {
        auto* R=Weak.Get();
        if(R && Receivers.Contains(Weak) && IsValid(R->GetOwner()) && FVector::DistSquared(R->GetOwner()->GetActorLocation(),Copy.WorldLocation)<=FMath::Square(Copy.Radius)) R->EvaluateStimulus(Copy);
    }
    return true;
}
