#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "Input/DFInputGlyphTypes.h"
#include "DFPlayerController.generated.h"

class UTDFInputConfig;
class UTDFInteractionPromptWidget;
class UTDFInteractionComponent;

UCLASS(BlueprintType, Blueprintable)
class DROWNEDFREQUENCY_API ATDFPlayerController : public APlayerController
{
    GENERATED_BODY()
public:
    UPROPERTY(EditDefaultsOnly,Category="UI") TSubclassOf<class UTDFSaveDebugWidget> SaveDebugWidgetClass;
    UPROPERTY(Transient) TObjectPtr<class UTDFSaveDebugWidget> SaveDebugWidget;
    void SetWorldTransitionLock(FName Reason,bool bLocked);
    UFUNCTION(BlueprintPure) bool IsWorldTransitionLocked() const {return !WorldTransitionLocks.IsEmpty();}
    UPROPERTY(EditDefaultsOnly, Category="UI") TSubclassOf<class UTDFAcousticCaptionWidget> CaptionWidgetClass;
    UPROPERTY(Transient) TObjectPtr<class UTDFAcousticCaptionWidget> CaptionWidget;
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="UI") TSubclassOf<class UTDFRecorderWidget> RecorderWidgetClass;
    UPROPERTY(Transient, BlueprintReadOnly, Category="UI") TObjectPtr<class UTDFRecorderWidget> RecorderWidget;
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="UI") TSubclassOf<class UTDFInspectionWidget> InspectionWidgetClass;
    UPROPERTY(Transient, BlueprintReadOnly, Category="UI") TObjectPtr<UTDFInspectionWidget> InspectionWidget;
    void HandleInspectAction();
    void HandleBackAction();
    UPROPERTY(Transient, BlueprintReadOnly, Category="Inspection") TObjectPtr<class UTDFItemDataAsset> CurrentInspectedItem;
    UFUNCTION(BlueprintPure, Category="Inspection") bool IsInspectingItem() const { return CurrentInspectedItem != nullptr; }
    UFUNCTION(BlueprintCallable, Category="Inspection") bool OpenInspection(UTDFItemDataAsset* Item);
    UFUNCTION(BlueprintCallable, Category="Inspection") void CloseInspection();
    virtual bool InputKey(const FInputKeyEventArgs& Params) override;
    UPROPERTY(BlueprintReadOnly, Category="Input") ETDFInputDevice LastInputDevice = ETDFInputDevice::KeyboardMouse;
    UPROPERTY(BlueprintAssignable, Category="Input") FTDFInputDeviceChanged OnInputDeviceChanged;
    UFUNCTION(BlueprintPure, Category="Input") FText GetActionInputLabel(const class UInputAction* Action) const;
    virtual void FlushPressedKeys() override;
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="UI") TSubclassOf<UTDFInteractionPromptWidget> PromptWidgetClass;
    UPROPERTY(Transient, BlueprintReadOnly, Category="UI") TObjectPtr<UTDFInteractionPromptWidget> PromptWidget;
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Input")
    TObjectPtr<UTDFInputConfig> InputConfig;
    UFUNCTION(BlueprintPure, Category="Input")
    bool AreInputContextsReady() const { return bInputContextsReady; }
    /** Idempotent setup; invalid configuration is reported without touching existing mappings. */
    bool InitializePlayerInputContexts();
    UFUNCTION(BlueprintCallable, Category="Input") bool RebindAction(FName ActionName, FKey NewKey, ETDFInputDevice Device, bool bSaveToDisk=true);
    UFUNCTION(BlueprintCallable, Category="Input") bool ResetBindingsToDefault(bool bSaveToDisk=true);
    UFUNCTION(BlueprintCallable, Category="Input") void LoadInputSettings();
protected:
    virtual void BeginPlay() override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
    virtual void OnPossess(APawn* InPawn) override;
    virtual void OnUnPossess() override;
    UFUNCTION() void HandleFocusChanged(AActor* FocusedActor);
    void RefreshInteractionPrompt();
    void AttachInteraction();
private:
    TSet<FName> WorldTransitionLocks;
    uint8 MovementModeBeforeLock=1;
    void ApplyInputOverrides();
    UPROPERTY(Transient) TObjectPtr<class UTDFPlayerInputSettingsSave> InputSettings;
    UPROPERTY(Transient) TObjectPtr<class UInputMappingContext> RuntimeKeyboardContext;
    UPROPERTY(Transient) TObjectPtr<class UInputMappingContext> RuntimeGamepadContext;
    bool bInputContextsReady = false;
    TWeakObjectPtr<UTDFInteractionComponent> BoundInteraction;
    FTimerHandle PromptRefreshTimer;
};
