# Save, Streaming, UI, Accessibility, Difficulty, and Steam

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

## What i did
- Created a granular UE5 C++ build sequence for The Drowned Frequency covering save/load, checkpoints, objective/chapter progression, streamed sectors, menus/settings, accessibility, subtitles/waveform compass, difficulty profiles, and Steam integration timing.
- Structured the plan as tiny implementation tasks with suggested class and asset names, logic, editor hookups, tests, done criteria, failure modes, and copy-paste AI prompts.
- No files were created or modified.

- Files created or modified: []
- Issues encountered: []
## Build sequence

### 0_foundation_rules

- Phase: 0_foundation_rules
- Goal: Lock naming, save identity, and test-first workflow before feature work.
#### Tasks

##### 0.1 — Create stable naming conventions

- Id: 0.1
- Title: Create stable naming conventions
###### Classes assets
- UTDFGameInstance
- ATDFGameMode
- ATDFPlayerController
- ATDFPlayerCharacter
- UTDFSaveGame
- UTDFSaveSubsystem
- UTDFCheckpointSubsystem
- UTDFObjectiveSubsystem
- UTDFLevelStreamingSubsystem
- UTDFSettingsSubsystem
- UTDFAccessibilitySubsystem
- UTDFDifficultySubsystem
- UTDFSteamSubsystem

###### Precise logic
- Prefix all game-specific C++ classes with TDF.
- Keep systems that must persist across maps as UGameInstanceSubsystems.
- Keep per-world systems as UWorldSubsystems only if they depend on world actors.
- All persistent actor references must use FGuid StableActorId, never raw actor pointer paths.

###### Editor hookups
- Project Settings > Maps & Modes: set GameInstanceClass to UTDFGameInstance.
- Set default GameMode to BP_TDFGameMode derived from ATDFGameMode.
- Set PlayerController to BP_TDFPlayerController.
- Set Default Pawn to BP_TDFPlayerCharacter.

###### Tests
- Automation test: TDF.Naming.SubsystemsExist verifies subsystems can be retrieved from GameInstance.
- PIE smoke test loads persistent level and confirms player controller, pawn, and game instance classes.

###### Done criteria
- All planned systems have stub classes compiling.
- Persistent level opens without missing class warnings.
- Subsystem access returns non-null where expected.

###### Failure modes
- Subsystem unavailable because wrong base class was used.
- Blueprint defaults still point to old template classes.
- GameInstance not set in project settings.

- Copy paste ai prompt: In this UE5 C++ project, create compile-only stubs for UTDFGameInstance, ATDFGameMode, ATDFPlayerController, ATDFPlayerCharacter, UTDFSaveSubsystem, UTDFCheckpointSubsystem, UTDFObjectiveSubsystem, UTDFLevelStreamingSubsystem, UTDFSettingsSubsystem, UTDFAccessibilitySubsystem, UTDFDifficultySubsystem, and UTDFSteamSubsystem. Use TDF prefixes, no gameplay logic yet, and add a minimal automation test proving the game instance subsystems can be retrieved.

##### 0.2 — Add stable actor ID interface

- Id: 0.2
- Title: Add stable actor ID interface
###### Classes assets
- ITDFStableActorInterface
- UTDFStableActorComponent
- FTDFStableActorRecord

###### Precise logic
- ITDFStableActorInterface exposes GetStableActorId(), SetStableActorId(FGuid), ShouldPersistInSave().
- UTDFStableActorComponent owns FGuid StableActorId marked EditInstanceOnly and SaveGame.
- OnConstruction: if StableActorId is invalid in editor, generate one only when actor is placed, not for class default object.
- At BeginPlay: register StableActorId with UTDFSaveSubsystem; log error on duplicate IDs.

###### Editor hookups
- Add UTDFStableActorComponent to every save-relevant placed actor blueprint.
- Expose StableActorId in Details panel but discourage manual editing.
- Add editor utility later for duplicate scan, but do not block initial implementation.

###### Tests
- Automation test: placed fake actor with StableActorComponent gets non-zero GUID.
- Automation test: duplicate IDs are detected and reported.
- Automation test: transient actors can opt out with ShouldPersistInSave=false.

###### Done criteria
- Every saveable placed actor has a stable GUID.
- Duplicate GUIDs are detected deterministically.
- Runtime-spawned temporary actors are ignored unless explicitly saveable.

###### Failure modes
- GUID regenerated each load, breaking saves.
- Duplicated actors in editor inherit same GUID.
- Save system stores actor names instead of GUIDs.

- Copy paste ai prompt: Implement ITDFStableActorInterface and UTDFStableActorComponent for UE5 C++. The component must store an EditInstanceOnly SaveGame FGuid StableActorId, generate one for placed actors when invalid, expose ShouldPersistInSave, and register with UTDFSaveSubsystem at BeginPlay. Add automation tests for valid IDs, duplicate detection, and opt-out persistence.



### 1_versioned_save_load

- Phase: 1_versioned_save_load
- Goal: Build robust versioned saves before Steam or complex progression.
#### Tasks

##### 1.1 — Define save schema

- Id: 1.1
- Title: Define save schema
###### Classes assets
- UTDFSaveGame
- FTDFSaveHeader
- FTDFPlayerSaveData
- FTDFActorSaveData
- FTDFObjectiveSaveData
- FTDFSettingsSaveData

###### Precise logic
- UTDFSaveGame contains int32 SaveVersion, FString BuildVersion, FDateTime SavedAtUtc, FName CurrentChapterId, FName CurrentCheckpointId, FName CurrentPersistentMap, TArray<FName> LoadedSectorIds.
- FTDFPlayerSaveData stores FVector Location, FRotator Rotation, float Health, inventory key item IDs, difficulty ID.
- FTDFActorSaveData stores FGuid StableActorId, FTransform Transform, FName StateName, TMap<FName, FString> SerializedFields.
- SaveVersion starts at 1 and only increments when serialized schema changes.
- Never remove old fields without migration.

