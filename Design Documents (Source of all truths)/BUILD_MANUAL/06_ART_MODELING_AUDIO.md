# Art, Modeling, Materials, VFX, Lighting, and Audio

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

- Title: The Drowned Frequency Production Manual
- Version: 1.0
- Scope: Granular environment, art, modeling, materials, VFX, lighting, animation, audio, and optimization production manual for a storm-bound coastal hydroacoustic station built in UE5 with Lumen, Nanite, Virtual Shadow Maps, and scalable fallbacks.
## Locked visual identity

- Core pitch: A storm-bound coastal hydroacoustic research station where industrial ocean infrastructure, Cold War archive systems, analog signal equipment, and impossible acoustic phenomena converge.
### Must feel like
- Salt-corroded coastal science facility
- Hydrophone machinery and signal infrastructure
- Wet concrete, steel, glass, cable trays, archival paper, amber warning lights
- Storm pressure, low visibility, ocean impact, leaking interiors
- Sound is physical, architectural, and threatening
- The Signal Cathedral is sacred-industrial, not fantasy-magical
- The enemy silhouette is readable, uncanny, and tied to pressure, water, and signal distortion

### Avoid
- Generic abandoned asylum
- Clean sci-fi lab
- Pirate/shipwreck aesthetic
- Overdesigned horror gore
- Magic crystals unless treated as acoustic/pressure phenomena
- Asset-store kitbash with no custom identity layer

### Hero custom assets
- Custom field recorder/player device
- Station signage, maps, labels, and emergency diagrams
- Hydrophone machinery and winch/control equipment
- Archive core
- Creature silhouette
- Signal Cathedral architectural centerpiece

### Buyable generic assets
- Concrete walls/floors if restyled
- Industrial props
- Cables, pipes, valves, ducts
- Generic office/lab furniture
- Rocks, cliffs, coastline set dressing
- Rain decals, puddle materials, grime atlases
- Utility lights, doors, railings, stairs
- Generic animation packs for humanoid locomotion


## Production principles

### Graybox first when
- A room's gameplay role is not proven
- Sightline, hiding, pursuit, or navigation readability is uncertain
- Player scale, door width, stair pitch, crawlspace dimensions, or interaction reach affects play
- The space depends on audio propagation, occlusion, or spatial reveals
- The hero prop footprint affects interaction timing or framing
- The storm exterior composition is still being blocked for silhouette and navigation

### Do not model final art until
- Player path and camera framing are stable
- Interaction beats are timed
- Sound occlusion zones are tested
- Lighting contrast and fog readability are acceptable
- Hero asset scale is locked in-engine
- Collision needs are known

### Buy assets when
- The asset is background or repeated infrastructure
- It does not carry story identity
- It can be restyled with wet materials, decals, signage, and layout
- It saves time without compromising silhouette language
- The player will not inspect it closely for narrative information

### Model custom when
- It appears in close-up
- It communicates mechanics or story
- It must match the hydroacoustic identity
- It is used in marketing screenshots
- It has unusual interaction, animation, or readability needs
- Asset-store alternatives would make the game look generic

- Identity layer rule: Every bought or generic module must receive at least two identity passes: material/weathering pass and station-specific labeling/cabling/signage pass.

## Scale and metrics

- Engine units: 1 Unreal Unit = 1 cm
### Human reference

- Standing height cm: 180
- Eye height cm: 165
- Door height cm: 210
- Door width standard cm: 90
- Door width gameplay cm: 100
- Corridor min width cm: 140
- Corridor comfort width cm: 180
- Stair rise cm: 17
- Stair run cm: 28
- Handrail height cm: 95
- Desk height cm: 75
- Console height cm: 100

### Module grid

- Primary grid cm: 100
- Secondary grid cm: 50
- Fine grid cm: 10
- Wall module width cm: 400
- Wall module height cm: 300
- Floor tile cm: 400
- Ceiling module cm: 400
- Corridor module length cm: 400

### Blender setup
- Set Unit System to Metric
- Set Unit Scale to 0.01 only if using centimeter-style export workflow, otherwise keep meters and verify FBX scale
- Use a 180 cm mannequin proxy in every hero prop scene
- Apply transforms before export
- Use real-world dimensions for doors, consoles, machinery, and signage


## Folder and naming conventions

### Unreal content layout

- Environment: Station kits, cliffs, coastline, modular pieces
- HeroProps: Recorder, hydrophones, archive core, Signal Cathedral elements
- Materials: Master materials, instances, functions, wetness layers
- VFX: Niagara systems, fog cards, rain, sparks, drips
- Audio: MetaSounds, SoundWaves, SoundCues, submixes, impulse responses
- Lighting: Lighting scenarios, LUTs, post-process volumes
- Blueprints: Interactables, doors, machinery controllers, audio emitters
- Decals: Leaks, rust, salt, labels, maps, hazard markings
- Animations: Creature, machines, first-person hand/device animations

### Asset prefixes

- SM: Static Mesh
- SK: Skeletal Mesh
- M: Master Material
- MI: Material Instance
- MF: Material Function
- T: Texture
- NS: Niagara System
- NE: Niagara Emitter
- BP: Blueprint
- MS: MetaSound
- SC: Sound Cue
- SW: Sound Wave
- PP: Post Process
- LS: Lighting Scenario
- DA: Data Asset

### Example names
- SM_HydrophoneWinch_A_01
- SM_RecorderHero_A_01
- MI_Concrete_WetSalt_A
- NS_ExteriorStormRain_Heavy_A
- MS_HydrophoneDrone_Layered_A
- BP_ArchiveCore_Interactable_A


## Production phases

### Phase 0 - Reference and Identity Lock

- Phase: Phase 0 - Reference and Identity Lock
- Goal: Create visual and audio targets before making assets.
#### Tasks
- Collect references for coastal listening stations, hydrophones, sonar rooms, Cold War archives, ocean weather stations, cable rooms, wet industrial concrete, corroded marine metal, and analog tape/signal gear.
- Create one mood board each for exterior, interior, machinery, archive, creature, Signal Cathedral, and audio-device interaction.
- Define a 12-color material palette: wet concrete, aged steel, oxidized brass, black rubber, salt-white residue, amber glass, red emergency paint, paper tan, green CRT glow, deep ocean blue, storm gray, signal cyan.
- Define lighting contrast target: low-key base, amber/red utility accents, cold lightning pulses, localized device glow.

#### Tests
- Can a reviewer describe the game from the mood board without reading text?
- Do hero custom assets visibly differ from generic industrial horror?
- Are bought assets clearly subordinate to custom station identity?

#### Done criteria
- Reference boards exist for all major asset groups
- Hero asset list is locked
- Buy-vs-custom list is approved
- Color/material palette is approved
- One target screenshot paintover or reference composite exists per main zone

