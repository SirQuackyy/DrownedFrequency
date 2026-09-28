# Player, Input, Interaction, and World Objects

This is a task execution manual, not a request to implement everything at once.

For every numbered task:
1. Give the task's copy-paste prompt to the implementation AI.
2. Require it to inspect the current repository before editing.
3. Require tests and a real UE build or packaged run appropriate to the task.
4. Do not advance until every listed done criterion passes.
5. Commit only after Andrew reviews the diff and test output.

Locked naming used by this manual:
- Project/module: `DrownedFrequency`
- C++ class prefix: `TDF`
- Content root: `Content/Game`
- Source root: `Source/DrownedFrequency`
- Sound categories: Voice, Footstep, Machine, Bell/Foghorn, Water, Static, Impact

## Summary

- What i did: Designed a granular UE5 C++ build sequence for the locked Drowned Frequency architecture: C++ reusable mechanics and persisted truth, Blueprint presentation, Data Asset definitions.
- What i accomplished: Covered player movement, camera, interaction tracing, doors, switches, item inspection, key items, UI prompts, keyboard/mouse, controller, remapping, crouch, sprint, lean, and hold breath.
- Files created or modified: None. Design only; no implementation or file writes.
- Issues encountered: None.

## Global architecture rules
- Use C++ for reusable mechanics, authority, saveable state, input routing, interaction rules, and inventory truth.
- Use Blueprint only for presentation: meshes, sounds, animations, timelines, widgets, and level-specific hookups.
- Use Data Assets for definitions: item metadata, prompt text, icon references, interaction tuning, door/switch config where practical.
- Do not implement universal physics pickup.
- All player actions must support keyboard/mouse and controller through Enhanced Input.
- All rebinding must happen through Enhanced Input user settings or a project-owned mapping save object.
- Persisted truth belongs in SaveGame-friendly structs or components, not in level Blueprint-only variables.
- Each step should be implemented and tested before moving to the next.

## Build sequence

### 1 — Create core gameplay module folder layout

- Id: 1
- Title: Create core gameplay module folder layout
- Goal: Establish stable file locations before adding mechanics.
#### Classes files
- Source/DrownedFrequency/Player/DFPlayerCharacter.h/.cpp
- Source/DrownedFrequency/Player/DFPlayerController.h/.cpp
- Source/DrownedFrequency/Player/DFPlayerCameraComponent.h/.cpp
- Source/DrownedFrequency/Input/DFInputConfig.h/.cpp
- Source/DrownedFrequency/Interaction/DFInteractableInterface.h
- Source/DrownedFrequency/Interaction/DFInteractionComponent.h/.cpp
- Source/DrownedFrequency/Inventory/DFInventoryComponent.h/.cpp
- Source/DrownedFrequency/Inventory/DFItemDataAsset.h/.cpp
- Source/DrownedFrequency/World/DFDoorActor.h/.cpp
- Source/DrownedFrequency/World/DFSwitchActor.h/.cpp
- Source/DrownedFrequency/UI/DFInteractionPromptWidget.h/.cpp
- Source/DrownedFrequency/Save/DFPersistentStateTypes.h

#### Logic or pseudocode
- Create folders by responsibility, not by level.
- Keep player input in Player/Input.
- Keep world interactables in Interaction/World.
- Keep inventory and item data independent from UI.
- Avoid direct references from world actors to UI widgets.

#### Blueprint editor hookup
- Create BP_TDFPlayerCharacter derived from ATDFPlayerCharacter.
- Create BP_TDFPlayerController derived from ATDFPlayerController.
- Set project GameMode default pawn to BP_TDFPlayerCharacter.
- Set project GameMode player controller to BP_TDFPlayerController.

#### Test
- Compile editor.
- Create empty test map.
- Press Play and confirm pawn spawns.
- Confirm no Blueprint compile errors.

#### Done criteria
- C++ compiles cleanly.
- GameMode uses project character and controller.
- No mechanics implemented yet.

- Common failure: Adding gameplay logic into Level Blueprint early creates throwaway coupling.
- Copy paste ai prompt: In this UE5 C++ project, create only the gameplay folder/class skeletons for Drowned Frequency: ATDFPlayerCharacter, ATDFPlayerController, UTDFPlayerCameraComponent, UTDFInputConfig, UTDFInteractionComponent, UTDFInventoryComponent, UTDFItemDataAsset, ATDFDoorActor, ATDFSwitchActor, UTDFInteractionPromptWidget, and persistent state types. Do not implement gameplay behavior yet. Keep C++ as reusable mechanics and Blueprint as presentation.

### 2 — Add Enhanced Input asset contract

- Id: 2
- Title: Add Enhanced Input asset contract
- Goal: Define all player actions up front so keyboard/mouse and controller remain first-class.
#### Classes files
- Input/DFInputConfig.h/.cpp
- DataAsset: DA_InputConfig
- Input Actions: IA_Move, IA_Look, IA_Jump, IA_Crouch, IA_Sprint, IA_Lean, IA_HoldBreath, IA_Interact, IA_Inspect, IA_Back, IA_Pause
- Input Mapping Contexts: IMC_Gameplay_KBM, IMC_Gameplay_Gamepad

#### Logic or pseudocode
- UCLASS(BlueprintType) UTDFInputConfig : UDataAsset
- UPROPERTY(EditDefaultsOnly) TObjectPtr<UInputAction> MoveAction;
- UPROPERTY(EditDefaultsOnly) TObjectPtr<UInputAction> LookAction;
- UPROPERTY(EditDefaultsOnly) TObjectPtr<UInputAction> CrouchAction;
- UPROPERTY(EditDefaultsOnly) TObjectPtr<UInputAction> SprintAction;
- UPROPERTY(EditDefaultsOnly) TObjectPtr<UInputAction> LeanAction;
- UPROPERTY(EditDefaultsOnly) TObjectPtr<UInputAction> HoldBreathAction;
- UPROPERTY(EditDefaultsOnly) TObjectPtr<UInputAction> InteractAction;
- UPROPERTY(EditDefaultsOnly) TObjectPtr<UInputAction> InspectAction;
- UPROPERTY(EditDefaultsOnly) TObjectPtr<UInputMappingContext> KeyboardMouseContext;
- UPROPERTY(EditDefaultsOnly) TObjectPtr<UInputMappingContext> GamepadContext;

#### Blueprint editor hookup
- Create IA_Move as Axis2D.
- Create IA_Look as Axis2D.
- Create IA_Lean as Axis1D.
- Create boolean actions for sprint, crouch, hold breath, interact, inspect, back, pause.
- Map WASD and left stick to IA_Move.
- Map mouse delta and right stick to IA_Look.
- Map Ctrl/B to crouch.
- Map Shift/L3 to sprint.
- Map Q/E and left/right shoulder or dpad to lean axis.
- Map RMB/LT to hold breath.
- Map E/X or A to interact depending project convention.
- Assign all assets to DA_InputConfig.

