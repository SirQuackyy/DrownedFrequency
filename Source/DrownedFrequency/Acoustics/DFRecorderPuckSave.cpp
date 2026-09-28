#include "Acoustics/DFRecorderOperatorComponent.h"
#include "Acoustics/DFAcousticPuckActor.h"
#include "Save/DFSaveGame.h"
#include "Engine/World.h"
TArray<FTDFPuckSaveData> UTDFRecorderOperatorComponent::CapturePlacedPucks() const
{
    TArray<FTDFPuckSaveData> Out; for(const auto& Weak:PlacedPucks) if(const auto* P=Weak.Get()) {FTDFPuckSaveData S; S.InstanceId=P->PuckInstanceId; S.Transform=P->GetActorTransform(); Out.Add(S);} return Out;
}
void UTDFRecorderOperatorComponent::RestorePlacedPucks(const TArray<FTDFPuckSaveData>& Saved)
{
    RestorePucksAtSafeDesk(); if(!GetWorld() || !GetOwner()) return;
    TSet<FGuid> Restored;
    for(const auto& S:Saved)
    {
        // The GDD guarantees one usable puck after reload. Return any third puck to inventory.
        if(PlacedPucks.Num()>=2) break;
        if(!S.InstanceId.IsValid() || Restored.Contains(S.InstanceId) || S.Transform.ContainsNaN()) continue;
        FActorSpawnParameters Params; Params.Owner=GetOwner(); Params.SpawnCollisionHandlingOverride=ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
        if(auto* P=GetWorld()->SpawnActor<ATDFAcousticPuckActor>(PuckClass?PuckClass.Get():ATDFAcousticPuckActor::StaticClass(),S.Transform,Params)) {P->PuckInstanceId=S.InstanceId; PlacedPucks.Add(P); Restored.Add(S.InstanceId);}
    }
    // One-shot playback is transient: restoration never emits a second puzzle stimulus.
    Feedback(TEXT("Recorder and recoverable pucks restored."));
}