#### Pitfalls
- Starting final models before identity lock
- Letting marketplace asset style define the game
- Overusing sci-fi emissive panels
- Making the station too clean or too abandoned

- Copy paste prompt: Create a visual development brief for a storm-bound coastal hydroacoustic station horror game. Focus on wet concrete, marine corrosion, hydrophone machinery, analog signal equipment, archival rooms, storm exteriors, and sacred-industrial acoustic architecture. Separate custom hero assets from generic buyable assets. Include material palette, lighting palette, silhouette language, and anti-references.

### Phase 1 - Graybox

- Phase: Phase 1 - Graybox
- Goal: Prove layout, scale, navigation, interaction timing, horror reveals, and audio propagation before art production.
#### Tasks
- Block rooms with simple BSP or primitive static meshes on a 100 cm grid.
- Add mannequin scale references at doors, consoles, machinery, hiding spots, and Signal Cathedral viewpoint.
- Mark player route, chase route, blocked paths, maintenance shortcuts, vertical transitions, and reveal points.
- Place temporary audio emitters for storm, machinery, radio signal, creature, and water leaks.
- Use temporary colored materials: playable path, danger, locked, interactable, sound source, vista, hidden route.
- Block hero prop footprints with labeled boxes at final approximate scale.
- Add first-pass fog volumes and light blockers to test visibility.
- Record a walkthrough video for review.

#### Tests
- Player can navigate without final art
- Important doors and interactables are visible from intended angles
- Creature silhouette has at least three readable reveal distances: 25 m, 10 m, 3 m
- Storm exterior reads as coastal and dangerous even with primitives
- Audio sources guide or mislead intentionally
- No critical gameplay path depends on decorative detail

#### Done criteria
- All major areas are traversable
- Interaction beats are timed
- Audio zones are placed
- Hero prop footprints are approved
- Graybox receives design signoff before final modeling

#### Pitfalls
- Graybox too pretty and slow to change
- No audio testing until late
- Hallways too narrow for camera and AI
- Not testing fog and darkness early
- Hero props modeled before interaction scale is known

- Copy paste prompt: Generate a graybox task list for a UE5 horror level set in a coastal hydroacoustic station. Include room metrics, route testing, player scale, audio emitter placement, fog visibility checks, hero prop proxy placement, and design signoff criteria before final art.


## Modular station kit

- Purpose: Build most station interiors from reusable modules while preserving custom identity through decals, materials, cables, signage, and hero inserts.
### Modules to create or adapt

#### Concrete wall straight

- Name: Concrete wall straight
- Size cm: 400x300
- Notes: Use trim sheets and vertex paint wetness.

#### Concrete wall with door

- Name: Concrete wall with door
- Size cm: 400x300
- Notes: Door opening 100x210 cm; include frame sockets.

#### Concrete wall with window/observation glass

- Name: Concrete wall with window/observation glass
- Size cm: 400x300
- Notes: Use thick wet glass, gasket trim, condensation decals.

#### Floor slab

- Name: Floor slab
- Size cm: 400x400
- Notes: Use puddle masks and slope decals.

#### Ceiling slab

- Name: Ceiling slab
- Size cm: 400x400
- Notes: Sockets for pipes, cable trays, lights, vents.

#### Bulkhead corridor arch

- Name: Bulkhead corridor arch
- Size cm: 400 length
- Notes: Use marine/weather-station feel, not spaceship.

#### Stair and landing kit

- Name: Stair and landing kit
- Size cm: 400 grid
- Notes: Readable, collision-simple, wet edges.

#### Railing kit

- Name: Railing kit
- Size cm: 100/200/400 segments
- Notes: Salt corrosion at bases and welds.

#### Pipe and cable tray kit

- Name: Pipe and cable tray kit
- Size cm: 100/200/400 segments
- Notes: Bought assets acceptable if restyled and snapped cleanly.

#### Door kit

- Name: Door kit
- Size cm: 100x210 opening
- Notes: Heavy utility doors, pressure doors, archive doors, maintenance hatches.


### Adaptation workflow for bought kits
- Import original asset into a quarantine folder.
- Check scale against UE mannequin.
- Remove or hide pieces that contradict visual identity.
- Create material instances using project wet/corrosion master materials.
- Add decals: station ID, warning labels, waterline stains, salt residue, rust streaks.
- Add cable trays, hydrophone conduits, and signal tags.
- Create simplified collision if marketplace collision is excessive.
- Create LODs or enable Nanite depending on mesh type.
- Move approved adapted assets into production folder with project naming.

### Tests
- Modules snap on grid with no light gaps
- Door openings match gameplay metric
- Repeated walls do not look copy-pasted after decal/material variation
- Collision is simple and stable
- Nanite meshes do not use inappropriate masked materials

### Done criteria
- Core station kit covers at least 80 percent of interior construction
- All modules have pivots placed for grid snapping
- All materials are project instances
- All collisions reviewed
- All modules tested in one kit demo map

- Copy paste prompt: Create a modular environment kit plan for a storm-bound hydroacoustic station in UE5. Include module dimensions, pivots, snapping grid, collision type, Nanite eligibility, material instance strategy, decal variation, and tests for kit assembly.

## Custom hero prop workflows

### Common blender pipeline

#### Blockout
- Start from approved in-engine proxy dimensions.
- Use simple primitives and bevels to establish silhouette.
- Keep separate logical parts: casing, handles, knobs, screens, cables, bolts, labels.
- Export blockout to UE early and test scale in hand/player view.

#### Scale
- Use real dimensions in centimeters.
- Place mannequin or hand proxy in Blender.
- Test interaction distance and camera framing in UE before high poly.

#### Topology
- Use clean quads for deforming or animated parts.
- Use weighted normals and bevel modifiers for hard surface props.
- Avoid tiny geometry for labels that should be texture/decal.
- Separate high-poly detail from low-poly game mesh.

#### Uvs
- Use texel density targets: hero close-up 1024 px/m or higher, medium props 512 px/m, background 256 px/m.
- Straighten UVs for panels and labels.
- Pack with padding appropriate to texture size.
- Use trim sheets for repeated bolts, rails, panels, and edges.

#### Baking
- Bake normal, AO, curvature, thickness, position, and material ID maps.
- Use cage or averaged normals to prevent projection errors.
- Explode bake if parts intersect.
- Check bakes under harsh light before texturing.

#### Textures
- Use PBR metal/roughness workflow.
- Author base color, roughness, metallic, normal, AO, emissive if needed.
- Add layered salt, corrosion, water streaks, hand grime, chipped paint, paper labels.
- Use decals for story-specific text when localization or iteration is likely.

#### Lod nanite
- Enable Nanite for dense static hard-surface hero props if they do not rely on masked materials, skeletal deformation, or heavy WPO.
- Use manual LODs for interactable, skeletal, translucent, masked, or animated props.
- Keep LOD0 for close-up, LOD1 for gameplay distance, LOD2 for room distance.
- Use impostors only for distant exterior machinery or coastline pieces.