#### Test
- Open each Input Action and verify value type.
- Open both Mapping Contexts and verify no missing actions.
- Run PIE and confirm contexts are applied later only after controller step.

#### Done criteria
- Every required action has both keyboard/mouse and controller bindings.
- DA_InputConfig references every required Input Action and Mapping Context.

- Common failure: Using keyboard-only bindings and retrofitting controller later causes inconsistent prompt/UI behavior.
- Copy paste ai prompt: Create a UE5 Enhanced Input contract for Drowned Frequency using UTDFInputConfig as a Data Asset reference hub. Include Move, Look, Jump, Crouch, Sprint, Lean, HoldBreath, Interact, Inspect, Back, and Pause actions. Require keyboard/mouse and gamepad mapping contexts. Do not implement character behavior yet.

### 3 — Initialize player controller input contexts

- Id: 3
- Title: Initialize player controller input contexts
- Goal: Make Enhanced Input contexts activate reliably for local players.
#### Classes files
- Player/DFPlayerController.h/.cpp
- Input/DFInputConfig.h/.cpp

#### Logic or pseudocode
- ATDFPlayerController has UPROPERTY(EditDefaultsOnly) TObjectPtr<UTDFInputConfig> InputConfig;
- In BeginPlay:
- LocalPlayer = GetLocalPlayer();
- Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(LocalPlayer);
- Subsystem->ClearAllMappings();
- Subsystem->AddMappingContext(InputConfig->KeyboardMouseContext, 0);
- Subsystem->AddMappingContext(InputConfig->GamepadContext, 0);
- SetInputMode(FInputModeGameOnly());
- bShowMouseCursor = false;

#### Blueprint editor hookup
- Open BP_TDFPlayerController.
- Assign DA_InputConfig.
- Set BP_TDFPlayerController as GameMode default controller.

#### Test
- Add temporary UE_LOG in BeginPlay confirming both contexts added.
- PIE with keyboard and controller connected.
- Verify no null InputConfig warnings.

#### Done criteria
- Input contexts are added once on BeginPlay.
- Missing InputConfig logs a clear warning instead of crashing.

- Common failure: Adding mapping contexts in Character before LocalPlayer exists can silently fail in some PIE/network setups.
- Copy paste ai prompt: Implement ATDFPlayerController Enhanced Input initialization. It should read UTDFInputConfig, add keyboard/mouse and gamepad mapping contexts to UEnhancedInputLocalPlayerSubsystem, set game-only input mode, hide cursor, and fail safely with clear logs if assets are missing.

### 4 — Implement baseline first-person character movement

- Id: 4
- Title: Implement baseline first-person character movement
- Goal: Create stable move/look/jump/crouch plumbing before special states.
#### Classes files
- Player/DFPlayerCharacter.h/.cpp
- Input/DFInputConfig.h/.cpp

#### Logic or pseudocode
- ATDFPlayerCharacter derives from ACharacter.
- Components:
- UCapsuleComponent inherited.
- UCameraComponent or UTDFPlayerCameraComponent attached to capsule.
- In SetupPlayerInputComponent:
- Cast UEnhancedInputComponent.
- Bind Move Started/Triggered to HandleMove.
- Bind Look Triggered to HandleLook.
- Bind Jump Started/Completed.
- Bind Crouch Started/Completed or Toggle depending chosen design.
- HandleMove(FInputActionValue Value):
- FVector2D Axis = Value.Get<FVector2D>();
- AddMovementInput(GetActorForwardVector(), Axis.Y);
- AddMovementInput(GetActorRightVector(), Axis.X);
- HandleLook:
- FVector2D Axis = Value.Get<FVector2D>();
- AddControllerYawInput(Axis.X * LookYawScale);
- AddControllerPitchInput(Axis.Y * LookPitchScale);

#### Blueprint editor hookup
- Open BP_TDFPlayerCharacter.
- Assign first-person camera position at eye height.
- Set Use Controller Rotation Yaw true.
- Set CharacterMovement Orient Rotation to Movement false.
- Enable Can Crouch in CharacterMovement Nav Agent settings.
- Tune Max Walk Speed to baseline walking speed.

#### Test
- PIE on flat test map.
- WASD moves relative to facing.
- Left stick moves relative to facing.
- Mouse/right stick look rotates camera.
- Jump works only if allowed.
- Crouch changes capsule height and camera height.

#### Done criteria
- Keyboard/mouse and controller can move/look.
- Movement direction is camera/controller-relative.
- No sprint/lean/hold breath yet.

- Common failure: Using actor rotation instead of controller rotation makes movement feel wrong after looking around.
- Copy paste ai prompt: Implement baseline first-person movement in ATDFPlayerCharacter using Enhanced Input bindings from UTDFInputConfig. Support Move Axis2D, Look Axis2D, Jump, and Crouch. Keep logic in C++; expose tuning variables to Blueprint. Do not add sprint, lean, hold breath, interaction, or inventory yet.

### 5 — Add movement state model

- Id: 5
- Title: Add movement state model
- Goal: Centralize walk, crouch, sprint, lean, and hold breath state so camera/UI can query one truth.
#### Classes files
- Player/DFPlayerCharacter.h/.cpp
- Player/DFMovementStateTypes.h

#### Logic or pseudocode
- UENUM(BlueprintType) ETDFMovementModeState { Walking, Crouching, Sprinting };
- USTRUCT(BlueprintType) FTDFPlayerMovementState { bool bWantsSprint; bool bIsSprinting; bool bIsCrouching; float LeanAxis; bool bHoldingBreath; };
- ATDFPlayerCharacter owns CurrentMovementState.
- CanSprint(): return !bIsCrouching && !bHoldingBreath && GetVelocity().Size2D() > MinSprintSpeed;
- RefreshMovementState():
- bIsSprinting = bWantsSprint && CanSprint();
- CharacterMovement->MaxWalkSpeed = bIsSprinting ? SprintSpeed : bIsCrouching ? CrouchSpeed : WalkSpeed;
- Broadcast OnMovementStateChanged if state changed.

#### Blueprint editor hookup
- Expose WalkSpeed, SprintSpeed, CrouchSpeed, MinSprintSpeed to BP_TDFPlayerCharacter.
- Set conservative defaults: Walk 220-280, Sprint 420-520, Crouch 120-180 depending horror pacing.
- Do not implement camera effects in Blueprint yet; only verify exposed values.

#### Test
- Use PIE console/log to print state transitions.
- Sprint button while standing and moving sets sprinting.
- Sprint while crouched does not sprint.
- Hold breath later should block sprint.

#### Done criteria
- Movement speed is driven by one C++ state struct.
- Invalid state combinations are impossible or resolved immediately.

