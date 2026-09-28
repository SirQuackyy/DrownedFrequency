# Room-by-Room Content Construction Draft

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

IMPORTANT: This file is the granular content-unit draft. The locked campaign order in `../01_MASTER_GDD.md` remains authoritative. `00_MASTER_BUILD_ORDER.md` gives the reconciled act order. “Masked traversal” means movement concealed by environmental noise or a recorder-mounted phase-inversion filter; it does not add face-worn equipment.

- Title: The Drowned Frequency granular level-content construction sequence
## Scope

- Game: The Drowned Frequency
### Acts
- Landfall
- Playback
- Pressure Rooms
- Choir Below
- Dead Air

### Constraints

- Location count: 1
- Target playtime: 4-6 hours
- Max direct jumpscare frequency: 1 per 20 minutes
- Max pursuits total: 3
#### Core puzzles
- acoustic lock
- routing
- authenticity
- masked traversal
- overdub

- Existing vertical slice: Dormitory flow


## Global build rules

### Build order
- Graybox navigation and sightlines
- Block critical interactables
- Add puzzle state logic as abstract beats
- Add clue placement and readability passes
- Add encounter pacing only after navigation works
- Add checkpoints and fail recovery
- Add shortcuts after first full-route validation
- Add transformations after the player understands the original space
- Add final art/audio only after the unit meets done criteria in graybox

### Final art audio policy

#### Do not add before
- Critical path can be completed without dev assistance
- Puzzle has at least one observed blind playtest solve
- Encounter has readable telegraph, escape, and recovery
- Checkpoint cannot trap the player in failure
- Performance budget is checked in representative graybox

#### Add temp audio when
- A puzzle depends on sound identification
- An enemy depends on sound lure or proximity
- A clue is audio-first
- A transformation must be understood through changed ambience

#### Add final audio when
- Timing windows are locked
- Room scale and materials are no longer changing
- Subtitle and accessibility text are approved
- No puzzle solution depends on placeholder naming

#### Add final art when
- Collision and traversal are locked
- Lighting composition has survived a readability pass
- Quest-critical silhouettes are stable
- Streaming and performance budgets are verified


### Reusable blueprint pieces

#### BP_Interactable_Generic

- Name: BP_Interactable_Generic
- Purpose: Shared examine/use behavior for props, doors, radios, notes, switches, valves, and cassette machines.

#### BP_AudioEmitter_Clue

- Name: BP_AudioEmitter_Clue
- Purpose: Spatial clue source with subtitle hook, distance falloff, occlusion setting, and inspection state.

#### BP_FrequencyTuner

- Name: BP_FrequencyTuner
- Purpose: Diegetic tuning interface for signal search, lock-on, interference, and clue reveal.

#### BP_AcousticLock

- Name: BP_AcousticLock
- Purpose: Door or device that opens only when required tone, rhythm, phrase, or source arrangement is satisfied.

#### BP_RoutingNode

- Name: BP_RoutingNode
- Purpose: Patch bay, breaker, valve, antenna switch, or cable junction with visible route state.

#### BP_AuthenticityComparator

- Name: BP_AuthenticityComparator
- Purpose: System that compares original and false recordings through metadata, artifacts, location cues, or contradictions.

#### BP_NoiseMaskVolume

- Name: BP_NoiseMaskVolume
- Purpose: Traversal gate that changes allowed paths, enemy perception, or interactable visibility while environmental masking or the phase-inversion filter is active.

#### BP_OverdubRecorder

- Name: BP_OverdubRecorder
- Purpose: Captures player-selected fragments and plays them back into the world as puzzle input.

#### BP_PursuerDirector

- Name: BP_PursuerDirector
- Purpose: Controls pursuit start, pressure, escape validation, cooldown, and global pursuit count.

#### BP_ScareDirector

- Name: BP_ScareDirector
- Purpose: Tracks scare budget and schedules non-direct scares, direct jumps, stingers, and cooldowns.

#### BP_CheckpointVolume

- Name: BP_CheckpointVolume
- Purpose: Safe checkpoint with inventory, puzzle state, enemy reset, and softlock prevention checks.

#### BP_ShortcutDoor

- Name: BP_ShortcutDoor
- Purpose: One-way or later-unlocked route that compresses backtracking after major solve.

#### BP_WorldTransformVolume

- Name: BP_WorldTransformVolume
- Purpose: Changes room layout, props, lighting state, wetness, decals, audio bed, or traversal affordances.

#### BP_ReadabilityBeacon

- Name: BP_ReadabilityBeacon
- Purpose: Debug-only marker for checking clue sightline, interaction radius, subtitle trigger, and player guidance.


### Global playtest gates
- A first-time player can identify the next objective within 90 seconds after each checkpoint.
- No critical clue is missable without an alternate recovery clue.
- Every puzzle has at least three escalating hints: environmental, inspectable, and explicit fallback.
- Every pursuit has a readable start, a committed chase space, an escape action, and a cooldown.
- Direct jumpscare count never exceeds one per 20 minutes in expected route timing.
- All shortcuts reduce repeated traversal by at least 30 seconds without skipping required knowledge.
- Transformations are first shown in a previously understood layout before being used as puzzle logic.


## Construction sequence

### A1-U01 — Storm Approach and Exterior Threshold

- Id: A1-U01
- Act: Landfall
- Type: sector
- Name: Storm Approach and Exterior Threshold
- Purpose: Establish the single-location premise, storm isolation, radio motif, and first safe objective: get inside.
#### Build order
- Block beach path, collapsed access road, locked station entry, and visible tower silhouette.
- Place one critical route from shoreline to service entrance.
- Add non-critical side pockets for storm debris and first lore fragments.
- Add entry checkpoint after the player reaches shelter.
- Add final weather art and exterior audio only after path timing is locked.

#### Graybox steps
- Create narrow linear approach with two bends to hide/reveal the building.
- Use scale markers for waves, fence, service stairs, and entry door.
- Add one blocked main entrance and one accessible maintenance hatch.
- Place temporary beacon lights for safe route readability.
- Add collision blockers to prevent early water exploration.

#### Reusable pieces
- BP_Interactable_Generic
- BP_AudioEmitter_Clue
- BP_CheckpointVolume
- BP_ReadabilityBeacon

#### Scripting steps
- Trigger storm radio burst when the tower first becomes visible.
- Enable entry objective after player inspects locked main door.
- Reveal maintenance hatch through repeated sound cue or flickering light.
- Save checkpoint after crossing into interior.

#### Playtest
- Player finds the maintenance entrance without a waypoint.
- Player understands the main door is not the immediate solution.
- Player reaches first checkpoint in 3-6 minutes.
- No player walks into ocean collision expecting a path.

#### Done criteria
- Exterior route is readable in grayscale lighting.
- Critical entry path is found by at least 4 of 5 blind testers.
- First radio burst is noticed but not interpreted as a puzzle yet.
- Entry checkpoint restores player inside, not outside.

- Final art audio: Add final storm, shoreline, exterior facade, wet materials, distant horn, and first music sting after route timing and entry framing are locked.
- Ai prompt: Build A1-U01 Storm Approach as a graybox level unit: define shoreline-to-service-entry layout, first radio clue, entry checkpoint, noncritical debris lore, playtest checks, and done criteria. No implementation.

### A1-U02 — Receiving Hall

- Id: A1-U02
- Act: Landfall
- Type: room
- Name: Receiving Hall
- Purpose: Introduce the building interior, safe hub language, locked wings, and first navigational loop.
#### Build order
- Block central receiving hall with three visible exits.
- Mark dormitory as available, transmitter stairs as locked, basement as flooded.
- Place readable station map with damaged labels.
- Add first return shortcut door placeholder.
- Add checkpoint once the hall objective is understood.

