# The Drowned Frequency — Exact Master Build Order

This is the dependency order. The eight specialist manuals contain the click-by-click actions, code shapes, tests, common failures, and copy-paste prompts.

Do one task at a time. Never hand an AI an entire phase. Never accept “implemented” without a real compile and the task’s stated test.

## Locked vocabulary

- Project and module: `DrownedFrequency`
- C++ prefix: `TDF`
- Content root: `Content/Game`
- Source root: `Source/DrownedFrequency`
- Sound categories: Voice, Footstep, Machine, Bell/Foghorn, Water, Static, Impact
- “Masked traversal” means moving while environmental noise masks player sound. The player does not wear a mask.
- C++ owns mechanics and persistent truth. Blueprints own presentation and room scripting. Data Assets own definitions and tuning.

## How to execute one task

1. Open the referenced manual and locate the exact task ID.
2. Copy its AI prompt into the task envelope from `09_AI_PROMPT_PROTOCOL.md`.
3. Give the AI the repository path and current branch.
4. Require it to inspect existing classes and project instructions before editing.
5. Require a diff, compile/build output, tests run, and remaining risks.
6. Review the diff. Reject unrelated refactors.
7. Run the task’s test yourself in Unreal when it requires editor judgment.
8. Commit only when every done criterion passes.

## Phase 0 — Design and toolchain lock

Execute:

1. Confirm one stable UE5 release and do not upgrade during the vertical slice.
2. Confirm a C++ toolchain: Xcode for Mac editor work; Windows Visual Studio toolchain or Windows CI for final Windows packaging.
3. Create the reference boards and identity palette in `06_ART_MODELING_AUDIO.md`, Phase 0.
4. Create an external asset-license ledger before downloading Marketplace or audio assets.
5. Record the minimum-spec Windows test machine. This is the real 60 FPS target.

Gate:

- Engine launches and can compile an empty C++ target.
- Windows packaging route is identified, even if development is on Mac.
- Visual identity, hero-asset list, and buy-versus-custom rules are approved.

## Phase 1 — Blank project to clean compile

Use `01_PROJECT_FOUNDATION.md` tasks 1–8 exactly:

1. Create blank C++ project.
2. Initialize Git and Git LFS.
3. Generate IDE files and compile.
4. Set project metadata.
5. Create content folder skeleton.
6. Commit the clean skeleton.
7. Enable Enhanced Input.
8. Add only the foundation Build.cs modules.

Then execute `05_SAVE_LEVELS_UI.md` task 0.1 to lock subsystem and class naming.

Gate:

- Editor opens, closes, and reopens without a missing-module dialog.
- Development Editor target compiles from the command line or IDE.
- Git status excludes Binaries, Saved, Intermediate, and DerivedDataCache.
- LFS tracks `.uasset`, `.umap`, audio, textures, FBX, and Blender source.

## Phase 2 — Player foundation

Skip foundation tasks 9–17 because `02_PLAYER_AND_INTERACTION.md` replaces their placeholder versions with the final architecture.

Execute `02_PLAYER_AND_INTERACTION.md` tasks 1–13:

1. Class/folder skeleton.
2. Enhanced Input asset contract.
3. Controller mapping contexts.
4. Baseline movement and look.
5. Central movement-state model.
6. Sprint.
7. Lean.
8. Hold breath.
9. Camera component.
10. Interactable interface.
11. Interaction trace.
12. Prompt widget contract.
13. Input-device and glyph tracking.

After task 13, return to `01_PROJECT_FOUNDATION.md` tasks 18–19:

- Create persistent and test maps.
- Set default editor and game maps.

Gate:

- Keyboard/mouse and controller both move, look, crouch, sprint, lean, hold breath, and focus an interactable.
- Sprint cannot coexist with crouch or held breath.
- Lean moves the camera, not the capsule.
- Missing input assets log an error rather than crash.
- All of this runs in a standalone Development build, not only PIE.

## Phase 3 — World interaction and key items