###### Editor hookups
- No editor hookup yet except project compiling with SaveGame module use.

###### Tests
- Automation test creates UTDFSaveGame, serializes to slot, reloads, and compares header fields.
- Automation test verifies SaveVersion defaults to current version.

###### Done criteria
- A save object can round-trip to disk.
- SaveVersion, checkpoint ID, chapter ID, and loaded sectors survive reload.

###### Failure modes
- SaveGame property metadata missing on fields expected to persist.
- Using UObject pointers inside save data.
- Forgetting streamed sector list.

- Copy paste ai prompt: Create UTDFSaveGame and plain USTRUCT save records for The Drowned Frequency. Include SaveVersion, build string, UTC save time, current chapter ID, checkpoint ID, persistent map, loaded sector IDs, player transform/health/inventory/difficulty, actor records keyed by stable FGuid, objective records, and settings records. Add an automation test that writes and reloads a slot and verifies core fields round-trip.

##### 1.2 — Implement save write path

- Id: 1.2
- Title: Implement save write path
###### Classes assets
- UTDFSaveSubsystem
- ITDFSaveableActorInterface
- FTDFSaveRequest

###### Precise logic
- UTDFSaveSubsystem::WriteSave(FName SlotName, ETDFSaveReason Reason) gathers player data, chapter/objective state, sector state, and registered stable actor data.
- Before writing, call CanSaveNow(). Return false during level streaming transition, cutscene lock, or combat if design forbids.
- Write to temp slot first, then final slot to reduce corruption risk.
- Broadcast OnSaveStarted, OnSaveFinished, OnSaveFailed.
- Store save reason: Manual, Checkpoint, ChapterStart, Debug.

###### Editor hookups
- Create BP_TDFSaveDebugWidget with buttons Save Slot 0, Load Slot 0, Delete Slot 0 for development map only.

###### Tests
- Unit automation: CanSaveNow returns false when streaming flag is true.
- Functional test: move player, write save, kill process-safe simulation by loading from disk, confirm saved location.

###### Done criteria
- Manual save writes a valid file.
- Save UI receives success/failure delegates.
- Save attempt is blocked during streaming transition.

###### Failure modes
- Partial save produced after crash.
- Save starts while sector unload destroys actors.
- Delegates not fired on failure path.

- Copy paste ai prompt: Implement UTDFSaveSubsystem::WriteSave with a temp-slot then final-slot pattern, CanSaveNow gates, save reason enum, and delegates for start/success/failure. Gather player, objective, streamed sector, and registered stable actor records. Add tests for CanSaveNow during streaming and for player transform round-trip.

##### 1.3 — Implement save load path

- Id: 1.3
- Title: Implement save load path
###### Classes assets
- UTDFSaveSubsystem
- FTDFLoadContext

###### Precise logic
- Load flow: read slot, validate version, migrate if needed, open persistent map if different, load required sectors, wait for streaming completion, spawn/place player, then apply actor states.
- Actor state application happens after all sectors report visible.
- If a saved actor ID is missing, log warning and continue.
- If a current actor is not present in save but has default state, keep default unless chapter state says destroyed.
- Player control remains disabled until load completes.

###### Editor hookups
- Main menu Load button calls UTDFSaveSubsystem::RequestLoadSlot.
- Development console command tdf.LoadSlot SlotName.

###### Tests
- Functional test: save in Sector_A, unload to menu, load, confirm Sector_A loaded and player placed.
- Automation test: missing actor ID logs warning without crashing.

###### Done criteria
- Loading restores persistent map, streamed sectors, player transform, objective state, and saveable actor state.
- No visible player fall-through before sectors are loaded.

###### Failure modes
- Actor states applied before streamed level actors exist.
- Player spawns at PlayerStart instead of saved/checkpoint transform.
- Hard crash on missing actor ID.

- Copy paste ai prompt: Implement UTDFSaveSubsystem load flow for UE5: validate/migrate save, open persistent map if needed, stream saved sectors, wait until all visible, disable input during load, place player, then apply saveable actor states by stable GUID. Missing actor IDs should warn and continue. Add functional tests for loading into a streamed sector.

##### 1.4 — Add save migrations

- Id: 1.4
- Title: Add save migrations
###### Classes assets
- UTDFSaveMigrationLibrary
- ITDFSaveMigrationStep

###### Precise logic
- CurrentSaveVersion constant lives in one header.
- Load rejects saves newer than current build with user-facing message.
- Older saves run sequential migration steps: V1 to V2, V2 to V3.
- Each migration must be deterministic and logged.

###### Editor hookups
- Add developer command tdf.DumpSaveVersion SlotName.

###### Tests
- Automation test loads fixture-like old save object in memory and migrates to current.
- Automation test newer save version returns ELoadFailure::SaveFromNewerBuild.

###### Done criteria
- Old save data can be upgraded without data loss.
- Newer unsupported saves fail gracefully.

###### Failure modes
- Migration silently overwrites fields.
- Newer save crashes or loads corrupted state.
- Version constant duplicated in several files.

- Copy paste ai prompt: Add a UTDFSaveMigrationLibrary with a single CurrentSaveVersion constant, sequential migration steps, deterministic logs, and graceful rejection of saves from newer builds. Add tests for old-version migration and newer-version rejection.



### 2_checkpoints

- Phase: 2_checkpoints
- Goal: Layer checkpoint saves on top of the save system, not beside it.
#### Tasks

##### 2.1 — Create checkpoint actor

- Id: 2.1
- Title: Create checkpoint actor
###### Classes assets
- ATDFCheckpointActor
- UTDFCheckpointSubsystem
- DT_TDFCheckpoints

###### Precise logic
- ATDFCheckpointActor has FName CheckpointId, FName ChapterId, FTransform RespawnTransform, TArray<FName> RequiredLoadedSectors, bool bAutoTrigger, bool bAllowManualSaveAfter.
- On overlap with player, call UTDFCheckpointSubsystem::ActivateCheckpoint(CheckpointId).
- Activation updates current checkpoint and asks UTDFSaveSubsystem to write checkpoint save.
- Do not activate same checkpoint repeatedly unless bForceRewrite is true.

