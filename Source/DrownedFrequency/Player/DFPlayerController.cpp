#include "Player/DFPlayerController.h"
#include "Input/DFInputConfig.h"
#include "EnhancedInputSubsystems.h"
#include "Engine/LocalPlayer.h"
#include "Player/DFPlayerCharacter.h"
#include "UI/DFInteractionPromptWidget.h"
#include "Interaction/DFInteractionComponent.h"
#include "Interaction/DFInteractableInterface.h"
#include "TimerManager.h"
#include "InputKeyEventArgs.h"
#include "Inventory/DFItemDataAsset.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "UI/DFInspectionWidget.h"
#include "UI/DFRecorderWidget.h"
#include "UI/DFAcousticCaptionWidget.h"
#include "UI/DFSaveDebugWidget.h"
#include "Save/DFSaveSubsystem.h"
#include "Engine/GameInstance.h"

bool ATDFPlayerController::OpenInspection(UTDFItemDataAsset* Item)
{
    if (!Item || !Item->HasValidDefinition() || !Item->bCanInspect || IsInspectingItem() || IsWorldTransitionLocked()) return false;
    if (IsLocalController() && GetLocalPlayer())
    {
        if (!InspectionWidgetClass) { UE_LOG(LogTemp, Warning, TEXT("TDF Inspection: assign WBP_Inspection.")); return false; }
        InspectionWidget=CreateWidget<UTDFInspectionWidget>(this,InspectionWidgetClass);
        if (!InspectionWidget) return false;
        InspectionWidget->AddToViewport(20);
        InspectionWidget->ShowItem(Item);
    }
    CurrentInspectedItem=Item;
    SetIgnoreMoveInput(true);
    SetIgnoreLookInput(true);
    if (auto* P=Cast<ATDFPlayerCharacter>(GetPawn()))
    {
        P->ResetTransientInput();
        P->GetCharacterMovement()->StopMovementImmediately();
    }
    if (PromptWidget) PromptWidget->HidePrompt();
    SetInputMode(FInputModeGameOnly());
    RefreshInteractionPrompt();
    return true;
}
void ATDFPlayerController::CloseInspection()
{
    if (!IsInspectingItem()) return;
    CurrentInspectedItem=nullptr;
    if (InspectionWidget) { InspectionWidget->RemoveFromParent(); InspectionWidget=nullptr; }
    SetIgnoreMoveInput(false);
    SetIgnoreLookInput(false);
    SetInputMode(FInputModeGameOnly());
    bShowMouseCursor=false;
}
void ATDFPlayerController::HandleBackAction() { if (IsInspectingItem()) CloseInspection(); }
void ATDFPlayerController::HandleInspectAction()
{
    if (IsInspectingItem()) { CloseInspection(); return; }
    auto* P=Cast<ATDFPlayerCharacter>(GetPawn());
    if (!P || !P->Interaction) return;
    P->Interaction->RefreshFocus();
    AActor* Target=P->Interaction->GetFocusedActor();
    if (IsValid(Target) && ITDFInteractableInterface::Execute_CanInspect(Target,P)) ITDFInteractableInterface::Execute_Inspect(Target,P);
}

bool ATDFPlayerController::InputKey(const FInputKeyEventArgs& Params)
{
#if !UE_BUILD_SHIPPING
    if(Params.Event==IE_Pressed && !IsInspectingItem() && !IsWorldTransitionLocked())
    {
        if(Params.Key==EKeys::F5) {if(SaveDebugWidget) SaveDebugWidget->SaveSlot(); else GetGameInstance()->GetSubsystem<UTDFSaveSubsystem>()->WriteSave(TEXT("Manual0"),ETDFSaveReason::Manual); return true;}
        if(Params.Key==EKeys::F9) {if(SaveDebugWidget) SaveDebugWidget->LoadSlot(); else GetGameInstance()->GetSubsystem<UTDFSaveSubsystem>()->RequestLoadSlot(TEXT("Manual0")); return true;}
        if(Params.Key==EKeys::F10 && SaveDebugWidget) {SaveDebugWidget->ToggleControls(); return true;}
    }
#endif
    const bool bMeaningful = Params.Event == IE_Pressed || Params.Event == IE_Repeat
        || (Params.Event == IE_Axis && FMath::Abs(Params.AmountDepressed) > (Params.Key.IsGamepadKey() ? .18f : .01f));
    if (bMeaningful)
    {
        const ETDFInputDevice Device = Params.Key.IsGamepadKey() ? ETDFInputDevice::Gamepad : ETDFInputDevice::KeyboardMouse;
        if (Device != LastInputDevice)
        {
            LastInputDevice = Device;
            OnInputDeviceChanged.Broadcast(Device);
            RefreshInteractionPrompt();
        }
    }
    return Super::InputKey(Params);
}

