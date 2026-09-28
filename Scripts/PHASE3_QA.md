# Phase 3 review checklist

Map: `/Game/Game/Maps/Test/M_Test_PlayerInteraction`.

## Repeatable route

1. Follow the lit approach around the blind corner. Try the B-04 bulkhead before finding the key: it stays locked.
2. Follow SERVICE ACCESS to the low opening. Crouch through, collect the key from the bench, and crouch back out.
3. Approach the disconnected relay on the opposite bench. Interact to inspect. Read its title and record, rotate it with mouse/right stick, and return with Tab/View (Escape/Menu also returns). I/Y toggles inspection on a focused inspectable.
4. During inspection, movement, leaning, sprinting, and world interaction must remain blocked. After closing, those controls must recover.
5. Return to B-04. The prompt changes to Unlock. Interact: the bulkhead unlocks and opens. The key remains owned.
6. Cross the threshold, operate the isolation switch, and pass through its linked exit. Toggle again to test closing. Check the lever visibly changes position.
7. Repeat with controller only. Watch prompts change when switching devices.

## Checks requiring human judgment

- Physical controller aim, analog speed, and independent yaw/pitch sensitivity feel comfortable.
- Critical objects and crouch access remain identifiable within the dark lighting.
- Blind corners and isolated practical lights create uncertainty without making traversal frustrating.
- Text is readable at the intended display distance. Inspection input never traps the player.
- Review door collision when standing in its swing path; the current proxy animation does not implement a physical pushing door.
- Review sustained frame time on the agreed minimum-spec machine. Screenshots and automated traversal are not a performance signoff.

## Automated coverage

`run_phase3.ps1 -Step 26 -Filter TDF.` compiles the Development Editor target and runs the applicable editor suite. It includes inventory/door red-green tests, one-shot switches, GUID/restore contracts, settings serialization, mapped rebinding, player regressions, and actual traversal of the authored route with both simulated input schemes.

`build_phase3.ps1 -Package` builds/cooks the Windows Development package. `test_phase3_package.ps1` runs the standalone remapping and both route tests. Add `-Rendered` to capture entrance, blind corner, crouch opening, and the inspection panel with its 3D preview.

Input override methods reject conflicting or wrong-device keys. Digital actions are remappable; directional axis rows and the full options menu remain work for the later settings/UI phase. Tests use in-memory overrides and do not overwrite the player's saved bindings.

Actor state records use authored GUIDs. Full disk game saves, checkpoints, acoustic gameplay, and threat AI belong to later phases. This phase adds the save-ready contracts and duplicate registry only.