#### Collision
- Use UCX collision meshes for complex interactables.
- Use simple boxes/capsules for blocking and pickup props.
- Never use complex collision for frequently interacted dynamic props unless justified.
- Test player movement, trace hits, physics, and interaction prompts.

#### Export
- Apply transforms.
- Set origin/pivot intentionally: bottom center for placed props, hinge for doors, grip for handheld items, center of mechanism for rotating parts.
- Export FBX with selected objects only.
- Name mesh pieces and materials cleanly.
- Export collision meshes with UCX_ prefix if needed.

#### Import to unreal
- Verify scale immediately with mannequin.
- Assign project material instances.
- Enable Nanite only when appropriate.
- Set collision complexity intentionally.
- Generate or import LODs.
- Set lightmap UVs only if using baked fallback/static lighting.
- Add sockets for VFX, audio, hand placement, cables, or interaction points.


### Recorder device

- Role: Player-facing custom recorder/player, central interaction prop and brand identity asset.
#### Design requirements
- Readable silhouette in first-person
- Analog tactile controls: tape wheel, frequency dial, hydrophone input jack, worn speaker grille, small waveform display
- Storm-proof/rubberized field equipment feel
- Labels tied to station nomenclature
- Can emit light/audio feedback without looking sci-fi

#### Modeling steps
- Blockout in Blender as boxy handheld object approximately 22x14x6 cm.
- Add handle, rubber corner guards, large tuning dial, record/play buttons, speaker grille, display recess, cable port.
- Export blockout to UE and test first-person framing.
- Create high-poly with bevels, screws, seams, molded rubber, worn metal inserts.
- Create low-poly preserving silhouette and control shapes.
- Bake normal/AO/curvature from high to low.
- Texture with black rubber, scratched painted metal, amber glass, worn labels, salt residue around seams.
- Create material slots for body, glass/display, rubber, labels, emissive display.
- Add sockets: AudioEmitter, ScreenGlow, HandGrip_R, HandGrip_L, CableAttach.
- Import to UE and make BP_RecorderHero with interaction states.

#### Tests
- Readable in first-person at normal FOV
- Controls identifiable at 1 m
- Labels legible where intended
- No texture swimming or bake artifacts
- Audio/light feedback syncs with interaction state
- Collision does not block camera or hands

#### Done criteria
- Approved UE first-person screenshot
- Final mesh imported with materials
- Sockets named and tested
- LOD/Nanite decision documented
- Interaction blueprint uses prop without scale hacks

#### Pitfalls
- Too many tiny buttons
- Modern smartphone silhouette
- Display too clean/sci-fi
- No readable grip/handle
- Text baked too early when labels may change

- Copy paste prompt: Design a Blender production breakdown for a handheld analog hydroacoustic recorder for a UE5 horror game. Include dimensions, silhouette, controls, blockout, high poly, low poly, UVs, baking, PBR texture layers, sockets, collision, export settings, UE import settings, tests, done criteria, and pitfalls.

### Station signage maps

- Role: Narrative identity layer and navigation system.
#### Design requirements
- Custom station name, sector codes, hydrophone array maps, emergency evacuation plans
- Aged marine-industrial graphic design
- Readable at gameplay distance
- Reusable decal/sign system

#### Modeling steps
- Create sign blanks as simple planes or thin beveled boards in Blender.
- Use reusable sizes: A4, A3, wall map 100x70 cm, door plate 30x10 cm, hazard strip 100x15 cm.
- Create UVs with straight islands.
- Author signs in vector/design software or texture tool.
- Use material instances for paper, enamel metal, plastic laminate, backlit glass.
- Use decals for wall-painted markings and floor arrows.
- Import signs as static meshes or decals depending on usage.

#### Tests
- Critical navigation text readable at expected distance
- Noncritical flavor text does not distract
- Signs match station naming and map layout
- Decals do not z-fight
- Localization-sensitive text is not unnecessarily baked into geometry

#### Done criteria
- Complete signage style sheet
- Reusable sign mesh set
- Core maps and warning signs implemented
- All gameplay-critical signs tested in level lighting

#### Pitfalls
- Using generic English warning decals from marketplace
- Too much tiny lore text
- Overly clean typography
- Signs that do not match actual level layout

- Copy paste prompt: Create a signage and map production plan for a fictional coastal hydroacoustic station. Include sign sizes, materials, typography direction, station sector codes, hydrophone array diagrams, emergency maps, decal usage, readability tests, localization concerns, and UE implementation.

### Hydrophone machinery

- Role: Primary industrial identity: winches, cable drums, signal racks, hydrophone control systems.
#### Design requirements
- Looks mechanically plausible
- Combines marine winch hardware with signal analysis equipment
- Supports animation, audio, sparks, vibration, and warning lights
- Strong silhouette from room distance

#### Modeling steps
- Blockout winch base, cable drum, motor housing, control console, cable guides, hanging hydrophone element.
- Use dimensions around 250x180x220 cm for large room machinery.
- Separate animated parts: drum, belt, lever, indicator lights, cable.
- Create high-poly bevels and bolts where close to player.
- Use trims for repeated metal edges and panels.
- Use spline meshes in UE for cables where possible.
- Texture with painted steel, bare worn edges, grease, salt bloom, rust at joints, wet streaks.
- Add sockets for Niagara sparks, dripping water, audio emitters, rotating components.

#### Tests
- Machine function understandable from silhouette
- Animated parts pivot correctly
- Cable routing does not clip during animation
- Player collision blocks only where expected
- Audio syncs with motion
- Readable warning lights in fog

#### Done criteria
- Machinery imported as modular static/skeletal components
- Animation-ready pivots verified
- Sockets tested
- Wet/corrosion material instances assigned
- Gameplay interaction traces hit correct controls

#### Pitfalls
- Making random pipes with no apparent function
- Baking moving parts into one mesh
- Overdense bolts instead of trim/normal detail
- No space for player navigation around machine

- Copy paste prompt: Break down production of a large hydrophone winch and signal-control machine for UE5. Include plausible mechanical parts, Blender blockout, animated pivots, high-poly detail, low-poly mesh, UVs, bake maps, wet marine-industrial materials, sockets for VFX/audio, collision, export/import, tests, and done criteria.

### Archive core

- Role: Narrative and visual focal point combining records, analog storage, and signal memory.
#### Design requirements
- Feels like a physical archive, not a generic server room
- Mixes card catalogs, tape reels, waterproof document bins, acoustic logs, and central listening terminal
- Supports reveal lighting and interactable drawers/reels/screens

