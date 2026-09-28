# Project Foundation

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

- What i did: Created a granular UE5 project-foundation build sequence for The Drowned Frequency from blank C++ project to first packaged Windows build.
### What it covers
- Exact editor actions
- Folder, class, asset, and file names
- Build.cs module setup
- Git/Git LFS source-control setup
- Enhanced Input setup
- Persistent level plus streamed sectors foundation
- C++ mechanics / Blueprint presentation / Data Asset tuning workflow
- Test step after every task
- Likely failure modes
- Copy-paste AI prompt per task

- Files created or modified: None. This is a planning/build-sequence deliverable only.
- Issues encountered: None.

## Assumptions

- Engine: UE5 stable release
- Project type: C++
- Target: Windows packaged build
### Development hosts
- Windows
- Mac allowed for development, but Windows packaging should be verified on Windows or with an explicit Windows toolchain/CI setup

### Game

- Title: The Drowned Frequency
- Genre: Solo first-person UE5 horror
- Length: 4-6 hours
- Combat: None
- Core mechanic: Authored sound recording mechanic
- Architecture: C++ mechanics, Blueprint presentation, Data Assets for tuning, persistent level plus streamed sectors

- Project name: DrownedFrequency
- Module name: DrownedFrequency
- Content root: Content/Game
- Source root: Source/DrownedFrequency

## Sequence

### 1 — Create the blank C++ UE5 project

- Id: 1
- Title: Create the blank C++ UE5 project
- Goal: Create a clean C++ project with the final project/module name.
#### Editor actions
- Open Epic Games Launcher or Unreal Project Browser.
- Choose Games > Blank.
- Set Project Type to C++.
- Set Target Platform to Desktop.
- Set Quality Preset to Maximum or Scalable; choose one and keep it consistent.
- Disable Starter Content.
- Disable Raytracing unless the project explicitly needs it.
- Set project name to DrownedFrequency.
- Create the project in the chosen development directory.

#### Files or assets
- DrownedFrequency.uproject
- Source/DrownedFrequency/DrownedFrequency.Build.cs
- Source/DrownedFrequency/DrownedFrequency.cpp
- Source/DrownedFrequency/DrownedFrequency.h

#### Test
- Let the editor open successfully.
- Close the editor.
- Reopen DrownedFrequency.uproject.
- Confirm the project opens without compile prompts or missing module errors.

#### Likely failure modes
- Project path contains unusual characters or cloud-sync paths, causing build issues.
- C++ toolchain is missing: Visual Studio Build Tools on Windows or Xcode on Mac.
- Project name contains spaces, causing awkward module/class paths.

- Copy paste ai prompt: Create a new Unreal Engine 5 stable C++ blank project named DrownedFrequency. Use no Starter Content. Target Desktop. Do not add gameplay yet. Verify the editor opens, closes, and reopens cleanly. Report the exact project path and any toolchain errors.

### 2 — Create the first source-control baseline

- Id: 2
- Title: Create the first source-control baseline
- Goal: Initialize Git before any assets accumulate.
#### Editor actions
- Close Unreal Editor.
- Open a terminal in the project root containing DrownedFrequency.uproject.
- Run git init.
- Create a UE5-safe .gitignore.
- Create a .gitattributes with Git LFS rules for binary Unreal assets.
- Run git lfs install if Git LFS is available.
- Track Unreal binary asset types with LFS.

#### Files or assets
- .gitignore
- .gitattributes

#### Recommended gitignore entries
- Binaries/
- DerivedDataCache/
- Intermediate/
- Saved/
- .vs/
- .vscode/.browse.VC.db*
- .vscode/ipch/
- *.VC.db
- *.opensdf
- *.sdf
- *.sln
- *.suo
- *.xcodeproj/
- *.xcworkspace/

#### Recommended gitattributes entries
- *.uasset filter=lfs diff=lfs merge=lfs -text
- *.umap filter=lfs diff=lfs merge=lfs -text
- *.wav filter=lfs diff=lfs merge=lfs -text
- *.flac filter=lfs diff=lfs merge=lfs -text
- *.mp3 filter=lfs diff=lfs merge=lfs -text
- *.mp4 filter=lfs diff=lfs merge=lfs -text
- *.mov filter=lfs diff=lfs merge=lfs -text
- *.fbx filter=lfs diff=lfs merge=lfs -text
- *.png filter=lfs diff=lfs merge=lfs -text
- *.tga filter=lfs diff=lfs merge=lfs -text
- *.exr filter=lfs diff=lfs merge=lfs -text

#### Test
- Run git status.
- Confirm ignored directories such as Intermediate and Saved are not staged.
- Run git lfs track and confirm uasset and umap patterns appear.
- Commit with message: Initial UE5 project baseline.

#### Likely failure modes
- Git LFS is not installed.
- Intermediate, Binaries, or Saved accidentally staged.
- Large .uasset files committed without LFS.

- Copy paste ai prompt: In the DrownedFrequency project root, initialize Git and Git LFS for a UE5 C++ project. Add a UE5 .gitignore and .gitattributes that LFS-tracks .uasset, .umap, audio, video, texture, and FBX files. Do not commit Intermediate, Saved, Binaries, DerivedDataCache, IDE databases, or generated solution/workspace files. Verify with git status and git lfs track, then create an initial commit.

### 3 — Generate IDE files and compile the empty project

- Id: 3
- Title: Generate IDE files and compile the empty project
- Goal: Confirm the C++ toolchain works before adding code.
#### Editor actions
- Right-click DrownedFrequency.uproject.
- Choose Generate Visual Studio project files on Windows or Generate Xcode project on Mac.
- Open the generated IDE project.
- Build the Development Editor target for the host platform.
- Open the editor after the build succeeds.

#### Files or assets
- DrownedFrequency.sln on Windows, ignored by Git
- DrownedFrequency.xcodeproj on Mac, ignored by Git

#### Test
- Build target DrownedFrequencyEditor Development.
- Confirm 0 compile errors.
- Open editor and press Play in the blank default map.

#### Likely failure modes
- Missing Windows SDK or MSVC workload.
- Mac has Xcode command line tools but not full Xcode selected.
- Generated IDE files are accidentally staged despite being ignored.

- Copy paste ai prompt: Generate IDE files for DrownedFrequency, build the Development Editor target, and open the editor. Do not add gameplay code. If building on Mac, use the Mac editor target only; note that final Windows packaging must still be verified on Windows or CI. Report compile errors exactly if any.

