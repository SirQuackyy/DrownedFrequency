#include "Player/DFPlayerController.h"
#include "UI/DFSaveDebugWidget.h"
#include "Player/DFPlayerCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Camera/PlayerCameraManager.h"
void ATDFPlayerController::SetWorldTransitionLock(FName Reason,bool bLocked)
{
    const bool WasLocked=IsWorldTransitionLocked(); if(bLocked) WorldTransitionLocks.Add(Reason); else WorldTransitionLocks.Remove(Reason);
    if(WasLocked==IsWorldTransitionLocked()) return;
    if(SaveDebugWidget) SaveDebugWidget->CloseControls();
    CloseInspection(); FlushPressedKeys(); SetIgnoreMoveInput(!WasLocked); SetIgnoreLookInput(!WasLocked);
    if(auto* P=Cast<ATDFPlayerCharacter>(GetPawn()))
    {
        auto* Movement=P->GetCharacterMovement(); P->ResetTransientInput(); Movement->StopMovementImmediately();
        if(!WasLocked) {MovementModeBeforeLock=uint8(Movement->MovementMode); Movement->DisableMovement();}
        else Movement->SetMovementMode(EMovementMode(MovementModeBeforeLock==0?1:MovementModeBeforeLock));
    }
    if(PlayerCameraManager) PlayerCameraManager->StartCameraFade(WasLocked?1.f:0.f,WasLocked?0.f:1.f,WasLocked?.35f:.15f,FLinearColor::Black,false,!WasLocked);
}