- Common failure: Letting sprint, crouch, and hold breath each directly set MaxWalkSpeed causes stale speed when actions overlap.
- Copy paste ai prompt: Add a C++ movement state model to ATDFPlayerCharacter for Walking, Crouching, Sprinting, LeanAxis, and HoldingBreath. Centralize speed selection in RefreshMovementState and expose tuning variables to Blueprint. Ensure sprint cannot activate while crouched or holding breath.

### 6 — Implement sprint input

- Id: 6
- Title: Implement sprint input
- Goal: Add hold-to-sprint behavior with clean cancellation.
#### Classes files
- Player/DFPlayerCharacter.h/.cpp

#### Logic or pseudocode
- Bind SprintAction Started -> StartSprint.
- Bind SprintAction Completed/Canceled -> StopSprint.
- StartSprint(): CurrentMovementState.bWantsSprint = true; RefreshMovementState();
- StopSprint(): CurrentMovementState.bWantsSprint = false; RefreshMovementState();
- OnMovementUpdated or Tick:
- RefreshMovementState if velocity/crouch/hold breath changed.

#### Blueprint editor hookup
- Verify SprintAction mapped to Left Shift and controller L3 or left stick press.
- Tune SprintSpeed in BP_TDFPlayerCharacter.

#### Test
- Hold sprint while moving: speed increases.
- Release sprint: speed returns to walk.
- Press sprint while stationary: state should not become sprinting unless design permits windup.
- Crouch while sprinting: sprint cancels or blocks.

#### Done criteria
- Sprint is hold-based.
- Sprint uses same state model.
- No stuck sprint after releasing input.

- Common failure: Binding only Started and not Completed leaves sprint stuck on controller disconnect or focus loss.
- Copy paste ai prompt: Implement hold-to-sprint in ATDFPlayerCharacter using Enhanced Input Started and Completed/Canceled events. It must update bWantsSprint and call RefreshMovementState. Sprint must cancel when crouched or holding breath and must never leave MaxWalkSpeed stuck.

### 7 — Implement lean input and lean target state

- Id: 7
- Title: Implement lean input and lean target state
- Goal: Add left/right lean as a camera offset target without moving collision.
#### Classes files
- Player/DFPlayerCharacter.h/.cpp
- Player/DFPlayerCameraComponent.h/.cpp

#### Logic or pseudocode
- LeanAction is Axis1D: Q=-1, E=+1, gamepad shoulder/dpad left/right.
- HandleLean(Value): CurrentMovementState.LeanAxis = Clamp(Value.Get<float>(), -1, 1);
- Camera component reads LeanAxis each tick.
- TargetRoll = LeanAxis * MaxLeanRollDegrees;
- TargetSideOffset = LeanAxis * MaxLeanSideOffset;
- Interpolate current roll/offset toward target.
- Do not move capsule collision.
- Optional: trace side offset to avoid clipping into wall.

#### Blueprint editor hookup
- Expose MaxLeanRollDegrees, MaxLeanSideOffset, LeanInterpSpeed.
- Set initial values small: Roll 5-8 degrees, side offset 8-15 cm.
- Map Q/E and controller shoulders/dpad.

#### Test
- Hold Q: camera leans left.
- Hold E: camera leans right.
- Release: camera returns smoothly.
- Lean near wall does not clip badly if side trace implemented.

#### Done criteria
- Lean affects camera only.
- Capsule remains stable.
- Keyboard and controller support lean.

- Common failure: Moving the whole character sideways for lean causes collision popping and unintended peeking through geometry.
- Copy paste ai prompt: Implement first-person lean for Drowned Frequency. Lean input is Axis1D. Store LeanAxis in ATDFPlayerCharacter state. Apply only camera roll and local side offset in UTDFPlayerCameraComponent with interpolation. Do not move the capsule. Expose tuning variables to Blueprint.

### 8 — Implement hold breath camera/movement modifier

- Id: 8
- Title: Implement hold breath camera/movement modifier
- Goal: Add hold-breath as a player state that steadies camera and blocks sprint.
#### Classes files
- Player/DFPlayerCharacter.h/.cpp
- Player/DFPlayerCameraComponent.h/.cpp

#### Logic or pseudocode
- Bind HoldBreath Started -> StartHoldBreath.
- Bind HoldBreath Completed/Canceled -> StopHoldBreath.
- StartHoldBreath(): bHoldingBreath = true; bWantsSprint = false; RefreshMovementState();
- StopHoldBreath(): bHoldingBreath = false; RefreshMovementState();
- Camera component:
- If bHoldingBreath: reduce sway amplitude and look sensitivity multiplier.
- Optional breath stamina later, but do not add until required.

#### Blueprint editor hookup
- Map RMB and controller LT to hold breath.
- Expose HoldBreathLookMultiplier, HoldBreathSwayMultiplier.
- Do not add UI meter yet unless design requires.

#### Test
- Hold breath while walking: sprint cannot start.
- Start sprint then hold breath: sprint ends.
- Camera sway/look sensitivity changes while held.
- Release returns to normal.

#### Done criteria
- Hold breath is a C++ state.
- It cleanly interacts with sprint.
- It is available on mouse and controller.

- Common failure: Implementing hold breath only in camera code means movement rules cannot know it should block sprint.
- Copy paste ai prompt: Add hold-breath support to ATDFPlayerCharacter and UTDFPlayerCameraComponent. Hold breath should be a C++ player state, should cancel/block sprint, and should modify camera steadiness/look multiplier. Bind RMB and controller LT through Enhanced Input.

### 9 — Create first-person camera component

- Id: 9
- Title: Create first-person camera component
- Goal: Move camera effects out of character so movement remains clean.
#### Classes files
- Player/DFPlayerCameraComponent.h/.cpp

#### Logic or pseudocode
- UTDFPlayerCameraComponent derives from UCameraComponent.
- Properties:
- BaseEyeHeightOffset
- CrouchCameraInterpSpeed
- LeanInterpSpeed
- WalkSwayAmplitude
- SprintSwayAmplitude
- HoldBreathSwayMultiplier
- TickComponent:
- OwnerCharacter = Cast<ATDFPlayerCharacter>(GetOwner());
- Read movement state.
- TargetLocalLocation.Z = bIsCrouching ? CrouchEyeZ : StandingEyeZ;
- TargetLocalLocation.Y += LeanSideOffset;
- TargetRotation.Roll = LeanRoll;
- Apply smooth interpolation.
- Apply minimal procedural sway from velocity/time.

#### Blueprint editor hookup
- Replace default camera on BP_TDFPlayerCharacter with UTDFPlayerCameraComponent if not already.
- Set relative location to standing eye height.
- Tune crouch camera speed and sway values in Blueprint.

#### Test
- Walk: subtle sway.
- Sprint: stronger sway if enabled.
- Crouch: camera lowers smoothly.
- Hold breath: sway reduces.
- Lean: roll/offset interpolates.

#### Done criteria
- All camera presentation derives from C++ state.
- Blueprint only tunes values.
- No movement logic inside camera component.