Execute `02_PLAYER_AND_INTERACTION.md` tasks 14–26:

- Door actor
- Switch actor
- Item Data Asset
- Key-item inventory
- World-item actor
- Inspection mode and rotation
- Locked door/key integration
- Persistent state IDs/structs
- Remapping-ready settings
- Controller polish
- Integrated test map
- Automation/manual checklist

Important dependency correction:

- Task 22 defines only the initial persistent-state contract.
- `05_SAVE_LEVELS_UI.md` task 0.2 is the authoritative stable actor ID implementation. Use `FGuid`, detect duplicates, and never regenerate IDs at load.

Gate:

- The test map contains one locked door, one key, one inspectable item, one switch, one crouch route, and one lean corner.
- The complete route works without Level Blueprint gameplay truth.
- Both control schemes complete it.
- Inventory and door lock behavior have automated tests.

## Phase 4 — Acoustic spine

Execute `03_ACOUSTIC_SYSTEM.md` tasks A01–A14 in order:

- Tags
- Core types
- Audio Anomaly Data Asset
- Audio Anomaly actor
- Recorded signal
- Slot set
- Recorder component
- Local playback
- Projection puck
- Acoustic stimulus struct
- Acoustic world subsystem
- Player-noise conversion
- Receiver component
- No-softlock ledger

Use this exact category mapping:

- `Acoustic.Category.Voice`
- `Acoustic.Category.Footstep`
- `Acoustic.Category.Machine`
- `Acoustic.Category.Bell`
- `Acoustic.Category.Water`
- `Acoustic.Category.Static`
- `Acoustic.Category.Impact`

Do not add an “Anomaly” category. An anomaly is a source/asset type, not one of the seven sound categories.

Gate:

- A placed authored source records into one of three slots.
- Local playback emits both audio presentation and `FAcousticStimulus`.
- A puck emits the same signal identity from a different world position.
- A receiver evaluates deterministic metadata, not the sound waveform.
- Deleting every carried clip still leaves required puzzles recoverable.
- Automation tests enforce exactly seven categories and 3→4→5 slot progression.

## Phase 5 — Vertical-slice acoustic depth

Execute `03_ACOUSTIC_SYSTEM.md` tasks A15–A25:

- Fidelity/repetition
- Contamination/overdub
- Pipe routing
- Speaker routing
- Captions
- Blueprint presentation
- Debug tools
- Save integration contract
- Editor templates/data validation
- Functional acoustic test map
- Full validation suite

Do not expose overdub to the player in the vertical slice; implement and test its data path, then gate the ability until Act V.

Gate:

- Clean, degraded, and contaminated variants are authored assets.
- Repetition changes AI credibility metadata deterministically.
- Pipe, speaker, and puck routing preserve source identity and route history.
- Every critical sound has caption metadata.
- Data Validation and acoustic automation tests pass unattended.

## Phase 6 — Local saves before complex AI

Execute `05_SAVE_LEVELS_UI.md` tasks in this order:

1. 0.2 stable actor ID interface/component
2. 1.1 save schema
3. 1.2 write path
4. 1.3 load path
5. 1.4 migrations
6. 2.1 checkpoint actor
7. 2.2 death/respawn restore
8. 3.1 chapter/objective Data Assets
9. 3.2 objective subsystem
10. 4.1 sector data
11. 4.2 safe streaming transitions
12. 4.3 streamed-actor registration timing

Then execute `01_PROJECT_FOUNDATION.md` task 20 only as a cross-check; do not create a second streaming manager.

Gate:

- Save, quit, relaunch, and load restore player, signals, key items, objective, puzzle actor states, and loaded sectors.
- Save is blocked during streaming transitions.
- Duplicate stable GUIDs fail validation.
- Checkpoint reload waits for its sector before placing the player.
- A missing old actor logs and continues instead of crashing.

## Phase 7 — Listener AI foundation

The AI manual is phrased as design/specification prompts. When executing each task, tell the implementation AI to write the listed test first and then implement only that task.