#### Modeling steps
- Blockout room centerpiece as circular or radial archive structure.
- Use modular shelves, drawers, tape racks, map tables, cable bundles, and central terminal.
- Model custom archive terminal and key drawers; buy/adapt generic shelves if needed.
- Create paper/card/tape material set with damp edges and mold.
- Use decals for labels, archive codes, water stains, and redacted documents.
- Add sockets for paper flutter VFX, dust/fog, screen glow, and low-frequency audio emitters.

#### Tests
- Player instantly reads archive as information space
- Interactable drawers/terminals stand out without UI dependency
- Wet/damp damage feels localized and plausible
- Performance remains stable with many repeated objects
- Lighting guides eye to central interaction

#### Done criteria
- Archive centerpiece built from modular and custom pieces
- Interactable elements are distinct
- Material variation prevents repetition
- Fog/audio/light reveal tested

#### Pitfalls
- Looks like office storage
- Too many unique paper meshes
- Illegible labels
- No central focal object

- Copy paste prompt: Create a production plan for an archive core in a storm-bound hydroacoustic station. Include modular shelves, custom terminal, tape reels, maps, damp paper materials, decals, interaction points, lighting reveal, fog/audio emitters, optimization for repeated documents, tests, and done criteria.

### Signal cathedral

- Role: Major setpiece where acoustic machinery becomes sacred architecture.
#### Design requirements
- Sacred-industrial scale
- Built from hydrophone arrays, cable conduits, resonance chambers, suspended transducers, archive/signal architecture
- Not fantasy temple; all forms should imply sound, pressure, water, and machinery
- Strong vertical silhouette and central vanishing point

#### Modeling steps
- Graybox composition first using cylinders, ribs, suspended forms, cable bundles, and central aperture.
- Test player approach, reveal angle, fog density, and silhouette lighting in UE before detailed modeling.
- Model repeating ribs/modules as kit pieces.
- Use custom hero centerpiece: suspended hydrophone array or resonant core.
- Use bought pipes/cables only as secondary dressing.
- Create layered materials: wet concrete, corroded brass/steel, black cable rubber, emissive signal glass.
- Add Niagara sockets for mist columns, water drips, signal motes, electrical arcs, dust, and pressure ripples.
- Use spatial audio emitters arranged vertically and radially.

#### Tests
- Recognizable from single screenshot
- Silhouette reads without fog/VFX
- Player understands where to walk
- No visual noise hides interaction target
- Runs within performance budget with VFX active
- Audio makes the room feel physically larger

#### Done criteria
- Graybox setpiece approved
- Modular ribs and centerpiece imported
- Lighting and fog pass complete
- Audio and VFX pass complete
- Optimization pass complete

#### Pitfalls
- Accidentally making a fantasy cathedral
- Too much emissive blue sci-fi
- No plausible acoustic function
- Overusing unique geometry where modules would work
- Centerpiece not visible from entrance

- Copy paste prompt: Design a UE5 production plan for a Signal Cathedral setpiece: a sacred-industrial hydroacoustic chamber inside a storm-battered station. Include graybox composition, modular ribs, suspended hydrophone centerpiece, Blender modeling workflow, materials, fog, Niagara, lighting, spatial audio, optimization, tests, done criteria, and pitfalls.

### Creature silhouette

- Role: Enemy identity and horror readability.
#### Design requirements
- Readable silhouette at long, medium, and close range
- Suggests pressure, drowning, signal distortion, and deep-sea adaptation
- Can be partially hidden by fog, rain, glass, and darkness
- Animation supports unnatural but physically grounded motion
- Avoids generic zombie, fish monster, or tall pale humanoid cliché

#### Modeling steps
- Start with 10 silhouette thumbnails before sculpting.
- Choose one primary readable shape language: elongated pressure-suit bulk, cable-like appendages, collapsed diver anatomy, or hydrophone-mimic body.
- Block in Blender/ZBrush using simple masses only.
- Test silhouette in UE with black material, fog, backlight, and storm rain.
- Retopologize if skeletal animation is needed.
- Use clean deformation loops at shoulders, hips, spine, neck, jaw, and appendages.
- UV for close-up readability: skin/suit/cable/signal membranes.
- Texture with wet rubber, pressure-wrinkled skin, barnacle-like damage, corroded metal fragments, subtle emissive signal organs if needed.
- Rig with extra bones for trailing cables, breathing sacs, jaw, or antennae.
- Create LODs manually; skeletal Nanite is not the default production path.
- Use physics assets for ragdoll and collision capsules for gameplay.

#### Tests
- Silhouette recognizable at 25 m in fog
- Not confused with station machinery
- Close-up detail supports the same identity as far silhouette
- Animation does not destroy silhouette
- Hit/collision capsules match gameplay expectations
- Does not look like a marketplace monster after material pass

#### Done criteria
- Approved silhouette sheet
- UE fog/backlight silhouette test
- Rigged skeletal mesh imported
- At least idle, locomotion, turn, investigate, attack/reach, and retreat animations working
- Audio/VFX attachment sockets tested

#### Pitfalls
- Detailing before silhouette approval
- Too humanoid or too fishlike
- Thin appendages unreadable in darkness
- Overbright emissive parts reducing fear
- No animation test until after final sculpt

- Copy paste prompt: Create a creature production plan for a UE5 storm-bound hydroacoustic horror game. Focus on silhouette-first design, pressure/water/signal identity, Blender sculpt/blockout, retopology, UVs, baking, wet materials, rigging, animation adaptation, LODs, physics asset, collision, fog/backlight tests, done criteria, and pitfalls.


## Materials

### Master materials

#### M_Master_WetSurface

- Name: M_Master_WetSurface
##### Features
- Base color/normal/roughness/metallic inputs
- Wetness scalar reducing roughness and darkening base color
- Puddle mask using vertex color, height blend, or texture mask
- Water streak normal overlay
- Salt residue mask
- Distance-based macro variation


#### M_Master_CorrodedMetal

- Name: M_Master_CorrodedMetal
##### Features
- Paint layer
- Bare metal exposed edges
- Rust/salt corrosion masks
- Edge wear from curvature map
- Oil/grease darkening
- Wetness response


#### M_Master_ConcreteMarine

- Name: M_Master_ConcreteMarine
##### Features
- Concrete base
- Cracks and aggregate normal
- Waterline staining
- Mold/algae darkening
- Salt bloom
- Vertex paint blend for leak paths


#### M_Master_GlassWet

- Name: M_Master_GlassWet
##### Features
- Condensation mask
- Rain streak normals
- Interior grime
- Low opacity/transmission fallback
- Cracked glass decal compatibility


#### M_Master_PaperDamp

- Name: M_Master_PaperDamp
##### Features
- Fiber normal
- Edge darkening
- Mold spots
- Water stains
- Decal or texture text overlay