- Common failure: Driving camera transitions from multiple Blueprint timelines creates conflicting transforms.
- Copy paste ai prompt: Implement UTDFPlayerCameraComponent for first-person camera presentation. It should read ATDFPlayerCharacter movement state and apply crouch height, lean roll/offset, and subtle sway with interpolation. Do not put movement authority in the camera component.

### 10 — Define interactable interface

- Id: 10
- Title: Define interactable interface
- Goal: Create a reusable C++ interaction contract for doors, switches, items, and future actors.
#### Classes files
- Interaction/DFInteractableInterface.h

#### Logic or pseudocode
- UINTERFACE(BlueprintType) UTDFInteractableInterface : UInterface
- class IDFInteractableInterface
- BlueprintNativeEvent bool CanInteract(const ATDFPlayerCharacter* Player) const;
- BlueprintNativeEvent FText GetInteractionPrompt(const ATDFPlayerCharacter* Player) const;
- BlueprintNativeEvent void Interact(ATDFPlayerCharacter* Player);
- BlueprintNativeEvent bool WantsPromptVisible(const ATDFPlayerCharacter* Player) const;
- Optional:
- BlueprintNativeEvent FName GetInteractionId() const;

#### Blueprint editor hookup
- No actor hookup yet.
- Blueprint classes may override prompt presentation later, but base authority should remain C++.

#### Test
- Compile interface.
- Create dummy Blueprint actor implementing interface.
- Confirm functions appear in Blueprint override menu.

#### Done criteria
- Any actor can expose interaction without coupling to player class internals.
- Interface includes prompt and execution paths.

- Common failure: Using casts to specific door/switch/item classes in the player interaction trace prevents extension.
- Copy paste ai prompt: Create a C++ BlueprintType interactable interface for Drowned Frequency. It must expose CanInteract, GetInteractionPrompt, Interact, WantsPromptVisible, and optional GetInteractionId as BlueprintNativeEvent methods. Do not implement specific interactables yet.

### 11 — Implement player interaction trace component

- Id: 11
- Title: Implement player interaction trace component
- Goal: Detect the focused interactable and trigger interactions without per-actor coupling.
#### Classes files
- Interaction/DFInteractionComponent.h/.cpp
- Player/DFPlayerCharacter.h/.cpp

#### Logic or pseudocode
- UTDFInteractionComponent derives from UActorComponent.
- Properties:
- InteractionDistance = 220 cm;
- TraceRadius = 8 cm;
- TraceChannel = Visibility or custom Interaction;
- FocusedActor weak pointer;
- OnFocusedInteractableChanged delegate;
- TickComponent:
- Start = Camera->GetComponentLocation();
- End = Start + Camera->GetForwardVector() * InteractionDistance;
- SphereTraceSingleByChannel;
- If hit actor implements UTDFInteractableInterface and CanInteract(Player), set focus.
- Else clear focus.
- TryInteract():
- If FocusedActor valid and CanInteract, Execute_Interact(FocusedActor, Player).

#### Blueprint editor hookup
- Add UTDFInteractionComponent to ATDFPlayerCharacter constructor.
- Bind InteractAction Started to InteractionComponent->TryInteract.
- Create custom collision channel Interaction if desired.
- Set interactable objects to block Interaction trace.

#### Test
- Place dummy interactable actor.
- Look at actor: focus changes.
- Look away: focus clears.
- Press interact: dummy actor receives Interact.
- Gamepad interact also works.

#### Done criteria
- Only one focused interactable at a time.
- Interaction depends on camera aim.
- No actor-specific code inside player.

- Common failure: Line trace with no radius feels too unforgiving for controller users.
- Copy paste ai prompt: Implement UTDFInteractionComponent as a C++ actor component on the player. It should sphere-trace from the first-person camera, detect actors implementing UTDFInteractableInterface, track focused actor, broadcast focus changes, and expose TryInteract. Bind InteractAction to TryInteract in ATDFPlayerCharacter.

### 12 — Create UI prompt widget contract

- Id: 12
- Title: Create UI prompt widget contract
- Goal: Show interaction prompts from C++ focus state while allowing Blueprint styling.
#### Classes files
- UI/DFInteractionPromptWidget.h/.cpp
- Player/DFPlayerController.h/.cpp
- Interaction/DFInteractionComponent.h/.cpp

#### Logic or pseudocode
- UTDFInteractionPromptWidget derives from UUserWidget.
- BlueprintImplementableEvent SetPromptText(FText Text);
- BlueprintImplementableEvent SetInputGlyph(FName InputActionName or ECommonInputType);
- BlueprintCallable ShowPrompt(FText Prompt);
- BlueprintCallable HidePrompt();
- ATDFPlayerController owns widget instance.
- On possession:
- Bind to pawn InteractionComponent->OnFocusedInteractableChanged.
- When focus valid: PromptWidget->ShowPrompt(GetInteractionPrompt).
- When focus cleared: PromptWidget->HidePrompt().

#### Blueprint editor hookup
- Create WBP_InteractionPrompt derived from UTDFInteractionPromptWidget.
- Add text block for prompt.
- Add optional image for input glyph.
- In BP_TDFPlayerController assign PromptWidgetClass.
- In widget Blueprint implement SetPromptText to update text block.
- Anchor prompt near screen center/lower center.

#### Test
- Look at dummy interactable.
- Prompt appears.
- Look away.
- Prompt hides.
- Switch between keyboard and controller if input glyph system exists; prompt should not show wrong hardcoded key.

#### Done criteria
- Prompt visibility follows focused interactable.
- Prompt text comes from interactable.
- Widget styling is Blueprint-owned.

- Common failure: Hardcoding 'Press E' in the widget breaks controller and rebinding support.
- Copy paste ai prompt: Implement the interaction prompt UI bridge. ATDFPlayerController should create a UTDFInteractionPromptWidget subclass, bind to the player's UTDFInteractionComponent focus-change delegate, and show/hide prompt text from the focused interactable. Keep widget visuals in Blueprint and avoid hardcoded key names.

### 13 — Add input glyph and device tracking foundation

- Id: 13
- Title: Add input glyph and device tracking foundation
- Goal: Prepare prompts to show keyboard or controller controls correctly.
#### Classes files
- Player/DFPlayerController.h/.cpp
- UI/DFInteractionPromptWidget.h/.cpp
- Input/DFInputGlyphTypes.h
- Optional DataAsset: UTDFInputGlyphSet

#### Logic or pseudocode
- Track last input device:
- enum ETDFInputDevice { KeyboardMouse, Gamepad };
- On any enhanced input action from gamepad source, set Gamepad.
- On mouse/key input, set KeyboardMouse.
- Simpler fallback:
- Override InputKey in PlayerController and inspect FKey:
- if Key.IsGamepadKey() LastDevice=Gamepad else KeyboardMouse.
- Broadcast OnInputDeviceChanged.
- Prompt widget receives action name + device and chooses glyph/text.
- Do not hardcode only E/A in interactable.

