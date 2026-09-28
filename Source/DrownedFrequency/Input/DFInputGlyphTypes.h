#pragma once

#include "CoreMinimal.h"
#include "DFInputGlyphTypes.generated.h"

UENUM(BlueprintType)
enum class ETDFInputDevice : uint8 { KeyboardMouse, Gamepad };

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FTDFInputDeviceChanged, ETDFInputDevice, Device);