### Wet material rules
- Wet surfaces are darker and glossier but not mirror-polished everywhere.
- Use roughness variation, not uniform shine.
- Puddles form in depressions, floor edges, under leaks, and near exterior entrances.
- Salt residue gathers on edges, seams, lower walls, and evaporated water paths.
- Rust follows gravity, bolts, scratches, welds, and pooling zones.
- Interior wetness should tell where water enters: doors, cracks, ceiling leaks, broken windows, cable penetrations.

### Texture budget guidance

- Hero props: 2K to 4K depending on close-up use; prefer material layering and decals over too many unique maps
- Modular walls: Tileables plus trim sheets; 1K to 2K material sets
- Small props: 512 to 1K unless inspected
- Signage: Use atlases for repeated signs; unique maps only for critical maps/documents
- Creature: 2K to 4K for main body depending on camera proximity

### Tests
- Wetness reads under both flashlight and emergency lighting
- Materials remain readable in fog
- No surface looks uniformly plastic
- Tiling is hidden by decals and macro variation
- Fallback quality acceptable with lower reflections

### Done criteria
- Master materials documented
- Material instances created for all major surfaces
- Wetness variation tested in representative rooms
- Performance cost checked with shader complexity view

- Copy paste prompt: Create UE5 master material specifications for a wet coastal hydroacoustic station. Include wet concrete, corroded metal, wet glass, damp paper, black rubber, salt residue, puddles, vertex painting, decals, shader parameters, texture budget, tests, done criteria, and optimization pitfalls.

## Storm exterior

### Environment goals
- Low-visibility coastal storm
- Station silhouette against ocean and cliffs
- Wind-driven rain and spray
- Wet rock, warning lights, cable towers, antennae, hydrophone deployment cranes
- Ocean is dangerous and present even when not directly reachable

### Asset strategy

#### Buy
- Rocks and cliffs
- Ocean shader/system if production-proven
- Generic rain/splash VFX as base
- Industrial railings/stairs if restyled

#### Custom
- Station exterior facade silhouette
- Hydrophone crane/deployment structure
- Exterior signage and warning lights
- Signal Cathedral visible architectural element if exterior-facing
- Storm-specific hero vista composition


### Implementation steps
- Graybox coastline, station massing, cliff paths, exterior walkways, and key silhouettes.
- Define safe playable area and lethal ocean boundaries.
- Place large-scale wind direction markers for rain, spray, cloth/cables, and audio.
- Use layered fog: distant ocean fog, ground mist, rain haze, interior/exterior transition fog.
- Use wet material instances for all exterior surfaces.
- Use decals for water runoff, salt bands, rust streaks, warning paint, moss/algae.
- Place lightning as timed light events with audio delay variation.
- Use LOD/Nanite for rocks and static architecture; avoid excessive translucent overdraw from rain/fog.

### Tests
- Station silhouette readable in storm
- Player route visible without overbrighting
- Ocean danger is clear
- Lightning supports composition and does not reveal unfinished background
- Rain/fog performance acceptable on target hardware
- Exterior sound sells wind direction and scale

### Done criteria
- Exterior blockout approved
- Hero vista implemented
- Storm VFX and audio layered
- Navigation readable in low visibility
- Scalability settings tested

### Pitfalls
- Too much uniform fog
- Rain visible only in screenshots but not motion
- No wind direction consistency
- Ocean shader dominates performance
- Exterior too generic due to bought rocks/industrial props

- Copy paste prompt: Create a UE5 storm exterior production checklist for a coastal hydroacoustic station. Include coastline graybox, station silhouette, bought vs custom assets, ocean boundary, rain, spray, fog, lightning, wet materials, navigation readability, audio layering, optimization, tests, done criteria, and pitfalls.

## Lighting

### Ue5 primary path
- Use Lumen for dynamic global illumination and reflections.
- Use Virtual Shadow Maps for high-quality dynamic shadows.
- Use Nanite for dense static architecture and rocks where appropriate.
- Use local lights sparingly with strong purpose: emergency red, sodium amber, cold lightning, device glow, CRT/signal green/cyan.
- Use fog and darkness to shape visibility, not to hide all detail.
- Use light functions for rotating beacons, flicker, water-caustic-like motion if justified.

### Fallback path
- Provide scalability tiers for Lumen quality, shadow quality, volumetric fog, reflection quality, VFX density, and rain density.
- Use baked/static or stationary lighting only if level design permits and dynamic storm effects remain acceptable.
- Create non-Lumen reflection captures or SSR fallback where needed.
- Reduce movable light count in lower tiers.
- Use cheaper fog cards where volumetric fog is too expensive.

### Lighting zones

#### 1

- Zone: Exterior storm
- Style: Cold blue-gray ambient, hard lightning, amber safety lights, red beacons

#### 2

- Zone: Operations room
- Style: CRT/signal green, amber panels, dark wet corners, practical overheads

#### 3

- Zone: Archive core
- Style: Warm paper lamps, low red emergency accents, localized terminal glow

#### 4

- Zone: Hydrophone machinery
- Style: Industrial amber, hazard red, sparks, deep shadows behind machinery

#### 5

- Zone: Signal Cathedral
- Style: Sacred vertical shafts, cold mist, pulsing signal glow, silhouette-first


### Tests
- Critical interactables readable without UI dependency
- Creature silhouette readable in intended reveal lighting
- Lighting supports navigation
- No uncontrolled blown-out wet reflections
- Lighting remains acceptable on medium scalability
- Performance captures show light/shadow cost within budget

### Done criteria
- Lighting pass exists per zone
- Post-process volume tuned
- Scalability tiers tested
- Hero screenshots approved
- Performance pass complete

- Copy paste prompt: Create a UE5 lighting production plan for a storm-bound hydroacoustic horror station using Lumen, Nanite, and Virtual Shadow Maps with scalable fallbacks. Include zone palettes, practical lights, lightning, fog integration, creature reveal lighting, wet reflection control, post-process, tests, done criteria, and pitfalls.

## Vfx niagara

### Systems

#### NS_StormRain_Exterior

- Name: NS_StormRain_Exterior
##### Components
- Camera-facing rain streaks
- World rain sheets
- Collision splashes
- Wind gust variation
- LOD density controls


#### NS_OceanSpray_Cliff

- Name: NS_OceanSpray_Cliff
##### Components
- Mist plumes
- Spray bursts
- Foam particles
- Wind-driven drift


#### NS_InteriorLeaks

- Name: NS_InteriorLeaks
##### Components
- Ceiling drips
- Pipe leaks
- Puddle ripples
- Impact decals or flipbooks


#### NS_ElectricalSparks

- Name: NS_ElectricalSparks
##### Components
- Short spark bursts
- Tiny smoke
- Point light optional and rare
- Audio event trigger


#### NS_SignalDistortion

- Name: NS_SignalDistortion
##### Components
- Subtle motes
- Pressure ripple rings
- Screen/static particles
- Creature proximity response


#### NS_CathedralMist

- Name: NS_CathedralMist
##### Components
- Vertical mist shafts
- Low floor fog
- Signal pulse response
- Volume-aware density tiers