#### Blueprint editor hookup
- Create DA_InputGlyphSet with entries for Interact, Inspect, Back, Pause.
- Assign keyboard and controller icons/text.
- WBP_InteractionPrompt switches icon based on LastInputDevice.

#### Test
- Move mouse: prompt shows keyboard/mouse glyph.
- Touch controller stick/button: prompt switches to controller glyph.
- Interact still works after switching devices.

#### Done criteria
- Prompts are device-aware.
- Device switching does not require reopening UI.
- Prompt text remains action-based, not key-hardcoded.

- Common failure: Detecting device only once at BeginPlay makes prompts wrong after switching between controller and keyboard.
- Copy paste ai prompt: Add last-input-device tracking to ATDFPlayerController using InputKey or Enhanced Input source data. Broadcast device changes to UTDFInteractionPromptWidget so prompts can show keyboard/mouse or gamepad glyphs. Keep prompts action-based and compatible with rebinding.

### 14 — Implement door actor base

- Id: 14
- Title: Implement door actor base
- Goal: Create reusable interactive doors with C++ state and Blueprint presentation hooks.
#### Classes files
- World/DFDoorActor.h/.cpp
- Interaction/DFInteractableInterface.h
- Save/DFPersistentStateTypes.h

#### Logic or pseudocode
- ATDFDoorActor derives from AActor and implements IDFInteractableInterface.
- Components:
- USceneComponent* Root;
- UStaticMeshComponent* DoorMesh;
- UPROPERTY(EditInstanceOnly) FName PersistentId;
- UPROPERTY(EditDefaultsOnly) bool bStartsOpen;
- UPROPERTY(VisibleInstanceOnly) bool bIsOpen;
- UPROPERTY(EditDefaultsOnly) bool bIsLocked;
- UPROPERTY(EditDefaultsOnly) FName RequiredKeyId;
- CanInteract(Player): return !bIsAnimating;
- GetInteractionPrompt(Player):
- if bIsLocked && !Player.Inventory.HasKey(RequiredKeyId) return 'Locked';
- return bIsOpen ? 'Close' : 'Open';
- Interact(Player):
- if locked and missing key: OnDoorLockedFeedback(); return;
- SetOpen(!bIsOpen);
- SetOpen(bool): bIsOpen = NewValue; OnDoorStateChanged(bIsOpen);
- BlueprintImplementableEvent OnDoorStateChanged(bool bOpen);
- BlueprintImplementableEvent OnDoorLockedFeedback();

#### Blueprint editor hookup
- Create BP_Door_Base derived from ATDFDoorActor.
- Assign mesh.
- In Blueprint implement OnDoorStateChanged using Timeline or Level Sequence to rotate mesh.
- Set unique PersistentId per placed door.
- Use collision preset blocking player and interaction trace.

#### Test
- Place door in test map.
- Look at door: prompt says Open.
- Interact: door opens using Blueprint animation.
- Interact again: prompt says Close and door closes.
- Set locked with missing key: prompt or feedback shows locked and door does not open.

#### Done criteria
- Door open/locked truth lives in C++.
- Visual animation lives in Blueprint.
- Door supports key requirement without direct UI coupling.

- Common failure: Putting bIsOpen only inside Blueprint timeline prevents save/load and key logic from knowing real door state.
- Copy paste ai prompt: Implement ATDFDoorActor as a C++ interactable. It must store PersistentId, bIsOpen, bIsLocked, RequiredKeyId, implement CanInteract/GetInteractionPrompt/Interact, and expose BlueprintImplementableEvents for door animation and locked feedback. C++ owns state; Blueprint owns visuals.

### 15 — Implement switch actor base

- Id: 15
- Title: Implement switch actor base
- Goal: Create reusable switches that can affect doors or broadcast events without level Blueprint truth.
#### Classes files
- World/DFSwitchActor.h/.cpp
- Interaction/DFInteractableInterface.h

#### Logic or pseudocode
- ATDFSwitchActor derives from AActor and implements IDFInteractableInterface.
- Properties:
- FName PersistentId;
- bool bIsOn;
- bool bOneShot;
- TArray<TObjectPtr<AActor>> LinkedActors;
- FMulticastScriptDelegate OnSwitchChanged;
- Interact(Player):
- if bOneShot && bHasBeenUsed return;
- SetSwitchState(!bIsOn or true for one-shot);
- for Actor in LinkedActors:
- if Actor implements known receiver interface, call receive;
- BlueprintImplementableEvent OnSwitchVisualChanged(bool bOn);
- Optional interface IDFSwitchReceiverInterface::OnSwitchStateChanged(FName SwitchId, bool bOn);

#### Blueprint editor hookup
- Create BP_Switch_Base derived from ATDFSwitchActor.
- Assign mesh and interaction collision.
- Implement OnSwitchVisualChanged for lever/button animation and sound.
- Link target doors or receiver actors in Details panel.

#### Test
- Place switch and linked door.
- Interact with switch.
- Switch changes state and linked receiver responds.
- One-shot switch cannot be reused if bOneShot true.

#### Done criteria
- Switch state is C++ truth.
- Visual/sound feedback is Blueprint.
- Linked actors respond without Level Blueprint state.

- Common failure: Directly calling a specific door Blueprint custom event from the switch prevents reuse and save/load.
- Copy paste ai prompt: Implement ATDFSwitchActor as a reusable C++ interactable with PersistentId, bIsOn, bOneShot, LinkedActors, and a BlueprintImplementableEvent for visual changes. It should broadcast or notify receiver actors when toggled. Do not store switch truth in Level Blueprint.

### 16 — Create item data asset

- Id: 16
- Title: Create item data asset
- Goal: Define all item metadata in assets instead of actor classes.
#### Classes files
- Inventory/DFItemDataAsset.h/.cpp

#### Logic or pseudocode
- UENUM ETDFItemType { Inspectable, KeyItem, ConsumableFuture };
- UCLASS(BlueprintType) UTDFItemDataAsset : UPrimaryDataAsset
- Properties:
- FName ItemId;
- FText DisplayName;
- FText ShortDescription;
- ETDFItemType ItemType;
- TSoftObjectPtr<UStaticMesh> InspectionMesh;
- TSoftObjectPtr<UTexture2D> Icon;
- bool bCanInspect;
- bool bCanPickUp;
- bool bPersistsInInventory;
- FName KeyId;

#### Blueprint editor hookup
- Create DA_Item_* assets for first test items.
- For a key item, set ItemType=KeyItem, bCanPickUp=true, bPersistsInInventory=true, KeyId='BasementKey' or similar.
- For inspectable-only object, set bCanInspect=true and bPersistsInInventory=false.

#### Test
- Open each data asset and verify required fields are not empty.
- Compile after adding soft references.

