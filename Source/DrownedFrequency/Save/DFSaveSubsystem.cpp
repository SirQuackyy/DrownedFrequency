#include "Save/DFSaveSubsystem.h"
#include "GameFramework/Actor.h"
#include "Engine/Level.h"
bool UTDFSaveSubsystem::RegisterStableActor(AActor* Actor, FGuid Id)
{
    if (!IsValid(Actor) || !Id.IsValid()) return false;
    const TWeakObjectPtr<AActor>* Existing=RegisteredActors.Find(Id);
    if (Existing && Existing->IsValid() && Existing->Get()!=Actor)
    {
        UE_LOG(LogTemp,Error,TEXT("TDF Persistence: duplicate stable actor ID %s (%s in sector/level %s and %s in sector/level %s)."),*Id.ToString(),*Existing->Get()->GetName(),*Existing->Get()->GetLevel()->GetOutermost()->GetName(),*Actor->GetName(),*Actor->GetLevel()->GetOutermost()->GetName());
        return false;
    }
    RegisteredActors.Add(Id,Actor);
    return true;
}
void UTDFSaveSubsystem::UnregisterStableActor(AActor* Actor, FGuid Id)
{
    if (const auto* Existing=RegisteredActors.Find(Id); Existing && Existing->Get()==Actor) RegisteredActors.Remove(Id);
}
