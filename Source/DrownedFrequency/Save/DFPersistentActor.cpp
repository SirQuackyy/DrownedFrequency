#include "Save/DFPersistentActor.h"
ATDFPersistentActor::ATDFPersistentActor() { StableIdentity=CreateDefaultSubobject<UTDFStableActorComponent>(TEXT("StableIdentity")); }
void ATDFPersistentActor::SetStableActorId_Implementation(FGuid Id)
{
    if (!HasActorBegunPlay()) StableIdentity->StableActorId=Id;
}
void ATDFPersistentActor::OnConstruction(const FTransform& Transform)
{
    Super::OnConstruction(Transform);
    StableIdentity->EnsurePlacedEditorId();
}
#if WITH_EDITOR
void ATDFPersistentActor::PostDuplicate(EDuplicateMode::Type DuplicateMode)
{
    Super::PostDuplicate(DuplicateMode);
    if (DuplicateMode!=EDuplicateMode::PIE && StableIdentity) StableIdentity->AssignNewEditorId();
}
void ATDFPersistentActor::PostEditImport() { Super::PostEditImport(); if (StableIdentity) StableIdentity->AssignNewEditorId(); }
#endif
