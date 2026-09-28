# Listener AI and Encounter Director

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

- Status: complete
## Summary

- What i did: Created a granular no-implementation build sequence for The Listener AI system, including architecture, classes/assets, tasks, done-tests, and copy-paste AI prompts.
### What is included
- Listener AI architecture
- Sound scoring and acoustic stimulus pipeline
- Patrol/investigate/search/stalk/chase/retreat state flow
- Behavior Tree/Blackboard plus C++ truth ownership
- Encounter director
- Drowned Technician zone controller
- Antenna Choir sector hazard
- Fairness tells
- Navmesh and doors
- Death/retry adaptation
- Debugging and testing plan

- Files created or modified: []
- Issues encountered: []

## Implementation policy

- Scope: Planning only. No implementation.
### Principles
- C++ owns truth, rules, scoring, transitions, and persistence.
- Blueprints own presentation, VFX, audio routing, animation, tuning instances, and level hooks.
- Data Assets own tuning values.
- Player failures must be legible.
- Never spawn behind the player without warning.
- Every chase space must support at least two viable routes.
- After two deaths, reduce persistence and aggression until the player succeeds.


## Recommended architecture

- Core choice: C++ hierarchical state machine mirrored into Blackboard keys for Behavior Tree execution.
### Why
- C++ state machine keeps authoritative transitions deterministic and testable.
- Behavior Tree handles movement/task orchestration without becoming the source of truth.
- Blackboard exposes readable state for designers, debugging, and Blueprint presentation.

### Runtime flow
- UAcousticStimulusSubsystem receives and normalizes sound events.
- UListenerSoundScorerComponent scores stimuli against Listener tuning and current state.
- UListenerBrainComponent decides state transitions and writes Blackboard keys.
- BT_Listener reads state keys and executes state-specific movement/tasks.
- AEncounterDirector manages macro pressure, cooldowns, spawn eligibility, and fairness.
- Zone controllers modify local rules, hazards, tells, and escape affordances.


## Class and asset inventory

### Cxx structs
- FAcousticStimulus
- FAcousticStimulusScore
- FListenerStateSnapshot
- FListenerInvestigationMemory
- FListenerChaseContext
- FEncounterPressureSample
- FFairnessTellRequest
- FDoorTraversalRequest
- FDeathRetryProfile

### Cxx enums
- EAcousticStimulusType
- EListenerAIState
- EListenerAlertLevel
- EListenerFairnessTellType
- EEncounterIntensity
- EListenerManifestationType
- EDoorTraversalMode
- EDeathRetryTier

### Cxx classes
- UAcousticStimulusSubsystem
- UListenerSoundScorerComponent
- UListenerBrainComponent
- UListenerBlackboardSyncComponent
- UListenerPerceptionComponent
- UListenerNavigationComponent
- UListenerDoorInteractionComponent
- UListenerFairnessTellComponent
- UListenerDebugComponent
- AListenerCharacter
- AListenerAIController
- AEncounterDirector
- ADrownedTechnicianZoneController
- AAntennaChoirSectorController
- AListenerManifestationBase
- ADrownedTechnicianManifestation
- AAntennaChoirManifestation
- AListenerPatrolAnchor
- AListenerInvestigationPoint
- AListenerEscapeRouteMarker
- AListenerFairnessTellActor
- AListenerDoorActor

### Data assets
- UDA_ListenerTuning
- UDA_ListenerSoundScoringTuning
- UDA_ListenerStateTuning
- UDA_EncounterDirectorTuning
- UDA_DrownedTechnicianZoneTuning
- UDA_AntennaChoirSectorTuning
- UDA_FairnessTellTuning
- UDA_DeathRetryTuning
- UDA_NavDoorTuning

### Blueprint assets
- BP_ListenerCharacter
- BP_ListenerAIController
- BP_EncounterDirector
- BP_DrownedTechnicianZoneController
- BP_AntennaChoirSectorController
- BP_DrownedTechnicianManifestation
- BP_AntennaChoirManifestation
- BP_ListenerFairnessTellActor
- BP_ListenerDoorActor

### Behavior tree assets
- BT_Listener
- BB_Listener
- BTS_SyncListenerState
- BTTask_MoveToListenerTarget
- BTTask_PlayFairnessTell
- BTTask_InvestigateStimulus
- BTTask_SearchPattern
- BTTask_StartChase
- BTTask_Retreat
- BTTask_UseDoorOrBreakLine