#### Done criteria
- Items are data-defined.
- Actors can reference item data without duplicating display names or key ids.

- Common failure: Using actor class names as item IDs makes save data fragile after renames.
- Copy paste ai prompt: Create UTDFItemDataAsset as a UPrimaryDataAsset for Drowned Frequency items. Include stable ItemId, display name, description, item type, inspection mesh, icon, pickup/inspect flags, persistence flag, and KeyId for key items. Do not implement pickup or inspection behavior yet.

### 17 — Implement inventory component for key items

- Id: 17
- Title: Implement inventory component for key items
- Goal: Store key item truth on the player in C++.
#### Classes files
- Inventory/DFInventoryComponent.h/.cpp
- Inventory/DFItemDataAsset.h/.cpp
- Player/DFPlayerCharacter.h/.cpp

#### Logic or pseudocode
- UTDFInventoryComponent derives from UActorComponent.
- Properties:
- TSet<FName> OwnedItemIds;
- TSet<FName> OwnedKeyIds;
- AddItem(UTDFItemDataAsset* Item):
- if !Item return false;
- OwnedItemIds.Add(Item->ItemId);
- if Item->ItemType == KeyItem and Item->KeyId != NAME_None: OwnedKeyIds.Add(Item->KeyId);
- Broadcast OnInventoryChanged.
- HasItem(FName ItemId): return OwnedItemIds.Contains(ItemId);
- HasKey(FName KeyId): return OwnedKeyIds.Contains(KeyId);
- RemoveItem only if design requires; avoid for permanent keys.

#### Blueprint editor hookup
- Add UTDFInventoryComponent to ATDFPlayerCharacter constructor.
- Expose read-only inventory queries to Blueprint for UI.

#### Test
- Call AddItem with test key asset.
- HasItem returns true for ItemId.
- HasKey returns true for KeyId.
- Door requiring that KeyId opens after item added.

#### Done criteria
- Key possession is queried through UTDFInventoryComponent.
- Doors do not maintain their own copy of player key state.

- Common failure: Checking key possession by searching visible world actors fails after pickup/despawn/save.
- Copy paste ai prompt: Implement UTDFInventoryComponent for player-owned item and key truth. It should store stable ItemIds and KeyIds, expose AddItem, HasItem, HasKey, and OnInventoryChanged. Add it to ATDFPlayerCharacter. Keep UI and pickup visuals separate.

### 18 — Implement world item actor

- Id: 18
- Title: Implement world item actor
- Goal: Allow placed items to be inspected and/or picked up through the common interaction system.
#### Classes files
- World/DFWorldItemActor.h/.cpp
- Inventory/DFItemDataAsset.h/.cpp
- Interaction/DFInteractableInterface.h

#### Logic or pseudocode
- ATDFWorldItemActor derives from AActor and implements IDFInteractableInterface.
- Components:
- USceneComponent* Root;
- UStaticMeshComponent* Mesh;
- Properties:
- UTDFItemDataAsset* ItemData;
- FName PersistentId;
- bool bHasBeenPickedUp;
- GetInteractionPrompt:
- if ItemData->bCanPickUp return FText::Format('Take {0}', DisplayName);
- if ItemData->bCanInspect return FText::Format('Inspect {0}', DisplayName);
- Interact(Player):
- if bCanPickUp:
- Player.Inventory.AddItem(ItemData);
- bHasBeenPickedUp = true;
- SetActorHiddenInGame(true); SetActorEnableCollision(false);
- OnPickedUp(Player);
- else if bCanInspect:
- PlayerController->OpenInspection(ItemData);
- BlueprintImplementableEvent OnPickedUp;

#### Blueprint editor hookup
- Create BP_WorldItem_Base derived from ATDFWorldItemActor.
- Assign mesh from ItemData or manually in Blueprint.
- Place key item in map and assign DA_Item_Key.
- Set collision to block interaction trace.

#### Test
- Look at item: prompt appears.
- Interact with pickup item: inventory gains item, actor disappears/collision disables.
- Door requiring key can now open.
- Interact with inspect-only item: inspection mode opens instead of pickup.

#### Done criteria
- World item behavior comes from item data.
- Pickup changes inventory truth.
- No physics pickup is introduced.

- Common failure: Destroying pickup actors immediately before save state records pickup can cause the item to respawn after reload.
- Copy paste ai prompt: Implement ATDFWorldItemActor as a C++ interactable using UTDFItemDataAsset. It should support pickup into UTDFInventoryComponent for key items and opening inspection for inspectable items. It must not implement physics pickup. Include PersistentId and picked-up state for future save support.

### 19 — Implement item inspection mode state

- Id: 19
- Title: Implement item inspection mode state
- Goal: Create a controlled inspection mode that disables normal movement interaction and shows item details.
#### Classes files
- Player/DFPlayerController.h/.cpp
- UI/DFInspectionWidget.h/.cpp
- Inventory/DFItemDataAsset.h/.cpp

#### Logic or pseudocode
- ATDFPlayerController properties:
- bool bIsInspectingItem;
- TObjectPtr<UTDFItemDataAsset> CurrentInspectedItem;
- OpenInspection(UTDFItemDataAsset* Item):
- if !Item or !Item->bCanInspect return;
- bIsInspectingItem = true;
- CurrentInspectedItem = Item;
- SetIgnoreMoveInput(true);
- SetInputMode(FInputModeGameAndUI or GameOnly depending widget design);
- Create/show WBP_Inspection;
- Widget receives DisplayName, Description, InspectionMesh/Icon.
- CloseInspection():
- bIsInspectingItem=false;
- CurrentInspectedItem=null;
- Hide widget;
- SetIgnoreMoveInput(false);
- SetInputMode(GameOnly);
- Bind BackAction or InspectAction Completed to close while inspecting.

#### Blueprint editor hookup
- Create WBP_Inspection derived from UTDFInspectionWidget.
- Add name text, description text, optional mesh preview viewport or rendered image.
- Assign widget class in BP_TDFPlayerController.
- Map Back to Escape/B gamepad face button/right mouse depending project convention.

#### Test
- Interact with inspectable item.
- Movement input is ignored.
- Look/mouse behavior follows chosen design.
- Back closes inspection.
- After closing, movement and interaction work again.

#### Done criteria
- Inspection is a controller-owned mode.
- Normal interaction cannot fire while inspection is open.
- Controller can close inspection.

- Common failure: Leaving movement input enabled lets the player walk away while inspecting, desyncing UI and world focus.
- Copy paste ai prompt: Implement item inspection mode in ATDFPlayerController. OpenInspection should accept UTDFItemDataAsset, show UTDFInspectionWidget, store CurrentInspectedItem, ignore movement input, and bind Back/Inspect to close. CloseInspection should restore game-only input and movement. Blueprint owns widget visuals.

### 20 — Add inspection object rotation support