#### Graybox steps
- Create central rectangular hall with reception desk, bulletin wall, stair silhouette, and side corridor.
- Use blocked doors with distinct silhouettes for future acts.
- Place temporary map board facing entry route.
- Add safe alcove for inventory/tutorial prompts.
- Measure loop from entrance to dormitory corridor under 25 seconds.

#### Reusable pieces
- BP_Interactable_Generic
- BP_CheckpointVolume
- BP_ShortcutDoor
- BP_ReadabilityBeacon

#### Scripting steps
- Set initial objective to find power or radio access.
- Gate transmitter stairs with a silent electronic lock.
- Gate basement by flooding and pressure warning.
- Flag receiving hall as temporary safe zone.

#### Playtest
- Player can name at least two unavailable destinations after one minute.
- Player chooses dormitory corridor as next route without confusion.
- Player recognizes the hall as a place they will return to.

#### Done criteria
- All future-route blockers are visually distinct.
- Station map communicates one-location structure.
- No tester expects to solve basement or transmitter gate immediately.
- Checkpoint cannot reload before objective state is initialized.

- Final art audio: Add final interior storm muffling, dripping, signage, reception dressing, and map art after all act routes using the hall are locked.
- Ai prompt: Build A1-U02 Receiving Hall as the first interior hub: exits, blockers, station map, dormitory route, checkpoint, future shortcut placeholders, playtest checks, and done criteria. No implementation.

### A1-U03 — Dormitory Corridor Vertical Slice

- Id: A1-U03
- Act: Landfall
- Type: sector
- Name: Dormitory Corridor Vertical Slice
- Purpose: Use the existing vertical slice flow as the first complete loop: explore, hear, tune, unlock, return.
#### Build order
- Preserve the existing dormitory critical path as baseline.
- Audit every room for readable entry, clue, interaction, and exit.
- Standardize interactables to reusable pieces.
- Add checkpoint before first acoustic lock attempt.
- Add final art/audio last, after revalidating the old slice against full-game rules.

#### Graybox steps
- Lay out corridor with 5-7 rooms: bunk room, washroom, matron office, linen storage, locked broadcast nook, return door.
- Mark old slice geometry that must remain stable.
- Remove any one-off prototype interactions and replace with shared content categories.
- Add sightline from corridor to final locked nook.
- Add loopback route to Receiving Hall.

#### Reusable pieces
- BP_Interactable_Generic
- BP_AudioEmitter_Clue
- BP_FrequencyTuner
- BP_AcousticLock
- BP_CheckpointVolume
- BP_ShortcutDoor

#### Scripting steps
- Initialize first tuner tutorial when player hears hidden dormitory signal.
- Track collection of three acoustic clue fragments.
- Allow acoustic lock attempt only after tuner is introduced.
- Open return shortcut after broadcast nook is solved.

#### Playtest
- New players learn inspect, listen, and tune in this sector without external explanation.
- Returning testers from the vertical slice still understand the revised route.
- Average completion time is 25-40 minutes.
- No direct jumpscare fires in this tutorial loop.

#### Done criteria
- Every required clue has a backup cue.
- First acoustic lock is solved by at least 3 of 5 blind testers without brute force.
- Shortcut back to Receiving Hall is discovered naturally.
- The sector can be completed after reloading from any internal checkpoint.

- Final art audio: Refresh final dormitory art/audio only after the integrated slice passes full-game checkpoint, hint, and reusable-piece audits.
- Ai prompt: Convert the existing dormitory vertical slice into A1-U03: preserve its flow, define 5-7 rooms, acoustic tutorial, clue chain, checkpoint, shortcut, playtest checks, and done criteria. No implementation.

### A1-U04 — First Acoustic Lock

- Id: A1-U04
- Act: Landfall
- Type: puzzle
- Name: First Acoustic Lock
- Purpose: Teach that sound, rhythm, and spatial listening open the station.
#### Build order
- Define required audio pattern before dressing the room.
- Place three clue sources in nearby rooms.
- Block lock interface in a quiet alcove.
- Add wrong-answer feedback and reset behavior.
- Add final lock audio only after solve timing is stable.

#### Graybox steps
- Place locked broadcast nook at corridor end.
- Place clue source one near bunks, source two in washroom pipe, source three in matron office recorder.
- Create a quiet interaction zone free from overlapping ambience.
- Use simple placeholder tones labeled only in editor notes, not in player-facing text.

#### Reusable pieces
- BP_AcousticLock
- BP_AudioEmitter_Clue
- BP_FrequencyTuner
- BP_Interactable_Generic

#### Scripting steps
- Track whether each clue has been heard long enough to count as learned.
- Allow lock input using rhythm, ordered tones, or tuned frequency depending on final design.
- Give low-stakes failure feedback without damage or chase.
- Unlock broadcast nook and set objective to recover the first tape.

#### Playtest
- Tester can explain why the answer works.
- Tester does not solve only by cycling possibilities.
- Wrong answers teach direction, not just rejection.
- Subtitles or accessibility layer preserve solution fairness.

#### Done criteria
- Puzzle has one intended solution and one accessibility-supported route.
- No clue is hidden behind the lock it solves.
- Average blind solve time is 5-12 minutes.
- Failure state cannot permanently mute or desync clue audio.

- Final art audio: Add final tones, lock mechanism, reactive room tone, captions, and subtle success motif after blind solve rate is acceptable.
- Ai prompt: Design A1-U04 First Acoustic Lock: three clue sources, lock interaction, fair wrong-answer feedback, accessibility support, playtest checks, and done criteria. No implementation.

### A1-U05 — Dormitory Tape: Landfall Log

- Id: A1-U05
- Act: Landfall
- Type: clue
- Name: Dormitory Tape: Landfall Log
- Purpose: Deliver first narrative proof that the station recorded more than broadcasts.
#### Build order
- Place tape inside broadcast nook after acoustic lock.
- Make it impossible to miss during critical path.
- Attach it to playback tutorial in Receiving Hall.
- Add duplicate transcript access in journal.
- Finalize voice only after script is locked.

#### Graybox steps
- Use large placeholder cassette prop on central table.
- Frame tape with light shaft or contrast blockout.
- Ensure pickup path faces return shortcut.
- Reserve space for transcript UI or physical log nearby.

#### Reusable pieces
- BP_Interactable_Generic
- BP_AudioEmitter_Clue

#### Scripting steps
- On pickup, mark Act 1 core clue complete.
- Unlock playback station objective.
- Add transcript to player records.
- Trigger distant building response after pickup.

#### Playtest
- Player always notices the tape after opening the nook.
- Player understands it should be played elsewhere.
- No one mistakes it for optional collectible-only content.

#### Done criteria
- Tape pickup is mandatory but not intrusive.
- Objective updates within two seconds of acquisition.
- Transcript is accessible after leaving the room.
- Narrative clue points toward Playback act.

- Final art audio: Record final voice, tape handling, and first intelligible station motif after clue text and act transition are locked.
- Ai prompt: Build A1-U05 Dormitory Tape clue: placement, pickup readability, playback objective, transcript fallback, playtest checks, and done criteria. No implementation.

### A1-U06 — First Non-Direct Scare: Occupied Bunk

- Id: A1-U06
- Act: Landfall
- Type: scare
- Name: First Non-Direct Scare: Occupied Bunk
- Purpose: Establish haunting rules without spending a direct jumpscare.
#### Build order
- Place scare on return route after tape pickup.
- Use peripheral movement, sound, or changed prop state instead of camera attack.
- Ensure it cannot interrupt puzzle completion.
- Add scare budget tracking.
- Finalize only after route timing confirms no direct scare conflict.

#### Graybox steps
- Choose one bunk visible at corridor angle.
- Set before/after prop state: empty blanket becomes body-shaped or radio-lit.
- Leave player movement unrestricted.
- Keep alternate path clear if player backs away.