### Blackboard keys
- ListenerState
- AlertLevel
- CurrentStimulusLocation
- CurrentStimulusScore
- LastKnownPlayerLocation
- PredictedPlayerLocation
- InvestigationAnchor
- SearchRadius
- SearchTimeRemaining
- HasLineOfSight
- ChaseRouteA
- ChaseRouteB
- CanSpawnHere
- FairnessTellRequired
- FairnessTellType
- PersistenceScale
- DoorTarget
- RetreatTarget
- ZoneController
- EncounterIntensity


## Implementation sequence

### 01_foundation_contracts

- Phase: 01_foundation_contracts
- Goal: Define contracts before behavior.
#### Tasks

##### 01.01 — Create AI state and stimulus vocabulary

- Id: 01.01
- Title: Create AI state and stimulus vocabulary
###### Classes or assets
- EListenerAIState
- EAcousticStimulusType
- EListenerAlertLevel
- FAcousticStimulus
- FAcousticStimulusScore

###### Small tasks
- Define states: Dormant, Patrol, Investigate, Search, Stalk, Chase, Retreat, Suppressed.
- Define stimulus fields: source actor, location, loudness, tag/type, timestamp, occlusion hint, floor/zone id, player-caused flag.
- Define score fields: raw loudness, distance falloff, occlusion penalty, recency, repetition bonus, danger modifier, final score.

###### Done tests
- Automation test can construct FAcousticStimulus with all required fields.
- Automation test verifies every EListenerAIState has a debug display name.
- No Blueprint class is needed to evaluate a sound score struct.

- Copy paste ai prompt: Create only the C++ type-contract plan for Listener AI states and acoustic stimulus structs. Do not implement code. Include enum names, struct field names, comments, and automation tests that should be written first.

##### 01.02 — Define tuning Data Asset schema

- Id: 01.02
- Title: Define tuning Data Asset schema
###### Classes or assets
- UDA_ListenerTuning
- UDA_ListenerSoundScoringTuning
- UDA_ListenerStateTuning
- UDA_FairnessTellTuning
- UDA_DeathRetryTuning

###### Small tasks
- List all tunable values as Data Asset fields.
- Separate sound scoring values from state transition values.
- Add per-difficulty or per-act overrides as optional nested structs.
- Make every fairness rule tunable but bounded by hard C++ invariants.

###### Done tests
- Data validation test fails if chase persistence min exceeds max.
- Data validation test fails if spawn warning lead time is zero.
- Data validation test fails if chase route requirement is below two.

- Copy paste ai prompt: Design the Data Asset schema for Listener AI tuning in UE5. Do not implement. Include asset names, field groups, validation rules, and specific automation tests for invalid tuning.



### 02_acoustic_stimulus_subsystem

- Phase: 02_acoustic_stimulus_subsystem
- Goal: Make sound events first-class and testable.
#### Tasks

##### 02.01 — Specify acoustic stimulus registration API

- Id: 02.01
- Title: Specify acoustic stimulus registration API
###### Classes or assets
- UAcousticStimulusSubsystem
- FAcousticStimulus

###### Small tasks
- Define RegisterStimulus API for C++ callers.
- Define BlueprintCallable wrapper for presentation or level scripting.
- Define ring buffer/history query API by time window, zone, and stimulus type.
- Define event delegate for listeners such as AI, debug widgets, and director.

###### Done tests
- Registering one stimulus makes it queryable by time window.
- Expired stimuli are not returned after configured lifetime.
- Blueprint-facing API cannot omit required fields.

- Copy paste ai prompt: Plan the UAcousticStimulusSubsystem API for UE5. No implementation. Include C++ method names, Blueprint callable wrappers, event delegates, history queries, and tests to prove expiry and query behavior.

##### 02.02 — Specify sound emission integration points

- Id: 02.02
- Title: Specify sound emission integration points
###### Classes or assets
- UAcousticStimulusSubsystem
- AListenerDoorActor
- PlayerCharacter
- Interactables

###### Small tasks
- List sound sources: footsteps, sprinting, crouch mistakes, thrown objects, doors, inventory noise, radios, water splashes, generator events.
- Assign stimulus type and default loudness per source.
- Define designer-facing tags for special sounds.
- Define rule that fake/hazard sounds are flagged non-player-caused unless explicitly baiting the Listener.

###### Done tests
- Each required gameplay sound source has a mapped stimulus type.
- Door open and door slam produce different loudness values.
- Non-player ambient loops do not increase player blame score.

- Copy paste ai prompt: Create a sound-source mapping plan for a UE5 horror Listener AI. No code. Include gameplay sources, stimulus types, loudness defaults, player-caused flags, and done-tests.



### 03_sound_scoring

- Phase: 03_sound_scoring
- Goal: Turn sound events into legible AI intent.
#### Tasks

##### 03.01 — Specify sound scoring formula