Execute `04_LISTENER_AI.md`:

1. 01.01–01.02 contracts and Data Assets
2. 02.01–02.02 stimulus API/integration cross-check
3. 03.01–03.02 scoring and arbitration
4. 04.01–04.02 brain, Blackboard, and Behavior Tree shell
5. 05.01 Patrol
6. 05.02 Investigate
7. 05.03 Search
8. 05.04 Stalk
9. 05.05 Chase
10. 05.06 Retreat

Reuse the existing acoustic subsystem. Do not create another `UAcousticStimulusSubsystem` if `UAcousticWorldSubsystem` already exists; rename references in the prompt to the actual class.

Gate:

- C++ owns state transitions; the Behavior Tree executes tasks only.
- A debug overlay explains every stimulus score factor.
- An occluded sound scores below an equivalent unobstructed sound.
- Sound alone does not teleport the AI into Chase.
- All six states have deterministic transition tests.

## Phase 8 — Fairness, manifestations, and director

Execute `04_LISTENER_AI.md` sections 06–13:

- Encounter director pressure and eligibility
- Drowned Technician zone controller and counterplay
- Antenna Choir sector controller and counterplay
- Fairness tells and telemetry
- Navmesh/two-route validation
- Enemy-compatible doors
- Death cause and retry scaling
- Debug overlay/logging
- Automated and manual QA maps

Gate:

- No dangerous spawn is enabled before its warning test passes.
- Every chase test map validates two traversable routes.
- A threat cannot appear behind the player without warning lead time.
- After two deaths, persistence or hazard intensity decreases.
- Drowned Technician and Choir use zone/sector controllers, not second and third full roaming AIs.

## Phase 9 — Build the vertical slice in graybox

Use `02_VERTICAL_SLICE.md` as the authoritative slice flow.

Build in this order:

1. Safe radio room
2. Dormitory corridor
3. Bunk rooms
4. Laundry lure space
5. Flooded maintenance crawlspace
6. Generator relay vestibule
7. Generator hall overlook

For each room:

- Block collision and sightlines.
- Place the player route and at least one alternative around the Listener.
- Add temporary audio required for gameplay.
- Add puzzle state, checkpoint/reset behavior, and debug readouts.
- Blind-test before adding final art.

Do not use `07_LEVEL_CONTENT_DRAFT.md` A1 placement to redefine the full campaign yet; the dormitory is a standalone slice until it passes the greenlight criteria.

Gate:

- Four of five testers explain the tool/risk loop.
- Four of five explain why detection occurred.
- Median completion is 18–30 minutes.
- Nobody soft-locks.
- Four of five want to continue.
- Save/load restores clip, puck, puzzle, and AI state.

## Phase 10 — Art the vertical slice

Execute `06_ART_MODELING_AUDIO.md` Phase 1 and the relevant custom pipelines only after the graybox gate passes.

Order:

1. Adapt one modular industrial kit.
2. Build project master materials and wetness functions.
3. Model the recorder from its approved UE proxy.
4. Model one generator/hydrophone hero machine.
5. Build the Listener silhouette using a licensed base plus custom mesh/material treatment.
6. Add room-specific decals, signs, cable IDs, and maps.
7. Add Lumen lighting state transitions.
8. Add sparse fog, leaks, dust/condensation feedback, and storm transmission.
9. Build MetaSound ambience/submix routing.
10. Profile the worst view before polish.

Every hero model follows:

- In-engine proxy and dimensions
- Blender blockout
- UE scale test
- High/low or Nanite decision
- UV/bake/texturing
- Collision and sockets
- FBX import
- In-engine material/interaction test
- Review at intended camera distance

Gate:

- Bought assets receive both identity passes: project materials/weathering and Northpoint signage/cabling.
- Critical interaction silhouettes remain readable.
- Storm never masks required threat or puzzle audio.
- The representative slice reaches the target frame budget on the minimum-spec test machine.

## Phase 11 — Greenlight and campaign graybox