#### Reusable pieces
- BP_ScareDirector
- BP_WorldTransformVolume
- BP_AudioEmitter_Clue

#### Scripting steps
- Arm scare after tape pickup and before return shortcut.
- Trigger only if player looks near bunk or crosses midpoint.
- Log as non-direct scare.
- Do not repeat after checkpoint reload unless explicitly reset for testing.

#### Playtest
- Player notices unease without feeling attacked.
- Player continues toward the objective within 60 seconds.
- No tester thinks they failed or triggered combat.

#### Done criteria
- Scare is optional in perception but reliable in setup.
- No direct camera lunge, scream cut, or forced rotation occurs.
- Scare budget records it as non-direct.
- Reloading cannot stack or spam the event.

- Final art audio: Add final blanket shape, creak, low reversed whisper, and subtle light change after scare timing is locked.
- Ai prompt: Design A1-U06 non-direct bunk scare: trigger, before/after state, no attack, scare-budget handling, playtest checks, and done criteria. No implementation.

### A1-U07 — Receiving Hall Return Checkpoint

- Id: A1-U07
- Act: Landfall
- Type: checkpoint
- Name: Receiving Hall Return Checkpoint
- Purpose: Secure the end of Landfall and prepare transition into Playback.
#### Build order
- Place checkpoint after dormitory shortcut return.
- Save tape possession, acoustic lock state, and shortcut state.
- Verify player cannot return to unsolved state.
- Set next objective to use playback equipment.
- Add final save-room dressing after hub flow is stable.

#### Graybox steps
- Mark safe zone near reception desk or playback table.
- Ensure all enemies and scares are inactive here.
- Place obvious playback station silhouette nearby.
- Keep routes to locked wings visible.

#### Reusable pieces
- BP_CheckpointVolume
- BP_Interactable_Generic
- BP_ShortcutDoor

#### Scripting steps
- Persist dormitory solved state.
- Persist tape inventory and transcript.
- Open or mark shortcut as usable.
- Queue Playback act objective.

#### Playtest
- Reload after checkpoint and continue without losing tape.
- Reload after death or quit and no puzzle needs re-solving unless intended.
- Player identifies playback station as next action.

#### Done criteria
- Checkpoint has no active threat.
- Save state includes all Landfall critical flags.
- Objective text and world framing agree.
- No duplicate tape or duplicate scare appears after reload.

- Final art audio: Add safe hub ambience, reception lamp state, and subtle act-transition music after save-state verification passes.
- Ai prompt: Build A1-U07 Receiving Hall checkpoint: save flags, safe zone, playback objective, reload tests, and done criteria. No implementation.

### A2-U01 — Playback Booth

- Id: A2-U01
- Act: Playback
- Type: room
- Name: Playback Booth
- Purpose: Introduce replaying tapes as an investigative mechanic and open the archive wing.
#### Build order
- Block booth adjacent to Receiving Hall.
- Place playback machine, speaker, transcript surface, and locked archive control.
- Connect Landfall tape to first route unlock.
- Add checkpoint after successful playback.
- Finalize equipment art/audio after interaction timing is locked.

#### Graybox steps
- Create small acoustically isolated booth with one entrance and one observation window.
- Put machine directly on critical sightline.
- Place archive door visible but locked from booth.
- Add space for tape storage UI or physical rack.

#### Reusable pieces
- BP_Interactable_Generic
- BP_AudioEmitter_Clue
- BP_RoutingNode
- BP_CheckpointVolume

#### Scripting steps
- Allow Landfall tape playback.
- During playback, reveal hidden frequency or spoken instruction.
- Unlock archive wing access after playback completes or key phrase is logged.
- Add transcript and mark Act 2 started.

#### Playtest
- Player understands they are extracting actionable information, not just listening to lore.
- Player can find the newly unlocked archive route.
- Playback can be paused/replayed without breaking objective state.

#### Done criteria
- Tape playback produces a clear next objective.
- Archive door changes state visibly and audibly.
- Checkpoint after playback preserves unlocked route.
- Accessibility transcript includes puzzle-relevant information.

- Final art audio: Add final tape deck, booth glass, reel sounds, voice treatment, and room tone after all playback interaction rules are final.
- Ai prompt: Build A2-U01 Playback Booth: tape playback interaction, hidden instruction reveal, archive unlock, checkpoint, playtest checks, and done criteria. No implementation.

### A2-U02 — Archive Wing

- Id: A2-U02
- Act: Playback
- Type: sector
- Name: Archive Wing
- Purpose: Expand the station through records, introduce conflicting evidence, and prepare authenticity puzzle language.
#### Build order
- Graybox archive stacks, listening desk, file cage, and maintenance overlook.
- Place three record rooms with different clue types.
- Add one loop back to Playback Booth.
- Add first low-risk stalker sighting without pursuit.
- Finalize archive dressing only after search paths are validated.

#### Graybox steps
- Create readable stack rows with numbered bays.
- Avoid maze density; use landmarks at each branch.
- Place locked file cage visible from entry.
- Add maintenance overlook as future shortcut.
- Use temp signage for Records A, B, C.

#### Reusable pieces
- BP_Interactable_Generic
- BP_AudioEmitter_Clue
- BP_AuthenticityComparator
- BP_ScareDirector
- BP_ShortcutDoor

#### Scripting steps
- Track three evidence categories: date, voice, room tone.
- Trigger distant silhouette crossing after first evidence pickup.
- Unlock file cage after basic evidence loop is complete.
- Enable return shortcut from maintenance overlook.

#### Playtest
- Player can navigate archive without map after 5 minutes.
- Player understands evidence categories before formal puzzle.
- Stalker sighting creates tension but not chase behavior.

#### Done criteria
- No stack row looks identical at decision points.
- Three clue categories are discovered in any order.
- Shortcut reduces return route to Playback Booth by at least 30 seconds.
- Sighting does not count as a pursuit.

- Final art audio: Add final paper clutter, labels, shelf silhouettes, archival hums, and distant movement after route and evidence placement are locked.
- Ai prompt: Build A2-U02 Archive Wing: stack layout, evidence rooms, file cage, stalker sighting, shortcut, playtest checks, and done criteria. No implementation.

### A2-U03 — Routing Puzzle: Patch the Playback Signal

- Id: A2-U03
- Act: Playback
- Type: puzzle
- Name: Routing Puzzle: Patch the Playback Signal
- Purpose: Teach route-state logic through cables, speakers, and recording devices.
#### Build order
- Define source, route nodes, blocked channel, and destination.
- Place physical routing nodes in archive and booth.
- Add visual feedback for each route state.
- Add wrong-route feedback with harmless distortion.
- Finalize cable art and sound only after state readability passes.

#### Graybox steps
- Create three routing stations: booth output, archive patch bay, listening desk speaker.
- Use colorless shape labels in graybox to avoid relying only on color.
- Place one broken route that must be bypassed through the file cage.
- Keep all nodes within a compact traversal loop.

#### Reusable pieces
- BP_RoutingNode
- BP_Interactable_Generic
- BP_AudioEmitter_Clue
- BP_CheckpointVolume

#### Scripting steps
- Represent route states as visible connected/disconnected steps.
- On correct routing, send hidden tape layer to listening desk.
- On incorrect routing, play distortion that hints at missing node.
- Save route state at midpoint checkpoint.

#### Playtest
- Player can predict what a routing change will affect.
- Player checks destination after changing source.
- Player solves in 8-15 minutes without brute force.

#### Done criteria
- Every node has visible local feedback.
- Correct path can be inferred from environmental labels and sound.
- Puzzle state persists after checkpoint reload.
- No route can softlock the playback machine.

- Final art audio: Add final cable meshes, patch labels, speaker crackle, filtered signal layers, and success sting after route-state readability is proven.
- Ai prompt: Design A2-U03 Routing Puzzle: source, route nodes, destination, feedback, persistence, playtest checks, and done criteria. No implementation.

