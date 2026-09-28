# Phase 6: local saves, checkpoints and streamed progression

Default development map: `/Game/Game/Maps/Test/M_Test_SaveStreaming`.
Press Play to enter Archive 01. Record its service bell, replay it at the archive relay, then visit Radio Room 02 and operate its switch. The objective text updates from gameplay events. Each doorway checkpoint stores a safe respawn position. The connector floor stays in the persistent map so transitions have a stable crossing surface.

Development controls: F5 saves `Manual0`, F9 loads it, F10 opens/closes the save controls. The panel offers save/load, deletion of that manual slot, sector requests and a death test. It appears only in the save development map; it is not the future production menu. Movement/look are paused while the panel or a world transition owns input. Use the normal recorder controls shown in its panel.

Progress files are `Saved/SaveGames/TDF_Progress_<Slot>.sav`. Input bindings remain in the separate input-settings profile. Save writes verify a temporary file before replacing the final file. A failed replacement keeps the recovery temporary file and displays an error. Do not remove that file as routine cleanup.

Save version 2 stores the player, difficulty/health, inventory, recorded signals and acoustic ledger, recoverable deployed puck identities, objectives, checkpoint metadata, streamed sector IDs and stable actor states. Reload returns at least one puck to inventory to preserve the GDD recovery guarantee. Transient one-shot playback does not resume or emit duplicate puzzle events. V1 migrates deterministically; newer versions are rejected with a readable message.

Loading freezes input, opens the saved persistent map if necessary, waits for visible sectors plus actor registration, restores the snapshot, places the player, then fades back in. Missing old actor IDs warn and continue. Room transitions cache outgoing actor state and restore it after re-entry. Rapid requests settle on the final requested set. Timeout recovery retains visible floors, cancels pending streaming work and releases controls/save blocking.

Death tries the active checkpoint, manual save and chapter-start save in order, then restarts the current chapter map if no usable file exists. Health uses the authored respawn profile. Safe objective boundaries refresh the death-recovery checkpoint; checkpoint-required sectors are included even if the player has left them. Saving is blocked during streaming, loading, cutscene/combat locks and death.

Authored data resides in `/Game/Game/Data/Progression`, `/Game/Game/Data/Sectors` and `/Game/Game/Data/Difficulty`. Stable IDs are identifiers, not asset names. The prototype chapter uses `QA_Archive`; it does not invent final story progression. The Standard difficulty asset is the minimum respawn contract, not the later difficulty system. Traditional streamed levels use one manager. The existing persistent map contains the same Intro/RadioRoom references required by foundation task 20.

Development commands: `tdf.SaveSlot Manual0`, `tdf.LoadSlot Manual0`, `tdf.DumpSaveVersion Manual0`, `tdf.ValidateStableIds AllSectors`.

## Reproducing validation

- `run_phase6.ps1 -Step Final -Filter TDF.` builds the editor and runs the editor regression suite.
- `run_phase3_assets.ps1 -Script phase6_asset_audit.py` audits authored GUIDs and sector references without changing assets.
- `build_phase6.ps1 -Package` creates the Windows Development package under `Saved/Builds/Phase6/Windows`.
- `test_phase6_package.ps1` runs the packaged gameplay checks, then writes and reads a save in separate processes. The read process starts in a different map.
- `test_phase6_package.ps1 -Rendered` captures the archive, active captions and radio room, plus the actual Unreal audio mixer output.
- `validate_audio_output.py <wav>` checks mixer duration, format, level and clipping.

Close project instances before build/automation. The runners use one compiler action at a time for this workstation's memory limits. Generated binaries, caches, local saves and test outputs stay out of Git; original audio sources and source-of-truth documents are tracked.

The development rooms use proxy geometry and existing materials, with cold practical lighting, controlled exposure, restrained fog, dark recesses and original station ambience. Final models/materials remain for the requested later Blender work. These tests do not constitute a human controller-feel review or a GPU performance benchmark.