### Niagara rules
- Every VFX system needs scalability controls.
- Avoid stacking many translucent systems in the same view.
- Use GPU particles for large environmental effects when appropriate.
- Use CPU particles for gameplay-critical collision or events.
- Use fixed bounds where possible.
- Tie major VFX to audio and lighting states.
- Use material parameter collections for storm intensity and signal intensity.

### Tests
- VFX readable in motion, not just screenshots
- No overdraw hotspots in common views
- Rain direction matches wind audio and environment motion
- Interior leaks create visible wetness context
- Signal effects are distinct from generic magic particles
- Scalability settings reduce density cleanly

### Done criteria
- All core Niagara systems implemented
- Scalability user parameters exposed
- Performance profiled in representative level
- Audio/light sync verified for key systems

### Pitfalls
- Too many translucent fog/rain layers
- Magic-looking particles
- No fixed bounds causing culling bugs
- Uncontrolled particle lights
- Rain appearing indoors unintentionally

- Copy paste prompt: Create Niagara VFX production specs for a UE5 coastal hydroacoustic horror game. Include storm rain, ocean spray, interior leaks, puddle ripples, sparks, signal distortion, cathedral mist, GPU/CPU emitter choices, scalability parameters, audio/light sync, overdraw tests, done criteria, and pitfalls.

## Animation pipeline

### Acquisition strategy

#### Buy or capture
- Generic humanoid locomotion
- Crouch/walk/run/turn cycles
- Basic hand interactions as starting points
- Door/open/lever interaction bases
- Creature locomotion reference if heavily modified

#### Custom
- Recorder first-person interactions
- Hydrophone machinery operation
- Creature signature idle/approach/reveal/attack motions
- Signal Cathedral setpiece motion
- Any animation tied to exact hero prop controls


### Adaptation steps
- Choose skeleton strategy early: UE mannequin-compatible for humanoids, custom rig for creature if silhouette demands it.
- Import purchased animation into a test project or quarantine folder first.
- Retarget to project skeleton using IK Retargeter.
- Clean foot sliding, root motion, turn rate, and pose exaggeration.
- Layer custom upper-body or additive poses for horror behavior.
- For creature, preserve silhouette during motion; do not let animation collapse readable shape.
- Use animation notifies for audio, VFX, camera shake, footstep surface events, and interaction state changes.
- Create animation blueprints with state machines and blend spaces.
- Test in actual fog/lighting, not preview scene only.

### Creature required animation set
- Idle distant
- Idle listening
- Slow patrol
- Investigate sound
- Sudden head/body orientation toward player
- Approach
- Retreat into fog/water
- Reach/attack
- Stagger or signal-disruption reaction
- Vent/crawl/doorway transition if used

### First person required animation set
- Raise recorder
- Lower recorder
- Tune frequency dial
- Start/stop recording
- Playback
- Connect cable
- Inspect display
- Signal overload reaction

### Tests
- No foot sliding in gameplay speed
- Animation silhouette readable in level lighting
- Notifies trigger audio/VFX at correct frames
- First-person hands do not clip recorder/camera
- Retargeted animations do not look like stock marketplace motion
- Creature motion remains threatening at low visibility

### Done criteria
- Animation list implemented in engine
- Retargeting documented
- AnimBP functional
- Notifies wired
- Gameplay test passed in representative level

### Pitfalls
- Relying on bought animation without identity pass
- Testing only in animation preview scene
- Creature moving too fast for silhouette readability
- No audio/VFX notifies
- First-person prop not modeled to fit hands

- Copy paste prompt: Create an animation acquisition and adaptation plan for a UE5 horror game with a custom hydroacoustic recorder and an uncanny pressure/signal creature. Include what to buy, what to custom animate, retargeting, IK, animation blueprints, notifies, first-person interactions, creature motion tests, done criteria, and pitfalls.

## Audio pipeline

### Audio identity

#### Core layers
- Storm wind and rain
- Ocean impact and distant surf
- Hydrophone low-frequency drones
- Analog tape hiss and recorder handling
- Metal stress, cable tension, machinery hum
- Muffled PA/signals
- Creature vocalizations as pressure/signal artifacts
- Signal Cathedral resonances

- Principle: Audio should feel like the station is listening back.

### Metasound pipeline
- Create MetaSound sources for procedural or layered loops: storm, hydrophone drone, machine hum, signal pulses, creature proximity.
- Expose parameters such as StormIntensity, InteriorMuffling, SignalStrength, CreatureDistance, MachineryState, PlayerDepth/Floor, and RecorderMode.
- Use random modulation for pitch, filter cutoff, volume, and layer selection.
- Use envelopes for tension ramps and event stingers.
- Route interactable objects to dedicated attenuation and submix paths.

### Submix structure

- Master: Final output and limiter
- Ambience: Storm, ocean, room tones
- Machinery: Winches, hums, motors, electrical systems
- Signal: Hydrophone tones, recorder playback, Cathedral resonance
- Creature: Enemy body, vocal, signal artifacts
- UI Device: Recorder UI, clicks, dials, feedback
- Music: If used, sparse and stem-based
- ReverbSends: Room-specific wet sends
- LowHealthOrPressure: Optional filtered bus for physiological state

### Spatial audio
- Use attenuation assets per category: small props, machinery, storm exterior, creature, large architecture.
- Use occlusion traces for doors/walls where reliable.
- Use reverb volumes or audio volumes per room type: concrete corridor, machinery hall, archive, exterior, cathedral.
- Use interior/exterior transition states to crossfade storm layers.
- Place multiple emitters for large objects instead of one point source.
- For Signal Cathedral, arrange emitters vertically/radially to create scale.
- Use low-frequency non-directional layers carefully; preserve gameplay readability.

### Recorder audio mechanic
- Recorder input changes mix state or reveals hidden layers.
- Tuning dial controls MetaSound parameter for bandpass frequency/noise.
- Playback uses degraded tape coloration and spatialized source memory if appropriate.
- Signal overload triggers distortion, haptic/camera response, light flicker, and creature behavior.

### Implementation steps
- Build audio style guide and asset list.
- Record/source raw material: water, wind, metal, cable, tape, radio static, machinery, low drones.
- Edit into clean one-shots, loops, and layers.
- Create naming convention and loudness target.
- Import SoundWaves with correct compression/loop settings.
- Build MetaSounds for procedural systems.
- Set attenuation and submix routing.
- Place emitters in graybox early.
- Bind audio states to gameplay variables.
- Mix in-engine with representative storm/VFX/lighting active.

### Tests
- Player can localize important threats
- Storm is intense but does not mask key cues
- Recorder mechanic produces understandable audio feedback
- Interior/exterior transitions feel physical
- Creature audio is identifiable but not repetitive
- Submix levels leave headroom and avoid clipping
- Audio scales with performance settings if needed