FText ATDFPlayerController::GetActionInputLabel(const UInputAction* Action) const
{
    const ULocalPlayer* Local = GetLocalPlayer();
    const auto* Input = Local ? Local->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>() : nullptr;
    if (Input && Action)
    {
        // Query effective mappings, so future user remaps need no widget changes.
        for (const FKey& Key : Input->QueryKeysMappedToAction(Action))
            if (Key.IsGamepadKey() == (LastInputDevice == ETDFInputDevice::Gamepad)) return Key.GetDisplayName();
    }
    return NSLOCTEXT("TDF", "UnboundInput", "Unbound");
}

void ATDFPlayerController::FlushPressedKeys()
{
    Super::FlushPressedKeys();
    if (auto* PlayerCharacter = Cast<ATDFPlayerCharacter>(GetPawn())) PlayerCharacter->ResetTransientInput();
}

bool ATDFPlayerController::InitializePlayerInputContexts()
{
    if (!IsLocalController()) return false;
    ULocalPlayer* Local = GetLocalPlayer();
    UEnhancedInputLocalPlayerSubsystem* Input = Local ? Local->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>() : nullptr;
    if (!InputConfig || !InputConfig->HasRequiredAssets() || !Input)
    {
        UE_LOG(LogTemp, Error, TEXT("TDF Input: missing InputConfig, required action/context, or local player subsystem on %s. Assign DA_InputConfig."), *GetName());
        return false;
    }
    if (bInputContextsReady) return true;
    Input->AddMappingContext(InputConfig->KeyboardMouseContext, 0);
    Input->AddMappingContext(InputConfig->GamepadContext, 0);
    bInputContextsReady = true;
    LoadInputSettings();
    UE_LOG(LogTemp, Display, TEXT("TDF Input: keyboard/mouse and gamepad contexts initialized."));
    return true;
}

void ATDFPlayerController::BeginPlay()
{
    Super::BeginPlay();
    if (!IsLocalController()) return;
    SetInputMode(FInputModeGameOnly());
    bShowMouseCursor = false;
    if (!InitializePlayerInputContexts()) return;
    if(SaveDebugWidgetClass && GetWorld()->GetName().Contains(TEXT("SaveStreaming"))) {SaveDebugWidget=CreateWidget<UTDFSaveDebugWidget>(this,SaveDebugWidgetClass); if(SaveDebugWidget) SaveDebugWidget->AddToViewport(3);}
    if(CaptionWidgetClass) {CaptionWidget=CreateWidget<UTDFAcousticCaptionWidget>(this,CaptionWidgetClass); if(CaptionWidget) CaptionWidget->AddToViewport(2);}
    if(RecorderWidgetClass)
    {
        RecorderWidget=CreateWidget<UTDFRecorderWidget>(this,RecorderWidgetClass);
        if(RecorderWidget) {RecorderWidget->AddToViewport(1); RecorderWidget->BindPlayer(Cast<ATDFPlayerCharacter>(GetPawn()));}
    }
    if (PromptWidgetClass)
    {
        PromptWidget = CreateWidget<UTDFInteractionPromptWidget>(this, PromptWidgetClass);
        if (PromptWidget) { PromptWidget->AddToViewport(); PromptWidget->HidePrompt(); }
    }
    AttachInteraction();
    GetWorldTimerManager().SetTimer(PromptRefreshTimer, this, &ThisClass::RefreshInteractionPrompt, .1f, true);
}

