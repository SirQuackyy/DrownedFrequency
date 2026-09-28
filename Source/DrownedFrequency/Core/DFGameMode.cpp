#include "Core/DFGameMode.h"
#include "Player/DFPlayerCharacter.h"
#include "Player/DFPlayerController.h"

ATDFGameMode::ATDFGameMode()
{
    DefaultPawnClass = ATDFPlayerCharacter::StaticClass();
    PlayerControllerClass = ATDFPlayerController::StaticClass();
}