If the slice passes, build campaign content one act at a time. Use the locked story in `../01_MASTER_GDD.md`; use `07_LEVEL_CONTENT_DRAFT.md` for the small-unit method and reusable Blueprint checklist.

Reconciled act order:

1. Landfall: cliff approach → boathouse → generator annex → reception/Operations hub → upper monitoring hall
2. Playback: tape archive → mess/staff quarters/dormitory → intercom exchange
3. Pressure Rooms: pump control → flooded lower grid → test chamber → maintenance crawlspaces
4. Choir Below: sonar well → Rusk lab → collapsed dormitory → deep array → Signal Cathedral
5. Dead Air: archive core → array junction → transmitter tower → evacuation route

For each room, create a build unit containing:

- Purpose
- Entry and exit
- Graybox dimensions
- Required reusable actors
- Puzzle state diagram
- Clue and backup clue
- Threat state and counterplay
- Checkpoint/reset behavior
- Shortcut/transformation
- Blind-playtest criteria
- Final art/audio hold gate

Do not art an act until its complete graybox can be played from chapter start to chapter end after a fresh load.

## Phase 12 — Campaign systems and content completion

Execute chapter content in this mechanic order:

1. Teach local recording and one acoustic lock.
2. Add remote puck projection and basic Listener misdirection.
3. Add clip contamination and authenticity tests.
4. Add environmental masking and flooded traversal.
5. Add pipe and station-speaker routing.
6. Add Drowned Technician zone encounters.
7. Add Antenna Choir hazard and cancellation.
8. Unlock overdub only for late-game composite puzzles.
9. Assemble final signal and physical ending controls.

Scare budget:

- No more than one direct jump per 20 expected minutes.
- Maximum three pursuits in the game.
- Every pursuit tests a learned mechanic.
- Every loud scare has setup and aftermath.
- Quiet recovery follows every major threat.

Gate after each act:

- Blind critical-path completion
- Reload from every checkpoint
- No missing/reacquisition failure for required sounds
- Shortcut persistence
- Caption/waveform-only solvability pass
- Performance capture

## Phase 13 — Menus, settings, accessibility, and difficulty

Execute `05_SAVE_LEVELS_UI.md`:

- 5.1 main menu
- 5.2 pause/manual save-load
- 5.3 settings subsystem
- 6.1 accessibility model
- 6.2 reduced flicker/shake hooks
- 7.1 subtitle model
- 7.2 waveform compass
- 8.1 difficulty Data Assets
- 8.2 gameplay hookups

Gate:

- Entire game is completable controller-only.
- Every required sound is solvable with captions/waveform support.
- Story/Standard/Survival change values, never puzzle solutions.
- Reduced flicker and shake change real effects, not only menu booleans.
- Settings persist separately from progress saves.

## Phase 14 — Alpha to release

Use `08_QA_RELEASE_POSTLAUNCH.md` in order.

Before Steam:

1. Complete local save/version migration tests.
2. Run map/cook validation.
3. Package Windows Development and Shipping builds.
4. Run a clean-machine smoke pass.
5. Complete all endings from valid saves.
6. Run minimum-spec and worst-case performance captures.

Then execute `05_SAVE_LEVELS_UI.md` 9.1 and 10.1–10.3:

- Steam readiness gate
- Achievements
- Cloud saves after local saves are proven

Final release gate:

- No severity-one defect
- No progression blocker or corrupt save
- Clean install and upgrade install pass
- Rollback build archived
- Licenses/credits audited
- Store screenshots and trailer show real gameplay
- System requirements come from measured builds

## Permanent stop rules

Stop and fix the current phase if:

- A build no longer packages.
- A save cannot migrate or reload.
- A critical sound cannot be reacquired.
- A player cannot explain detection.
- A puzzle requires developer explanation.
- A room is being polished before its graybox passes.
- Performance is being guessed instead of profiled.
- An AI changes unrelated systems in one task.

The next task is always the first unchecked item in the current phase—not the most exciting later item.