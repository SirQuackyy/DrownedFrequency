#include "Save/DFPersistentActor.h"
#include "World/DFObjectiveSubsystem.h"
#include "Engine/World.h"
#include "Engine/GameInstance.h"
void ATDFPersistentActor::CompleteAssignedObjective()
{
    if(ObjectiveIdToComplete.IsNone()) return;
    if(auto* W=GetWorld()) if(auto* GI=W->GetGameInstance()) GI->GetSubsystem<UTDFObjectiveSubsystem>()->CompleteObjective(ObjectiveIdToComplete);
}
