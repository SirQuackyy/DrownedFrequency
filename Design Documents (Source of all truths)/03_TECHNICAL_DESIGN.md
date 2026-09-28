# The Drowned Frequency — Unreal Engine 5 Technical Design

## 1. Project setup

Create a First Person C++ project with Starter Content disabled. Target Windows PC and DirectX 12. Use a stable UE5 production release and isolate any experimental feature behind a replaceable wrapper.

Enable initially:

- Enhanced Input
- Gameplay Tags
- MetaSound
- AIModule and Navigation System
- UMG; Common UI only if controller/menu complexity justifies it

Enable later:

- Steam integration after save format and packaged builds are stable
- Movie Render Queue for trailer capture

Avoid unless proven necessary:

- Gameplay Ability System
- Mass Entity
- Full open-world World Partition
- Destruction-heavy Chaos systems
- Purchased all-in-one inventory, AI, or horror frameworks

## 2. Source-control policy

For solo development, use Git with LFS from the first commit. Track at minimum:

- `*.uasset`
- `*.umap`
- `*.wav`
- `*.fbx`
- `*.png`, `*.tga`, `*.exr`, `*.psd`
- `*.blend`

Ignore Binaries, DerivedDataCache, Intermediate, Saved, IDE state, and packaged output.

Branches:

- `main`: verified milestone state
- `dev`: normal integration
- `feature/*`: only risky isolated systems
- `release-candidate`: stabilization
- `hotfix/*`: post-release corrections

Tag prototype, vertical slice, alpha, beta, and release candidate. Maintain an off-machine backup in addition to the remote repository.

## 3. Content structure

- `Content/Game/Core`
- `Content/Game/Characters`
- `Content/Game/Environment/Station`
- `Content/Game/Environment/Coast`
- `Content/Game/Props`
- `Content/Game/Audio`
- `Content/Game/UI`
- `Content/Game/Levels`
- `Content/Game/Data`
- `Content/Game/Cinematics`
- `Source/DrownedFrequency`

Naming:

- `BP_` Blueprint
- `DA_` Primary Data Asset
- `DT_` Data Table
- `M_` / `MI_` material / instance
- `MS_` MetaSound
- `WBP_` widget
- `L_` level
- `IA_` input action
- `IMC_` mapping context

## 4. C++ and Blueprint boundary

C++ owns reusable mechanics and persisted truth:

- Interaction tracing/focus
- Recorder state and signal structs
- Inventory/key-item identities
- Sound stimulus events
- Save serialization and versioning
- AI state plumbing
- Player movement extensions
- Subsystems and validation
- Automated tests and debug commands

Blueprint owns authored presentation:

- Room-specific puzzle graphs
- Door, lever, receiver, and speaker variants
- Encounter route selection
- Sequencer moments
- VFX, lighting events, and set dressing
- UI layout
- Audio placement and tuning

Data Assets own tuning and definitions:

- Audio anomalies
- Receiver requirements
- Enemy tuning
- Puzzle configuration
- Interactable definitions
- Difficulty profiles
- Scare events

Do not put chapter progression truth into a Level Blueprint. Level Blueprints trigger presentation; a subsystem stores progression.

## 5. Core gameplay types

### Framework

`UDrownedGameInstance`

- User settings
- Save-slot discovery
- Platform initialization

`ADrownedGameMode`

- Spawn/checkpoint rules
- Death and retry routing

`ADrownedGameState`

- Chapter
- Global power phase
- Storm phase
- Lockdown/final-state flags

`ADrownedPlayerController`

- Mapping contexts
- Menu routing
- Controller glyph selection
- Subtitle/accessibility preferences

`ADrownedPlayerCharacter`

Components:

- Camera
- `UInteractionComponent`
- `URecorderComponent`
- `UKeyItemComponent`
- `UPlayerNoiseComponent`
- `UPlayerStateComponent` for stamina/injury/composure
- `UFootstepComponent`

### Interaction

`IStationInteractable`

- `CanInteract`
- `GetPrompt`
- `BeginFocus`
- `EndFocus`
- `Interact`
- `GetStableSaveId`

`UInteractionComponent`

- Short shape trace from camera
- Cached focused actor
- Configurable trace interval
- Hold interaction support
- Focus presentation events

Avoid universal physics pickup. Use authored inspect/pickup actors.

### Acoustic data

`FRecordedSignal`

- Signal ID
- Category Gameplay Tag
- Fidelity
- Contamination state
- Capture location tag
- Use count
- Last playback timestamp

`UDA_AudioAnomaly`

- Clean/degraded/contaminated audio references
- Capture conditions
- Receiver compatibility tags
- AI stimulus profile
- Subtitle/caption metadata
- Story metadata

`AAudioAnomalyActor`