###### Editor hookups
- Place BP_TDFCheckpointActor in each sector entrance, combat-safe rest point, and chapter start.
- Use visible arrow component to show respawn facing direction.
- Set CheckpointId format CH##_CP_##_ShortName.

###### Tests
- Functional test: overlap checkpoint writes checkpoint save.
- Automation test: duplicate activation does not spam writes.

###### Done criteria
- Crossing checkpoint updates current checkpoint ID.
- Respawn from death uses checkpoint transform and required sectors.
- Save file contains checkpoint ID.

###### Failure modes
- Checkpoint triggers while player is falling through streaming boundary.
- Respawn transform faces wrong direction.
- Repeated overlaps create save spam.

- Copy paste ai prompt: Implement ATDFCheckpointActor and UTDFCheckpointSubsystem. Checkpoints need CheckpointId, ChapterId, RespawnTransform, RequiredLoadedSectors, auto-trigger overlap, duplicate-trigger suppression, and integration with UTDFSaveSubsystem checkpoint saves. Add tests for overlap save and duplicate suppression.

##### 2.2 — Implement death/respawn restore

- Id: 2.2
- Title: Implement death/respawn restore
###### Classes assets
- UTDFCheckpointSubsystem
- ATDFPlayerCharacter

###### Precise logic
- On player death: freeze input, fade out, load current checkpoint save if available, otherwise load last manual save, otherwise restart chapter start checkpoint.
- Respawn loads required sectors before setting player transform.
- Reset health to difficulty profile respawn health.
- Do not reset solved puzzles unless save says they were unsolved at checkpoint.

###### Editor hookups
- Hook player health component OnDeath to UTDFCheckpointSubsystem::HandlePlayerDeath.
- Add fade widget WBP_TDFFade.

###### Tests
- Functional test: die after checkpoint, respawn at checkpoint with correct sector loaded.
- Functional test: puzzle solved before checkpoint stays solved after death.

###### Done criteria
- Death never sends player to incorrect PlayerStart.
- No input during fade/load.
- Checkpoint state is consistent with save.

###### Failure modes
- Respawn loads persistent map only and omits sector.
- Puzzle state reverts incorrectly.
- Input enabled before fade completes.

- Copy paste ai prompt: Implement checkpoint respawn logic. On death, fade out, disable input, load checkpoint save or fallback save, stream required sectors, set player transform, restore health from difficulty profile, then fade in and re-enable input. Add functional tests for respawn sector loading and puzzle persistence.



### 3_objectives_and_chapters

- Phase: 3_objectives_and_chapters
- Goal: Create objective/chapter progression as data-driven state saved by ID.
#### Tasks

##### 3.1 — Define chapter and objective data assets

- Id: 3.1
- Title: Define chapter and objective data assets
###### Classes assets
- UTDFChapterDataAsset
- UTDFObjectiveDataAsset
- ETDFObjectiveState
- DT_TDFObjectiveText

###### Precise logic
- UTDFChapterDataAsset contains ChapterId, DisplayName, PersistentMapName, EntryCheckpointId, TArray<FName> ObjectiveIds.
- UTDFObjectiveDataAsset contains ObjectiveId, ChapterId, ShortText, LongText, OptionalTargetActorId, bShowCompass, bShowDistance, NextObjectiveIds.
- ETDFObjectiveState values: Locked, Active, Completed, Failed, Hidden.
- All objective IDs are FName and must be stable forever once shipped.

###### Editor hookups
- Create DA_CH01_RadioMast, DA_CH02_FloodedSubstation, etc.
- Create DA_OBJ_CH01_001_FindSignalSource style assets.
- Objective widget reads display text from active objective asset.

###### Tests
- Automation test: all chapter objective IDs resolve to data assets.
- Automation test: no duplicate objective IDs.

###### Done criteria
- Objective graph is data-driven.
- Duplicate/missing IDs are caught by validation.
- Save stores objective state by ObjectiveId.

###### Failure modes
- Designer renames asset and breaks save because asset path was used as ID.
- Objective graph loops without intended loop flag.
- Missing objective text in localized table.

- Copy paste ai prompt: Create data-driven chapter/objective progression for UE5. Add UTDFChapterDataAsset, UTDFObjectiveDataAsset, ETDFObjectiveState, stable FName IDs, validation for duplicates/missing objective IDs, and save records storing state by ObjectiveId. Add automation tests for graph validity.

##### 3.2 — Implement objective progression subsystem

- Id: 3.2
- Title: Implement objective progression subsystem
###### Classes assets
- UTDFObjectiveSubsystem
- FTDFObjectiveRuntimeState

###### Precise logic
- StartChapter(ChapterId) activates chapter entry objective and checkpoint.
- CompleteObjective(ObjectiveId) sets state Completed, broadcasts OnObjectiveCompleted, then activates each NextObjectiveId if locked.
- SetObjectiveTarget(ObjectiveId, StableActorId) allows runtime target assignment.
- Objective state changes immediately request autosave only at safe progression boundaries, not every UI tick.
- Chapter completes when all required objectives are Completed.

###### Editor hookups
- WBP_TDFObjectiveTracker binds to OnObjectiveChanged.
- Trigger volumes call CompleteObjective via BlueprintCallable function.
- Interactable actors expose ObjectiveIdToComplete.

###### Tests
- Automation test: completing objective unlocks next objective.
- Automation test: save/load restores active/completed objective states.
- Functional test: interactable completes objective and UI updates.

###### Done criteria
- Objective UI updates from subsystem event.
- Chapter state persists through save/load.
- Completing a required objective cannot be lost after checkpoint.

###### Failure modes
- Objective completed twice fires duplicate rewards.
- Objective UI polls every frame instead of event updates.
- Autosave fires during unsafe streaming moment.