### A2-U04 — Contradictory Broadcast Fragments

- Id: A2-U04
- Act: Playback
- Type: clue
- Name: Contradictory Broadcast Fragments
- Purpose: Seed the authenticity puzzle by making two recordings disagree.
#### Build order
- Write three contradictions: date, speaker identity, impossible background sound.
- Place fragments across archive records.
- Tie each contradiction to an inspectable physical clue.
- Add journal comparison entries.
- Record final voice only after contradiction logic is locked.

#### Graybox steps
- Place one fragment in file cage, one at listening desk, one in mislabeled tape box.
- Use temporary text slugs for contradictions.
- Put physical corroboration within 10-20 seconds of each fragment.
- Avoid hiding all contradictions in audio-only form.

#### Reusable pieces
- BP_AudioEmitter_Clue
- BP_Interactable_Generic
- BP_AuthenticityComparator

#### Scripting steps
- Log each contradiction when player hears or inspects it.
- Unlock authenticity comparator only after at least two contradictions are logged.
- Mark optional third contradiction as hint-strengthening evidence.
- Update objective to determine which broadcast is real.

#### Playtest
- Player notices at least two contradictions without being told.
- Player can explain what conflicts.
- Player does not need perfect hearing to solve.

#### Done criteria
- Contradictions are fair in transcript form.
- Each audio clue has physical or textual support.
- Optional evidence helps but is not required.
- The next authenticity puzzle has enough setup.

- Final art audio: Add final voice variants, tape wear artifacts, archival notes, and contradiction subtitles after puzzle dependencies are fixed.
- Ai prompt: Build A2-U04 Contradictory Broadcast clue chain: three contradictions, physical corroboration, journal logging, playtest checks, and done criteria. No implementation.

### A2-U05 — First Pursuit: Archive Stack Chase

- Id: A2-U05
- Act: Playback
- Type: encounter
- Name: First Pursuit: Archive Stack Chase
- Purpose: Spend the first of three pursuits after the player understands archive routes.
#### Build order
- Do not build until archive navigation is stable.
- Define start trigger, chase route, hiding/escape action, and cooldown safe zone.
- Place pursuit on return from authenticity setup.
- Add checkpoint before trigger and after escape.
- Finalize enemy art/audio after route metrics are validated.

#### Graybox steps
- Choose a loop through stack rows with two corners, one obstacle, and one escape door.
- Mark no-dead-end zones and fallback escape visibility.
- Place pre-pursuit checkpoint outside file cage.
- Place post-pursuit checkpoint in Playback Booth or maintenance overlook.

#### Reusable pieces
- BP_PursuerDirector
- BP_CheckpointVolume
- BP_ScareDirector
- BP_ShortcutDoor

#### Scripting steps
- Trigger pursuit after player collects key contradiction or routes hidden layer.
- Start with audible warning before visual contact.
- Direct player through known archive loop, not new maze.
- End pursuit when player reaches booth and seals door.
- Increment global pursuit count to 1.

#### Playtest
- Player understands where to run within 3 seconds.
- At least 4 of 5 testers escape within three attempts.
- Failure reload is before the trigger with a short retry path.
- Pursuit lasts 45-90 seconds.

#### Done criteria
- This is counted as pursuit 1 of 3.
- Escape route uses previously learned space.
- No direct jumpscare occurs within the same 20-minute window unless budget allows.
- Post-pursuit cooldown is clearly safe.

- Final art audio: Add final pursuer silhouette, footfalls, breath, chase mix, door slam, and cooldown ambience only after chase readability and retry pacing pass.
- Ai prompt: Design A2-U05 first pursuit in Archive stacks: trigger, route, escape, checkpoints, global pursuit count, playtest checks, and done criteria. No implementation.

### A2-U06 — Maintenance Overlook Shortcut

- Id: A2-U06
- Act: Playback
- Type: shortcut
- Name: Maintenance Overlook Shortcut
- Purpose: Compress archive backtracking and expose pressure infrastructure for Act 3.
#### Build order
- Place shortcut exit after first pursuit cooldown.
- Connect archive overlook to Receiving Hall upper landing.
- Show locked pressure-room access beyond it.
- Add one-way opening from archive side.
- Finalize art after Act 3 connection is confirmed.

#### Graybox steps
- Block narrow service balcony above Receiving Hall.
- Add one-way latch or dropped ladder.
- Frame pressure gauge door as next major blocker.
- Measure route reduction from archive to hub.

#### Reusable pieces
- BP_ShortcutDoor
- BP_Interactable_Generic
- BP_CheckpointVolume

#### Scripting steps
- Unlock shortcut after first pursuit escape.
- Persist latch state.
- Update map/hub objective toward pressure infrastructure.
- Save checkpoint after shortcut opens.

#### Playtest
- Player recognizes the shortcut returns to hub.
- Player sees Act 3 destination before it opens.
- Shortcut reduces repeated route by at least 45 seconds.

#### Done criteria
- Shortcut cannot be opened early from hub side.
- Shortcut remains open after reload.
- Act 3 pressure door is visible and memorable.
- No player misses the return path after pursuit.

- Final art audio: Add final catwalk, ladder, metal resonance, and pressure-door foreshadowing after hub connection is locked.
- Ai prompt: Build A2-U06 Maintenance Overlook shortcut: archive-to-hub connection, one-way latch, Act 3 foreshadowing, persistence, playtest checks, and done criteria. No implementation.

### A3-U01 — Pump Gallery

- Id: A3-U01
- Act: Pressure Rooms
- Type: sector
- Name: Pump Gallery
- Purpose: Introduce water pressure as spatial state and open lower station access.
#### Build order
- Graybox pump gallery off Receiving Hall lower route.
- Define dry, wet, and flooded traversal states.
- Place pressure map and first valve tutorial.
- Add safe checkpoint before pressure state changes.
- Finalize water visuals/audio after traversal states are locked.

#### Graybox steps
- Create long pump room with upper catwalk, lower trench, valve wall, and pressure door.
- Block lower path with knee-deep/flooded placeholder volumes.
- Use gauges as landmark props.
- Place visible but unreachable lower access door.

#### Reusable pieces
- BP_RoutingNode
- BP_Interactable_Generic
- BP_CheckpointVolume
- BP_WorldTransformVolume

#### Scripting steps
- Represent pressure as routed state between rooms.
- Let first valve visibly drain one local area.
- Update route availability after pressure change.
- Save pressure state only at safe checkpoint.

#### Playtest
- Player understands pressure changes physical access.
- Player can identify which area drained.
- Player does not confuse decorative gauges with usable nodes.

#### Done criteria
- At least one before/after water state is obvious.
- No pressure state traps player away from objective.
- Checkpoint reload restores safe pressure state.
- Pump Gallery teaches Act 3 rules in under 10 minutes.

- Final art audio: Add final pumps, water shaders, pipe groans, gauge animations, and low-frequency machinery after pressure traversal states stop changing.
- Ai prompt: Build A3-U01 Pump Gallery: water pressure rules, valve tutorial, dry/wet/flooded states, checkpoint, playtest checks, and done criteria. No implementation.

### A3-U02 — Pressure Routing Puzzle

- Id: A3-U02
- Act: Pressure Rooms
- Type: puzzle
- Name: Pressure Routing Puzzle
- Purpose: Combine routing logic with physical room transformation through water levels.
#### Build order
- Define target rooms and required pressure sequence.
- Block three pressure chambers around central valve manifold.
- Add readable cause/effect from each valve.
- Add reset/failsafe route.
- Finalize water/audio only after sequence is fair.

#### Graybox steps
- Build Chamber A as source, Chamber B as traversal route, Chamber C as destination lock.
- Place central manifold with three clear outputs.
- Add observation windows or gauge boards showing remote effects.
- Keep reset valve near manifold, not hidden in a chamber.