### Done criteria
- Core submix graph implemented
- MetaSounds functional for storm, signal, machinery, recorder, creature
- Attenuation assets assigned
- Audio volumes placed
- Gameplay states drive audio parameters
- Representative mix pass approved

### Pitfalls
- Adding audio only after art lock
- Storm too loud to hear gameplay
- Creature vocal sounds like generic monster library
- No occlusion/reverb differentiation between spaces
- Using stereo loops for everything instead of spatial emitters
- No loudness discipline

- Copy paste prompt: Create a UE5 audio production pipeline for a hydroacoustic horror game. Include MetaSounds, submix routing, spatial audio, attenuation, occlusion, audio volumes, storm layers, hydrophone drones, recorder mechanics, creature signal vocals, Signal Cathedral resonance, implementation steps, tests, done criteria, and pitfalls.

## Optimization

### Targets to define
- Target platforms
- Resolution and upscaling assumptions
- Frame-rate target
- Memory budget
- GPU budget for Lumen/VSM/fog/VFX
- Audio voice count
- Texture pool budget

### Ue5 optimization rules
- Use Nanite for dense static opaque geometry: rocks, concrete damage, machinery shells, architectural setpieces.
- Avoid Nanite for skeletal meshes, translucent/masked-heavy assets, WPO-heavy meshes, and small simple props where overhead is not worthwhile.
- Use Virtual Shadow Maps but control movable light count and shadow casting.
- Use HLODs or streaming for large exterior/station zones.
- Use level streaming or World Partition depending on project scale.
- Use material instances and shared master materials.
- Limit translucent overdraw from rain, fog, glass, and particles.
- Create scalability tiers for Lumen, shadows, fog, reflections, VFX density, foliage/rock density, and audio voice count.
- Use texture groups and mip bias appropriately.
- Profile in representative gameplay, not empty test maps.

### Collision optimization
- Use simple collision for modular architecture.
- Use UCX collision for hero props and machinery.
- Avoid complex-as-simple collision on repeated assets.
- Check collision view for hidden high-poly blockers.
- Separate visual detail from gameplay blocking.

### Vfx optimization
- Use fixed Niagara bounds.
- Cull distant systems.
- Reduce spawn counts by scalability.
- Avoid particle lights except rare hero moments.
- Use flipbooks/cards for distant rain and fog.
- Minimize overlapping translucent sheets.

### Audio optimization
- Set concurrency limits by category.
- Cull or virtualize distant emitters.
- Use streaming for long ambience/music.
- Avoid excessive always-on emitters in dense machinery rooms.
- Group ambience where individual localization is unnecessary.

### Profiling tools
- stat unit
- stat gpu
- stat rhi
- stat niagara
- stat audio
- Unreal Insights
- Shader Complexity view
- Quad Overdraw view
- Nanite visualization
- Lumen visualization
- Collision view
- Texture streaming stats

### Tests
- Representative worst-case exterior storm scene hits target frame time
- Signal Cathedral with fog/VFX/audio active hits target
- Machinery room with lights and moving parts hits target
- Texture pool does not overflow
- No shader complexity hotspots in common camera paths
- No runaway audio voices
- Scalability tiers produce measurable savings

### Done criteria
- Performance budget documented
- Worst-case scenes profiled
- Scalability settings implemented
- Optimization fixes applied and re-tested
- No known blocker spikes remain

- Copy paste prompt: Create an optimization checklist for a UE5 horror environment using Lumen, Nanite, Virtual Shadow Maps, volumetric fog, Niagara rain, wet materials, spatial audio, and modular station assets. Include profiling commands, budgets, Nanite rules, collision rules, VFX overdraw control, audio concurrency, scalability tiers, tests, done criteria, and pitfalls.

## Promptable task backlog

### ENV-001

- Id: ENV-001
- Task: Build reference and anti-reference boards
- Prompt: Create a reference and anti-reference board brief for The Drowned Frequency: a storm-bound coastal hydroacoustic station horror game. Include exterior, interior, machinery, archive, creature, Signal Cathedral, wet materials, lighting, audio identity, buyable generic assets, and custom hero assets.
- Test: Reviewer can identify the game's unique identity from boards alone.
- Done: Boards and approved identity notes exist.
- Pitfalls: Too generic industrial horror; no hydroacoustic specificity.

### ENV-002

- Id: ENV-002
- Task: Graybox full station route
- Prompt: Create a UE5 graybox task list for the complete station route, including metrics, room roles, audio zones, reveal points, hero prop proxies, storm exterior path, Signal Cathedral approach, tests, and signoff criteria.
- Test: Playable route works with primitives only.
- Done: Walkthrough recorded and approved.
- Pitfalls: Starting art before layout and audio pacing are proven.

### KIT-001

- Id: KIT-001
- Task: Create/adapt modular station kit
- Prompt: Create production specs for a modular coastal hydroacoustic station kit: dimensions, wall/floor/ceiling/door/stair/railing modules, pivots, snapping, materials, decals, collision, Nanite settings, and UE tests.
- Test: Demo room assembled cleanly on grid.
- Done: Core kit covers most interiors.
- Pitfalls: Marketplace kit remains visually generic.

### PROP-001

- Id: PROP-001
- Task: Model custom recorder
- Prompt: Create a Blender-to-UE5 task breakdown for a handheld analog hydroacoustic recorder, including dimensions, blockout, high poly, low poly, UVs, bake maps, textures, sockets, LOD/Nanite choice, collision, export/import, first-person tests, done criteria, and pitfalls.
- Test: Recorder reads in first-person and interactions align.
- Done: Final mesh/materials/sockets imported and verified.
- Pitfalls: Too modern, too sci-fi, unreadable controls.

### PROP-002

- Id: PROP-002
- Task: Create signage and maps
- Prompt: Create a signage/map production plan for The Drowned Frequency, including station sector codes, hydrophone array maps, evacuation diagrams, warning signs, decal strategy, sign mesh sizes, typography direction, materials, readability tests, and pitfalls.
- Test: Gameplay-critical signs readable in actual lighting.
- Done: Reusable sign system and key signs implemented.
- Pitfalls: Generic warning decals; signs not matching layout.

### PROP-003

- Id: PROP-003
- Task: Model hydrophone machinery
- Prompt: Create a Blender production breakdown for hydrophone winch machinery and signal-control equipment. Include blockout, mechanical logic, animated parts, pivots, high/low poly, UVs, baking, wet corroded materials, sockets for VFX/audio, collision, export/import, UE tests, and pitfalls.
- Test: Machine function and scale readable in room.
- Done: Imported machinery with tested pivots, materials, collision, and sockets.
- Pitfalls: Random pipes with no functional logic.

### PROP-004