- Id: 03.01
- Title: Specify sound scoring formula
###### Classes or assets
- UListenerSoundScorerComponent
- UDA_ListenerSoundScoringTuning
- FAcousticStimulusScore

###### Small tasks
- Define scoring terms: base loudness, distance falloff, occlusion penalty, recency curve, repetition bonus, zone sensitivity, state multiplier.
- Define minimum score thresholds for Investigate, Search escalation, Stalk escalation, and Chase escalation.
- Define separate suspicion memory so repeated small sounds can matter without instantly causing chase.
- Define cap to prevent one accidental micro-sound from becoming unfair death.

###### Done tests
- Louder closer sound scores higher than quieter farther sound.
- Occluded sound scores lower than unoccluded sound.
- Repeated low sounds can trigger Investigate but not direct Chase unless threshold is met.
- Old sounds decay below investigation threshold.

- Copy paste ai prompt: Design the Listener sound scoring system. No implementation. Include formula terms, thresholds, memory behavior, clamping rules, and automation tests for distance, occlusion, recency, and repetition.

##### 03.02 — Specify stimulus arbitration

- Id: 03.02
- Title: Specify stimulus arbitration
###### Classes or assets
- UListenerSoundScorerComponent
- UListenerBrainComponent

###### Small tasks
- Define how the Listener chooses among multiple stimuli.
- Prioritize player-caused high-confidence stimuli.
- Allow director-authored bait stimuli only when fairness cooldown allows.
- Prevent far noisy hazards from constantly stealing attention.

###### Done tests
- Highest final score wins when all else is equal.
- Recent player-caused sound beats older ambient hazard at similar score.
- Stimulus below threshold cannot interrupt active chase.
- Director bait cannot override fairness lockout.

- Copy paste ai prompt: Plan stimulus arbitration for The Listener AI. No code. Include priority rules, interruption rules, director-authored bait limits, and done-tests.



### 04_listener_brain_and_blackboard

- Phase: 04_listener_brain_and_blackboard
- Goal: Create authoritative C++ state transitions with readable BT execution.
#### Tasks

##### 04.01 — Specify Listener brain component

- Id: 04.01
- Title: Specify Listener brain component
###### Classes or assets
- UListenerBrainComponent
- FListenerStateSnapshot
- UListenerBlackboardSyncComponent

###### Small tasks
- Define state snapshot fields.
- Define TickDecision or EvaluateState API.
- Define transition guard functions for each state.
- Define event hooks: OnStateEntered, OnStateExited, OnTargetChanged, OnFairnessTellRequired.
- Define blackboard sync outputs.

###### Done tests
- Patrol transitions to Investigate when stimulus exceeds investigate threshold.
- Investigate transitions to Search when target not found after arrival.
- Search transitions to Patrol after timeout if no new evidence.
- Chase transitions to Retreat after persistence budget expires.
- State transitions write expected Blackboard keys.

- Copy paste ai prompt: Design UListenerBrainComponent as the C++ truth for Listener AI state transitions. No implementation. Include API, state snapshot fields, transition guards, events, Blackboard sync keys, and tests.

##### 04.02 — Specify Blackboard and Behavior Tree shell

- Id: 04.02
- Title: Specify Blackboard and Behavior Tree shell
###### Classes or assets
- BB_Listener
- BT_Listener
- BTS_SyncListenerState

###### Small tasks
- Define Blackboard keys and types.
- Create BT root selector by ListenerState.
- Map each state to a subtree or task sequence.
- Keep transition logic out of decorators except for simple key checks.
- Add debug labels per active branch.

###### Done tests
- BT has one branch per state.
- BT does not own score thresholds.
- Blackboard contains all keys required by state branches.
- A missing key fails validation before playtest.

- Copy paste ai prompt: Plan the UE5 Behavior Tree and Blackboard shell for The Listener. No implementation. Include key list, branch layout, responsibilities, and validation tests.



### 05_state_behaviors

- Phase: 05_state_behaviors
- Goal: Define each Listener state as small testable behavior.
#### Tasks

##### 05.01 — Patrol state

- Id: 05.01
- Title: Patrol state
###### Classes or assets
- AListenerPatrolAnchor
- BTTask_MoveToListenerTarget
- UDA_ListenerStateTuning

###### Small tasks
- Define patrol anchor selection by zone, director pressure, and recent player path.
- Use patrol as low-pressure presence, not constant pursuit.
- Allow low-score sounds to bend route without full investigation.
- Expose Blueprint events for idle audio and animation tells.

###### Done tests
- Patrol picks only reachable anchors.
- Patrol does not choose anchors inside player safe room.
- Low-score sound can update facing/route without state escalation.
- Patrol cannot spawn behind player without warning.