- Copy paste ai prompt: Implement UTDFObjectiveSubsystem with StartChapter, CompleteObjective, objective state map by FName, next-objective activation, target actor IDs, event broadcasts, and save/load serialization. Add tests for unlock chain, duplicate completion suppression, and save/load restore.



### 4_streamed_level_architecture

- Phase: 4_streamed_level_architecture
- Goal: Create a manual streamed sector architecture with persistent level, deterministic sector IDs, and safe transitions.
#### Tasks

##### 4.1 — Define sector data

- Id: 4.1
- Title: Define sector data
###### Classes assets
- UTDFSectorDataAsset
- UTDFLevelStreamingSubsystem
- ATDFSectorVolume
- DA_Sector_CH01_Docks
- DA_Sector_CH01_RadioTower

###### Precise logic
- UTDFSectorDataAsset contains SectorId, LevelPackageName, NeighborSectorIds, RequiredForCheckpoints, bKeepLoadedAfterVisit.
- UTDFLevelStreamingSubsystem owns LoadedSectorIds and PendingSectorIds.
- SectorId is stable FName and saved.
- Use LoadStreamLevel/UnloadStreamLevel or level instance equivalent consistently; do not mix ad hoc Blueprint streaming.

###### Editor hookups
- Persistent map contains only global lighting, sky, managers, nav baseline, player start fallback, and sector trigger volumes.
- Each sector level contains gameplay actors for that sector.
- Place BP_TDFSectorVolume at entrances/exits with EnterSectorIds and ExitSectorIds.

###### Tests
- Automation validation: all SectorData assets reference valid map packages.
- Functional test: entering SectorVolume loads target sector and unloads old sector if allowed.

###### Done criteria
- Sectors can be loaded/unloaded manually by ID.
- Loaded sector list is saved and restored.
- Persistent level remains thin.

###### Failure modes
- Gameplay actor placed in persistent level by accident.
- Sector unload destroys actor before save captures state.
- Two sector assets reference same SectorId.

- Copy paste ai prompt: Implement manual streamed sector architecture. Add UTDFSectorDataAsset, ATDFSectorVolume, and UTDFLevelStreamingSubsystem with stable SectorId FNames, LevelPackageName references, loaded/pending sets, delegates for sector loaded/unloaded, and validation tests for duplicate IDs and valid packages.

##### 4.2 — Add safe streaming transition logic

- Id: 4.2
- Title: Add safe streaming transition logic
###### Classes assets
- UTDFLevelStreamingSubsystem
- WBP_TDFLoadingStinger

###### Precise logic
- RequestSectorSet(TargetSectorIds, Reason) computes sectors to load and unload.
- Set bStreamingTransitionActive=true before starting.
- Block manual save while bStreamingTransitionActive.
- Load all new sectors, wait visible, move blockers/nav state, then unload old sectors.
- Set bStreamingTransitionActive=false and broadcast OnSectorSetReady.

###### Editor hookups
- Sector transition volumes call RequestSectorSet.
- Optional loading stinger appears only when estimated transition exceeds threshold.

###### Tests
- Functional test: rapid back-and-forth volume overlap coalesces requests.
- Automation test: save subsystem CanSaveNow false during transition.

###### Done criteria
- No save writes during transition.
- No player fall-through during unloaded geometry gap.
- Repeated sector requests settle into final requested set.

###### Failure modes
- Unload old sector before new collision is ready.
- Race condition from overlapping volumes.
- Loading stinger remains forever after failed load.

- Copy paste ai prompt: Add safe streaming transitions to UTDFLevelStreamingSubsystem. RequestSectorSet should coalesce rapid requests, set a transition flag, load and wait for visibility before unloading old sectors, block saves during transition, handle failure with timeout, and broadcast completion. Add tests for transition save blocking and rapid overlap coalescing.

##### 4.3 — Add streamed actor registration timing

- Id: 4.3
- Title: Add streamed actor registration timing
###### Classes assets
- UTDFSaveSubsystem
- UTDFStableActorComponent

###### Precise logic
- When a sector becomes visible, its actors register stable IDs.
- Load system waits for registration count or one frame after visibility before applying actor save data.
- When unloading, sector actors unregister after save subsystem has had a chance to snapshot if needed.
- On duplicate actor ID, include sector ID and actor name in error.

###### Editor hookups
- Add validation map command tdf.ValidateStableIds AllSectors.

###### Tests
- Functional test: actor in streamed sector restores state after load.
- Automation test: duplicate ID error includes sector name.

###### Done criteria
- Save/load works for actors inside streamed levels.
- Duplicate ID reports are actionable.

###### Failure modes
- Actor state applied before BeginPlay registration.
- Unregistered actor remains in save registry after sector unload.
- Duplicate IDs only visible in packaged build.

- Copy paste ai prompt: Integrate streamed actor registration with save/load. Ensure stable actors register when streamed sectors become visible, unregister on unload, load waits until registration before applying actor data, and duplicate errors include sector and actor names. Add functional test for state restore inside a streamed sector.



### 5_menus_and_settings

- Phase: 5_menus_and_settings
- Goal: Create main menu, pause menu, save/load UI, and persistent settings before accessibility polish.
#### Tasks

##### 5.1 — Create menu map and main menu flow

- Id: 5.1
- Title: Create menu map and main menu flow
###### Classes assets
- L_Menu
- WBP_TDFMainMenu
- WBP_TDFSaveSlot
- UTDFMenuViewModel

###### Precise logic
- New Game starts Chapter 1 at EntryCheckpointId and writes initial autosave after sector ready.
- Continue loads newest valid save.
- Load Game lists manual and checkpoint saves with timestamp, chapter, difficulty, playtime.
- Disable Continue if no valid save exists.
- Never initialize Steam dependency here yet.

###### Editor hookups
- Project default map can remain dev map until packaged flow is ready.
- Add L_Menu to packaging list.
- Main menu widget added by menu GameMode on BeginPlay.

