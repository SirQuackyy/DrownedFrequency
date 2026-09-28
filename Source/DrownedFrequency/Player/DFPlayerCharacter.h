#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Player/DFMovementStateTypes.h"
#include "DFPlayerCharacter.generated.h"

class UTDFPlayerCameraComponent;
class UTDFInteractionComponent;
struct FInputActionValue;
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FTDFMovementStateChanged, FTDFPlayerMovementState, State);

UCLASS(BlueprintType, Blueprintable)
class DROWNEDFREQUENCY_API ATDFPlayerCharacter : public ACharacter
{
    GENERATED_BODY()
public:
    ATDFPlayerCharacter();
    UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="Player") TObjectPtr<class UTDFHealthComponent> Health;
    virtual void Tick(float DeltaSeconds) override;
    virtual void Landed(const FHitResult& Hit) override;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Acoustics") TObjectPtr<class UTDFRecorderComponent> Recorder;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Acoustics") TObjectPtr<class UTDFRecorderOperatorComponent> RecorderOperator;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Acoustics") TObjectPtr<class UTDFRecorderPlaybackComponent> RecorderPlayback;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Acoustics") TObjectPtr<class UTDFAcousticPuzzleLedgerComponent> AcousticLedger;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Acoustics") TObjectPtr<class UTDFPlayerNoiseEmitterComponent> NoiseEmitter;
    UFUNCTION(BlueprintCallable, Category="Movement") void RefreshMovementState();
    UFUNCTION(BlueprintCallable, Category="Input") void ResetTransientInput();
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Input") float HoldBreathLookMultiplier = .65f;
    UPROPERTY(BlueprintReadOnly, Category="Movement") FTDFPlayerMovementState CurrentMovementState;
    UPROPERTY(BlueprintAssignable, Category="Movement") FTDFMovementStateChanged OnMovementStateChanged;
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Movement") float WalkSpeed = 250.f;
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Movement") float SprintSpeed = 460.f;
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Movement") float CrouchSpeed = 145.f;
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Movement") float MinSprintSpeed = 10.f;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Camera")
    TObjectPtr<UTDFPlayerCameraComponent> PlayerCamera;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Inventory") TObjectPtr<class UTDFInventoryComponent> Inventory;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Interaction") TObjectPtr<UTDFInteractionComponent> Interaction;
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Movement")
    bool bAllowJump = false;
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Input")
    float MouseLookScale = 0.12f;
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Input")
    float GamepadLookRate = 100.f;
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Input", meta=(ClampMin="10",ClampMax="300"))
    float GamepadLookPitchRate = 85.f;
protected:
    virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
    virtual void OnStartCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust) override;
    virtual void OnEndCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust) override;
    void HandleMove(const FInputActionValue& Value);
    void HandleLook(const FInputActionValue& Value);
    void StartJump();
    void EndJump();
    void StartCrouch();
    void EndCrouch();
    void StartSprint();
    void StopSprint();
    void HandleLean(const FInputActionValue& Value);
    void StopLean();
    void StartHoldBreath();
    void StopHoldBreath();
    void HandleInteract();
    void HandleInspect();
    void HandleBack();
    void HandleProjectPuck();
    bool IsGameplayInputBlocked() const;
private:
    FTDFPlayerMovementState PublishedMovementState;
};