- Copy paste ai prompt: Design the Patrol state for The Listener AI. No code. Include patrol anchor rules, director pressure influence, low-score sound reactions, Blueprint presentation hooks, and done-tests.

##### 05.02 — Investigate state

- Id: 05.02
- Title: Investigate state
###### Classes or assets
- AListenerInvestigationPoint
- BTTask_InvestigateStimulus
- UListenerNavigationComponent

###### Small tasks
- Move to scored stimulus location or nearest reachable projection.
- Play arrival tell before local search if player is nearby.
- Remember investigated location and source confidence.
- Escalate to Search if evidence remains ambiguous.
- Escalate to Chase only with line of sight or strong confirmation.

###### Done tests
- Unreachable stimulus projects to nearest valid nav point.
- Investigate does not instantly chase from sound alone unless tuning explicitly allows and fairness checks pass.
- Repeated stimulus at same location increases confidence.
- Arrival tell fires before close-range sweep.

- Copy paste ai prompt: Design the Investigate state for The Listener. No implementation. Include reachable projection, memory, escalation rules, arrival tells, and tests.

##### 05.03 — Search state

- Id: 05.03
- Title: Search state
###### Classes or assets
- BTTask_SearchPattern
- FListenerInvestigationMemory
- UDA_ListenerStateTuning

###### Small tasks
- Define search patterns: spiral, room sweep, doorway check, last-known path fork.
- Scale radius by confidence and director pressure.
- Limit search duration with persistence budget.
- Allow player stealth recovery if they stop generating evidence.

###### Done tests
- Search radius grows with confidence but stays within tuning max.
- Search ends after timeout with no new evidence.
- Search checks at least one nearby doorway when available.
- Player silence reduces confidence over time.

- Copy paste ai prompt: Plan the Search state for a UE5 horror Listener AI. No code. Include search pattern types, confidence scaling, duration limits, stealth recovery, and done-tests.

##### 05.04 — Stalk state

- Id: 05.04
- Title: Stalk state
###### Classes or assets
- UListenerBrainComponent
- BTTask_MoveToListenerTarget
- UListenerFairnessTellComponent

###### Small tasks
- Define Stalk as visible/audible pressure before direct chase.
- Maintain partial distance and line-of-threat without constant line of sight.
- Require fairness tells before entering close stalk range.
- Allow player to avoid chase by changing route, hiding, or staying quiet.
- Use Stalk for near-miss dread, not cheap capture.

###### Done tests
- Stalk cannot begin behind player without prior warning.
- Stalk remains outside configured minimum unfair proximity unless tell has fired.
- Player breaking evidence trail can downgrade Stalk to Search.
- Stalk can escalate to Chase only with confirmation.

- Copy paste ai prompt: Design the Stalk state for The Listener. No implementation. Include pressure behavior, distance constraints, fairness tells, player counterplay, escalation/downgrade rules, and done-tests.

##### 05.05 — Chase state

- Id: 05.05
- Title: Chase state
###### Classes or assets
- FListenerChaseContext
- AListenerEscapeRouteMarker
- BTTask_StartChase
- UListenerNavigationComponent

###### Small tasks
- Require ChaseStartFairnessCheck before entering Chase.
- Capture last known player location and predicted route.
- Verify two viable escape routes in current chase space.
- Apply persistence budget and route pressure instead of infinite pursuit.
- Bias toward visible pursuit lanes and readable audio cues.
- Do not teleport during active chase except scripted transitions with tells.

###### Done tests
- Chase cannot start if fewer than two viable escape routes are available.
- Chase cannot start from behind player without warning window.
- Persistence budget eventually forces Retreat or Search.
- Closed but openable doors count as routes only if traversal is possible.
- Chase start emits a tell event.

- Copy paste ai prompt: Design the Chase state for The Listener AI. No code. Include fairness gate, two-route validation, persistence budget, route pressure, no-teleport rule, and automation tests.

##### 05.06 — Retreat state

- Id: 05.06
- Title: Retreat state
###### Classes or assets
- BTTask_Retreat
- UListenerNavigationComponent
- AEncounterDirector

###### Small tasks
- Define retreat targets out of immediate player path.
- Clear chase-only aggression values.
- Notify director that pressure has cooled.
- Allow diegetic retreat tell: distant knock, radio decay, wet footsteps fading.
- Transition to Patrol, Dormant, or zone-specific manifestation after cooldown.

###### Done tests
- Retreat target is reachable.
- Retreat target is not inside player immediate escape route unless scripted.
- Director pressure decreases after retreat.
- Retreat cannot immediately re-enter Chase without cooldown.

