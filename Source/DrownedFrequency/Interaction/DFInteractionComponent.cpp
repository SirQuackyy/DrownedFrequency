#include "Interaction/DFInteractionComponent.h"
#include "Interaction/DFInteractableInterface.h"
#include "Player/DFPlayerCharacter.h"
#include "Player/DFPlayerCameraComponent.h"
#include "Engine/World.h"
#include "Camera/CameraTypes.h"
#include "Player/DFPlayerController.h"

UTDFInteractionComponent::UTDFInteractionComponent()
{
    PrimaryComponentTick.bCanEverTick = true;
    PrimaryComponentTick.TickInterval = .05f;
    PrimaryComponentTick.TickGroup = TG_PostUpdateWork;
}

void UTDFInteractionComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
    RefreshFocus();
}

void UTDFInteractionComponent::RefreshFocus()
{
    const auto* Player = Cast<ATDFPlayerCharacter>(GetOwner());
    AActor* NewFocus = nullptr;
    const auto* PC = Player ? Cast<ATDFPlayerController>(Player->GetController()) : nullptr;
    if (Player && Player->IsLocallyControlled() && Player->PlayerCamera && !(PC && (PC->IsInspectingItem() || PC->IsWorldTransitionLocked())))
    {
        FMinimalViewInfo View;
        Player->PlayerCamera->GetCameraView(0.f, View);
        FHitResult Hit;
        FCollisionQueryParams Query(SCENE_QUERY_STAT(TDFInteraction), false, Player);
        const FVector End = View.Location + View.Rotation.Vector() * InteractionDistance;
        if (GetWorld()->SweepSingleByChannel(Hit, View.Location, End, FQuat::Identity, TraceChannel, FCollisionShape::MakeSphere(TraceRadius), Query))
        {
            AActor* Candidate = Hit.GetActor();
            if (IsValid(Candidate) && Candidate->Implements<UTDFInteractableInterface>() && ITDFInteractableInterface::Execute_CanInteract(Candidate, Player)) NewFocus = Candidate;
        }
    }
    if (FocusedActor.Get() != NewFocus || bHadFocus != (NewFocus != nullptr))
    {
        FocusedActor = NewFocus;
        bHadFocus = NewFocus != nullptr;
        OnFocusedInteractableChanged.Broadcast(NewFocus);
    }
}

bool UTDFInteractionComponent::TryInteract()
{
    // Revalidate aim, distance, and occlusion at activation, not just on the last trace tick.
    RefreshFocus();
    auto* Player = Cast<ATDFPlayerCharacter>(GetOwner());
    AActor* Target = FocusedActor.Get();
    if (!Player || !IsValid(Target) || !ITDFInteractableInterface::Execute_CanInteract(Target, Player)) return false;
    ITDFInteractableInterface::Execute_Interact(Target, Player);
    RefreshFocus();
    return true;
}