###### Tests
- Functional test: no saves disables Continue.
- Functional test: New Game creates initial save after first checkpoint.
- Functional test: Load Game lists valid saves only.

###### Done criteria
- Main menu can start, continue, and load.
- Invalid/corrupt saves are hidden or marked unusable with message.
- No crash if save folder empty.

###### Failure modes
- Continue tries to load nonexistent slot.
- New Game overwrites manual save without confirmation.
- Menu requires Steam before Steam integration is ready.

- Copy paste ai prompt: Build the main menu flow for The Drowned Frequency. Add WBP_TDFMainMenu, save slot widget/view model, New Game, Continue newest valid save, Load Game list, disabled Continue when no save exists, and no Steam dependency. Add functional tests for empty saves, new game initial save, and save list filtering.

##### 5.2 — Create pause menu and manual save/load

- Id: 5.2
- Title: Create pause menu and manual save/load
###### Classes assets
- WBP_TDFPauseMenu
- WBP_TDFConfirmDialog
- ATDFPlayerController

###### Precise logic
- Pause toggles input mode UI only and pauses world unless streaming transition or non-pausable cutscene.
- Manual Save button enabled only when UTDFSaveSubsystem::CanSaveNow returns true.
- Load button asks confirmation because unsaved progress may be lost.
- Settings button opens settings stack without unpausing.

###### Editor hookups
- Bind Escape/Gamepad Start to IA_Pause.
- PlayerController owns menu stack.
- Pause widget uses Enhanced Input UI navigation.

###### Tests
- Functional test: Save disabled during streaming transition.
- Functional test: Load confirmation appears before load.
- Functional test: Pause/unpause restores game input.

###### Done criteria
- Pause menu works with keyboard/mouse and controller.
- Manual save respects CanSaveNow.
- No stuck UI input mode after closing.

###### Failure modes
- Game remains paused after load.
- Input mode stuck in UI only.
- Manual save enabled during unsafe moment.

- Copy paste ai prompt: Implement WBP_TDFPauseMenu and PlayerController menu stack. Bind pause input, support controller navigation, enable Manual Save only when CanSaveNow is true, add load confirmation dialog, and ensure input mode restores correctly. Add functional tests for save disabled during streaming, confirmation, and pause input restoration.

##### 5.3 — Create settings subsystem

- Id: 5.3
- Title: Create settings subsystem
###### Classes assets
- UTDFSettingsSubsystem
- UTDFUserSettings
- WBP_TDFSettingsMenu
- WBP_TDFInputRemapRow

###### Precise logic
- Store display, audio, gameplay, accessibility, input mappings, subtitle, and difficulty-related preferences separately from progress saves.
- Apply settings on GameInstance init.
- Settings save immediately on Apply, not every slider tick.
- Input remap supports keyboard/mouse and controller actions through Enhanced Input mapping contexts.
- Reset to Defaults affects settings only, not progress saves.

###### Editor hookups
- Create IMC_TDFGameplay and IMC_TDFMenu.
- Create input actions: IA_Move, IA_Look, IA_Interact, IA_Crouch, IA_Run, IA_Pause, IA_ObjectiveFocus.
- Settings menu exposes remap rows for all player-facing actions.

###### Tests
- Automation test: settings serialize separately from UTDFSaveGame.
- Functional test: remap Interact to another key and verify action fires.
- Functional test: Reset Defaults restores original mapping.

###### Done criteria
- Settings persist across restart.
- Progress save deletion does not delete settings unless explicitly requested.
- Remapping works and displays conflicts.

###### Failure modes
- Settings stored inside progress slot only.
- Remap creates duplicate bindings without warning.
- Apply button changes runtime but not saved config.

- Copy paste ai prompt: Create UTDFSettingsSubsystem and UTDFUserSettings separate from progress saves. Include display/audio/gameplay/accessibility/subtitle/input mapping settings, Apply/Reset Defaults, Enhanced Input remapping with conflict detection, and settings persistence across restart. Add tests for separate serialization, remap behavior, and reset defaults.



### 6_accessibility

- Phase: 6_accessibility
- Goal: Implement accessibility as first-class settings and testable gameplay modifiers.
#### Tasks

##### 6.1 — Add accessibility settings model

- Id: 6.1
- Title: Add accessibility settings model
###### Classes assets
- UTDFAccessibilitySubsystem
- FTDFAccessibilitySettings
- WBP_TDFAccessibilityMenu

###### Precise logic
- Settings include bReducedFlicker, bReducedCameraShake, bHoldInsteadOfMash, bToggleAimOrFocus, bHighContrastInteractables, bLargeText, bObjectiveDirectionCaptions, bSubtitleSpeakerNames, bSubtitleBackground.
- Subsystem applies changes by broadcasting OnAccessibilitySettingsChanged.
- Camera shake and flicker systems must query this subsystem before playing intense effects.

###### Editor hookups
- Add Accessibility tab in settings menu.
- Expose reduced flicker/shake toggles near top, not buried.
- Add preview area for subtitle/caption text size and background.

###### Tests
- Automation test: settings save/load preserves accessibility values.
- Functional test: reduced shake disables or scales camera shake.
- Functional test: reduced flicker swaps flickering light pattern.

###### Done criteria
- Accessibility settings persist.
- Reduced flicker and shake affect real gameplay effects.
- UI preview matches in-game subtitle style.

###### Failure modes
- Toggle exists but effect systems ignore it.
- Only new effects respect setting, old placed effects do not.
- Text size clips in menu.

- Copy paste ai prompt: Implement UTDFAccessibilitySubsystem and FTDFAccessibilitySettings with reduced flicker, reduced camera shake, hold instead of mash, toggles, high contrast interactables, large text, objective direction captions, subtitle speaker names, and subtitle background. Wire settings menu controls and add tests proving settings persist and affect camera shake/flicker systems.

##### 6.2 — Hook reduced flicker and shake into effect components

- Id: 6.2
- Title: Hook reduced flicker and shake into effect components
###### Classes assets
- UTDFFlickerLightComponent
- UTDFCameraShakeRouter
- DA_TDFEffectAccessibilityRules