- Copy paste ai prompt: Plan the Retreat state for The Listener. No implementation. Include target selection, director notification, presentation tells, cooldown behavior, and done-tests.



### 06_encounter_director

- Phase: 06_encounter_director
- Goal: Control macro pacing and prevent unfair stacking.
#### Tasks

##### 06.01 — Specify encounter director pressure model

- Id: 06.01
- Title: Specify encounter director pressure model
###### Classes or assets
- AEncounterDirector
- UDA_EncounterDirectorTuning
- FEncounterPressureSample

###### Small tasks
- Track player stress indicators: recent chase, recent death, objective progress, hiding time, noise rate, zone danger.
- Track AI pressure: Listener active state, manifestation count, hazard active state.
- Output EncounterIntensity: Calm, Uneasy, Threatening, Peak, Cooldown.
- Enforce cooldown after chase and death.
- Never allow Listener, Drowned Technician, and Antenna Choir to all peak simultaneously unless scripted and validated.

###### Done tests
- Recent death lowers intensity ceiling.
- Successful objective can raise intensity after grace period.
- Cooldown prevents immediate re-chase.
- Director rejects simultaneous peak hazards when stacking rule forbids it.

- Copy paste ai prompt: Design an Encounter Director for a solo UE5 horror game. No code. Include pressure inputs, intensity outputs, cooldown rules, hazard stacking limits, and tests.

##### 06.02 — Specify spawn and manifestation eligibility

- Id: 06.02
- Title: Specify spawn and manifestation eligibility
###### Classes or assets
- AEncounterDirector
- AListenerManifestationBase
- AListenerFairnessTellActor

###### Small tasks
- Define spawn candidates by zone, line of sight, player facing, distance, and nav reachability.
- Block behind-player spawn unless prior warning tell has fired.
- Require spatial tell or audio tell before close manifestation.
- Rate-limit manifestations.
- Expose director approval API for zone controllers.

###### Done tests
- Spawn behind player is rejected without warning.
- Spawn in front peripheral cone is allowed only above minimum distance.
- Spawn location must be reachable or intentionally non-interactive.
- Repeated manifestation requests are cooldown-limited.

- Copy paste ai prompt: Plan spawn eligibility and manifestation approval for a horror Encounter Director. No implementation. Include behind-player warning rules, line-of-sight constraints, cooldowns, and done-tests.



### 07_drowned_technician_zone_controller

- Phase: 07_drowned_technician_zone_controller
- Goal: Create a cheaper local manifestation with clear zone rules.
#### Tasks

##### 07.01 — Specify Drowned Technician zone behavior

- Id: 07.01
- Title: Specify Drowned Technician zone behavior
###### Classes or assets
- ADrownedTechnicianZoneController
- ADrownedTechnicianManifestation
- UDA_DrownedTechnicianZoneTuning

###### Small tasks
- Define zone triggers: flooded rooms, breaker panels, waterlogged corridors, maintenance tunnels.
- Define manifestation modes: silhouette in water reflection, crawling cable drag, panel knock, short path blocker.
- Keep it cheaper than Listener: limited nav, short lifetime, no full chase unless scripted.
- Emit acoustic stimuli that can bait Listener only when director approves.
- Use water/electric tells before harm.

###### Done tests
- Drowned Technician cannot start full Listener chase by itself without director approval.
- Zone hazard warns before damage or path block.
- Manifestation lifetime expires cleanly.
- It cannot spawn inside the player capsule or immediate camera blind spot without tell.

- Copy paste ai prompt: Design the Drowned Technician zone controller for a UE5 horror game. No code. Include zone triggers, cheap manifestation modes, Listener bait limits, water/electric tells, and done-tests.

##### 07.02 — Specify Drowned Technician counterplay

- Id: 07.02
- Title: Specify Drowned Technician counterplay
###### Classes or assets
- ADrownedTechnicianZoneController
- AListenerDoorActor
- Interactables

###### Small tasks
- Define counterplay: cut breaker, avoid water, crouch through cable area, throw object to misdirect, close insulated door.
- Mark safe traversal hints with readable environmental art hooks.
- Define cooldown after successful counterplay.
- Report counterplay success to Encounter Director.

###### Done tests
- Player has at least one non-damage route through active zone.
- Counterplay success suppresses hazard for configured duration.
- Suppressed hazard cannot immediately retrigger from same tile.
- Environmental tell exists before first forced interaction.

- Copy paste ai prompt: Plan counterplay for the Drowned Technician hazard. No implementation. Include player options, safe traversal hints, suppression cooldown, director reporting, and tests.



### 08_antenna_choir_sector_hazard

