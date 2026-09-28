#include "Save/DFStableActorComponent.h"
#include "Save/DFSaveSubsystem.h"
#include "Engine/GameInstance.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"
void UTDFStableActorComponent::EnsurePlacedEditorId()
{
#if WITH_EDITOR
    AActor* Owner=GetOwner();
    if (Owner && !StableActorId.IsValid() && !Owner->HasAnyFlags(RF_WasLoaded|RF_ClassDefaultObject|RF_ArchetypeObject|RF_Transient)
        && Owner->GetWorld() && Owner->GetWorld()->WorldType==EWorldType::Editor) AssignNewEditorId();
#endif
}
void UTDFStableActorComponent::AssignNewEditorId()
{
#if WITH_EDITOR
    AActor* Owner=GetOwner();
    if (!Owner || Owner->HasAnyFlags(RF_ClassDefaultObject|RF_ArchetypeObject) || !Owner->GetWorld() || Owner->GetWorld()->WorldType!=EWorldType::Editor) return;
    Modify();
    StableActorId=FGuid::NewGuid();
    bPersistInSave=true;
    Owner->MarkPackageDirty();
#endif
}
void UTDFStableActorComponent::BeginPlay()
{
    Super::BeginPlay();
    if (!ShouldPersistInSave()) return;
    if (!StableActorId.IsValid())
    {
        UE_LOG(LogTemp,Error,TEXT("TDF Persistence: missing authored stable ID on %s. Runtime IDs are never regenerated."),*GetOwner()->GetName());
        return;
    }
    if (auto* GI=GetWorld()->GetGameInstance()) GI->GetSubsystem<UTDFSaveSubsystem>()->RegisterStableActor(GetOwner(),StableActorId);
}
void UTDFStableActorComponent::EndPlay(const EEndPlayReason::Type Reason)
{
    if (auto* World=GetWorld())
        if (auto* GI=World->GetGameInstance())
            if (auto* Saves=GI->GetSubsystem<UTDFSaveSubsystem>()) Saves->UnregisterStableActor(GetOwner(),StableActorId);
    Super::EndPlay(Reason);
}