void ATDFPlayerController::EndPlay(const EEndPlayReason::Type Reason)
{
    CloseInspection();
    GetWorldTimerManager().ClearTimer(PromptRefreshTimer);
    if(SaveDebugWidget) {SaveDebugWidget->RemoveFromParent(); SaveDebugWidget=nullptr;}
    if(CaptionWidget) {CaptionWidget->RemoveFromParent(); CaptionWidget=nullptr;}
    if(RecorderWidget) {RecorderWidget->RemoveFromParent(); RecorderWidget=nullptr;}
    if (BoundInteraction.IsValid()) BoundInteraction->OnFocusedInteractableChanged.RemoveDynamic(this, &ThisClass::HandleFocusChanged);
    BoundInteraction.Reset();
    if (PromptWidget) { PromptWidget->RemoveFromParent(); PromptWidget = nullptr; }
    if (bInputContextsReady && InputConfig)
    {
        if (ULocalPlayer* Local = GetLocalPlayer())
        {
            if (auto* Input = Local->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
            {
                Input->RemoveMappingContext(InputConfig->KeyboardMouseContext);
                Input->RemoveMappingContext(InputConfig->GamepadContext);
                if (RuntimeKeyboardContext) Input->RemoveMappingContext(RuntimeKeyboardContext);
                if (RuntimeGamepadContext) Input->RemoveMappingContext(RuntimeGamepadContext);
            }
        }
    }
    bInputContextsReady = false;
    Super::EndPlay(Reason);
}

void ATDFPlayerController::OnPossess(APawn* InPawn) { Super::OnPossess(InPawn); AttachInteraction(); if(RecorderWidget) RecorderWidget->BindPlayer(Cast<ATDFPlayerCharacter>(InPawn)); }
void ATDFPlayerController::OnUnPossess()
{
    CloseInspection();
    if(RecorderWidget) RecorderWidget->BindPlayer(nullptr);
    if (BoundInteraction.IsValid()) BoundInteraction->OnFocusedInteractableChanged.RemoveDynamic(this, &ThisClass::HandleFocusChanged);
    BoundInteraction.Reset();
    if (PromptWidget) PromptWidget->HidePrompt();
    Super::OnUnPossess();
}
void ATDFPlayerController::AttachInteraction()
{
    if (BoundInteraction.IsValid()) BoundInteraction->OnFocusedInteractableChanged.RemoveDynamic(this, &ThisClass::HandleFocusChanged);
    auto* PlayerCharacter = Cast<ATDFPlayerCharacter>(GetPawn());
    BoundInteraction = PlayerCharacter ? PlayerCharacter->Interaction.Get() : nullptr;
    if (BoundInteraction.IsValid()) BoundInteraction->OnFocusedInteractableChanged.AddUniqueDynamic(this, &ThisClass::HandleFocusChanged);
    RefreshInteractionPrompt();
}
void ATDFPlayerController::HandleFocusChanged(AActor* FocusedActor) { RefreshInteractionPrompt(); }
void ATDFPlayerController::RefreshInteractionPrompt()
{
    if (!PromptWidget) return;
    if (IsInspectingItem())
    {
        PromptWidget->HidePrompt();
        if (InspectionWidget && InputConfig) InspectionWidget->UpdateControls(FText::Format(NSLOCTEXT("TDF","InspectControls","{0}  /  Return\nLook input  /  Rotate"),GetActionInputLabel(InputConfig->BackAction)));
        return;
    }
    AActor* Target = BoundInteraction.IsValid() ? BoundInteraction->GetFocusedActor() : nullptr;
    auto* PlayerCharacter = Cast<ATDFPlayerCharacter>(GetPawn());
    if (!IsValid(Target) || !PlayerCharacter || !ITDFInteractableInterface::Execute_WantsPromptVisible(Target, PlayerCharacter))
    {
        PromptWidget->HidePrompt();
        return;
    }
    PromptWidget->ShowPrompt(ITDFInteractableInterface::Execute_GetInteractionPrompt(Target, PlayerCharacter));
    PromptWidget->UpdateInputGlyph(GetActionInputLabel(InputConfig ? InputConfig->InteractAction.Get() : nullptr));
}