- Phase: 08_antenna_choir_sector_hazard
- Goal: Create an area hazard that manipulates sound, perception, and pressure.
#### Tasks

##### 08.01 — Specify Antenna Choir sector rules

- Id: 08.01
- Title: Specify Antenna Choir sector rules
###### Classes or assets
- AAntennaChoirSectorController
- AAntennaChoirManifestation
- UDA_AntennaChoirSectorTuning

###### Small tasks
- Define sector triggers: radio towers, PA rooms, antenna shadows, server aisles.
- Define effects: false directional whispers, stimulus amplification, UI/static distortion, temporary sound masking.
- Never fake a fairness-critical cue without a differentiating tell.
- Amplify player sounds only within visible/readable sector boundaries.
- Allow director to cap hazard intensity after death.

###### Done tests
- Amplification applies only inside active sector bounds.
- Critical Listener chase tell is never fully masked.
- False whispers are tagged non-player-caused unless configured bait is approved.
- After two deaths, sector amplification scale is reduced.

- Copy paste ai prompt: Design the Antenna Choir sector hazard. No code. Include sector triggers, sound amplification, false cues, fairness constraints, death adaptation, and tests.

##### 08.02 — Specify Antenna Choir counterplay

- Id: 08.02
- Title: Specify Antenna Choir counterplay
###### Classes or assets
- AAntennaChoirSectorController
- Interactables
- UDA_AntennaChoirSectorTuning

###### Small tasks
- Define counterplay: retune radio, disable relay, crouch during broadcast pulse, use shielded corridor, wait for pulse gap.
- Expose pulse cadence to Blueprint for synchronized lights/audio.
- Add debug sector overlay for pulse state and amplification value.
- Notify Listener scorer of active amplification.

###### Done tests
- Pulse has readable windup before amplification spike.
- Shielded corridor suppresses sector amplification.
- Relay disable lowers sector intensity.
- Debug overlay shows current sector state.

- Copy paste ai prompt: Plan Antenna Choir counterplay and pulse readability. No implementation. Include player actions, pulse cadence, shielded routes, debug overlay, and done-tests.



### 09_fairness_tells

- Phase: 09_fairness_tells
- Goal: Make every failure understandable before it kills the player.
#### Tasks

##### 09.01 — Specify fairness tell system

- Id: 09.01
- Title: Specify fairness tell system
###### Classes or assets
- UListenerFairnessTellComponent
- AListenerFairnessTellActor
- UDA_FairnessTellTuning
- FFairnessTellRequest

###### Small tasks
- Define tell types: audio sting, directional knock, light flicker, subtitle/radio hint, wet footprint, camera-safe silhouette.
- Define required tells before: behind-player spawn, close stalk, chase start, hazardous sector pulse, damaging manifestation.
- Define lead time, cooldown, spatial origin, and minimum audibility.
- Allow Blueprint presentation but C++ tracks whether tell requirement is satisfied.

###### Done tests
- Behind-player spawn request fails until required tell is satisfied.
- Chase start produces tell event before aggression begins.
- Tell cooldown prevents spam.
- Tell satisfaction expires after configured window.

- Copy paste ai prompt: Design a fairness tell system for The Listener AI. No implementation. Include tell types, required situations, timing rules, Blueprint hooks, C++ truth tracking, and tests.

##### 09.02 — Specify legibility telemetry

- Id: 09.02
- Title: Specify legibility telemetry
###### Classes or assets
- UListenerDebugComponent
- AEncounterDirector

###### Small tasks
- Record why each state transition happened.
- Record which tell justified each dangerous action.
- Record player death cause chain.
- Expose last 10 fairness events in debug UI.

###### Done tests
- Every Chase entry has a recorded cause.
- Every death has a cause chain including last stimulus or hazard.
- Dangerous action without tell emits validation warning.
- Debug UI can show recent fairness events.

- Copy paste ai prompt: Plan legibility telemetry for horror AI failures. No code. Include transition reasons, tell records, death cause chain, debug UI fields, and tests.



### 10_navmesh_and_doors

- Phase: 10_navmesh_and_doors
- Goal: Guarantee navigation supports fair chase and readable obstruction.
#### Tasks

##### 10.01 — Specify navmesh validation for chase spaces

- Id: 10.01
- Title: Specify navmesh validation for chase spaces
###### Classes or assets
- UListenerNavigationComponent
- AListenerEscapeRouteMarker
- UDA_NavDoorTuning

###### Small tasks
- Define chase spaces and route markers.
- Validate at least two viable routes from chase start zones.
- Classify route states: open, door-blocked, crawl-only, hazard-risk, temporarily locked.
- Expose editor validation commandlet or automation test map pass.
- Mark invalid chase starts as unavailable to director.