- Id: 20
- Title: Add inspection object rotation support
- Goal: Allow controller and mouse rotation of inspected items without world physics pickup.
#### Classes files
- UI/DFInspectionWidget.h/.cpp
- Player/DFPlayerController.h/.cpp
- Input/DFInputConfig.h/.cpp

#### Logic or pseudocode
- While bIsInspectingItem:
- Route IA_Look Axis2D to inspection widget instead of player camera.
- InspectionWidget exposes RotatePreview(FVector2D Delta).
- Yaw += Delta.X * InspectRotateSpeed;
- Pitch = Clamp(Pitch + Delta.Y * InspectRotateSpeed, MinPitch, MaxPitch);
- Apply to preview mesh/scene capture actor if using 3D preview.
- Do not attach world actor to player.
- Do not simulate physics.

#### Blueprint editor hookup
- In WBP_Inspection add preview mesh using UI material, viewport widget, or scene capture approach.
- Expose rotation variables to Blueprint.
- Map right stick and mouse movement to same look action.

#### Test
- Open inspectable item.
- Move mouse: preview rotates.
- Move right stick: preview rotates.
- Close inspection: look input controls camera again.

#### Done criteria
- Inspection rotation works with mouse and controller.
- No world actor physics manipulation is used.

- Common failure: Rotating the actual placed world item causes save/collision/lighting side effects.
- Copy paste ai prompt: Add inspection preview rotation. While ATDFPlayerController is in item inspection mode, route IA_Look Axis2D to UTDFInspectionWidget::RotatePreview instead of player camera. Support mouse and right stick. Rotate only the UI/preview representation, not the world actor, and do not use physics pickup.

### 21 — Add locked door plus key item integration

- Id: 21
- Title: Add locked door plus key item integration
- Goal: Connect inventory key truth to door unlocking.
#### Classes files
- World/DFDoorActor.h/.cpp
- Inventory/DFInventoryComponent.h/.cpp
- World/DFWorldItemActor.h/.cpp

#### Logic or pseudocode
- In ATDFDoorActor::Interact:
- Inventory = Player->FindComponentByClass<UTDFInventoryComponent>();
- if bIsLocked:
- if RequiredKeyId != NAME_None && Inventory && Inventory->HasKey(RequiredKeyId):
- bIsLocked = false;
- OnDoorUnlocked();
- SetOpen(true);
- else:
- OnDoorLockedFeedback();
- return;
- else SetOpen(!bIsOpen);
- GetInteractionPrompt:
- if locked and missing key: return LockedPrompt;
- if locked and has key: return UnlockPrompt;
- return open/close prompt;

#### Blueprint editor hookup
- Create DA_Item_BasementKey with KeyId=BasementKey.
- Place BP_WorldItem_Base using DA_Item_BasementKey.
- Place BP_Door_Base with bIsLocked=true and RequiredKeyId=BasementKey.
- Implement OnDoorUnlocked sound/animation in Blueprint if desired.

#### Test
- Try locked door before key: does not open.
- Pick up key.
- Try locked door again: unlocks and opens.
- Prompt changes appropriately.

#### Done criteria
- Door checks inventory component by key id.
- Key item pickup unlocks target door.
- Blueprint only handles feedback.

- Common failure: Using DisplayName or item object pointer for key matching breaks localization and asset duplication.
- Copy paste ai prompt: Integrate key items with ATDFDoorActor. Locked doors should check UTDFInventoryComponent::HasKey(RequiredKeyId). If the player has the key, clear bIsLocked, call OnDoorUnlocked, and open. If not, call OnDoorLockedFeedback. Match by stable FName KeyId only.

### 22 — Add persistent state IDs and save structs

- Id: 22
- Title: Add persistent state IDs and save structs
- Goal: Prepare doors, switches, and picked-up items for save/load without implementing full save flow yet.
#### Classes files
- Save/DFPersistentStateTypes.h
- World/DFDoorActor.h/.cpp
- World/DFSwitchActor.h/.cpp
- World/DFWorldItemActor.h/.cpp

#### Logic or pseudocode
- USTRUCT FTDFDoorPersistentState { FName PersistentId; bool bIsOpen; bool bIsLocked; };
- USTRUCT FTDFSwitchPersistentState { FName PersistentId; bool bIsOn; bool bHasBeenUsed; };
- USTRUCT FTDFWorldItemPersistentState { FName PersistentId; bool bHasBeenPickedUp; };
- Actors expose:
- GetPersistentState() const;
- ApplyPersistentState(State);
- Validation:
- On BeginPlay warn if PersistentId is NAME_None for save-relevant actor.

#### Blueprint editor hookup
- For every placed save-relevant door/switch/item, set unique PersistentId in Details panel.
- Adopt naming convention: MapName_ActorType_Number, e.g., RadioStation_Door_001.

#### Test
- Place two doors with same PersistentId and verify warning plan or editor validation catches duplicate later.
- Toggle door/switch and call GetPersistentState in debug.
- Apply state and confirm actor visuals update through Blueprint events.

#### Done criteria
- Each interactive actor can export/import its truth.
- PersistentId is explicit and stable.
- No save system dependency is required yet.

- Common failure: Saving actor names instead of explicit PersistentId breaks when level actors are renamed or duplicated.
- Copy paste ai prompt: Add persistent state structs and actor methods for doors, switches, and world items. Each save-relevant actor should have an explicit FName PersistentId, GetPersistentState, ApplyPersistentState, and a BeginPlay warning if PersistentId is missing. Do not implement the full SaveGame manager yet.

### 23 — Implement remapping-ready input settings

- Id: 23
- Title: Implement remapping-ready input settings
- Goal: Make controller and keyboard rebinding possible without rewriting prompts later.
#### Classes files
- Input/DFInputConfig.h/.cpp
- Input/DFPlayerInputSettingsSave.h/.cpp
- Player/DFPlayerController.h/.cpp

#### Logic or pseudocode
- Preferred UE5 path:
- Use Enhanced Input Player Mappable Input Config or Mapping Context entries marked player-mappable.
- Fallback project path:
- UCLASS UTDFPlayerInputSettingsSave : USaveGame
- TMap<FName, FKey> KeyboardOverrides;
- TMap<FName, FKey> GamepadOverrides;
- On load:
- Apply overrides to EnhancedInputLocalPlayerSubsystem or user settings.
- Prompt glyph reads resolved key for action, not default hardcoded key.
- Expose RebindAction(ActionName, NewKey, Device).
- Expose ResetBindingsToDefault().

#### Blueprint editor hookup
- Mark mapping entries as player mappable where UE version supports it.
- Create options menu later that calls RebindAction.
- Prompt glyph set must resolve current mapping for Interact/Inspect/Back.

#### Test
- Change Interact keyboard key from E to F using debug call.
- Prompt updates to F.
- Interact works on F and no longer requires E if override replaces binding.
- Reset restores default.

