# Player foundation — working record

Source of truth: `Design Documents (Source of all truths)/BUILD_MANUAL/00_MASTER_BUILD_ORDER.md`, Phase 2. Implementation follows player manual tasks 1–13, then foundation tasks 18–19. Unreal is locked to the installed 5.8.3 build; the existing project descriptor stays `Horror.uproject`, with the specified `DrownedFrequency` module and `TDF` class prefix.

## Scope and presentation

C++ owns movement, camera constraints, input/device routing, focus and interaction eligibility. Blueprint classes and the prompt Widget Blueprint own presentation. `DA_InputConfig` references all eleven actions and both Enhanced Input layouts. The prompt resolves the effective binding instead of embedding a key in an interactable.

The persistent map is a playable foundation room, not a completed campaign sector. The intro and radio-room maps are saved empty sector shells; streaming comes later. The original editor scene is preserved at `/Game/Game/Maps/Backup/L_PrePhase2`.

Lighting direction follows the coastal-station design: cold overhead lighting, amber utility areas, strong architectural silhouettes and restrained volumetric haze. DX12/SM6, Lumen, hardware ray tracing, Nanite support and Virtual Shadow Maps remain enabled. TSR now replaces disabled antialiasing. Motion blur and chromatic aberration are off. Geometry uses engine primitives and three plain color instances of the engine's stock material; final models, materials, textures and audio remain deferred. Do not treat these placeholders as final art. Rebuilding the room uses `phase2_foundation_maps.py`, followed by `phase2_visual_tune.py` for its saved contrast pass.

## Step-by-step implementation

| Manual task | Implemented result |
|---|---|
| Player 1 | Runtime module, responsibility folders, core Blueprint shells and GameMode wiring; later-phase classes are stubs. |
| Player 2 | Eleven actions, keyboard/mouse and gamepad contexts, axis modifiers, stick dead zones and validated input Data Asset. |
| Player 3 | Local-player context initialization, idempotent setup, explicit missing-asset error and owned-context cleanup. |
| Player 4 | Controller-relative movement/look, held crouch, optional jump disabled by default; 165 cm standing and 110 cm crouched eye heights. |
| Player 5 | One movement-state struct and change delegate, including grounded/speed/exclusion rules. |
| Player 6 | Held sprint; walk 250, sprint 460, crouch 145 cm/s. Crouch and breath cancel sprint. |
| Player 7 | Camera-only lean, 12 cm lateral travel and 7 degrees roll, smoothed and swept against camera-blocking geometry. |
| Player 8 | Held breath state, canceled sprint, reduced sway and look sensitivity, safe release/focus-loss reset. |
| Player 9 | 85-degree camera, smoothed crouch height, subtle movement/breathing sway, Blueprint-exposed reduced-motion option. |
| Player 10 | Blueprint-overrideable eligibility, prompt, activation, visibility and identity interaction contract. |
| Player 11 | 20 Hz focus trace, 220 cm range, 8 cm sphere, range/occlusion/eligibility rechecked on activation. |
| Player 12 | Controller-owned prompt widget, focus delegate binding/cleanup, lower-center Blueprint layout and interactable-owned text. |
| Player 13 | Live device-change delegate, neutral-axis filtering, effective Enhanced Input binding labels and unbound fallback. |
| Foundation 18 | Persistent room, input gym and two empty sector shells; all maps saved and reopened. |
| Foundation 19 | Persistent editor/game startup map, project GameMode and GameInstance, explicit packaging map list. |

## Controls

| Action | Keyboard / mouse | Controller |
|---|---|---|
| Move / look | WASD / mouse | Left / right stick |
| Crouch (hold) | Ctrl | B / right face button |
| Sprint (hold) | Shift | Left stick press |
| Lean | Q / E | Left / right shoulder |
| Hold breath | X or temporary RMB | Left trigger |
| Interact | Left mouse | X / left face button |
| Jump contract, disabled by default | Space | A / bottom face button |
| Inspect contract | I | Y / top face button |
| Back / pause contracts | Tab / Escape | View / Menu |

Left mouse avoids colliding with E lean and the future F recorder binding. RMB is the player manual's temporary breath binding; revisit it when listen-focus is implemented. Inspect, Back, Pause and contextual traversal behaviors belong to later phases. Sprint has no stamina system; crouch and held breath cancel sprint. Reduced camera motion is exposed on the camera component.

## Repeating validation

Save and close Unreal before builds on this workstation. `test_phase2.ps1 -Task 19` runs editor automation. `build_phase2.ps1` builds the standalone Development target; add `-Package` to cook and stage the four foundation maps under `Saved/Builds/Phase2`.

Editor checks exercise real mapped keyboard/controller events, possession, movement exclusions, camera lean and collision, crouch eye height, reduced motion, focus range/occlusion/destruction, actual prompt text, input-device switching, effective binding labels and missing-input-asset handling. Standalone test `TDF.Player.Standalone.InputFlow` repeats the gameplay path in the packaged executable. `TDF.Visual.FoundationCaptures` captures the actual rendered room and interaction prompt for visual review.

Hardware-controller feel and minimum-spec 60 FPS require hands-on hardware/performance signoff; simulated controller input is not physical-controller validation. The current workstation has hit Windows commit-memory exhaustion while other large apps are running. Keep operations sequential; do not close unrelated apps or alter Windows memory settings without authorization.

## Validation status

- Tasks 1–13: incremental Development Editor builds and cumulative tests passed.
- Foundation maps: all four saved and reopened successfully.
- Final editor automation after map/default changes: 4 passed, 0 failed, including the missing-config error path.
- Standalone Development executable and Windows package: built successfully; final package log ends `BUILD SUCCESSFUL`, exit code 0.
- Final packaged gameplay test: 1 passed, 0 failed (`TDF.Player.Standalone.InputFlow`).
- Final packaged rendered capture test: 1 passed, 0 failed (`TDF.Visual.FoundationCaptures`); both fresh images inspected. Contrast, sign orientation, plain surfaces and prompt readability reviewed.
- Editor tooling is restricted to editor targets. Game Features is explicitly enabled at runtime to satisfy the project's existing asset-manager scan; this resolved a packaged-startup ensure that PIE did not reveal.
- Human signoff still pending: physical-controller feel, sustained performance on the agreed minimum-spec machine, and approval of the temporary lighting/art direction. No measured 60 FPS claim is made.

## Review outputs

- Launch the built game: `Saved/Builds/Phase2/Windows/DrownedFrequency.exe`.
- Room and prompt captures: `Saved/Phase2Preview/Phase2Lighting.png` and `Phase2Prompt.png`.
- Final editor report: `Saved/Tests/Phase2Task19/index.json`.
- Final packaged reports: `Saved/Tests/Phase2Standalone/index.json` and `Saved/Tests/Phase2Rendered/index.json`.
- Packaging log: `Saved/Logs/Phase2Package.log`.
- Unreal opens on `L_TDF_Persistent`; press Play to test. `open_phase2_preview.py` restores the overview camera and hides editor-only icons.

Shader preparation is limited to two workers (including the separate cooker setting) and two pipeline-precache threads on this workstation. This bounds startup/build memory; it does not lower the project's lighting or image-quality settings.

No commit or push has been made. Phase 3 door, switch, inventory, inspection and persistence behavior is not implemented; those classes remain skeletons.