###### Precise logic
- UTDFFlickerLightComponent uses normal curve when bReducedFlicker=false and reduced/intensity-clamped curve when true.
- UTDFCameraShakeRouter centralizes camera shake requests and applies multiplier 0.0 to 0.35 when reduced shake is enabled.
- No gameplay-critical information may rely only on flicker or shake.

###### Editor hookups
- Replace direct PlayWorldCameraShake calls with UTDFCameraShakeRouter.
- Replace raw flickering light timelines with UTDFFlickerLightComponent.

###### Tests
- Automation test: reduced shake multiplier applies.
- Functional test: placed flicker light changes behavior when setting toggled.

###### Done criteria
- No direct intense camera shake calls remain in gameplay code.
- Reduced flicker visibly changes flicker components.
- Accessibility toggle applies without map reload.

###### Failure modes
- Some Blueprints still call camera shake directly.
- Reduced flicker requires restart.
- Gameplay hint becomes invisible with flicker reduced.

- Copy paste ai prompt: Create UTDFCameraShakeRouter and UTDFFlickerLightComponent. Route camera shake through accessibility-aware multiplier, make flicker lights switch to reduced curves at runtime, and add tests for shake scaling and live flicker setting changes. Avoid direct camera shake calls outside the router.



### 7_subtitles_waveform_compass

- Phase: 7_subtitles_waveform_compass
- Goal: Build captions with speaker, direction, distance, and waveform/radio compass support.
#### Tasks

##### 7.1 — Create subtitle/caption data model

- Id: 7.1
- Title: Create subtitle/caption data model
###### Classes assets
- UTDFSubtitleSubsystem
- FTDFCaptionLine
- DT_TDFCaptions
- WBP_TDFSubtitleLine
- WBP_TDFSubtitleStack

###### Precise logic
- FTDFCaptionLine includes CaptionId, SpeakerName, Text, AudioEventId, SourceStableActorId optional, SourceWorldLocation optional, StartTime, Duration, bRadioTransmission, bImportantObjectiveHint.
- Subtitle display uses localized text table by CaptionId.
- If SourceStableActorId is valid, resolve to actor location each tick at low frequency, e.g. 5 Hz, not every frame.
- Distance text format: '<10m', '24m', 'far' depending accessibility setting.
- Direction text uses relative sectors: ahead, behind, left, right, above, below, or compass bearing if enabled.

###### Editor hookups
- Create WBP_TDFSubtitleStack anchored bottom center.
- Add subtitle preview in settings.
- Audio/dialogue triggers call UTDFSubtitleSubsystem::PushCaption.

###### Tests
- Automation test: source in front-left returns correct direction label.
- Automation test: missing source still shows text without direction/distance.
- Functional test: subtitle appears for radio trigger and disappears after duration.

###### Done criteria
- Captions can show text, speaker, direction, and distance.
- Captions remain readable with large text and background.
- Missing actor source does not crash.

###### Failure modes
- Direction calculated in world space instead of player-relative space.
- Distance updates every frame causing UI churn.
- Caption text hardcoded instead of localized.

- Copy paste ai prompt: Implement UTDFSubtitleSubsystem with FTDFCaptionLine, localized CaptionId text, speaker names, optional source stable actor/world location, direction and distance labels relative to player, radio flag, and WBP subtitle stack. Add tests for front-left direction, missing source fallback, and timed expiration.

##### 7.2 — Create waveform compass widget

- Id: 7.2
- Title: Create waveform compass widget
###### Classes assets
- WBP_TDFWaveformCompass
- UTDFWaveformCompassComponent
- MAT_TDFWaveformLine
- DA_TDFCompassTuning

###### Precise logic
- Waveform compass visualizes active radio/audio objective direction, not all sounds.
- Input source is ObjectiveTargetLocation or active radio transmission source.
- Compute bearing delta = normalized yaw from player forward to target direction.
- Widget displays waveform intensity from signal strength curve based on distance and story state.
- If bObjectiveDirectionCaptions=true, also emit caption hint: 'Signal ahead, 32m'.
- Do not reveal hidden objectives unless objective data allows compass.

###### Editor hookups
- Add WBP_TDFWaveformCompass to HUD.
- Bind visibility to active objective bShowCompass or active radio transmission.
- Tune colors and motion under reduced flicker rules.

###### Tests
- Automation test: target to player right produces positive right bearing.
- Functional test: objective with bShowCompass=false never appears.
- Functional test: reduced flicker lowers waveform flash intensity.

###### Done criteria
- Compass points toward eligible objective/radio sources.
- Distance/direction captions match subtitle accessibility setting.
- Reduced flicker affects waveform animation.

###### Failure modes
- Compass points to hidden objectives.
- Bearing flips left/right due coordinate sign error.
- Waveform animation violates reduced flicker setting.

- Copy paste ai prompt: Implement WBP_TDFWaveformCompass and supporting component. It should point only to eligible active objective or radio sources, compute player-relative bearing, scale waveform intensity by distance/story signal strength, emit optional direction/distance captions, and respect reduced flicker. Add tests for bearing sign, hidden objective suppression, and reduced flicker intensity.



### 8_difficulty_profiles

- Phase: 8_difficulty_profiles
- Goal: Add three difficulty profiles with explicit tunables and save-safe behavior.
#### Tasks

##### 8.1 — Define difficulty data assets

- Id: 8.1
- Title: Define difficulty data assets
###### Classes assets
- UTDFDifficultyProfile
- UTDFDifficultySubsystem
- DA_Difficulty_Story
- DA_Difficulty_Standard
- DA_Difficulty_Hard

###### Precise logic
- Three profiles: Story, Standard, Hard.
- Profile fields: PlayerDamageTakenMultiplier, EnemyAwarenessMultiplier, ResourceScarcityMultiplier, PuzzleHintDelaySeconds, CheckpointRespawnHealth, AssistCompassStrength, QTEWindowMultiplier.
- Difficulty choice is saved in progress save and can be changed from settings only if design allows.
- Changing difficulty applies immediately to runtime systems through OnDifficultyChanged.