- Id: PROP-004
- Task: Build archive core
- Prompt: Create an archive core production plan combining physical records, hydrophone logs, tape reels, damp paper, central terminal, modular shelves, decals, lighting, fog, audio emitters, optimization, tests, done criteria, and pitfalls.
- Test: Space reads as physical signal archive, not office storage.
- Done: Centerpiece and interactables implemented.
- Pitfalls: Too many unique papers; no focal point.

### SET-001

- Id: SET-001
- Task: Build Signal Cathedral setpiece
- Prompt: Create a production plan for the Signal Cathedral setpiece in UE5: graybox, silhouette, modular ribs, suspended hydrophone core, sacred-industrial identity, Blender modeling, materials, Niagara, fog, lighting, spatial audio, optimization, tests, done criteria, and pitfalls.
- Test: Recognizable from one screenshot without labels.
- Done: Setpiece art/audio/VFX/lighting pass approved.
- Pitfalls: Fantasy cathedral or generic sci-fi reactor.

### CHR-001

- Id: CHR-001
- Task: Design and produce creature silhouette
- Prompt: Create a silhouette-first creature production plan for a hydroacoustic horror game. Include design thumbnails, UE fog/backlight tests, Blender sculpt/blockout, retopology, UVs, baking, wet pressure/signal materials, rigging, animation set, physics asset, collision, LODs, tests, done criteria, and pitfalls.
- Test: Silhouette readable at 25 m, 10 m, and 3 m.
- Done: Rigged/imported creature with core animations and tested silhouette.
- Pitfalls: Generic humanoid/fish monster; detail before silhouette.

### MAT-001

- Id: MAT-001
- Task: Create wet material system
- Prompt: Create UE5 wet material master specs for marine concrete, corroded metal, wet glass, damp paper, black rubber, salt residue, rust streaks, puddles, vertex paint, decals, texture budgets, shader parameters, tests, done criteria, and pitfalls.
- Test: Wetness reads across lighting zones without plastic shine.
- Done: Master materials and instances applied in representative rooms.
- Pitfalls: Uniform gloss; no salt/rust logic.

### VFX-001

- Id: VFX-001
- Task: Create storm and signal Niagara systems
- Prompt: Create Niagara specs for storm rain, ocean spray, interior leaks, puddle ripples, electrical sparks, signal distortion, Cathedral mist, GPU/CPU choices, scalability, fixed bounds, audio/light sync, overdraw tests, done criteria, and pitfalls.
- Test: VFX readable in motion and within budget.
- Done: Core Niagara systems implemented and profiled.
- Pitfalls: Translucent overdraw and magic-looking particles.

### LGT-001

- Id: LGT-001
- Task: Lighting pass by zone
- Prompt: Create a UE5 lighting plan using Lumen, Virtual Shadow Maps, Nanite-compatible environments, zone palettes, practical lights, lightning, fog integration, wet reflections, creature reveal lighting, Signal Cathedral lighting, scalability fallbacks, tests, done criteria, and pitfalls.
- Test: Navigation and threat readability survive darkness/fog.
- Done: Approved lighting scenarios and scalability checks.
- Pitfalls: Hiding bad composition with darkness.

### AUD-001

- Id: AUD-001
- Task: Build MetaSound and submix pipeline
- Prompt: Create a UE5 audio pipeline for The Drowned Frequency with MetaSounds, submixes, spatial audio, attenuation, occlusion, audio volumes, storm layers, hydrophone drones, recorder mechanic, creature signal vocals, Signal Cathedral resonance, implementation steps, tests, done criteria, and pitfalls.
- Test: Player localizes threats and understands recorder feedback.
- Done: Submix graph, MetaSounds, attenuation, and audio volumes implemented.
- Pitfalls: Storm masks all gameplay cues.

### ANIM-001

- Id: ANIM-001
- Task: Acquire/adapt animation
- Prompt: Create an animation acquisition/adaptation plan for humanoid interactions, first-person recorder use, machinery operation, and creature movement. Include what to buy, what to custom animate, retargeting, IK, AnimBP, notifies, silhouette tests, done criteria, and pitfalls.
- Test: Motion looks authored for the game, not stock.
- Done: Core animation set retargeted/customized and tested in level.
- Pitfalls: Marketplace animation with no identity pass.

### OPT-001

- Id: OPT-001
- Task: Optimization and scalability pass
- Prompt: Create a UE5 optimization plan for a wet stormy horror station using Lumen, Nanite, VSM, volumetric fog, Niagara, spatial audio, and modular assets. Include profiling tools, budgets, Nanite rules, collision optimization, VFX overdraw, audio concurrency, texture pool, scalability tiers, tests, done criteria, and pitfalls.
- Test: Worst-case scenes hit target frame budget.
- Done: Performance captures and scalability settings approved.
- Pitfalls: Profiling only in empty test maps.


## Review checklists

### Asset review
- Scale correct in UE
- Pivot correct
- Naming correct
- Materials are project instances
- Textures packed and sized appropriately
- UVs clean with no obvious stretching
- Bake artifacts checked
- Collision intentional
- LOD/Nanite decision correct
- Sockets added if needed
- Looks wet/corroded only where physically plausible
- Supports locked visual identity

### Level art review
- Navigation readable
- Hero props framed
- Marketplace assets restyled
- Repetition broken with decals/material variation
- Wetness tells a water-source story
- Lighting supports gameplay
- Fog improves depth without hiding everything
- Audio emitters placed meaningfully
- Performance checked in representative view

### Audio review
- No clipping
- Important cues audible
- Storm intensity controllable
- Interior/exterior transitions convincing
- Creature audio distinct
- Recorder feedback understandable
- Submix routing correct
- Concurrency limits set

### Vfx review
- Effect has purpose
- Bounds correct
- Scalability works
- Overdraw acceptable
- Timing syncs with audio/light
- No generic magic look
- No indoor rain leaks unless intended


## Global pitfalls
- Producing final art before graybox signoff
- Buying too many environment assets and losing identity
- Treating wetness as a uniform gloss layer
- Adding fog late instead of designing around it
- Making audio a final polish task instead of a design pillar
- Overusing blue emissive sci-fi language
- Forgetting scalable fallbacks for Lumen/fog/VFX
- Using high-poly collision on repeated assets
- Letting creature detail override silhouette
- Creating signage that contradicts level layout
- Profiling only after content is already locked

## Definition of done for vertical slice
- One representative exterior storm approach playable
- One modular interior route dressed with adapted kit
- Recorder hero prop working in first-person
- Hydrophone machinery hero prop implemented
- Archive core or Signal Cathedral setpiece present as focal area
- Creature silhouette tested in fog/lighting with animation
- Wet material system applied across environment
- Storm, leak, signal, and machinery Niagara systems active
- Lighting pass with Lumen/VSM and fallback settings
- MetaSound/submix/spatial audio pipeline functioning
- Performance profiled in worst-case slice
- All custom identity assets visibly separate the game from marketplace-kit horror