### 4 — Set basic project metadata and maps policy

- Id: 4
- Title: Set basic project metadata and maps policy
- Goal: Establish project identity and avoid accidental default-map drift.
#### Editor actions
- Open Unreal Editor.
- Go to Edit > Project Settings > Description.
- Set Project Name to The Drowned Frequency.
- Set Company Name if known, otherwise leave blank.
- Go to Project Settings > Maps & Modes.
- Do not set final maps yet; this task only verifies where map defaults will be configured later.

#### Files or assets
- Config/DefaultGame.ini

#### Test
- Close and reopen Project Settings.
- Confirm project name persisted.

#### Likely failure modes
- Changing map defaults before maps exist creates broken soft references.
- Editing generated config files by hand while editor is open can be overwritten.

- Copy paste ai prompt: In DrownedFrequency, set basic Project Settings metadata for project name The Drowned Frequency. Do not set default maps yet because the project maps do not exist. Verify the setting persists after closing and reopening Project Settings.

### 5 — Create the content folder skeleton

- Id: 5
- Title: Create the content folder skeleton
- Goal: Create a predictable asset structure before importing or authoring assets.
#### Editor actions
- In Content Browser, create folder Content/Game.
- Inside Content/Game, create folders: Art, Audio, Blueprints, Data, Dev, Input, Maps, Materials, UI.
- Inside Content/Game/Blueprints, create folders: Actors, Characters, Components, Game, Player, World.
- Inside Content/Game/Audio, create folders: Recordings, SFX, Ambience, Voice, Mix.
- Inside Content/Game/Maps, create folders: Persistent, Sectors, Test.

#### Files or assets
- Content/Game/Art
- Content/Game/Audio
- Content/Game/Blueprints
- Content/Game/Data
- Content/Game/Dev
- Content/Game/Input
- Content/Game/Maps
- Content/Game/Materials
- Content/Game/UI

#### Test
- Right-click Content/Game and choose Fix Up Redirectors in Folder.
- Save All.
- Close and reopen editor.
- Confirm folders remain.

#### Likely failure modes
- Assets later created outside _TDF, fragmenting the project.
- Redirectors left behind after renames.
- Folder names with spaces causing inconsistent path conventions.

- Copy paste ai prompt: In the UE5 editor for DrownedFrequency, create the Content/Game folder skeleton with Art, Audio, Blueprints, Data, Dev, Input, Maps, Materials, and UI. Add the listed subfolders for Blueprints, Audio, and Maps. Save all, fix redirectors under _TDF, and verify the folders persist after reopening.

### 6 — Commit the clean content skeleton

- Id: 6
- Title: Commit the clean content skeleton
- Goal: Checkpoint asset layout separately from gameplay code.
#### Editor actions
- Close the editor.
- Run git status.
- Stage Config changes and Content/Game folder assets if any folder placeholder files were generated.
- Commit with message: Add project content folder skeleton.