#### Done criteria
- Input binding truth is not hardcoded into UI.
- Rebinding works for at least Interact on keyboard and controller path is designed.

- Common failure: Showing glyphs from default mapping assets while gameplay uses remapped keys makes prompts lie.
- Copy paste ai prompt: Add a remapping-ready input settings layer for Enhanced Input. Prefer UE5 player-mappable input settings; otherwise create UTDFPlayerInputSettingsSave with keyboard/gamepad overrides. Provide RebindAction and ResetBindingsToDefault. Ensure UI prompts resolve the active binding rather than hardcoded defaults.

### 24 — Add controller-specific movement polish

- Id: 24
- Title: Add controller-specific movement polish
- Goal: Make first-person movement and interaction usable with analog sticks.
#### Classes files
- Player/DFPlayerCharacter.h/.cpp
- Input/DFInputConfig.h/.cpp
- Interaction/DFInteractionComponent.h/.cpp

#### Logic or pseudocode
- Movement:
- Respect analog magnitude: AddMovementInput direction scaled by Axis length.
- Look:
- Use separate GamepadLookYawRate and GamepadLookPitchRate if input source is gamepad.
- Optional acceleration curve for right stick.
- Interaction:
- Use small sphere trace radius instead of strict line trace.
- Optional FocusGraceTime = 0.1 sec so prompt does not flicker when stick aim drifts.
- Prompt:
- Use controller glyphs when LastInputDevice=Gamepad.

#### Blueprint editor hookup
- Tune gamepad look sensitivity separately from mouse.
- Set interaction trace radius 8-15 cm.
- Set focus grace time small enough not to interact through movement.

#### Test
- Navigate test room with controller only.
- Look at small switch with right stick.
- Prompt remains stable enough to interact.
- Mouse sensitivity unaffected by controller tuning.

#### Done criteria
- Controller-only play can move, look, crouch, sprint, lean, hold breath, inspect, interact, and back out.
- Prompt targeting is not pixel-perfect.

- Common failure: Using mouse-style raw look scaling for gamepad makes camera either too slow near center or too fast at max deflection.
- Copy paste ai prompt: Polish controller support for Drowned Frequency. Add separate gamepad look sensitivity/rates, preserve analog movement magnitude, use forgiving interaction sphere trace radius, and optional focus grace time. Ensure controller-only play supports every required action.

### 25 — Create vertical slice test map

- Id: 25
- Title: Create vertical slice test map
- Goal: Verify all mechanics work together in one controlled room.
#### Classes files
- Map: M_Test_PlayerInteraction
- Blueprints: BP_Door_Base, BP_Switch_Base, BP_WorldItem_Base, WBP_InteractionPrompt, WBP_Inspection

#### Logic or pseudocode
- Test room contents:
- Flat floor.
- One locked door requiring BasementKey.
- One key item with DA_Item_BasementKey.
- One inspect-only note/radio part.
- One switch linked to an unlocked door or light.
- One narrow wall area for lean test.
- One low object/space for crouch test.
- No universal physics pickup objects.

#### Blueprint editor hookup
- Place player start.
- Assign GameMode with BP_TDFPlayerCharacter and BP_TDFPlayerController.
- Assign widget classes on controller.
- Set unique PersistentIds on placed door/switch/item actors.

#### Test
- Keyboard/mouse pass:
- Move, look, crouch, sprint, lean, hold breath.
- Prompt appears on item/door/switch.
- Inspect item and close inspection.
- Try locked door before key.
- Pick up key.
- Open locked door.
- Toggle switch.
- Controller pass repeats same flow without keyboard.

#### Done criteria
- Every mechanic works in the same map.
- No Level Blueprint gameplay truth is required.
- No physics pickup behavior exists.
- Both keyboard/mouse and controller complete the slice.

- Common failure: Testing each mechanic in isolation misses input mode conflicts between inspection UI, prompt focus, and movement.
- Copy paste ai prompt: Create a vertical slice test map for Drowned Frequency mechanics. Include one locked key door, one key item, one inspectable item, one switch, crouch space, and lean wall. Verify the complete flow with keyboard/mouse and controller. Do not put gameplay truth in Level Blueprint and do not add physics pickup.

### 26 — Add automation/manual test checklist before expanding content

- Id: 26
- Title: Add automation/manual test checklist before expanding content
- Goal: Freeze expected behavior so future content additions do not regress core mechanics.
#### Classes files
- Optional: Source/DrownedFrequencyTests/InteractionSpec.cpp
- Optional: QA/CorePlayerMechanicsChecklist.md if documentation files are allowed later

#### Logic or pseudocode
- Automation tests where practical:
- Inventory AddItem grants ItemId and KeyId.
- Door CanInteract returns true when not animating.
- Locked door missing key does not open.
- Locked door with key unlocks and opens.
- Switch one-shot cannot retrigger.
- Manual checklist for input/camera/UI because PIE automation is heavier.
- Follow red-green-refactor for implementation: write failing test first for inventory and door state logic.

#### Blueprint editor hookup
- Do not rely on screenshots as proof.
- Record manual pass/fail for keyboard/mouse and controller on the vertical slice map.

#### Test
- Run C++ automation/unit tests for pure logic.
- Run manual PIE checklist for movement/camera/UI/input.
- Fix any failure before adding new levels/items.

#### Done criteria
- Core logic has automated coverage where practical.
- Manual test flow is repeatable.
- Feature expansion starts only after vertical slice passes.

- Common failure: Skipping tests until content production makes controller and UI regressions expensive to isolate.
- Copy paste ai prompt: Add tests/checklists for the Drowned Frequency core mechanics. Use automated C++ tests where practical for inventory, door lock/key behavior, and switch state. Use a manual PIE checklist for camera, movement, prompts, inspection UI, and controller flow. Follow test-first red-green-refactor for new logic.


## Recommended implementation order
- 1. Folder/class skeletons
- 2. Enhanced Input assets
- 3. PlayerController mapping contexts
- 4. Baseline movement/look/crouch
- 5. Movement state model
- 6. Sprint
- 7. Lean
- 8. Hold breath
- 9. Camera component
- 10. Interactable interface
- 11. Interaction trace component
- 12. Prompt widget
- 13. Device/glyph tracking
- 14. Door actor
- 15. Switch actor
- 16. Item data asset
- 17. Inventory component
- 18. World item actor
- 19. Inspection mode
- 20. Inspection rotation
- 21. Locked door/key integration
- 22. Persistent state structs
- 23. Remapping support
- 24. Controller polish
- 25. Vertical slice test map
- 26. Test checklist/automation

## Non goals
- No universal physics pickup.
- No implementation performed in this task.
- No files created or modified.
- No full SaveGame manager yet; only save-ready state contracts.
- No content-heavy UI styling beyond widget contracts.
- No combat, stamina, health, or inventory menu unless separately requested.

