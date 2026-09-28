# Phase 3 — World interaction and key items

Implemented from `BUILD_MANUAL/00_MASTER_BUILD_ORDER.md`, player/interaction tasks 14–26, and the authoritative stable-GUID correction in save/UI task 0.2. Branch remains `main`. No commit or push was made.

## Play and review

The default editor/game map is now `/Game/Game/Maps/Test/M_Test_PlayerInteraction`. The earlier persistent map and Input Gym remain available, with their lighting reworked. The original pre-Phase-2 scene backup was preserved.

The new route includes a locked B-04 bulkhead, a permanent service key reached through a 125 cm crouch opening, an inspectable relay, a blind corner, and a switch-operated isolation door. There is no Level Blueprint gameplay truth or physics pickup system.

| Action | Keyboard/mouse | Controller |
| --- | --- | --- |
| Interact / take / open | Left mouse | X / left face button |
| Inspect focused item / close inspection | I | Y / top face button |
| Return from inspection | Tab; Escape also works | View; Menu also works |
| Rotate preview | Mouse movement | Right stick |
| Crouch | Hold Ctrl | Hold B / right face button |
| Lean | Q / E | Left / right shoulder |
| Sprint | Shift | Left stick press |
| Hold breath | X or right mouse | Left trigger |

See `PHASE3_QA.md` for the repeatable route and human review checklist.

## Implementation

- Door truth is native C++; `BP_Door_Base` animates the hinge and releases its animation lock. A linked switch cannot bypass a keyed lock. Matching keys unlock/open doors without consuming permanent keys.
- `BP_Switch_Base` presents native toggle/one-shot state. Receivers are notified through an interface, with null links safely ignored.
- Item Data Assets own stable IDs, names, descriptions, pickup/inspection flags and preview references. Invalid or duplicate inventory additions are rejected. Picked world actors retain state while becoming hidden and non-colliding.
- Inspection is controller-owned, balances its input locks, blocks normal world interaction, and restores control on exit/unpossession/end play. Mouse/stick input rotates a separate transient preview. The placed object remains untouched.
- Inspection presentation uses `WBP_Inspection` and a 512-pixel capture studio. World fog is excluded. Direct studio lighting and a persistent capture remove the initial fog/noise problem. Final models can replace `InspectionMesh`; clear the temporary `InspectionMaterialOverride` when using authored model materials.
- Doors, switches and items inherit a stable identity component. New editor placements receive GUIDs; duplication assigns new editor identities; PIE retains them. Runtime load never manufactures missing IDs. A GameInstance registry reports duplicates. State records reject foreign IDs, and switch restore does not replay commands over door records. Full disk game saves belong to the later save phase.
- Digital rebinding supports both devices, rejects conflicts and wrong-device keys, uses transient copies of mapping contexts, and saves a separate input profile. Prompts resolve active mappings. Axis-direction rows and the full options screen remain for the later settings/UI phase. Tests do not overwrite saved user bindings.
- Controller yaw and pitch rates are independently tunable. Analog movement, forgiving sphere targeting, and the Phase 2 movement/camera rules remain in place.

## Atmosphere

The new blockout uses tighter sightlines, service bays, grilles, overhead ducts, desaturated practical pools, restrained emergency red beyond the threshold, volumetric haze and deep unlit recesses. A rendered review caught excessive darkness; low-level spill now reveals nearby geometry without flattening the hidden areas. Fixed exposure, restrained bloom, no motion blur, Lumen, Virtual Shadow Maps and the existing DX12 quality path are retained.

Geometry and surfaces are temporary blockouts. No final Blender models, texture production or Phase 4 acoustic gameplay was added. Relay text is test-fixture copy, not a change to canonical story documents.

## Validation

- Door and inventory tests began with failing compile/behavior checks, then passed after implementation. Logs retain those red runs.
- Incremental Development Editor builds and component tests passed across tasks 14–24.
- Both complete authored routes were traversed by simulated mapped keyboard and controller input, including actual crouch traversal, locked-door refusal, pickup, inspection rotation/back, door animation and the linked exit.
- Final editor suite: 18 passed, 0 failed (`Saved/Tests/Phase3_26/index.json`). Two tests carried external EOS network-timeout warnings; their assertions passed.
- The test map's GUIDs were compared before and after save/reload. Asset scripts compile their Blueprints and preserve route actors/IDs on rerun.
- Rendered review corrected an empty Blueprint event graph, excessive darkness, fog obscuring the inspection capture, and its default checker surface.
- Windows Development package completed successfully (`Saved/Builds/Phase3/Windows`).
- Packaged gameplay checks: 3 passed, 0 failed (`Saved/Tests/Phase3Standalone/index.json`), covering remapping and both complete input-device routes. These ran before the final presentation-only inspection correction.
- Final packaged DX12 render check: 1 passed, 0 failed (`Saved/Tests/Phase3Rendered/index.json`). Four captured views are in `Saved/Phase3Preview`; the final inspection image was visually checked for a clean preview and opaque panel.

Build/test scripts use one project process at a time. Shader compilation remains capped at two workers. No unrelated apps were closed and no Windows memory settings changed.

Human signoff remains: physical-controller feel, the user's visual preference, and sustained performance on the agreed minimum-spec machine. No measured 60 FPS claim is made. The proxy door swings without a physical pushing-door simulation.