#### Reusable pieces
- BP_RoutingNode
- BP_WorldTransformVolume
- BP_AudioEmitter_Clue
- BP_CheckpointVolume

#### Scripting steps
- Track pressure routed to one or two chambers depending on design limit.
- Change doors, water heights, and floating objects based on route.
- Provide warning before dangerous pressure change.
- Unlock lower service tunnel when correct sequence is achieved.

#### Playtest
- Player predicts at least one valve effect before trying it.
- Player recovers from wrong sequence without reload.
- Average blind solve time is 12-20 minutes.
- Player recognizes final lower tunnel unlock.

#### Done criteria
- Each valve has distinct local and remote feedback.
- Puzzle has reset without erasing unrelated act progress.
- No state causes drowning or trap unless part of clearly telegraphed failure.
- Accessibility text conveys pressure state.

- Final art audio: Add final rising water, valve wheels, pipe hammer, alarm tones, and wet material changes after playtesters solve by reasoning rather than trial-only.
- Ai prompt: Design A3-U02 Pressure Routing Puzzle: three chambers, manifold, water-state transformations, reset, unlock, playtest checks, and done criteria. No implementation.

### A3-U03 — Compression Office

- Id: A3-U03
- Act: Pressure Rooms
- Type: room
- Name: Compression Office
- Purpose: Provide a quiet investigation room inside Act 3 and seed the phase-inversion filter used for acoustic masking.
#### Build order
- Place office off pump gallery after first pressure solve.
- Add records explaining experimental hearing protection and phase-cancellation work.
- Place a broken phase-inversion headset prototype as foreshadowing.
- Add optional safe checkpoint or journal update.
- Finalize props after filter rules are locked.

#### Graybox steps
- Create small room overlooking pump machinery.
- Place desk, wall chart, filter cabinet, and locked lower hatch view.
- Add one alternate exit back to pump gallery.
- Keep room enemy-free.

#### Reusable pieces
- BP_Interactable_Generic
- BP_AudioEmitter_Clue
- BP_CheckpointVolume

#### Scripting steps
- Log a filter-related clue when the prototype is inspected.
- Add a journal hint that lower-station crews tested phase-cancellation headsets.
- Optionally play old training tape fragment.
- Do not grant the recorder filter module yet.

#### Playtest
- Player notices the filter prototype as a future mechanic.
- Player understands office is safe enough to read.
- Player connects pressure workers to lower station access.

#### Done criteria
- Foreshadowing is clear without tutorializing a mechanic too early.
- No critical Act 3 puzzle depends on optional office lore.
- The filter rig silhouette is memorable.
- Room offers pacing relief after pressure puzzle.

- Final art audio: Add final office clutter, filter cabinet, training tape voice, fluorescent buzz, and machinery rumble after filter-module narrative is approved.
- Ai prompt: Build A3-U03 Compression Office: quiet room, phase-inversion filter foreshadowing, pressure-worker research, safe pacing, playtest checks, and done criteria. No implementation.

### A3-U04 — Pressure Diver Encounter

- Id: A3-U04
- Act: Pressure Rooms
- Type: encounter
- Name: Pressure Diver Encounter
- Purpose: Introduce an enemy that reacts to sound and water state without starting a full pursuit.
#### Build order
- Build after pressure routing puzzle works.
- Place enemy patrol in a partially flooded chamber.
- Define safe observation point and bypass route.
- Add sound lure tutorial using machinery noise.
- Finalize creature art/audio after stealth readability passes.

#### Graybox steps
- Create loop chamber with one central flooded obstacle and two dry walkways.
- Add noisy pump switch as lure.
- Place exit visible behind patrol path.
- Keep hiding alcove readable from entry.

#### Reusable pieces
- BP_AudioEmitter_Clue
- BP_RoutingNode
- BP_PursuerDirector
- BP_CheckpointVolume

#### Scripting steps
- Enemy investigates loud machine activation.
- Player can cross while enemy is distracted.
- Failure causes short reset or damage, not pursuit count increment.
- Checkpoint before encounter and after bypass.

#### Playtest
- Player observes before acting.
- Player discovers lure within 3 attempts.
- Enemy perception feels consistent.
- Encounter does not become a chase unless player repeatedly fails by choice.

#### Done criteria
- Not counted as one of three pursuits.
- Sound lure is demonstrated by environment before required.
- Bypass route remains visible.
- Failure recovery is under 20 seconds.

- Final art audio: Add final diver suit silhouette, wet footsteps, breathing tube, pump lure sound, and muffled threat mix after stealth logic is stable.
- Ai prompt: Design A3-U04 Pressure Diver encounter: sound lure, flooded chamber bypass, safe observation, failure recovery, playtest checks, and done criteria. No implementation.

### A3-U05 — Direct Scare: Porthole Face

- Id: A3-U05
- Act: Pressure Rooms
- Type: scare
- Name: Direct Scare: Porthole Face
- Purpose: Spend one direct jumpscare in a controlled Act 3 cooldown window.
#### Build order
- Schedule only if 20-minute direct scare cooldown is clear.
- Place after pressure puzzle completion, before lower tunnel checkpoint.
- Use a single contained porthole/window event.
- Avoid immediate pursuit follow-up.
- Finalize scare only after budget timeline is verified.

#### Graybox steps
- Place narrow corridor with one porthole or inspection glass.
- Ensure player path naturally faces it.
- Keep exit unobstructed after scare.
- Place calm checkpoint at least 30 seconds later.

#### Reusable pieces
- BP_ScareDirector
- BP_AudioEmitter_Clue
- BP_CheckpointVolume

#### Scripting steps
- Check global direct scare cooldown before arming.
- Trigger brief face/impact/audio sting when player crosses mark.
- Log direct scare use.
- Suppress other direct scares until cooldown expires.

#### Playtest
- Scare lands without causing route confusion.
- Player continues forward within 30 seconds.
- No tester is killed or chased immediately afterward.

#### Done criteria
- Counts as direct jumpscare.
- Respects one-per-20-minute rule in expected pacing.
- Does not overlap with pursuit or major puzzle failure.
- Can be disabled or softened for accessibility mode.

- Final art audio: Add final face asset, glass hit, stinger, water impact, and immediate silence after timeline budget is locked.
- Ai prompt: Build A3-U05 direct porthole scare: budget check, trigger framing, no pursuit follow-up, cooldown handling, playtest checks, and done criteria. No implementation.

### A3-U06 — Lower Service Tunnel Drain Transformation

- Id: A3-U06
- Act: Pressure Rooms
- Type: transformation
- Name: Lower Service Tunnel Drain Transformation
- Purpose: Show a previously flooded route becoming traversable and lead into Choir Below.
#### Build order
- Establish flooded tunnel visually before puzzle solve.
- After pressure routing, transform it into wet traversable tunnel.
- Add changed debris, exposed symbols, and new audio bed.
- Add checkpoint at tunnel mouth.
- Finalize art/audio after both states are navigable.

#### Graybox steps
- Build tunnel as visible blocked route from Pump Gallery.
- Create two versions: flooded blocker and drained traversal.
- Place landmark halfway through to prevent disorientation.
- Add return ladder as later shortcut.

#### Reusable pieces
- BP_WorldTransformVolume
- BP_CheckpointVolume
- BP_ShortcutDoor
- BP_AudioEmitter_Clue

#### Scripting steps
- Swap or alter tunnel state after pressure puzzle completion.
- Reveal symbols and choir-like resonance once drained.
- Save checkpoint before player commits to lower level.
- Update objective to follow the sound below.

#### Playtest
- Player remembers this tunnel was previously flooded.
- Transformation reads as consequence of their action.
- Player follows tunnel into Act 4 without needing a waypoint.