###### Editor hookups
- Create three data assets.
- New Game menu requires difficulty selection before starting.
- Settings Gameplay tab exposes difficulty change if permitted.

###### Tests
- Automation test: all three default profiles load.
- Functional test: New Game stores selected difficulty in save.
- Automation test: damage multiplier changes damage result.

###### Done criteria
- Story, Standard, Hard exist as data assets.
- Selected profile persists in save.
- At least damage and hint delay use the profile.

###### Failure modes
- Difficulty stored only in settings, causing all saves to change unintentionally.
- Profile exists but gameplay systems ignore it.
- Hard mode blocks accessibility options.

- Copy paste ai prompt: Implement UTDFDifficultyProfile data assets and UTDFDifficultySubsystem with Story, Standard, and Hard. Include damage, awareness, resource scarcity, hint delay, respawn health, compass assist, and QTE window multipliers. Save selected difficulty per progress slot, broadcast OnDifficultyChanged, and add tests for asset loading, save persistence, and damage multiplier behavior.

##### 8.2 — Hook difficulty into gameplay systems

- Id: 8.2
- Title: Hook difficulty into gameplay systems
###### Classes assets
- UTDFHealthComponent
- UTDFEnemyPerceptionComponent
- UTDFResourceSpawnerComponent
- UTDFHintSubsystem

###### Precise logic
- Health applies PlayerDamageTakenMultiplier to incoming damage before armor/resistance unless design says otherwise.
- Enemy perception scales sight/hearing thresholds by EnemyAwarenessMultiplier.
- Resource spawners scale spawn counts/chances by ResourceScarcityMultiplier.
- Hint subsystem uses PuzzleHintDelaySeconds and accessibility setting may request earlier non-spoiler hint without changing difficulty.

###### Editor hookups
- Replace hardcoded damage/perception/resource constants with profile queries.
- Add debug overlay showing active difficulty profile and current multipliers.

###### Tests
- Automation test: Story reduces damage compared to Standard.
- Automation test: Hard increases awareness compared to Standard.
- Functional test: hint delay differs by difficulty.

###### Done criteria
- All defined tunables affect real systems.
- Difficulty can be verified in debug overlay.
- Accessibility assists remain independent from difficulty.

###### Failure modes
- Difficulty changes require restart.
- Accessibility options locked behind Story mode.
- Resource scarcity affects key progression items.

- Copy paste ai prompt: Wire UTDFDifficultySubsystem into health, enemy perception, resource spawning, and hint timing. Replace hardcoded constants with profile values, keep accessibility independent from difficulty, and add tests comparing Story/Standard/Hard damage, awareness, and hint timing.



### 9_packaging_before_steam

- Phase: 9_packaging_before_steam
- Goal: Stabilize packaged Windows builds before Steam integration.
#### Tasks

##### 9.1 — Create packaged build smoke checklist

- Id: 9.1
- Title: Create packaged build smoke checklist
###### Classes assets
- Windows Shipping Build
- L_Menu
- L_Persistent_CH01

###### Precise logic
- Package Development build first, then Shipping.
- Run without editor.
- Verify main menu, new game, sector streaming, checkpoint save, manual save, quit, relaunch, continue, subtitles, settings persistence.
- Do not add Steam until these pass repeatedly.

###### Editor hookups
- Project Settings > Packaging: include all menu, persistent, and streamed sector maps.
- Add any data asset directories to cook rules if not referenced directly.
- Set startup map to L_Menu only when ready.

###### Tests
- Manual packaged smoke test checklist.
- Automated functional tests run in editor before packaging.
- Optional commandlet validation for assets and stable IDs.

###### Done criteria
- Packaged build can start new game and continue saved game after process restart.
- No missing map/asset cook errors.
- Save files work outside editor.

###### Failure modes
- Streamed maps not cooked.
- Data assets only referenced by soft path and omitted from cook.
- Save path differs between editor and packaged build.

- Copy paste ai prompt: Create a UE5 packaged-build readiness checklist and validation commandlets for The Drowned Frequency. Verify all maps/data assets are cooked, packaged Windows build can start New Game, stream sectors, checkpoint save, manual save, quit, relaunch, Continue, and persist settings. Do not add Steam yet.



### 10_steam_integration_timing

- Phase: 10_steam_integration_timing
- Goal: Integrate Steam only after saves and packaged builds are stable.
#### Tasks

##### 10.1 — Gate Steam integration behind readiness criteria

- Id: 10.1
- Title: Gate Steam integration behind readiness criteria
###### Classes assets
- UTDFSteamSubsystem
- UTDFPlatformSubsystem

###### Precise logic
- Steam work begins only after: save/load round-trip passes in packaged build, streamed sectors cook/load, menus/settings persist, and checkpoint respawn works.
- UTDFPlatformSubsystem abstracts platform calls so Steam is optional in development.
- No gameplay system should directly depend on Steam API.

###### Editor hookups
- Add OnlineSubsystemSteam config only on Steam branch or after readiness gate.
- Keep Null subsystem usable for non-Steam local testing.

###### Tests
- Automation test: game boots with Null platform subsystem.
- Packaged test: non-Steam build still saves/loads.

###### Done criteria
- Steam is not required for core game loop.
- Readiness criteria are all checked before Steam implementation starts.

###### Failure modes
- Steam overlay failure blocks save/load testing.
- Gameplay code calls Steam directly.
- Steam branch changes packaging settings before base build is stable.

- Copy paste ai prompt: Add a platform abstraction layer for The Drowned Frequency. Create UTDFPlatformSubsystem and UTDFSteamSubsystem stubs, but keep Steam disabled until packaged save/load, streaming, checkpoints, menus, and settings are stable. Ensure the game boots and saves with a Null platform subsystem.

