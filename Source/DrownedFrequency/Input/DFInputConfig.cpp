#include "Input/DFInputConfig.h"
UInputAction* UTDFInputConfig::FindRemappableAction(FName Name) const
{
    if (Name==TEXT("Record")) return RecordAction;
    if (Name==TEXT("Playback")) return PlaybackAction;
    if (Name==TEXT("Project")) return ProjectAction;
    if (Name==TEXT("NextClip")) return NextClipAction;
    if (Name==TEXT("PreviousClip")) return PreviousClipAction;
    if (Name==TEXT("DeleteClip")) return DeleteClipAction;
    if (Name==TEXT("Interact")) return InteractAction;
    if (Name==TEXT("Inspect")) return InspectAction;
    if (Name==TEXT("Back")) return BackAction;
    if (Name==TEXT("Pause")) return PauseAction;
    if (Name==TEXT("Jump")) return JumpAction;
    if (Name==TEXT("Crouch")) return CrouchAction;
    if (Name==TEXT("Sprint")) return SprintAction;
    if (Name==TEXT("HoldBreath")) return HoldBreathAction;
    return nullptr;
}
#include "InputAction.h"
#include "InputMappingContext.h"

bool UTDFInputConfig::HasRequiredAssets() const
{
    return MoveAction && LookAction && JumpAction && CrouchAction && SprintAction && LeanAction && HoldBreathAction && InteractAction && InspectAction && BackAction && PauseAction && KeyboardMouseContext && GamepadContext;
}