#### Done criteria
- Before and after states are distinct.
- Tunnel cannot revert and trap player unless explicitly designed with safe exit.
- Checkpoint starts player at correct transformed state.
- Act 4 tone is introduced through audio, not text alone.

- Final art audio: Add final receding water marks, exposed growth, resonant choir bed, dripping tunnel, and wet decals after transform timing is locked.
- Ai prompt: Build A3-U06 Lower Service Tunnel transformation: flooded-before state, drained-after state, Act 4 transition, checkpoint, playtest checks, and done criteria. No implementation.

### A4-U01 — Sub-Basement Choir Nave

- Id: A4-U01
- Act: Choir Below
- Type: sector
- Name: Sub-Basement Choir Nave
- Purpose: Reveal the station's oldest experimental acoustic chamber and introduce filter-assisted acoustic masking as a spatial rule.
#### Build order
- Graybox large nave-like sub-basement below station.
- Create an unfiltered route with blocked resonance gates.
- Place the recorder filter-module acquisition at a controlled midpoint.
- Add filter-active path previews before mechanic use.
- Finalize acoustic-chamber art/audio after filter traversal logic is proven.

#### Graybox steps
- Build central chamber, side aisles, listening stalls, filter-module alcove, and sealed resonant door.
- Use strong landmarks: flooded calibration platform, hanging cables, old speakers.
- Add areas visible but inaccessible while the filter is inactive.
- Keep initial entry safe.

#### Reusable pieces
- BP_NoiseMaskVolume
- BP_WorldTransformVolume
- BP_AudioEmitter_Clue
- BP_CheckpointVolume

#### Scripting steps
- Introduce whispers that become intelligible near the filter-module alcove.
- On module installation, alter resonance gates and enemy perception rules.
- Update objective to cross the choir gates with the filter active.
- Save checkpoint immediately after filter installation.

#### Playtest
- Player understands the filter changes audibility and routes, not merely the screen effect.
- Player can distinguish filter-active and filter-inactive states.
- Player finds the first filter-reactive gate within 60 seconds after installation.

#### Done criteria
- Filter-module acquisition cannot be missed.
- Filter-assisted traversal has a clear charge, attention, or hearing cost.
- Filter-inactive and filter-active layouts remain readable as the same place transformed.
- Checkpoint preserves filter state.

- Final art audio: Add final choir space, recorder filter attachment, state-change audio, spatial whispers, and industrial calibration lighting after traversal rules are stable.
- Ai prompt: Build A4-U01 Choir Nave: sub-basement layout, recorder filter-module installation, filter-reactive gates, state readability, checkpoint, playtest checks, and done criteria. No implementation.

### A4-U02 — Masked Traversal Puzzle

- Id: A4-U02
- Act: Choir Below
- Type: puzzle
- Name: Acoustic Masking Traversal Puzzle
- Purpose: Require switching perception/traversal states to cross the choir area and reach the resonant door.
#### Build order
- Define three route states: filter inactive and safe, filter active and visible, filter active and dangerous.
- Place gates, platforms, and clue marks that differ by state.
- Add safe swap points.
- Add failure recovery without long reruns.
- Finalize filter-state art/audio after the route is readable in graybox.

#### Graybox steps
- Create a loop with two filter-inactive corridors and two filter-active crossings.
- Place swap alcoves before each risky section.
- Use placeholder waveform feedback for filter-reactive symbols.
- Keep the final resonant door visible from multiple angles.

#### Reusable pieces
- BP_NoiseMaskVolume
- BP_WorldTransformVolume
- BP_CheckpointVolume
- BP_ReadabilityBeacon

#### Scripting steps
- Toggle resonance barriers and route markers based on filter state.
- Reveal the hidden choir path while the filter is active.
- Change threat perception only in specific filter-active zones.
- Unlock resonant door after correct crossings are completed.

#### Playtest
- Player knows when to activate and deactivate the filter.
- Player can recover from wrong state quickly.
- Player does not perceive filter-reactive paths as random hallucination.
- Average solve time is 10-18 minutes.

#### Done criteria
- Every filter-reactive gate has a visual or audio clue.
- State switching cannot strand the player.
- Failure recovery is less than 45 seconds.
- Puzzle teaches rules needed for Act 5 overdub traversal.

- Final art audio: Add final environmental masking feedbacks, choir harmonics, alternate-state materials, and threat cues after state readability and accessibility pass.
- Ai prompt: Design A4-U02 Acoustic Masking Traversal Puzzle: filter-state rules, gates, activation points, risk, recovery, playtest checks, and done criteria. No implementation.

### A4-U03 — Choir Ledger and Names

- Id: A4-U03
- Act: Choir Below
- Type: clue
- Name: Choir Ledger and Names
- Purpose: Reveal that recordings were assembled from real residents and prepare authenticity consequences.
#### Build order
- Place ledger after the first acoustic-masking traversal success.
- Connect names to dormitory and archive clues.
- Add optional cross-reference lines for attentive players.
- Unlock authenticity comparator update.
- Finalize handwriting and VO after narrative lock.

#### Graybox steps
- Place ledger on central choir stand.
- Add nearby wall of names or tuning forks.
- Create enough space for safe reading.
- Frame route forward behind ledger.

#### Reusable pieces
- BP_Interactable_Generic
- BP_AuthenticityComparator
- BP_AudioEmitter_Clue

#### Scripting steps
- Log ledger as major clue.
- Update previously collected recordings with identity metadata.
- Reveal at least one earlier voice was misattributed.
- Set objective to test which voices are authentic.

#### Playtest
- Player links ledger to earlier dormitory tape.
- Player understands authenticity now has stakes.
- Player is not blocked if optional archive fragments were missed.

#### Done criteria
- Ledger is readable and not lost during threat pressure.
- Names connect at least two previous acts.
- Critical authenticity information is duplicated in journal.
- No optional clue becomes mandatory retroactively.

- Final art audio: Add final ledger pages, ink, whispers tied to names, and subtle identity motifs after narrative continuity review.
- Ai prompt: Build A4-U03 Choir Ledger clue: name reveal, links to dormitory/archive, authenticity update, journal support, playtest checks, and done criteria. No implementation.

### A4-U04 — Authenticity Puzzle: Real Voice, False Voice

- Id: A4-U04
- Act: Choir Below
- Type: puzzle
- Name: Authenticity Puzzle: Real Voice, False Voice
- Purpose: Require the player to identify authentic recordings using contradictions, room tone, and identity clues.
#### Build order
- Define candidate recordings and evidence matrix.
- Place comparator station in choir control alcove.
- Require at least two evidence types per correct judgment.
- Add wrong judgment feedback that points to evidence, not failure.
- Finalize audio processing after evidence matrix is approved.

#### Graybox steps
- Create comparator alcove with three candidate slots.
- Place evidence board or diegetic notes nearby.
- Keep route to missed supporting clues available.
- Use temporary labels for voice A, B, C.

#### Reusable pieces
- BP_AuthenticityComparator
- BP_AudioEmitter_Clue
- BP_Interactable_Generic
- BP_CheckpointVolume

#### Scripting steps
- Allow players to compare candidate recordings.
- Evaluate choices based on logged or available evidence.
- On correct identification, unlock choir resonator.
- On incorrect choice, play contradiction cue and keep puzzle active.

#### Playtest
- Player justifies correct answer with evidence.
- Player does not rely on guessing voice quality only.
- Missed evidence can be recovered without excessive backtracking.
- Average solve time is 12-22 minutes.

#### Done criteria
- All required evidence exists in at least two accessible forms.
- Wrong answers provide diagnostic feedback.
- Puzzle supports transcript-based solving.
- Correct solve advances Act 4 without invalidating optional theories.