##### 10.2 — Add Steam achievements after progression is stable

- Id: 10.2
- Title: Add Steam achievements after progression is stable
###### Classes assets
- UTDFSteamSubsystem
- DT_TDFAchievements

###### Precise logic
- Achievements listen to objective/chapter events; they do not drive progression.
- Achievement IDs map from stable internal IDs to Steam stat/achievement API names.
- Queue achievement unlocks if Steam is unavailable and retry next session.
- Never block save/load on achievement result.

###### Editor hookups
- Create DT_TDFAchievements with InternalAchievementId, SteamAchievementName, TriggerObjectiveId or TriggerChapterId.
- Bind UTDFObjectiveSubsystem events to platform subsystem.

###### Tests
- Automation test: completing objective emits internal achievement event.
- Mock platform test: Steam unavailable queues unlock.
- Packaged Steam test: overlay and achievement unlock work after base build passes.

###### Done criteria
- Achievements unlock from progression events.
- Offline/unavailable Steam does not break game.
- Achievement queue persists safely.

###### Failure modes
- Achievement unlock happens before save and is lost/inconsistent.
- Steam API failure blocks objective completion.
- Achievement names differ from Steamworks backend.

- Copy paste ai prompt: Implement Steam achievements only as listeners to stable objective/chapter events. Add DT_TDFAchievements mapping internal IDs to Steam names, queue unlocks if Steam is unavailable, retry later, and never block progression or save/load. Add mock platform tests for unavailable Steam.

##### 10.3 — Add Steam Cloud only after local saves are proven

- Id: 10.3
- Title: Add Steam Cloud only after local saves are proven
###### Classes assets
- UTDFSteamCloudSaveAdapter
- UTDFSaveSubsystem

###### Precise logic
- Local save remains source of truth.
- Cloud upload happens after successful local save.
- Cloud download conflict UI compares timestamp, playtime, chapter, and checkpoint.
- Never silently overwrite newer local save.
- Cloud errors show non-blocking warning and keep local save usable.

###### Editor hookups
- Add WBP_TDFCloudConflictDialog.
- Enable Steam Cloud paths only after local packaged save test passes.

###### Tests
- Mock test: local newer than cloud chooses local by default.
- Mock test: cloud newer prompts conflict dialog.
- Mock test: cloud upload failure does not mark local save failed.

###### Done criteria
- Steam Cloud cannot corrupt local saves silently.
- Cloud failure does not prevent play.
- Conflict UI is understandable.

###### Failure modes
- Cloud overwrite destroys newer local progress.
- Upload failure reported as save failure.
- Conflict compares only timestamp and ignores playtime/checkpoint.

- Copy paste ai prompt: Design Steam Cloud save integration for The Drowned Frequency with local save as source of truth. Upload only after successful local save, compare conflicts using timestamp/playtime/chapter/checkpoint, never silently overwrite newer local data, and keep cloud errors non-blocking. Add mock tests for conflict and upload failure behavior.




## Global test matrix

### 1

- Area: save_load
#### Tests
- New game creates initial save after first sector/checkpoint ready.
- Manual save restores player transform, chapter, objective, sector set, actor states.
- Checkpoint save restores checkpoint transform and required sectors.
- Corrupt save fails gracefully with user-facing message.
- Older save migrates to current version.
- Newer save is rejected safely.


### 2

- Area: streaming
#### Tests
- Entering sector volume loads target sector.
- Leaving sector unloads old sector only after new sector is visible.
- Save is blocked during transition.
- Loaded sector set persists through save/load.
- Packaged build includes all streamed sector maps.


### 3

- Area: objectives_chapters
#### Tests
- StartChapter activates entry objective.
- CompleteObjective unlocks next objectives once.
- Objective UI updates by event.
- Objective state persists through save/load.
- Chapter completion fires when required objectives complete.


### 4

- Area: menus_settings
#### Tests
- Continue disabled with no saves.
- Load list shows valid saves only.
- Pause menu restores input mode.
- Manual save button respects CanSaveNow.
- Settings persist across restart.
- Input remap works and detects conflicts.


### 5

- Area: accessibility_subtitles_compass
#### Tests
- Reduced camera shake scales or suppresses shakes.
- Reduced flicker swaps flicker behavior.
- Subtitles show speaker, text, direction, distance.
- Missing subtitle source does not crash.
- Waveform compass points to eligible active target only.
- Large text does not clip key UI.


### 6

- Area: difficulty
#### Tests
- Story, Standard, Hard profiles load.
- Selected difficulty persists per save.
- Damage, awareness, resources, hint delay use profile values.
- Accessibility options remain independent from difficulty.


### 7

- Area: steam
#### Tests
- Null platform build boots and saves without Steam.
- Achievements listen to progression events only.
- Steam unavailable queues achievement unlocks.
- Cloud upload failure does not fail local save.
- Cloud conflict never silently overwrites newer local save.



## Recommended order
- Foundation and stable actor IDs
- Versioned local save/load
- Checkpoints and respawn
- Objective/chapter progression
- Manual streamed sector architecture
- Menus and settings
- Accessibility and effect hooks
- Subtitles and waveform compass
- Difficulty profiles
- Packaged Windows build stabilization
- Steam achievements
- Steam Cloud

## Hard gates

### 1

- Gate: Do not implement Steam before local saves are stable.
#### Criteria
- Packaged build can New Game, save, quit, relaunch, Continue.
- Checkpoint respawn works in packaged build.
- Streamed maps are cooked and restored from saves.
- Settings persist separately from progress saves.


### 2

- Gate: Do not expand content before stable actor IDs are validated.
#### Criteria
- Duplicate GUID scan passes.
- All saveable actors in streamed sectors register by stable ID.
- Save/load can tolerate missing actor IDs.


### 3

- Gate: Do not rely on accessibility toggles until hooked into real systems.
#### Criteria
- Reduced flicker affects placed flicker components.
- Reduced shake affects camera shake router.
- Subtitle and compass UI support large text/readability settings.