- Spatial source and capture volume
- State requirements
- Capture-progress presentation
- Stable save ID when persistent

`URecorderComponent`

- Recording mode
- Eligible anomaly query
- Slot management
- Replay and projection request
- Credibility/use tracking
- Blueprint events for presentation

`AAcousticReceiver`

- Required tags and fidelity range
- Rhythm/window configuration
- Puzzle-state output
- Feedback meter and lights

`AAcousticPuck`

- Selected signal
- Placed/throw state
- Loop timer
- Battery state if retained
- Sound stimulus origin
- Recall/reset policy

### Sound stimulus

Use a custom gameplay event even if AI Perception receives a mirrored hearing stimulus.

`FAcousticStimulus`

- World position
- Loudness
- Category tag
- Fidelity
- Repetition signature
- Duration
- Occlusion coefficient
- Instigator type
- Surface tag

`UAcousticWorldSubsystem`

- Broadcasts stimuli
- Maintains short-lived recent-event ring buffer
- Applies sector acoustic modifiers
- Feeds the Listener and encounter director
- Provides debug visualization

This prevents gameplay truth from depending entirely on opaque audio playback behavior.

## 6. AI architecture

### Classes

- `AListenerCharacter`
- `AListenerAIController`
- `UEnemySensesComponent`
- `UDA_EnemyTuning`
- Behavior Tree + Blackboard or a compact C++ state machine with Blueprint presentation hooks

### Blackboard/state data

- Current state
- Current stimulus position and score
- Last known player position
- Suspicion
- Current route ID
- Clip signatures recently heard
- Search timeout
- Can chase flag

### Stimulus score

Compute from authored inputs:

- Loudness after distance and occlusion
- Category interest
- Fidelity/credibility
- Repetition penalty
- Current state
- Player proximity
- Sector modifier

Keep values debuggable on screen. Every detection bug should expose the score and factors.

### Navigation strategy

- One active systemic enemy
- Zone-bounded NavMesh
- Mark specific enemy-compatible doors
- Authored route anchors for scare composition
- Scripted retreat/despawn at zone boundaries
- Disable perception and expensive updates outside active sectors
- Never simulate the entire facility continuously

### Drowned Technician

Implement as a zone encounter controller sharing acoustic stimuli:

- Spline or authored submerged route nodes
- Water-surface VFX actor
- Threat state machine
- Grab/death endpoint

Do not build a second general-purpose character AI.

### Antenna Choir

Implement as sector state plus speaker/antenna actors:

- Power graph inputs
- Composure/audio outputs
- False-cue event pool
- Cancellation puzzle

No NavMesh agent required.

## 7. Encounter director

`UEncounterDirectorSubsystem` inputs:

- Time since pressure event
- Player noise history
- Progression gate
- Death count
- Stuck time
- Resource state
- Current safe/teaching flags

Outputs choose only from authored, validated options:

- Scare event data asset
- Listener route ID
- Silence window
- Environmental failure
- Low-cost resource assistance

The director does not spawn arbitrary threats anywhere. It selects among room-authored possibilities and obeys cooldowns.

Required guard flags:

- `IsTeachingNewMechanic`
- `IsPrecisionPuzzleActive`
- `IsSafeRoom`
- `HasAudioForeshadowedThreat`
- `DeathsInEncounter`

## 8. Save system

`UDrownedSaveSubsystem`

- Async save wrapper
- Slot metadata
- Save-version migration
- Actor registration
- Checkpoint snapshots

`UDrownedSaveGame` stores:

- Version
- Chapter/checkpoint ID
- Player state
- Recorded signals and use counts
- Key items
- Objective flags
- Door/puzzle/power/pump states
- Destroyed/preserved recordings
- Ending variables
- Playtime and display metadata

`ISaveableActor`

- Stable editor-assigned GUID or FName
- Compact state struct
- Apply-after-level-load callback

Load order:

1. Load persistent level and required sector
2. Initialize subsystems
3. Apply global state
4. Apply registered actor state
5. Restore player at checkpoint
6. Resume objective and director cooldown

Tests must cover loading after every required major puzzle and during each legal power configuration.

## 9. Level architecture

Use a traditional persistent level with streamed sublevels or Level Instances.

Persistent:

- Exterior shell/sky/storm
- Facility connector shell
- Global managers
- Operations hub when needed

Streamed sectors:

- `L_Exterior_Coast`
- `L_Station_Admin`
- `L_Station_Dormitory`
- `L_Station_Maintenance`
- `L_Station_Hydrophone`
- `L_Station_DeepArray`
- `L_Station_Final`

Hide streaming behind:

- Bulkhead transitions
- Stairwells and elevators
- Bent corridors
- Heavy doors
- Fog and darkness with real occluders