- Final art audio: Add final comparator UI/art, voice degradation, room-tone fingerprints, and choir-resonator success audio after evidence fairness is validated.
- Ai prompt: Design A4-U04 Authenticity Puzzle: candidate recordings, evidence matrix, comparator, wrong-answer feedback, recovery, playtest checks, and done criteria. No implementation.

### A4-U05 — Second Pursuit: Choir Procession

- Id: A4-U05
- Act: Choir Below
- Type: encounter
- Name: Second Pursuit: Choir Procession
- Purpose: Spend pursuit two using filter-active/filter-inactive traversal under pressure.
#### Build order
- Build only after acoustic masking traversal is understood.
- Define chase path with two required state decisions.
- Place procession behind player after authenticity solve.
- Add clear escape into lift or stairwell.
- Finalize audio after pursuit timing is locked.

#### Graybox steps
- Use known nave route in reverse with newly opened resonator path.
- Add one filter-active shortcut and one filter-inactive safe corridor.
- Place escape gate visible at far end.
- Ensure no blind dead ends.

#### Reusable pieces
- BP_PursuerDirector
- BP_NoiseMaskVolume
- BP_CheckpointVolume
- BP_ScareDirector

#### Scripting steps
- Trigger after authenticity puzzle success and resonator activation.
- Start with choir swell and procession movement, not instant hit.
- Require one filter-active traversal and one filter-inactive safety choice.
- End when player reaches service lift or sealed stairwell.
- Increment global pursuit count to 2.

#### Playtest
- Player understands state decisions while stressed.
- Pursuit lasts 60-100 seconds.
- At least 4 of 5 testers escape within four attempts.
- Retry checkpoint is close and fair.

#### Done criteria
- This is pursuit 2 of 3.
- Uses learned acoustic masking traversal, not new rules.
- No direct jumpscare is stacked on start unless budget allows.
- Escape transitions cleanly to Dead Air setup.

- Final art audio: Add final procession silhouettes, choir swell, filter-state mix, chase lighting, and escape slam after route readability passes.
- Ai prompt: Design A4-U05 second pursuit Choir Procession: trigger, filter-state chase decisions, escape route, checkpoints, pursuit count, playtest checks, and done criteria. No implementation.

### A4-U06 — Service Lift Return

- Id: A4-U06
- Act: Choir Below
- Type: shortcut
- Name: Service Lift Return
- Purpose: Return player to upper station while permanently changing the hub for Act 5.
#### Build order
- Place lift escape at end of second pursuit.
- Connect to Receiving Hall or transmitter landing.
- Transform hub with choir influence and dead-air interference.
- Set Act 5 objective toward broadcast tower/core.
- Finalize hub transformation after all prior shortcuts are verified.

#### Graybox steps
- Create lift/stair exit into familiar hub angle.
- Block old lower route temporarily or mark it unsafe.
- Add transformed props in Receiving Hall.
- Open transmitter path that was locked since Act 1.

#### Reusable pieces
- BP_ShortcutDoor
- BP_WorldTransformVolume
- BP_CheckpointVolume
- BP_AudioEmitter_Clue

#### Scripting steps
- Persist service lift as shortcut.
- Apply hub transformation state.
- Unlock transmitter access.
- Save checkpoint after transformation and pursuit completion.

#### Playtest
- Player recognizes the hub but sees it has changed.
- Player knows transmitter path is now the objective.
- Shortcut prevents long Act 4 backtracking.

#### Done criteria
- All previous hub doors retain correct states.
- Transformation does not obscure required exits.
- Act 5 path is visible from checkpoint.
- Pursuit cooldown feels complete.

- Final art audio: Add final transformed hub dressing, dead-air interference, warped signage, lift sounds, and new objective motif after state persistence is verified.
- Ai prompt: Build A4-U06 Service Lift Return: pursuit escape, hub transformation, transmitter unlock, shortcut persistence, playtest checks, and done criteria. No implementation.

### A5-U01 — Transmitter Spine

- Id: A5-U01
- Act: Dead Air
- Type: sector
- Name: Transmitter Spine
- Purpose: Open the final vertical route through the station and combine all prior mechanics.
#### Build order
- Graybox vertical transmitter stair/catwalk route from transformed hub.
- Add locked broadcast core visible above.
- Place stations for acoustic, routing, authenticity, and phase-inversion filter callbacks.
- Add checkpoint at base and mid-spine.
- Finalize tower art/audio after combined route is stable.

#### Graybox steps
- Create three-level vertical spine with catwalks, equipment rooms, and tower window views.
- Use familiar landmarks from prior acts as callbacks.
- Build safe midpoint alcove.
- Keep final broadcast core visible but unreachable until overdub puzzle.

#### Reusable pieces
- BP_AcousticLock
- BP_RoutingNode
- BP_AuthenticityComparator
- BP_NoiseMaskVolume
- BP_CheckpointVolume

#### Scripting steps
- Require brief reuse of each prior mechanic in simplified form.
- Gate progress by knowledge, not inventory scavenger hunt.
- Escalate dead-air interference as player climbs.
- Save state at midpoint.

#### Playtest
- Player feels mechanics are being remixed, not retaught.
- Route remains readable despite verticality.
- Midpoint checkpoint prevents long replay.

#### Done criteria
- All prior mechanics are represented at least once.
- No callback is harder than its original act puzzle.
- Vertical route has no fatal ambiguity.
- Final core entrance is clearly the destination.

- Final art audio: Add final transmitter machinery, storm-height views, radio interference, metal groans, and escalating score after traversal and callback gates are locked.
- Ai prompt: Build A5-U01 Transmitter Spine: vertical final route, callbacks to prior mechanics, checkpoints, visible broadcast core, playtest checks, and done criteria. No implementation.

### A5-U02 — Overdub Puzzle

- Id: A5-U02
- Act: Dead Air
- Type: puzzle
- Name: Overdub Puzzle
- Purpose: Let the player assemble and broadcast a truthful composite from recovered fragments.
#### Build order
- Define required fragments and why each is valid.
- Place overdub recorder in broadcast core.
- Create selection, preview, and commit phases.
- Add feedback for false or incomplete composites.
- Finalize audio after narrative and evidence matrix are locked.

#### Graybox steps
- Build broadcast core room with recorder, waveform table, antenna route display, and exit lockdown door.
- Represent fragments as physical tapes, transcript cards, or diegetic channels.
- Reserve space for preview playback without threat interruption.
- Create clear commit control separate from preview.

#### Reusable pieces
- BP_OverdubRecorder
- BP_AuthenticityComparator
- BP_AudioEmitter_Clue
- BP_RoutingNode
- BP_CheckpointVolume

#### Scripting steps
- Allow player to choose fragments from prior acts.
- Validate composite against authenticity evidence.
- On incomplete composite, identify missing category.
- On false composite, play dead-air rejection and point to contradiction.
- On correct composite, arm final broadcast sequence.

#### Playtest
- Player can explain why chosen fragments are truthful.
- Player uses journal/transcripts rather than guessing.
- Wrong feedback is helpful but does not solve automatically.
- Average solve time is 15-25 minutes.

#### Done criteria
- Puzzle requires acoustic, routing, authenticity, and playback knowledge.
- All required fragments are recoverable before entering final lock-in.
- Commit action is unmistakable.
- Correct solution advances without requiring perfect optional completion.

- Final art audio: Add final fragment VO, waveform visuals, recorder mechanics, dead-air rejection layers, and truthful composite mix after final script lock and playtest fairness pass.
- Ai prompt: Design A5-U02 Overdub Puzzle: fragment selection, preview/commit phases, truth validation, wrong feedback, final broadcast arming, playtest checks, and done criteria. No implementation.

### A5-U03 — Third Pursuit: Dead Air Climb

