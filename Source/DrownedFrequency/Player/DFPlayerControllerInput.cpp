#include "Player/DFPlayerController.h"
#include "Input/DFInputConfig.h"
#include "Input/DFPlayerInputSettingsSave.h"
#include "InputAction.h"
#include "InputMappingContext.h"
#include "EnhancedInputSubsystems.h"
#include "Engine/LocalPlayer.h"
#include "Kismet/GameplayStatics.h"
#include "UI/DFRecorderWidget.h"

namespace { const FString InputSlot=TEXT("TDF_InputSettings"); }
void ATDFPlayerController::LoadInputSettings()
{
    InputSettings=UGameplayStatics::DoesSaveGameExist(InputSlot,0)
        ? Cast<UTDFPlayerInputSettingsSave>(UGameplayStatics::LoadGameFromSlot(InputSlot,0)) : nullptr;
    if (!InputSettings) InputSettings=NewObject<UTDFPlayerInputSettingsSave>(this);
    ApplyInputOverrides();
}
void ATDFPlayerController::ApplyInputOverrides()
{
    auto* Local=GetLocalPlayer();
    auto* Input=Local ? Local->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>() : nullptr;
    if (!Input || !InputConfig || !InputSettings) return;
    Input->RemoveMappingContext(InputConfig->KeyboardMouseContext);
    Input->RemoveMappingContext(InputConfig->GamepadContext);
    if (RuntimeKeyboardContext) Input->RemoveMappingContext(RuntimeKeyboardContext);
    if (RuntimeGamepadContext) Input->RemoveMappingContext(RuntimeGamepadContext);
    RuntimeKeyboardContext=nullptr; RuntimeGamepadContext=nullptr;
    auto Build=[this](UInputMappingContext* Base,const TMap<FName,FKey>& Overrides,bool bGamepad)->UInputMappingContext*
    {
        if (Overrides.IsEmpty()) return nullptr;
        auto* Copy=DuplicateObject<UInputMappingContext>(Base,this);
        TMap<FName,FKey> Valid;
        for (const auto& Pair:Overrides)
        {
            UInputAction* Action=InputConfig->FindRemappableAction(Pair.Key);
            if (!Action || !Pair.Value.IsValid() || Pair.Value.IsAnalog() || Pair.Value.IsGamepadKey()!=bGamepad) continue;
            Valid.Add(Pair);
            Copy->UnmapAllKeysFromAction(Action);
        }
        // Remove all replaced rows first, so loading a valid profile never depends on TMap iteration order.
        for (const auto& Pair:Valid) Copy->MapKey(InputConfig->FindRemappableAction(Pair.Key),Pair.Value);
        TMap<FKey,const UInputAction*> Claimed;
        for (const auto& Mapping:Copy->GetMappings())
        {
            if (const auto* Previous=Claimed.Find(Mapping.Key); Previous && *Previous!=Mapping.Action.Get())
            {
                UE_LOG(LogTemp,Warning,TEXT("TDF Input: conflicting saved profile; using default device bindings."));
                return nullptr;
            }
            Claimed.Add(Mapping.Key,Mapping.Action);
        }
        return Copy;
    };
    RuntimeKeyboardContext=Build(InputConfig->KeyboardMouseContext,InputSettings->KeyboardOverrides,false);
    RuntimeGamepadContext=Build(InputConfig->GamepadContext,InputSettings->GamepadOverrides,true);
    Input->AddMappingContext(RuntimeKeyboardContext ? RuntimeKeyboardContext.Get() : InputConfig->KeyboardMouseContext.Get(),0);
    Input->AddMappingContext(RuntimeGamepadContext ? RuntimeGamepadContext.Get() : InputConfig->GamepadContext.Get(),0);
    RefreshInteractionPrompt();
    if(RecorderWidget) RecorderWidget->Refresh();
}
bool ATDFPlayerController::RebindAction(FName Name,FKey NewKey,ETDFInputDevice Device,bool bSaveToDisk)
{
    UInputAction* Action=InputConfig ? InputConfig->FindRemappableAction(Name) : nullptr;
    const bool bGamepad=Device==ETDFInputDevice::Gamepad;
    if (!bInputContextsReady || !InputSettings || !Action || !NewKey.IsValid() || NewKey.IsAnalog() || NewKey.IsGamepadKey()!=bGamepad) return false;
    const UInputMappingContext* Context=bGamepad
        ? (RuntimeGamepadContext ? RuntimeGamepadContext.Get() : InputConfig->GamepadContext.Get())
        : (RuntimeKeyboardContext ? RuntimeKeyboardContext.Get() : InputConfig->KeyboardMouseContext.Get());
    for (const auto& Mapping:Context->GetMappings()) if (Mapping.Key==NewKey && Mapping.Action!=Action) return false;
    auto& Overrides=bGamepad ? InputSettings->GamepadOverrides : InputSettings->KeyboardOverrides;
    const auto Previous=Overrides;
    Overrides.Add(Name,NewKey);
    if (bSaveToDisk && !UGameplayStatics::SaveGameToSlot(InputSettings,InputSlot,0)) { Overrides=Previous; return false; }
    FlushPressedKeys();
    ApplyInputOverrides();
    return true;
}
bool ATDFPlayerController::ResetBindingsToDefault(bool bSaveToDisk)
{
    if (!InputSettings) return false;
    auto* Fresh=NewObject<UTDFPlayerInputSettingsSave>(this);
    if (bSaveToDisk && !UGameplayStatics::SaveGameToSlot(Fresh,InputSlot,0)) return false;
    InputSettings=Fresh;
    FlushPressedKeys();
    ApplyInputOverrides();
    return true;
}