###### Done tests
- Every chase start marker has two viable escape routes.
- Routes blocked by locked doors do not count.
- Openable doors count only if interaction time is fair.
- Invalid route marker emits editor validation error.

- Copy paste ai prompt: Design navmesh and chase-route validation for The Listener. No implementation. Include route marker classes, route viability rules, door classification, editor validation, and tests.

##### 10.02 — Specify door interaction rules

- Id: 10.02
- Title: Specify door interaction rules
###### Classes or assets
- AListenerDoorActor
- UListenerDoorInteractionComponent
- BTTask_UseDoorOrBreakLine

###### Small tasks
- Define door states: open, closed, locked, barricaded, noisy, jammed.
- Define AI traversal: open quietly, shove, wait, route around, lose interest.
- Define player affordance: close door to buy time, barricade to redirect, noisy open creates stimulus.
- Prevent doors from creating single-route death traps unless scripted with warning.

###### Done tests
- Closing a door during chase buys configured minimum time.
- Noisy door emits acoustic stimulus.
- AI can route around if alternate path exists.
- Door cannot lock both escape routes during active chase unless scripted safe sequence is active.

- Copy paste ai prompt: Plan Listener door interaction rules for fair horror chases. No code. Include door states, AI traversal choices, player affordances, acoustic emissions, anti-softlock tests.



### 11_death_and_retry_adaptation

- Phase: 11_death_and_retry_adaptation
- Goal: Reduce frustration while preserving dread.
#### Tasks

##### 11.01 — Specify death cause recording

- Id: 11.01
- Title: Specify death cause recording
###### Classes or assets
- FDeathRetryProfile
- AEncounterDirector
- UListenerDebugComponent

###### Small tasks
- Record death source: Listener chase, Drowned Technician, Antenna Choir, environmental, unknown.
- Record preceding state chain and last three stimuli.
- Record route availability at death.
- Record whether fairness tell fired.

###### Done tests
- Death record includes source and state chain.
- Death record includes whether two routes were available.
- Death from Listener includes last known chase context.
- Unknown death source emits debug warning.

- Copy paste ai prompt: Design death cause recording for a UE5 horror AI system. No implementation. Include fields, source taxonomy, fairness data, route data, and tests.

##### 11.02 — Specify retry scaling after repeated deaths

- Id: 11.02
- Title: Specify retry scaling after repeated deaths
###### Classes or assets
- UDA_DeathRetryTuning
- AEncounterDirector
- UListenerBrainComponent

###### Small tasks
- After first death, preserve intended tuning but add clearer tell on retry.
- After second death, reduce Listener persistence, search duration, sector amplification, and manifestation frequency.
- After success or checkpoint progress, gradually restore baseline.
- Never announce difficulty reduction diegetically unless desired by design.

###### Done tests
- Two deaths reduce chase persistence scale below baseline.
- Two deaths reduce Antenna Choir amplification if it contributed.
- Retry scaling resets upward after objective progress.
- Retry scaling does not disable core mechanics completely.

- Copy paste ai prompt: Plan death/retry adaptation for The Listener. No code. Include first-death tell increase, second-death persistence reduction, restoration after success, and tests.



### 12_debugging_and_visualization

- Phase: 12_debugging_and_visualization
- Goal: Make AI decisions inspectable in editor and builds.
#### Tasks

##### 12.01 — Specify debug overlay

- Id: 12.01
- Title: Specify debug overlay
###### Classes or assets
- UListenerDebugComponent
- AEncounterDirector
- UAcousticStimulusSubsystem

###### Small tasks
- Show current Listener state and transition reason.
- Show active stimulus scores above threshold.
- Show director intensity and cooldowns.
- Show fairness tell requirements and satisfaction windows.
- Show valid/invalid chase routes.
- Show death retry tier.

###### Done tests
- Debug overlay updates when state changes.
- Stimulus score list sorts by final score.
- Invalid chase route is visibly marked.
- Overlay can be toggled without affecting gameplay state.

- Copy paste ai prompt: Design a debug overlay for The Listener AI. No implementation. Include displayed fields, sorting, route visualization, fairness windows, director state, and tests.

##### 12.02 — Specify logging and replay hooks

- Id: 12.02
- Title: Specify logging and replay hooks
###### Classes or assets
- UListenerDebugComponent
- FListenerStateSnapshot
- FEncounterPressureSample

###### Small tasks
- Define structured log categories.
- Log state transitions, stimulus arbitration, fairness rejections, spawn approvals, chase starts, retreats, deaths.
- Define lightweight replay markers for QA.
- Add CSV export option for playtest analysis.