#### Files or assets
- Config/DefaultGame.ini
- Content/Game/**

#### Test
- Run git status.
- Confirm working tree is clean.

#### Likely failure modes
- Empty folders may not appear in Git unless Unreal generated assets inside them.
- Saved or Intermediate files accidentally staged.

- Copy paste ai prompt: Commit the current DrownedFrequency source-control state after creating the content folder skeleton. Only stage intended Config and Content/Game changes. Do not stage Saved, Intermediate, Binaries, DerivedDataCache, IDE files, or local user settings. Verify git status is clean after the commit.

### 7 — Enable and verify Enhanced Input plugin

- Id: 7
- Title: Enable and verify Enhanced Input plugin
- Goal: Ensure the project uses UE5 Enhanced Input, not legacy input mappings.
#### Editor actions
- Open Unreal Editor.
- Go to Edit > Plugins.
- Search for Enhanced Input.
- Confirm Enhanced Input is enabled.
- Restart editor if prompted.
- Go to Edit > Project Settings > Input.
- Confirm Default Player Input Class is EnhancedPlayerInput if visible.
- Confirm Default Input Component Class is EnhancedInputComponent if visible.

#### Files or assets
- DrownedFrequency.uproject
- Config/DefaultInput.ini

#### Test
- Restart editor.
- Open Plugins and confirm Enhanced Input remains enabled.

#### Likely failure modes
- Plugin enabled in editor but module missing from Build.cs, causing C++ include/link errors later.
- Legacy input mappings used accidentally.
- Editor restart skipped after enabling plugin.

- Copy paste ai prompt: Enable and verify the Enhanced Input plugin in DrownedFrequency. In Project Settings > Input, set or confirm EnhancedPlayerInput and EnhancedInputComponent are the default input classes where available. Restart if required and verify the plugin remains enabled.

### 8 — Update Build.cs modules for foundation systems

- Id: 8
- Title: Update Build.cs modules for foundation systems
- Goal: Add the C++ dependencies needed for first-person movement, Enhanced Input, UI hooks, and later audio-recording presentation.
#### Editor actions
- Close Unreal Editor.
- Open Source/DrownedFrequency/DrownedFrequency.Build.cs.
- Set PublicDependencyModuleNames to include: Core, CoreUObject, Engine, InputCore, EnhancedInput.
- Set PrivateDependencyModuleNames to include: UMG, Slate, SlateCore.
- Do not add unnecessary systems yet.

#### Files or assets
- Source/DrownedFrequency/DrownedFrequency.Build.cs

#### Build cs modules

##### Public
- Core
- CoreUObject
- Engine
- InputCore
- EnhancedInput

##### Private
- UMG
- Slate
- SlateCore


#### Test
- Regenerate IDE files.
- Build DrownedFrequencyEditor Development.
- Open editor.

#### Likely failure modes
- Typo in EnhancedInput module name.
- Adding modules to the wrong target file instead of Build.cs.
- Forgetting to regenerate IDE files after changing module dependencies.

- Copy paste ai prompt: Modify Source/DrownedFrequency/DrownedFrequency.Build.cs so PublicDependencyModuleNames contains Core, CoreUObject, Engine, InputCore, and EnhancedInput, and PrivateDependencyModuleNames contains UMG, Slate, and SlateCore. Regenerate IDE files, build the Development Editor target, and verify the editor opens.

### 9 — Create the core C++ gameplay classes

- Id: 9
- Title: Create the core C++ gameplay classes
- Goal: Establish native class names before Blueprint presentation subclasses are created.
#### Editor actions
- In Unreal Editor, go to Tools > New C++ Class.
- Create TDFGameModeBase derived from GameModeBase.
- Create TDFPlayerController derived from PlayerController.
- Create TDFCharacter derived from Character.
- Create TDFRecordingComponent derived from ActorComponent.
- Create TDFInteractionComponent derived from ActorComponent.
- Create TDFRecordingTuningData derived from PrimaryDataAsset or DataAsset.
- Let the editor compile or close editor and compile from IDE.

#### Files or assets
- Source/DrownedFrequency/Public/Game/TDFGameModeBase.h
- Source/DrownedFrequency/Private/Game/TDFGameModeBase.cpp
- Source/DrownedFrequency/Public/Player/TDFPlayerController.h
- Source/DrownedFrequency/Private/Player/TDFPlayerController.cpp
- Source/DrownedFrequency/Public/Characters/TDFCharacter.h
- Source/DrownedFrequency/Private/Characters/TDFCharacter.cpp
- Source/DrownedFrequency/Public/Components/TDFRecordingComponent.h
- Source/DrownedFrequency/Private/Components/TDFRecordingComponent.cpp
- Source/DrownedFrequency/Public/Components/TDFInteractionComponent.h
- Source/DrownedFrequency/Private/Components/TDFInteractionComponent.cpp
- Source/DrownedFrequency/Public/Data/TDFRecordingTuningData.h
- Source/DrownedFrequency/Private/Data/TDFRecordingTuningData.cpp

#### Test
- Build DrownedFrequencyEditor Development.
- Open editor.
- Confirm each class appears in Content Browser under C++ Classes > DrownedFrequency.

#### Likely failure modes
- Classes created in the root Source folder instead of organized Public/Private subfolders.
- Hot Reload creates duplicate class artifacts; prefer full editor restart after new classes.
- PrimaryDataAsset include/module issues if headers are missing.

- Copy paste ai prompt: Create these native UE5 C++ classes for DrownedFrequency without implementing gameplay yet: TDFGameModeBase : AGameModeBase, TDFPlayerController : APlayerController, TDFCharacter : ACharacter, TDFRecordingComponent : UActorComponent, TDFInteractionComponent : UActorComponent, and TDFRecordingTuningData : UPrimaryDataAsset or UDataAsset. Place headers under Public/Game, Public/Player, Public/Characters, Public/Components, Public/Data and cpp files under matching Private folders. Build and verify all classes appear in the editor.

### 10 — Add minimal first-person component layout to native character

- Id: 10
- Title: Add minimal first-person component layout to native character
- Goal: Give the character stable component names for Blueprint and future camera/recording logic.
#### Editor actions
- Close Unreal Editor.
- Edit TDFCharacter.
- Add a CameraComponent named FirstPersonCamera.
- Add a SceneComponent named RecorderMount attached to FirstPersonCamera.
- Add TDFRecordingComponent named RecordingComponent.
- Add TDFInteractionComponent named InteractionComponent.
- Expose camera and component pointers to Blueprint as VisibleAnywhere, BlueprintReadOnly.
- Do not add movement tuning yet.

#### Files or assets
- Source/DrownedFrequency/Public/Characters/TDFCharacter.h
- Source/DrownedFrequency/Private/Characters/TDFCharacter.cpp

#### Test
- Build Development Editor.
- Open editor.
- Create a temporary Blueprint child of TDFCharacter and confirm components appear with exact names.
- Delete the temporary Blueprint if not needed.

#### Likely failure modes
- Missing CameraComponent module include is usually solved by Engine module already present.
- Component pointer names change later, breaking Blueprint references.
- Attachment done in BeginPlay instead of constructor, making Blueprint hierarchy unclear.

- Copy paste ai prompt: Update TDFCharacter to define stable first-person components only: UCameraComponent FirstPersonCamera, USceneComponent RecorderMount attached to FirstPersonCamera, UTDFRecordingComponent RecordingComponent, and UTDFInteractionComponent InteractionComponent. Expose them as VisibleAnywhere BlueprintReadOnly. Do not implement recording behavior yet. Build and verify a Blueprint child shows these exact component names.

### 11 — Create Blueprint presentation subclasses

- Id: 11
- Title: Create Blueprint presentation subclasses
- Goal: Separate native mechanics from designer-facing Blueprint assets.
#### Editor actions
- In Content Browser, go to Content/Game/Blueprints/Game.
- Create Blueprint Class based on TDFGameModeBase named BP_TDFGameMode.
- Go to Content/Game/Blueprints/Player.
- Create Blueprint Class based on TDFPlayerController named BP_TDFPlayerController.
- Go to Content/Game/Blueprints/Characters.
- Create Blueprint Class based on TDFCharacter named BP_TDFCharacter.
- Open BP_TDFCharacter.
- Confirm FirstPersonCamera, RecorderMount, RecordingComponent, and InteractionComponent exist.
- Compile and Save.

#### Files or assets
- Content/Game/Blueprints/Game/BP_TDFGameMode.uasset
- Content/Game/Blueprints/Player/BP_TDFPlayerController.uasset
- Content/Game/Blueprints/Characters/BP_TDFCharacter.uasset

#### Test
- Open each Blueprint.
- Click Compile.
- Confirm no Blueprint compiler errors.

#### Likely failure modes
- Blueprints parented to vanilla Character or PlayerController instead of TDF classes.
- Native components not visible because C++ specifiers are wrong.
- Hot Reload stale class versions require editor restart.

- Copy paste ai prompt: Create Blueprint presentation subclasses under Content/Game: BP_TDFGameMode from TDFGameModeBase, BP_TDFPlayerController from TDFPlayerController, and BP_TDFCharacter from TDFCharacter. Open and compile each Blueprint. Verify BP_TDFCharacter exposes FirstPersonCamera, RecorderMount, RecordingComponent, and InteractionComponent.

### 12 — Create Enhanced Input actions

- Id: 12
- Title: Create Enhanced Input actions
- Goal: Define all foundation input assets before binding code.
#### Editor actions
- In Content Browser, open Content/Game/Input.
- Create Input Action named IA_Move.
- Set IA_Move Value Type to Axis2D.
- Create Input Action named IA_Look.
- Set IA_Look Value Type to Axis2D.
- Create Input Action named IA_Interact.
- Set IA_Interact Value Type to Boolean.
- Create Input Action named IA_Record.
- Set IA_Record Value Type to Boolean.
- Create Input Action named IA_Playback.
- Set IA_Playback Value Type to Boolean.
- Create Input Action named IA_Pause.
- Set IA_Pause Value Type to Boolean.

#### Files or assets
- Content/Game/Input/IA_Move.uasset
- Content/Game/Input/IA_Look.uasset
- Content/Game/Input/IA_Interact.uasset
- Content/Game/Input/IA_Record.uasset
- Content/Game/Input/IA_Playback.uasset
- Content/Game/Input/IA_Pause.uasset

#### Test
- Open each Input Action.
- Verify Value Type exactly matches the list.
- Save All.

#### Likely failure modes
- Move or Look created as Boolean instead of Axis2D.
- Input assets created outside Content/Game/Input.
- Action names changed later, breaking C++ references.

- Copy paste ai prompt: Create Enhanced Input Action assets in Content/Game/Input: IA_Move Axis2D, IA_Look Axis2D, IA_Interact Boolean, IA_Record Boolean, IA_Playback Boolean, and IA_Pause Boolean. Open each asset and verify its Value Type. Save all.

### 13 — Create the default input mapping context

- Id: 13
- Title: Create the default input mapping context
- Goal: Map keyboard and mouse controls for the first playable build.
#### Editor actions
- In Content/Game/Input, create Input Mapping Context named IMC_Player.
- Add IA_Move mappings: W, A, S, D.
- For IA_Move, set W to Y +1, S to Y -1, D to X +1, A to X -1 using swizzle/negate modifiers as needed.
- Add IA_Look mappings: Mouse X and Mouse Y.
- For IA_Look Mouse Y, apply negate if needed so mouse up looks up according to project preference.
- Add IA_Interact mapping: E.
- Add IA_Record mapping: Left Mouse Button or R; choose one and document it.
- Add IA_Playback mapping: Right Mouse Button or F; choose one and document it.
- Add IA_Pause mapping: Escape.

#### Files or assets
- Content/Game/Input/IMC_Player.uasset

#### Test
- Open IMC_Player.
- Verify every listed action has at least one mapping.
- Save All.

#### Likely failure modes
- Axis signs reversed.
- Mouse Y feels inverted.
- Record/playback keys conflict with future interaction design.

- Copy paste ai prompt: Create Content/Game/Input/IMC_Player as an Enhanced Input Mapping Context. Map IA_Move to WASD with correct 2D vector signs, IA_Look to Mouse X and Mouse Y, IA_Interact to E, IA_Record to Left Mouse Button or R, IA_Playback to Right Mouse Button or F, and IA_Pause to Escape. Verify the mapping context contains all actions.

### 14 — Expose input assets on the player controller

- Id: 14
- Title: Expose input assets on the player controller
- Goal: Let the Blueprint controller assign input assets while C++ owns binding behavior.
#### Editor actions
- Close Unreal Editor.
- Edit TDFPlayerController.
- Add UPROPERTY EditDefaultsOnly BlueprintReadOnly TObjectPtr<UInputMappingContext> DefaultMappingContext.
- Add UPROPERTY EditDefaultsOnly BlueprintReadOnly TObjectPtr<UInputAction> MoveAction.
- Add LookAction, InteractAction, RecordAction, PlaybackAction, PauseAction using the same pattern.
- Include EnhancedInput headers.
- Do not hard-code asset paths.

#### Files or assets
- Source/DrownedFrequency/Public/Player/TDFPlayerController.h
- Source/DrownedFrequency/Private/Player/TDFPlayerController.cpp

#### Test
- Build Development Editor.
- Open BP_TDFPlayerController.
- Confirm the input asset properties appear in Class Defaults.

#### Likely failure modes
- Missing EnhancedInput includes.
- Using hard-coded asset paths creates brittle cook failures.
- Properties not visible because specifiers are incorrect.

- Copy paste ai prompt: Update TDFPlayerController to expose EditDefaultsOnly BlueprintReadOnly properties for DefaultMappingContext, MoveAction, LookAction, InteractAction, RecordAction, PlaybackAction, and PauseAction using Enhanced Input asset types. Do not hard-code asset paths. Build and verify BP_TDFPlayerController Class Defaults shows these assignable fields.

### 15 — Bind the mapping context at runtime

- Id: 15
- Title: Bind the mapping context at runtime
- Goal: Ensure Enhanced Input activates for the local player.
#### Editor actions
- Edit TDFPlayerController.
- In BeginPlay, get the LocalPlayer.
- Get UEnhancedInputLocalPlayerSubsystem.
- Add DefaultMappingContext with priority 0 if assigned.
- Add clear warning logs if the mapping context is missing.
- Do not bind actions here if movement is implemented in TDFCharacter.

#### Files or assets
- Source/DrownedFrequency/Private/Player/TDFPlayerController.cpp

#### Test
- Build Development Editor.
- Open BP_TDFPlayerController.
- Assign IMC_Player to DefaultMappingContext.
- Assign IA_* assets to their matching properties.
- Compile and Save.

#### Likely failure modes
- Subsystem unavailable on non-local controllers.
- Mapping context property not assigned in Blueprint.
- Input appears dead because GameMode is not using BP_TDFPlayerController yet.

- Copy paste ai prompt: Implement only the Enhanced Input mapping-context activation in TDFPlayerController BeginPlay. Get the local player's UEnhancedInputLocalPlayerSubsystem and add DefaultMappingContext at priority 0 if assigned. Log clear warnings for missing assignments. Then assign IMC_Player and all IA_* assets in BP_TDFPlayerController and compile.

### 16 — Implement minimal movement and look bindings

- Id: 16
- Title: Implement minimal movement and look bindings
- Goal: Make the first-person character move and look using Enhanced Input.
#### Editor actions
- Edit TDFCharacter.
- Override SetupPlayerInputComponent.
- Cast PlayerInputComponent to UEnhancedInputComponent.
- Bind IA_Move to a Move handler.
- Bind IA_Look to a Look handler.
- Move handler reads FVector2D and calls AddMovementInput forward/right.
- Look handler reads FVector2D and calls AddControllerYawInput/AddControllerPitchInput.
- Add placeholder handlers for Interact, Record, Playback, and Pause that log only.

#### Files or assets
- Source/DrownedFrequency/Public/Characters/TDFCharacter.h
- Source/DrownedFrequency/Private/Characters/TDFCharacter.cpp

#### Test
- Build Development Editor.
- Open BP_TDFCharacter.
- Assign IA_* properties if they live on character, or confirm controller passes/binds them if designed that way.
- Press Play after GameMode is assigned in a later step; for now compile only.

#### Likely failure modes
- Binding action references live on PlayerController but character cannot access them cleanly.
- EnhancedInputComponent cast fails because default input component class is not set.
- Forward/right vectors use actor rotation including pitch, causing odd movement.

- Copy paste ai prompt: Implement minimal Enhanced Input bindings for TDFCharacter: IA_Move drives AddMovementInput forward/right, IA_Look drives yaw/pitch, and IA_Interact, IA_Record, IA_Playback, and IA_Pause only log placeholder messages. Use UEnhancedInputComponent. Keep mechanics minimal and build after the change.

### 17 — Assign Blueprint classes in GameMode

- Id: 17
- Title: Assign Blueprint classes in GameMode
- Goal: Make the playable defaults use the project-specific character and controller.
#### Editor actions
- Open BP_TDFGameMode.
- Set Default Pawn Class to BP_TDFCharacter.
- Set Player Controller Class to BP_TDFPlayerController.
- Compile and Save.

#### Files or assets
- Content/Game/Blueprints/Game/BP_TDFGameMode.uasset

#### Test
- Reopen BP_TDFGameMode.
- Confirm Default Pawn Class and Player Controller Class remain assigned.

#### Likely failure modes
- Default Pawn Class left as DefaultPawn.
- Using native classes instead of Blueprint subclasses, losing designer assignments.
- Blueprint compile stale after C++ class changes.

- Copy paste ai prompt: Open BP_TDFGameMode and assign Default Pawn Class to BP_TDFCharacter and Player Controller Class to BP_TDFPlayerController. Compile, save, close, reopen, and verify the assignments persisted.

### 18 — Create the persistent and test maps

- Id: 18
- Title: Create the persistent and test maps
- Goal: Create the map structure for persistent level plus streamed sectors.
#### Editor actions
- In Content/Game/Maps/Persistent, create a new Level named L_TDF_Persistent.
- In Content/Game/Maps/Test, create a new Level named L_TDF_InputGym.
- In Content/Game/Maps/Sectors, create levels named L_TDF_Sector_Intro and L_TDF_Sector_RadioRoom.
- Open L_TDF_Persistent.
- Add a Player Start.
- Add a simple floor using basic geometry or a cube.
- Save All.

#### Files or assets
- Content/Game/Maps/Persistent/L_TDF_Persistent.umap
- Content/Game/Maps/Test/L_TDF_InputGym.umap
- Content/Game/Maps/Sectors/L_TDF_Sector_Intro.umap
- Content/Game/Maps/Sectors/L_TDF_Sector_RadioRoom.umap

#### Test
- Open each map.
- Confirm it loads with no missing asset warnings.
- Save All.

#### Likely failure modes
- Map created in wrong folder.
- Persistent map lacks Player Start, causing spawn at world origin.
- Unsaved maps not included later in packaging.

- Copy paste ai prompt: Create the foundation maps for DrownedFrequency: L_TDF_Persistent under Content/Game/Maps/Persistent, L_TDF_InputGym under Content/Game/Maps/Test, and sector maps L_TDF_Sector_Intro and L_TDF_Sector_RadioRoom under Content/Game/Maps/Sectors. Add a Player Start and simple floor to L_TDF_Persistent. Save and verify all maps open.

### 19 — Set default editor and game maps

- Id: 19
- Title: Set default editor and game maps
- Goal: Ensure Play and package launch into the persistent level.
#### Editor actions
- Go to Edit > Project Settings > Maps & Modes.
- Set Default GameMode to BP_TDFGameMode.
- Set Editor Startup Map to /Game/_TDF/Maps/Persistent/L_TDF_Persistent.
- Set Game Default Map to /Game/_TDF/Maps/Persistent/L_TDF_Persistent.
- Save Config if prompted.

#### Files or assets
- Config/DefaultEngine.ini
- Config/DefaultGame.ini

#### Test
- Restart the editor.
- Confirm L_TDF_Persistent opens by default or is selected as startup map.
- Press Play and verify BP_TDFCharacter spawns.

#### Likely failure modes
- Map soft reference points to a renamed map.
- GameMode override set per-level differently from project setting.
- Spawning fails because no Player Start exists.

- Copy paste ai prompt: Set Maps & Modes for DrownedFrequency: Default GameMode BP_TDFGameMode, Editor Startup Map L_TDF_Persistent, and Game Default Map L_TDF_Persistent. Restart the editor, press Play, and verify the project-specific character spawns.

### 20 — Create streaming-sector management foundation

- Id: 20
- Title: Create streaming-sector management foundation
- Goal: Prepare authored sector streaming without building the full game.
#### Editor actions
- Open L_TDF_Persistent.
- Open Window > Levels if using traditional level streaming, or choose World Partition policy deliberately if the project uses it.
- For this foundation, add L_TDF_Sector_Intro and L_TDF_Sector_RadioRoom as streaming sublevels if not using World Partition.
- Set initial streaming method to Blueprint or Always Loaded for test, documenting the choice.
- Save L_TDF_Persistent.

#### Files or assets
- Content/Game/Maps/Persistent/L_TDF_Persistent.umap
- Content/Game/Maps/Sectors/L_TDF_Sector_Intro.umap
- Content/Game/Maps/Sectors/L_TDF_Sector_RadioRoom.umap

#### Test
- Reopen L_TDF_Persistent.
- Confirm the two sector maps appear in the Levels window or equivalent streaming setup.
- Press Play and confirm no missing level warnings.

#### Likely failure modes
- Mixing World Partition and legacy sublevel expectations without a clear policy.
- Sublevels not saved after being added.
- Packaged build omits maps not referenced or listed for cooking.

- Copy paste ai prompt: In L_TDF_Persistent, set up the initial streamed sector foundation. Add L_TDF_Sector_Intro and L_TDF_Sector_RadioRoom as streaming sublevels if using traditional level streaming, or document the equivalent World Partition setup if this UE5 project defaults to that workflow. Save, reopen, and verify the sector references persist.

### 21 — Create the recording tuning Data Asset

- Id: 21
- Title: Create the recording tuning Data Asset
- Goal: Prove the C++ mechanics / Data Asset tuning split exists.
#### Editor actions
- In Content/Game/Data, create a Data Asset based on TDFRecordingTuningData named DA_RecordingTuning_Default.
- Open DA_RecordingTuning_Default.
- If no fields exist yet, leave it as a placeholder and verify it opens.
- If fields were added, set safe defaults: MaxRecordingSeconds 10.0, PlaybackVolume 1.0, InteractionTraceDistance 300.0.
- Save.

#### Files or assets
- Content/Game/Data/DA_RecordingTuning_Default.uasset

#### Test
- Close and reopen DA_RecordingTuning_Default.
- Confirm values persist or placeholder asset opens cleanly.

#### Likely failure modes
- Data Asset class not marked BlueprintType or not asset-creatable.
- Asset created from wrong base class.
- Tuning values hard-coded in Blueprint instead of Data Asset.

- Copy paste ai prompt: Create Content/Game/Data/DA_RecordingTuning_Default from TDFRecordingTuningData. If the C++ class has no fields yet, verify the asset still opens as a placeholder. If fields exist, set conservative defaults for recording duration, playback volume, and interaction distance. Save and reopen to verify persistence.

### 22 — Add tuning asset reference to the recording component

- Id: 22
- Title: Add tuning asset reference to the recording component
- Goal: Let the native recording component read designer-tuned data later.
#### Editor actions
- Edit TDFRecordingComponent.
- Add an EditDefaultsOnly BlueprintReadOnly TObjectPtr<UTDFRecordingTuningData> RecordingTuning property.
- Do not implement recording capture yet.
- Build the project.
- Open BP_TDFCharacter.
- Select RecordingComponent.
- Assign DA_RecordingTuning_Default to RecordingTuning.
- Compile and Save.

#### Files or assets
- Source/DrownedFrequency/Public/Components/TDFRecordingComponent.h
- Source/DrownedFrequency/Private/Components/TDFRecordingComponent.cpp
- Content/Game/Blueprints/Characters/BP_TDFCharacter.uasset

#### Test
- Reopen BP_TDFCharacter.
- Select RecordingComponent.
- Confirm RecordingTuning remains assigned.

#### Likely failure modes
- Circular include between component and Data Asset; use forward declaration in header.
- Property not visible on component because specifiers are wrong.
- Asset reference omitted from packaged build if not reachable.

- Copy paste ai prompt: Add a RecordingTuning property to TDFRecordingComponent referencing UTDFRecordingTuningData. Use forward declarations where appropriate. Do not implement audio recording yet. Build, then assign DA_RecordingTuning_Default on BP_TDFCharacter's RecordingComponent and verify the assignment persists.

### 23 — Create a minimal interactable actor foundation

- Id: 23
- Title: Create a minimal interactable actor foundation
- Goal: Prepare the no-combat world interaction pattern.
#### Editor actions
- Create C++ class TDFInspectableActor derived from Actor.
- Give it a root SceneComponent and optional StaticMeshComponent named Mesh.
- Add BlueprintImplementableEvent or BlueprintNativeEvent named OnInspected for presentation.
- Do not create inventory, combat, or puzzle logic yet.
- Build.
- Create Blueprint child BP_Inspectable_TestRadio in Content/Game/Blueprints/Actors.
- Add a cube or placeholder mesh.
- Compile and Save.

#### Files or assets
- Source/DrownedFrequency/Public/Actors/TDFInspectableActor.h
- Source/DrownedFrequency/Private/Actors/TDFInspectableActor.cpp
- Content/Game/Blueprints/Actors/BP_Inspectable_TestRadio.uasset

#### Test
- Place BP_Inspectable_TestRadio in L_TDF_Persistent.
- Press Play.
- Confirm it appears in the world.

#### Likely failure modes
- Actor lacks a root component.
- Blueprint event not exposed correctly.
- Interaction system scope creeps into puzzles too early.

- Copy paste ai prompt: Create TDFInspectableActor as a minimal no-combat interactable base with a root SceneComponent, optional Mesh component, and an inspect event for Blueprint presentation. Then create BP_Inspectable_TestRadio under Content/Game/Blueprints/Actors, place it in L_TDF_Persistent, and verify it appears in Play.

### 24 — Wire placeholder interaction logging

- Id: 24
- Title: Wire placeholder interaction logging
- Goal: Confirm input, character, and interactable foundation communicate.
#### Editor actions
- Update TDFInteractionComponent or TDFCharacter placeholder Interact handler.
- On IA_Interact started, perform a camera-forward line trace using a short distance such as 300 units.
- If it hits TDFInspectableActor, call its inspect event or log the actor name.
- If it hits nothing, log a low-verbosity message.
- Do not add UI prompts yet.

#### Files or assets
- Source/DrownedFrequency/Public/Components/TDFInteractionComponent.h
- Source/DrownedFrequency/Private/Components/TDFInteractionComponent.cpp
- Source/DrownedFrequency/Private/Characters/TDFCharacter.cpp

#### Test
- Build.
- Open L_TDF_Persistent.
- Press Play.
- Look at BP_Inspectable_TestRadio and press E.
- Confirm Output Log shows the inspected actor name.

#### Likely failure modes
- Trace starts inside the character capsule.
- Collision channel does not hit the placeholder mesh.
- Interaction input is not bound because IA_Interact reference is missing.

- Copy paste ai prompt: Implement only placeholder interaction verification: when IA_Interact fires, trace forward from FirstPersonCamera for about 300 units and log the hit TDFInspectableActor name or call its inspect event. Do not build UI or puzzle logic. Test by pressing E on BP_Inspectable_TestRadio and confirm the Output Log shows the actor.

### 25 — Wire placeholder recording input states

- Id: 25
- Title: Wire placeholder recording input states
- Goal: Confirm the authored sound-recording mechanic has input and component seams before real audio work.
#### Editor actions
- Update TDFRecordingComponent with placeholder methods StartRecording, StopRecording, and PlayLastRecording.
- Make methods BlueprintCallable.
- Have them log clear messages only.
- Bind IA_Record started/completed to StartRecording/StopRecording.
- Bind IA_Playback started to PlayLastRecording.
- Do not use microphone APIs yet.

#### Files or assets
- Source/DrownedFrequency/Public/Components/TDFRecordingComponent.h
- Source/DrownedFrequency/Private/Components/TDFRecordingComponent.cpp
- Source/DrownedFrequency/Private/Characters/TDFCharacter.cpp

#### Test
- Build.
- Press Play.
- Press and release the Record input.
- Press Playback input.
- Confirm Output Log shows StartRecording, StopRecording, and PlayLastRecording in order.

#### Likely failure modes
- Record action mapped only to triggered instead of started/completed.
- Playback fires every frame if bound to Triggered instead of Started.
- Component pointer is null because it was not created in the character constructor.

- Copy paste ai prompt: Add placeholder recording methods to TDFRecordingComponent: StartRecording, StopRecording, and PlayLastRecording. They should be BlueprintCallable and log only. Bind IA_Record started/completed to start/stop and IA_Playback started to playback. Do not implement microphone capture. Test in Play and verify the logs.

### 26 — Create a first minimal HUD widget placeholder

- Id: 26
- Title: Create a first minimal HUD widget placeholder
- Goal: Reserve Blueprint/UI presentation for recording state without coupling it to mechanics.
#### Editor actions
- In Content/Game/UI, create Widget Blueprint named WBP_RecordingStatus.
- Add a TextBlock named TXT_Status.
- Set default text to Idle.
- Do not style final UI.
- Create Blueprint or C++ hook later; for now only create and compile the widget.

#### Files or assets
- Content/Game/UI/WBP_RecordingStatus.uasset

#### Test
- Open WBP_RecordingStatus.
- Compile.
- Confirm no widget compiler errors.

#### Likely failure modes
- UI design work expands too early.
- Widget named generically and becomes hard to find.
- TextBlock not named, making future binding brittle.

- Copy paste ai prompt: Create a minimal Widget Blueprint Content/Game/UI/WBP_RecordingStatus with one TextBlock named TXT_Status and default text Idle. Compile and save. Do not implement final UI styling or state binding yet.

### 27 — Add HUD widget spawn placeholder to player controller

- Id: 27
- Title: Add HUD widget spawn placeholder to player controller
- Goal: Verify UMG module setup and runtime widget creation before packaging.
#### Editor actions
- Update TDFPlayerController.
- Add EditDefaultsOnly TSubclassOf<UUserWidget> RecordingStatusWidgetClass.
- Add UPROPERTY TObjectPtr<UUserWidget> RecordingStatusWidget.
- In BeginPlay, if the widget class is assigned, create the widget and add it to viewport.
- Build.
- Open BP_TDFPlayerController.
- Assign WBP_RecordingStatus to RecordingStatusWidgetClass.
- Compile and Save.

#### Files or assets
- Source/DrownedFrequency/Public/Player/TDFPlayerController.h
- Source/DrownedFrequency/Private/Player/TDFPlayerController.cpp
- Content/Game/Blueprints/Player/BP_TDFPlayerController.uasset

#### Test
- Press Play.
- Confirm the Idle text appears on screen.
- Check Output Log for no UMG class assignment warnings.

#### Likely failure modes
- UMG module missing from Build.cs.
- Widget class not assigned in BP_TDFPlayerController.
- Widget appears in editor PIE but not packaged if not referenced correctly.

- Copy paste ai prompt: Add a RecordingStatusWidgetClass property to TDFPlayerController and spawn it in BeginPlay if assigned. Assign WBP_RecordingStatus in BP_TDFPlayerController. Build and test Play; verify the Idle text appears on screen.

### 28 — Create a packaging test map list

- Id: 28
- Title: Create a packaging test map list
- Goal: Prevent first package from omitting maps.
#### Editor actions
- Go to Edit > Project Settings > Packaging.
- Expand Advanced.
- Find List of maps to include in a packaged build.
- Add L_TDF_Persistent.
- Add L_TDF_InputGym.
- Add L_TDF_Sector_Intro.
- Add L_TDF_Sector_RadioRoom.
- Set Build Configuration to Development for the first package.
- Disable For Distribution for the first package.

#### Files or assets
- Config/DefaultGame.ini

#### Test
- Close and reopen Project Settings > Packaging.
- Confirm all four maps remain in the package map list.

#### Likely failure modes
- Packaged build opens black screen because default map was not cooked.
- Streaming sublevels omitted from cook.
- Shipping selected too early, hiding useful logs.

- Copy paste ai prompt: Configure Project Settings > Packaging for the first Development package. Add L_TDF_Persistent, L_TDF_InputGym, L_TDF_Sector_Intro, and L_TDF_Sector_RadioRoom to the map list. Do not enable For Distribution. Reopen Packaging settings and verify the map list persisted.

### 29 — Run a clean editor playtest checklist

- Id: 29
- Title: Run a clean editor playtest checklist
- Goal: Verify the foundation works before attempting a package.
#### Editor actions
- Open L_TDF_Persistent.
- Press Play in Selected Viewport.
- Move with WASD.
- Look with mouse.
- Press E while looking at BP_Inspectable_TestRadio.
- Press and release Record input.
- Press Playback input.
- Confirm WBP_RecordingStatus appears.
- Stop Play.
- Open Output Log and check for errors.

- Files or assets: []
#### Test
- WASD moves the character.
- Mouse look works.
- Interaction log appears.
- Recording placeholder logs start/stop/playback.
- HUD placeholder appears.
- No red errors in Output Log.

#### Likely failure modes
- Input mapping context not assigned.
- GameMode not assigned.
- Player starts as spectator or default pawn.
- Mouse not captured in PIE.
- Collision prevents line trace hit.

- Copy paste ai prompt: Run a full PIE foundation test in L_TDF_Persistent: verify movement, mouse look, E interaction on BP_Inspectable_TestRadio, Record start/stop logs, Playback log, and WBP_RecordingStatus visibility. Check Output Log for red errors and list any failures with likely asset or class assignment causes.

### 30 — Commit the playable foundation

- Id: 30
- Title: Commit the playable foundation
- Goal: Checkpoint the first playable editor state before packaging.
#### Editor actions
- Close Unreal Editor.
- Run git status.
- Stage Source, Config, Content/Game, DrownedFrequency.uproject, .gitignore, and .gitattributes changes as appropriate.
- Do not stage Saved, Intermediate, Binaries, DerivedDataCache, generated IDE files, or packaged output.
- Commit with message: Add playable foundation with Enhanced Input.

#### Files or assets
- Source/DrownedFrequency/**
- Config/**
- Content/Game/**
- DrownedFrequency.uproject

#### Test
- Run git status.
- Confirm working tree is clean after commit.
- If using remote hosting, push and verify LFS objects upload.

#### Likely failure modes
- Binary assets not tracked by LFS.
- Generated files accidentally committed.
- Large Content files fail to push because LFS remote is not configured.

- Copy paste ai prompt: Commit the playable foundation for DrownedFrequency. Stage only intended Source, Config, Content/Game, .uproject, .gitignore, and .gitattributes changes. Verify no generated folders are staged. Commit with message 'Add playable foundation with Enhanced Input' and confirm git status is clean.

### 31 — Prepare Windows package settings

- Id: 31
- Title: Prepare Windows package settings
- Goal: Set the first packaged build to Windows Development with useful diagnostics.
#### Editor actions
- Open Unreal Editor on Windows if possible.
- Go to Platforms > Windows.
- Confirm the project can target Windows.
- Go to Project Settings > Packaging.
- Set Build Configuration to Development.
- Disable Full Rebuild only if iterative packaging is intended; for first package prefer Full Rebuild enabled if available.
- Disable For Distribution.
- Optionally enable Include Debug Files for first internal package.
- Save settings.

#### Files or assets
- Config/DefaultGame.ini
- Config/DefaultEngine.ini

#### Test
- Restart editor.
- Return to Packaging settings.
- Confirm Development configuration and map list remain.

#### Likely failure modes
- Packaging Windows from Mac is not available without a specific cross-compile/remote/CI setup.
- Windows SDK or Visual Studio components missing.
- Package settings changed per-user rather than project config.

- Copy paste ai prompt: Prepare DrownedFrequency for its first Windows Development package. If on Mac, do not claim Windows packaging is complete unless using a verified Windows machine or CI. In Project Settings > Packaging, use Development, keep For Distribution off, include the required maps, and verify settings persist after restart.

### 32 — Run the first Windows package

- Id: 32
- Title: Run the first Windows package
- Goal: Produce the first executable build.
#### Editor actions
- On Windows, open DrownedFrequency.uproject.
- Choose Platforms > Windows > Package Project.
- Select an output folder outside the Git repository, such as Builds/Windows_Development outside the project root or a sibling folder.
- Wait for packaging to complete.
- Do not commit packaged output.

#### Files or assets
- Packaged Windows output folder outside repository

#### Test
- Confirm packaging reports success.
- Open the packaged folder.
- Run DrownedFrequency.exe.
- Verify the game opens to L_TDF_Persistent.
- Test movement, look, interaction, recording logs if visible, playback input, and HUD placeholder.

#### Likely failure modes
- Unknown Cook Failure due to missing referenced assets.
- Default map not cooked, causing black screen.
- Enhanced Input asset references missing because Blueprint assignments were not saved.
- Windows packaging unavailable from Mac host.
- Antivirus or controlled folder access blocks executable creation.

- Copy paste ai prompt: Package DrownedFrequency for Windows Development from a verified Windows UE5 environment. Output the package outside the Git repository. After packaging succeeds, run DrownedFrequency.exe and verify it opens to L_TDF_Persistent with movement, mouse look, HUD placeholder, interaction placeholder, and recording/playback placeholder inputs working. Do not commit packaged output.

### 33 — Collect first package logs and create packaging checkpoint

- Id: 33
- Title: Collect first package logs and create packaging checkpoint
- Goal: Record exactly what succeeded or failed for the next AI task.
#### Editor actions
- If packaging succeeds, save the packaging log path and packaged build path in project notes or issue tracker, not necessarily in the repo.
- If packaging fails, copy the first fatal error and the preceding 50-100 log lines.
- Run git status and ensure packaged output is not inside the repository.
- Commit any legitimate config fixes with message: Configure first Windows package.

#### Files or assets
- No packaged output committed
- Optional external notes only

#### Test
- Run git status.
- Confirm no packaged files are staged.
- Confirm the packaged executable was manually launched at least once if packaging succeeded.

#### Likely failure modes
- Only reporting Package Complete without launching the executable.
- Committing packaged binaries into Git.
- Losing the actual cook error by copying only the final AutomationTool failure line.

- Copy paste ai prompt: Finalize the first package checkpoint. If the Windows Development package succeeded, run the executable and record the package path and test result. If it failed, capture the first real cook/build error plus 50-100 preceding log lines. Ensure packaged output is not in Git. Commit only legitimate config fixes, not build artifacts.


## Acceptance criteria for first packaged build
- Project is named DrownedFrequency and opens cleanly.
- Git and Git LFS are initialized with UE5-safe ignore and LFS patterns.
- Build.cs includes Core, CoreUObject, Engine, InputCore, EnhancedInput, UMG, Slate, and SlateCore as appropriate.
- Native foundation classes exist for GameMode, PlayerController, Character, RecordingComponent, InteractionComponent, RecordingTuningData, and InspectableActor.
- Blueprint subclasses exist under Content/Game and are assigned in GameMode.
- Enhanced Input assets IA_Move, IA_Look, IA_Interact, IA_Record, IA_Playback, IA_Pause, and IMC_Player exist and are assigned.
- L_TDF_Persistent is the default game map and contains a Player Start.
- At least two sector maps exist and are referenced by the persistent level or documented World Partition setup.
- DA_RecordingTuning_Default exists and is assigned to the RecordingComponent.
- PIE test confirms movement, look, interaction placeholder, recording placeholder, playback placeholder, and HUD placeholder.
- Windows Development package succeeds on a verified Windows toolchain or CI.
- Packaged executable launches and opens to L_TDF_Persistent.
- Packaged output is not committed to source control.

## Mac development notes
- Developing on Mac is fine for editor gameplay work, C++ iteration, assets, and source control.
- Do not treat a Mac editor run as proof of Windows packaging.
- For the first Windows packaged build, use a Windows machine, Windows CI runner, or a deliberately configured remote build setup.
- Keep path casing consistent because Mac file systems may hide case mistakes that break on other machines or CI.
- Commit source, config, and assets before switching machines so Windows packaging tests the same revision.

