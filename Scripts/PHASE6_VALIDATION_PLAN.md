# Phase 6 validation plan

Source: BUILD_MANUAL/00_MASTER_BUILD_ORDER.md Phase 6; 05_SAVE_LEVELS_UI.md 0.2, 1.1–1.4, 2.1–2.2, 3.1–3.2, 4.1–4.3; foundation task 20 cross-check. Existing stable-ID registry is extended, not replaced. Traditional streamed sublevels remain the selected policy. Menus/settings/Steam and threat AI remain later phases.

Required evidence:
- Stable actor IDs survive reload, duplicate IDs are rejected with level context, transient actors opt out.
- Disk schema stores header, player, inventory, acoustic contract, actor records, objectives and sector IDs; progress loads do not overwrite input settings.
- Temp write completes and validates before replacing a final save; errors reach delegates.
- Loading freezes gameplay, waits for the correct map, visible sectors and actor registration, then restores state and control.
- Old fixtures migrate deterministically, newer/corrupt saves fail cleanly, missing old actor IDs warn and continue.
- Checkpoint overlaps suppress duplicate writes; death restores checkpoint/manual/chapter fallback, including puzzle state and health.
- Objective graph validates IDs/references/cycles; events advance objectives once and persist at safe boundaries.
- Sector changes load incoming geometry before unloading outgoing geometry; save attempts are blocked, rapid requests coalesce, failures time out safely.
- Unloaded sector actor state is cached and reapplied after registration.
- Separate packaged processes write then load the same save and verify player, signals, key items, objectives, puzzle state and sectors.
- Rendered map/audio capture checks readable horror lighting after integration.

Runtime presentation such as one-shot sound playback is transient. Persistent puck inventory and recoverable placed pucks must remain coherent across load; ongoing audio must not replay gameplay events while restoring.