Package and test streaming early. Editor-only smoothness is a charming liar.

## 10. Rendering and atmosphere

### Lumen

Use for dynamic GI/reflections and rapid power-state iteration. Build scalability fallbacks reducing GI/reflection quality. Profile wet reflective corridors during the vertical slice.

### Nanite

Use for:

- Static architecture
- Rocks/cliffs
- Large machinery
- High-detail static hero props

Avoid assuming it solves translucent water/glass, skeletal enemies, or every tiny prop.

### Lighting

- One readable key-light idea per room
- Darkness is selective, not uniform
- Emergency lighting creates navigation language
- Limit simultaneous movable shadow-casting lights
- Flashlight interaction must be profiled with VSM
- Constrain eye adaptation so doorways and interactables remain readable

### Fog and particles

- Exponential Height Fog for baseline
- Sparse local fog or Niagara cards for pockets
- Volumetric fog reserved for hero views
- Dust/condensation vibration communicates sound direction
- Fog never substitutes for composition or collision

### Post process

- Restrained grain and desaturation
- Chromatic aberration only during anomaly events
- Recorder material effect tied to capture state
- No permanent smeared “horror filter”

## 11. Audio architecture

Sound classes:

- Master
- SFX
- Ambience
- Threat
- Voice
- Music
- UI

Submixes:

- Master
- Recorder playback: band-pass, saturation, variable degradation
- Flooded/underwater
- Composure/fear

MetaSound uses:

- Storm intensity layers
- Station hum by power phase
- Recorder motor/static
- Randomized hydrophone pings
- Listener proximity texture
- Clip degradation parameters

Music remains sparse. Threat readability comes from diegetic sound before score.

Every critical audio clue must have caption metadata and optional waveform visualization. Test in stereo speakers, headphones, and mono-compatible mode.

## 12. Asset strategy

Buy or source consistently:

- One primary modular industrial interior kit
- Coastal cliffs/rocks
- Utility props, pipes, cable, lockers, desks
- Rust, leak, grime, and signage decal foundations
- Foley and ambience libraries
- Creature base mesh/animations only with modification-friendly commercial licensing

Create custom:

- Field recorder hero prop and UI
- Northpoint signage, maps, and labels
- Hydrophone machinery
- Archive core/final signal device
- Story documents
- Enemy silhouette and material treatment
- Signal Cathedral hero composition

Selection criteria:

- Consistent scale and texel density
- Parameterized PBR materials
- Acceptable collision
- No mandatory runtime dependency for basic art
- Commercial Steam license
- Visual compatibility after shared materials, decals, and lighting

## 13. Performance budget

Target stable 60 FPS on a defined mid-range Windows machine at 1080p/1440p with appropriate scalability presets.

CPU:

- No broad per-frame Blueprint polling
- Interaction trace at bounded frequency
- One active primary enemy
- Disable AI outside active zones
- Async or bounded save work outside danger

GPU:

- Limit shadow-casting movable lights per view
- Profile Lumen in dark wet rooms
- Use volumetrics selectively
- Control translucency overdraw
- Avoid stacked continuous full-screen effects

Memory:

- Do not use 4K textures universally
- Stream long ambience and music
- Unload inaccessible sectors
- Set importance-based texture sizes

Profiling tools:

- Unreal Insights
- Stat Unit/Game/GPU
- GPU Visualizer
- MemReport

Record baselines for every milestone and test packaged builds, not only PIE.

## 14. Testing

Automated or functional coverage:

- Signal slot add/delete/reacquire
- Receiver compatibility
- Interaction focus transitions
- Save serialization/version migration
- Door/puzzle persistence
- Map-load smoke tests
- Data Asset validation for missing audio/captions/tags

Manual passes:

- New game to first checkpoint
- Each chapter from clean checkpoint
- Full-game start-to-finish
- Controller-only
- Keyboard/mouse
- Subtitle/waveform-only audio-clue pass
- Low scalability
- Save/load after every major puzzle
- Windowed/fullscreen/alt-tab
- Clean-machine packaged build

Severity order:

1. Progression blocker or corrupt save
2. Player stuck or invalid checkpoint
3. Unfair AI detection or unreadable critical audio
4. Performance spike
5. Presentation defect

## 15. Shipping

Integrate Steam only after core loop, save format, and packaged build are stable.

Initial Steam scope:

- Launch configuration
- Achievements
- Cloud saves
- Controller metadata

Do not add Workshop, leaderboards, or multiplayer.

Release checks:

- Correct startup/default maps
- Only required maps cooked
- Redirectors fixed
- Input defaults verified
- Scalability defaults verified
- Save directory and cloud behavior verified
- Clean installation tested
- Screenshots captured from packaged build
- Minimum requirements based on real profiling
- Content warnings included