###### Done tests
- State transition log includes old state, new state, reason, and time.
- Fairness rejection log includes rejected action and missing requirement.
- Death log can be correlated to previous chase start.
- CSV export contains one row per major AI event.

- Copy paste ai prompt: Plan structured logging for Listener AI QA. No implementation. Include log categories, event fields, replay markers, CSV export, and tests.



### 13_testing_plan

- Phase: 13_testing_plan
- Goal: Define test-first acceptance before implementation begins.
#### Tasks

##### 13.01 — Create automation test matrix

- Id: 13.01
- Title: Create automation test matrix
###### Classes or assets
- AutomationSpec_ListenerSoundScoring
- AutomationSpec_ListenerStates
- AutomationSpec_EncounterDirector
- AutomationSpec_Fairness
- AutomationSpec_NavRoutes
- AutomationSpec_DeathRetry

###### Small tasks
- Write tests before implementation.
- Group tests by subsystem.
- Keep scoring tests deterministic.
- Use fake world/nav fixtures only where full map tests are too slow.
- Add map validation tests for chase spaces.

###### Done tests
- Every core subsystem has at least one failing test before code starts.
- State tests cover every transition.
- Fairness tests cover behind-player spawn, chase start, and hazard damage.
- Nav tests cover two-route requirement.
- Death retry tests cover two-death persistence reduction.

- Copy paste ai prompt: Create the test-first matrix for The Listener AI implementation. No production code. Include UE automation spec names, behavior cases, map validation cases, and red-green order.

##### 13.02 — Create manual QA scenarios

- Id: 13.02
- Title: Create manual QA scenarios
###### Classes or assets
- QA_Listener_TestMap
- QA_DrownedTechnician_TestMap
- QA_AntennaChoir_TestMap

###### Small tasks
- Build scenario list for patrol-to-investigate.
- Build scenario list for search escape.
- Build scenario list for fair chase with two routes.
- Build scenario list for behind-player warning.
- Build scenario list for two deaths reducing persistence.
- Build scenario list for Drowned Technician and Antenna Choir not stacking unfairly.

###### Done tests
- QA checklist has expected player-readable tell for each failure.
- Each chase scenario documents two escape routes.
- Each hazard scenario documents counterplay.
- Repeated-death scenario confirms lower persistence.

- Copy paste ai prompt: Create a manual QA checklist for The Listener AI and its two manifestations. No implementation. Include scenario setup, expected tells, pass/fail criteria, and notes for two-route and repeated-death fairness.



### 14_integration_order

- Phase: 14_integration_order
- Goal: Sequence implementation safely once planning is approved.
#### Tasks

##### 14.01 — Recommended implementation order

- Id: 14.01
- Title: Recommended implementation order
###### Classes or assets
- All listed systems

###### Small tasks
- Implement type contracts and Data Asset validation first.
- Implement UAcousticStimulusSubsystem second.
- Implement scoring third.
- Implement ListenerBrain transitions fourth.
- Implement Blackboard/BT shell fifth.
- Implement state tasks one state at a time: Patrol, Investigate, Search, Stalk, Chase, Retreat.
- Implement EncounterDirector after base Listener loop works.
- Implement fairness tells before dangerous spawn/chase behavior is enabled.
- Implement nav/door validation before chase tuning.
- Implement Drowned Technician.
- Implement Antenna Choir.
- Implement death/retry adaptation.
- Implement debug overlay and QA maps continuously, not at the end.

###### Done tests
- No dangerous behavior can be enabled before fairness tests pass.
- No chase can be enabled before two-route validation passes.
- No manifestation can be enabled before director spawn approval tests pass.
- No tuning asset can ship with validation errors.

- Copy paste ai prompt: Turn this Listener AI plan into a strict TDD implementation backlog. Do not write production code yet. Preserve C++ truth, Blueprint presentation, Data Asset tuning, fairness tells, two-route chases, and death/retry adaptation.




## Global acceptance criteria
- The Listener can patrol, investigate, search, stalk, chase, and retreat through C++-owned state transitions.
- Sound scoring is deterministic, tunable, and debuggable.
- Behavior Tree executes behavior but does not own authoritative transition rules.
- Encounter Director prevents unfair hazard stacking.
- Drowned Technician and Antenna Choir are cheaper manifestations with clear counterplay.
- No behind-player spawn occurs without prior warning.
- Every chase start validates at least two viable escape routes.
- After two deaths, persistence or hazard intensity is reduced.
- Every player failure can be explained by debug telemetry and an in-world tell.
- Automation and QA tests exist before implementation of each subsystem.

