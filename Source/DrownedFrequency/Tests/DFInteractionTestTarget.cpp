#include "Tests/DFInteractionTestTarget.h"
#include "Components/StaticMeshComponent.h"
#include "UObject/ConstructorHelpers.h"

ATDFInteractionTestTarget::ATDFInteractionTestTarget()
{
    PrimaryActorTick.bCanEverTick = false;
    TargetMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TargetMesh"));
    SetRootComponent(TargetMesh);
    static ConstructorHelpers::FObjectFinder<UStaticMesh> Cube(TEXT("/Engine/BasicShapes/Cube"));
    TargetMesh->SetStaticMesh(Cube.Object);
    TargetMesh->SetRelativeScale3D(FVector(.3f));
    TargetMesh->SetCollisionProfileName(TEXT("BlockAll"));
}
bool ATDFInteractionTestTarget::CanInteract_Implementation(const ATDFPlayerCharacter* Player) const { return bInteractionEnabled; }
FText ATDFInteractionTestTarget::GetInteractionPrompt_Implementation(const ATDFPlayerCharacter* Player) const { return Prompt; }
void ATDFInteractionTestTarget::Interact_Implementation(ATDFPlayerCharacter* Player) { if (bInteractionEnabled) ++InteractionCount; }
bool ATDFInteractionTestTarget::WantsPromptVisible_Implementation(const ATDFPlayerCharacter* Player) const { return bShowPrompt; }
FName ATDFInteractionTestTarget::GetInteractionId_Implementation() const { return TEXT("InputGym.TestTarget"); }