- Id: A5-U03
- Act: Dead Air
- Type: encounter
- Name: Third Pursuit: Dead Air Climb
- Purpose: Spend the final pursuit during escape/climb after the correct overdub is armed.
#### Build order
- Build only after overdub success path is stable.
- Define climb route, pressure gates, and final escape room.
- Use all established threat language, no new mechanics.
- Place checkpoint immediately before final sequence.
- Finalize enemy/audio after route timing is locked.

#### Graybox steps
- Create ascent from broadcast core to antenna platform or sealed control room.
- Add two simple obstacles based on prior mechanics: route switch and acoustically masked pass.
- Keep route forward-facing with strong lighting contrast.
- Place final safe room or broadcast lever at end.

#### Reusable pieces
- BP_PursuerDirector
- BP_RoutingNode
- BP_NoiseMaskVolume
- BP_CheckpointVolume
- BP_ScareDirector

#### Scripting steps
- Trigger only after correct overdub is armed.
- Start with dead-air collapse and clear escape objective.
- Require quick but readable interaction beats.
- End when player reaches final broadcast position.
- Increment global pursuit count to 3 and prevent any further pursuit starts.

#### Playtest
- Player always understands this is the finale chase.
- Pursuit lasts 90-150 seconds.
- At least 4 of 5 testers finish within five attempts.
- Retry starts at pre-chase checkpoint with overdub already armed.

#### Done criteria
- This is pursuit 3 of 3.
- No additional pursuit can occur afterward.
- Final route uses known mechanics only.
- Failure reload is fast and fair.

- Final art audio: Add final threat form, collapsing signal mix, tower alarms, chase score, and storm climax after pursuit metrics pass.
- Ai prompt: Design A5-U03 final Dead Air pursuit: post-overdub trigger, climb route, known-mechanic obstacles, checkpoint, pursuit count 3, playtest checks, and done criteria. No implementation.

### A5-U04 — Station-Wide Dead Air Transformation

- Id: A5-U04
- Act: Dead Air
- Type: transformation
- Name: Station-Wide Dead Air Transformation
- Purpose: Show the entire location reacting to the overdub and collapsing false recordings.
#### Build order
- Define which previously visited spaces change and which remain stable.
- Apply transformation during final act only.
- Prioritize critical path readability over spectacle.
- Add transformed shortcuts or blocked old routes as needed.
- Finalize art/audio after final route cannot break.

#### Graybox steps
- Mark Receiving Hall, Dormitory, Archive, Pump Gallery, and Choir Below with transformation states.
- Create simplified versions if revisiting is required.
- Avoid opening irrelevant old routes during finale.
- Use debug labels to confirm correct transformed state per checkpoint.

#### Reusable pieces
- BP_WorldTransformVolume
- BP_ShortcutDoor
- BP_ScareDirector
- BP_CheckpointVolume

#### Scripting steps
- Activate transformation after overdub arming or during final pursuit.
- Update ambience and geometry blockers per sector.
- Disable outdated puzzles that are no longer relevant.
- Maintain save-state consistency after reload.

#### Playtest
- Player recognizes familiar spaces but follows the final route.
- No one backtracks into obsolete content expecting new objectives.
- Transformation supports climax without hurting performance.

#### Done criteria
- Every transformed sector has an approved before/after list.
- No critical route is obscured by spectacle.
- Old puzzles cannot be re-triggered into invalid states.
- Checkpoint reload restores correct transformed version.

- Final art audio: Add final global interference, lighting corruption, water reversal, tape artifacts, choir residue, and environmental deformation after all final-act routing is locked.
- Ai prompt: Build A5-U04 station-wide transformation: before/after states for prior sectors, final-route readability, obsolete puzzle disabling, checkpoint consistency, playtest checks, and done criteria. No implementation.

### A5-U05 — Final Broadcast Position

- Id: A5-U05
- Act: Dead Air
- Type: room
- Name: Final Broadcast Position
- Purpose: Resolve the mechanics and story with the completed overdub broadcast.
#### Build order
- Block final room/platform after third pursuit escape.
- Place final broadcast control in unmistakable view.
- Add last interaction, aftermath beat, and exit framing.
- Add final checkpoint or autosave after resolution.
- Finalize ending art/audio after narrative signoff.

#### Graybox steps
- Create compact final space with one entrance, one broadcast console, and one aftermath view.
- Remove unnecessary interactables.
- Frame storm/tower/sea or station heart as visual endpoint.
- Reserve room for credits or post-game transition trigger.

#### Reusable pieces
- BP_OverdubRecorder
- BP_Interactable_Generic
- BP_CheckpointVolume
- BP_AudioEmitter_Clue

#### Scripting steps
- Allow final broadcast only if overdub correct and final pursuit complete.
- Play resolved composite.
- Apply final station response.
- Lock out further pursuits and direct scares.
- Trigger ending state and save completion.

#### Playtest
- Player understands this is the final action.
- Ending interaction cannot be missed or accidentally triggered too early.
- Aftermath gives emotional clarity without requiring optional lore.

#### Done criteria
- Final broadcast cannot happen with invalid overdub.
- All threat systems are disabled after resolution.
- Completion state saves correctly.
- Ending beat resolves the five-act mechanic chain.

- Final art audio: Add final broadcast mix, storm break or silence, resolved leitmotif, final lighting state, and ending cinematic/audio only after all gameplay lock conditions are verified.
- Ai prompt: Build A5-U05 Final Broadcast Position: final room, broadcast interaction, aftermath, completion save, threat shutdown, playtest checks, and done criteria. No implementation.


## Act level art audio milestones

### 1

- Milestone: After Landfall graybox lock
#### Add
- Final exterior storm palette
- Dormitory material pass
- First acoustic lock final audio
- Landfall tape voice

#### Hold back
- Hub final transformation art
- Any finale interference layers


### 2

- Milestone: After Playback puzzle and first pursuit validation
#### Add
- Archive dressing
- Tape playback equipment
- Routing cable art
- First pursuer audio identity

#### Hold back
- Final authenticity voice processing until Act 4 evidence is locked


### 3

- Milestone: After Pressure Rooms state persistence passes
#### Add
- Water materials
- Pump machinery
- Pressure valve sounds
- Porthole direct scare assets

#### Hold back
- Complex water spectacle that could obscure route readability


### 4

- Milestone: After Choir Below acoustic-masking traversal and authenticity solve pass
#### Add
- Environmental masking feedback
- Choir ambiences
- Ledger art
- Procession pursuit mix

#### Hold back
- Station-wide transformation until Act 5 route is locked


### 5

- Milestone: After Dead Air full run completion
#### Add
- Final transmitter art
- Overdub final mix
- Dead-air transformation pass
- Final pursuit score
- Ending broadcast

#### Hold back
- Nothing, unless playtest shows route readability regressions



## Full game verification

### Required full run tests
- Complete critical path from exterior to ending in one session without developer help.
- Complete from every checkpoint after quitting and reloading.
- Verify direct jumpscare spacing with expected-route timestamps.
- Verify exactly three pursuits occur: Archive Stack Chase, Choir Procession, Dead Air Climb.
- Verify every core puzzle is introduced, practiced, and reused: acoustic lock, routing, authenticity, masked traversal, overdub.
- Verify final art/audio additions do not change puzzle fairness.
- Verify all transcripts and accessibility supports preserve puzzle solvability.

### Target completion timing

- Landfall: 35-55 minutes
- Playback: 45-70 minutes
- Pressure Rooms: 45-70 minutes
- Choir Below: 50-80 minutes
- Dead Air: 45-75 minutes
- Total: 4-6 hours

### Done definition
- All build units meet their done criteria.
- No critical clue is missable without recovery.
- No checkpoint can reload into a softlock, active unavoidable damage, or invalid puzzle state.
- All shortcuts persist and reduce backtracking.
- All transformations have before/after readability.
- Final art/audio is added only after graybox validation for that unit.
- The final full run confirms scare and pursuit budgets.


- Files created or modified: []
- Issues encountered: []
