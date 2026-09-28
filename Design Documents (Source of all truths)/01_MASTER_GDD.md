# The Drowned Frequency — Master Game Design Document

## 1. Product definition

### High concept

During a violent coastal storm, freelance acoustic technician Mara Venn enters the decommissioned Northpoint Hydroacoustic Station to recover its final black-box archive. The station recorded a signal from beneath the sea that survives by inhabiting remembered sound. Mara must record, replay, and route sounds through the facility to escape—but every useful recording gives the presence another voice and teaches it another way to find her.

### Player fantasy

Be a capable technician trapped inside a place where listening is necessary, silence is tactical, and your own recordings return from rooms you have not entered.

### Genre and format

- First-person psychological/survival horror
- Environmental puzzles and stealth-lite encounters
- No weapons or combat progression
- One dense, revisited location
- 4–6 hour first playthrough
- Three primary endings plus one optional personal ending

### Design pillars

1. Acoustic agency: sound solves navigation, puzzles, and encounters.
2. Fear through consequence: using the correct tool often creates the next danger.
3. Legible vulnerability: the player can explain most failures.
4. Controlled revelation: behavior first, shape later, no bright full-body monster showcase.
5. Dense production: every room serves at least two roles among traversal, puzzle, scare, clue, and transformation.

### Explicit non-goals

- No open world or procedural generation
- No combat, crafting tree, or weapon upgrades
- No more than one fully systemic enemy active at once
- No long dialogue scenes or animated NPC cast
- No realistic runtime microphone recording or waveform analysis
- No puzzle that requires perfect pitch
- No constant hiding-loop or chase spam

## 2. Core loop

### Thirty-second loop

1. Enter a space and stop to listen.
2. Identify a useful source, acoustic receiver, unsafe surface, or threat cue.
3. Capture or select a short sound recording.
4. Replay it locally, throw a projection puck, or route it through station equipment.
5. Use the reaction to cross, unlock, distract, reveal, or mask.
6. Move before the entity reaches the new sound source.

### Five-minute loop

- Explore one puzzle space.
- Learn its acoustic rule through physical feedback.
- Obtain or recover a useful sound.
- Make a consequential sound choice.
- Survive the station’s response.
- Receive a route, upgrade, clue, or shortcut.

### Chapter loop

- Enter a sector from the Operations hub.
- Restore one system: power, pumps, intercom, hydrophones, or transmitter.
- Learn one new use of the recorder.
- Complete one major puzzle and one threat encounter.
- Open a shortcut to the hub.
- Return to find a familiar room acoustically altered.

## 3. Player verbs and controls

### Movement

- Walk: moderate noise
- Creep/crouch: slower and quieter
- Sprint: loud, stamina-limited escape tool
- Lean: inspect around corners without full exposure
- Hold breath: suppress breathing briefly while the entity listens nearby
- Contextual vault: only at authored obstacles

### Recorder

- Listen focus: raises gain and exposes directional waveform cues
- Record: captures an authored sound source while within its conditions
- Select: chooses one of three initial recording slots; expands to five
- Replay locally: powerful but reveals the player’s position
- Project remotely: throws or places an acoustic puck
- Loop: sustains a distraction while fidelity degrades
- Route: sends a clip through powered speakers, pipes, phones, or transducers
- Overdub: late-game combination of two compatible authored signals

### World interaction

- Open/close doors
- Operate switches, valves, breakers, cranks, and radio dials
- Replace or reroute fuses
- Inspect documents and equipment
- Pick up small key items
- Hide only in a few authored maintenance recesses
- Use safe-room tape desks and checkpoint terminals

### Suggested PC mapping

- WASD: move
- Mouse: look
- E / Left Mouse: interact or confirm
- Right Mouse: listen focus
- R: hold to record
- F: replay locally
- G: place/throw selected puck
- Mouse Wheel / 1–5: select clip
- Ctrl: crouch
- Shift: sprint
- Q/E: lean
- X: hold breath
- Tab: clipboard map and acoustic notebook

All bindings must be remappable. Controller inputs receive equivalent actions and glyphs.

## 4. Signature acoustic system

### Important implementation illusion

The game does not record arbitrary runtime audio. Every capturable source is an authored `Audio Anomaly` with:

- ID and sound category
- Clean, degraded, and contaminated audio layers
- Capture distance and angle
- Required station state
- Fidelity value
- Compatible receivers
- AI stimulus metadata
- Narrative metadata

The player experiences recording; the game stores a compact signal definition. This preserves the fantasy without building an audio laboratory inside the audio laboratory.

### Recording slots

- Start: 3
- Mid-game: 4
- Maximum: 5
- Clip length presentation: roughly 3 seconds initially, 6 seconds after upgrade
- Required clips can always be reacquired
- Deleting a clip never creates a permanent soft lock

### Seven sound categories

1. Voice: voiceprint locks, memory residues, strongest contamination risk
2. Footstep: patrol misdirection and route testing
3. Machine rhythm: synchronization, masking, power puzzles
4. Bell/foghorn: long-range lure and rare overload effect
5. Water: flooded-zone distraction and pump puzzles
6. Static: interference, cleansing, high-frequency repulsion
7. Impact: attention break, pressure switch, short lure

### Projection methods

- Local speaker: unlimited range only near Mara; highest personal risk
- Acoustic puck: remote source; three carried; rechargeable at safe desks
- Station speaker: sector-wide effect; requires routed power
- Pipe resonance: sends sound through walls with uncertain endpoint
- Hydrophone/transducer: late-game large-scale environmental effect

### Contamination

Every human-voice clip has a contamination state:

- Clean: exact source; reliable
- Frayed: subtle extra breaths or timing errors
- Contaminated: altered words, stronger entity response, may unlock dangerous residues

Contamination increases through repeated human-voice playback, specific story rooms, or voluntary exposure. Machine and natural sounds degrade but do not carry the same narrative corruption.

The player can clean one clip at a safe desk by destroying another contaminated personal recording. This creates an actual choice instead of a decorative morality meter.

### Sound credibility

The Listener tracks repeated signatures:

- First playback: strong investigation
- Second identical playback in a short window: cautious investigation
- Third: threat may ignore the center and search likely player routes

Reset credibility through time, a different sound category, a different projection method, or an environmental masking event.

## 5. Resources and failure

### Resources

Keep the visible economy small:

- Recorder charge: powers listen focus and local playback
- Puck charge: three reusable pucks, restored at safe desks
- Sector fuses: route power among doors, lights, pumps, and speakers
- Stamina: sprint and hold breath
- Composure: hidden pressure state controlling false audio, never a conventional sanity bar

Cut disposable batteries unless testing proves they create good decisions. Searching drawers for AA batteries is not the forbidden pinnacle of horror design.

### Fail states

- Entity capture
- Drowning during authored flood event
- Electrocution after clearly telegraphed power routing
- Fall during exterior climax

### Soft failures

- Threat enters search state
- A clip becomes contaminated
- A puck remains in a dangerous room until recovered or safe-desk reset
- Power reroutes and creates a harder path
- Composure introduces false cues

### Checkpoint rules

- Autosave after every major solved puzzle
- Checkpoint before each multi-stage encounter
- Never save during irreversible lethal danger
- Reload restores at least one usable puck and required recording access
- Safe-room manual slots provide chapter, time, and screenshot metadata

## 6. Threat design

### The entity

The Drowned Frequency is not a zoological sea monster. It is a parasitic pattern that localizes itself through recorded memory, pressure, and repetition. Its manifestations are facets of one being, preventing a three-monster production nightmare.

### Manifestation A: The Listener

Use: middle-game systemic enemy

Appearance: a partly humanoid acoustic absence wrapped in cable, wet fabric, and refraction; never shown cleanly.

Senses:

- Excellent hearing
- Limited vision
- Remembers recent sound signatures
- Responds to volume, category, fidelity, repetition, occlusion, and surface type

States:

- Dormant
- Patrol
- Curious
- Investigate
- Search
- Stalk
- Chase
- Retreat/reset

Counterplay:

- Creep on quieter surfaces
- Mask steps under machinery or storm surges
- Project footsteps away from the intended route
- Hold breath at close range
- Use static overload for a short retreat, never damage

Fairness tells:

- Head or upper-body orientation points toward the current stimulus
- Cable clicking accelerates with suspicion
- Radio static rises before lethal proximity
- It never appears directly behind the player without a prior acoustic warning

### Manifestation B: The Drowned Technician

Use: authored flooded-zone behavior, not a second full AI system

- Tracks splashes and pump vibration
- Uses a few fixed submerged traversal paths
- Appears beneath water, wet glass, or grating
- Withdraws from high-frequency static
- Encounters are state machines layered over the Listener’s shared stimulus model

### Manifestation C: Antenna Choir

Use: late-game environmental hazard

- Voices emerge from speakers, guy wires, and antenna dishes
- Creates false safe-room sounds and directional cues
- Raises composure pressure
- Forces choices among lighting, doors, and speakers
- Defeated through signal cancellation and power routing, not pursuit AI

### Encounter director

Inputs:

- Time since threat
- Recent player noise
- Current objective progress
- Resource level
- Deaths in segment
- Time stuck
- Repeated strategy use

Outputs:

- Distant audio event
- Listener route selection among authored options
- Silence window
- Minor false cue
- Environmental failure
- Optional scare
- Extra decoy resource after repeated failure

Hard rules:

- Never interrupt first-time teaching of a mechanic
- Never run more than two active pressures during a precision puzzle
- After two deaths, lower persistence or provide an additional route/decoy
- Do not randomly invalidate a hiding spot unless the player was seen entering
- Every chase has at least two viable escape routes

## 7. Narrative

### Mara Venn

Freelance acoustic systems technician hired by a salvage insurer. Practical, capable, and minimally voiced. Her older brother Tomas disappeared after analyzing material copied from Northpoint. The connection emerges through evidence rather than opening narration.

### Supporting cast

- Dr. Elian Rusk: lead researcher who believed the signal could preserve consciousness
- Chief Engineer Sol Anik: attempted to sabotage the archive after recognizing the danger
- Len Orta: young signal operator whose frightened recordings become the entity’s favorite lure
- Tomas Venn: Mara’s missing brother, present only in rare ambiguous fragments

No character appears as a live exposition ghost. Total clean archival dialogue should remain under roughly eight minutes per major supporting character.

### Backstory

Northpoint monitored submarines, seismic activity, and deep-ocean sound. Its array detected a repeating trench signal: first whale-like, then coded, then voices of dead relatives. Staff replayed the voices privately. Repetition gave the signal structure, then agency. It began replaying events before they happened and guiding people into flooded areas.

Anik tried to destroy the recording core during a storm. Rusk stopped him, convinced the archive contained people rather than copies. The station flooded and was sealed. Years later a storm restarts emergency power and transmits Tomas’s voice.

### Themes

- Memory as infection
- Grief as an exploitable signal
- Recording technology as séance
- The ocean as an archive
- Whether preserving a voice preserves or violates a person

### Clue delivery

Target ratio:

- 50% environment
- 30% audio fragments
- 15% functional documents
- 5% Mara reactions

Document rules:

- Most under 120 words
- Audio logs 15–30 seconds; hard maximum 45
- Notes must be physically justified: maintenance tag, shift board, insurance memo, evacuation checklist
- No single clue explains the mystery
- Each major truth is supported by three forms of evidence before confirmation

## 8. Five-act campaign

### Act I — Landfall (45–60 minutes)

Spaces: cliff path, boathouse, reception, generator annex, upper monitoring hall

Gameplay:

- Basic interaction and movement
- Capture machine rhythm
- Replay sound into a receiver
- Restore emergency power

Horror:

- No clear monster
- Dead equipment activating
- Impossible source direction

Signature scare: an emergency phone replays the sound of Mara knocking on the entrance door minutes earlier.

End: storm surge floods the exit; Len’s voice asks to be released.

### Act II — Playback (60–75 minutes)

Spaces: Operations hub, tape archive, mess, staff quarters, intercom exchange

Gameplay:

- Clip inventory
- Acoustic pucks
- Voice recordings and contamination
- Authenticity puzzle in a looping corridor

Horror:

- Mimicry of previously safe sounds
- Familiar clips coming from wrong rooms
- First indirect footprints and condensation response

End: the lower array broadcasts Tomas’s voice.

### Act III — Pressure Rooms (75–90 minutes)

Spaces: pump control, flooded lower grid, test chamber, maintenance crawlspaces

Gameplay:

- Water-level routing
- Pipe projection
- Movement masking under wave and machine rhythms
- First controlled systemic Listener encounters

Horror:

- Threat behind wet glass
- Movement under still water
- Required recording performed while danger approaches

End: the test chamber plays a high-fidelity recording of Mara panicking in a room she has not reached.

### Act IV — The Choir Below (75–90 minutes)

Spaces: sonar well, Rusk’s lab, collapsed dormitory, deep array control, Signal Cathedral

Gameplay:

- Speaker routing
- Multi-stage acoustic puzzles
- Cleanse, preserve, or destroy personal recordings
- Static cancellation

Horror:

- Multiple voices form one coordinated presence
- Partial symbolic reveal through water, strobe, refraction, and pressure
- Entity begins predicting repeated strategies

End: evidence suggests Tomas voluntarily entered the archive to test whether the voices were conscious.

### Act V — Dead Air (45–75 minutes)

Spaces: archive core, array junction, transmitter tower, evacuation lift

Gameplay:

- Assemble a final signal from selected recordings
- Revisit transformed hub spaces
- Short final pursuit testing recording, routing, masking, and silence
- Make final broadcast decision through physical controls

Horror:

- Recognition and consequence
- Safe sounds weaponized
- Long silence after the system powers down

No endless sprint ending. The climax alternates deliberate operation, short danger, and irreversible choice.

## 9. Puzzle families

### Acoustic lock

Receiver expects a category and rhythm. Nearby meters and mechanical feedback communicate progress.

Example: record a generator’s healthy thump, then replay it while operating the second starter in matching rhythm.

### Sound routing

Choose powered speakers, pipes, or transducers to move a stimulus through the station.

Example: send footsteps to the laundry speaker, drawing the Listener across a route while Mara crosses the dorm hall.

### Authenticity test

Compare a real ambient pattern with a mimic containing one wrong interval, direction, or mechanical layer.

Example: the looping staff corridor resets unless the player follows the intercom chime containing the real transformer hum.

### Masked traversal

Move during loud environmental windows.

Example: cross shallow water only during storm surges while projecting drips into a side tunnel.

### Composite/overdub

Late game only. Combine two known categories at a station desk, not arbitrary timeline editing.

Example: add a PA calibration tone beneath Rusk’s partial phrase to satisfy the archive voiceprint.

Puzzle rules:

- Required source exists in the current sector
- Optional carried clips may create shortcuts
- Failure creates pressure rather than erasing all progress
- No perfect pitch or tiny visual-code transcription
- Hints at 3, 6, and 10 minutes if enabled

## 10. Scare grammar

### Primary scare types

- Source confusion: sound from an impossible location
- Delayed repetition: the station returns a past player sound
- Useful bait: a clue is also a lure
- Negative space: expected ambience suddenly disappears
- Pressure presence: pipes, doors, and water respond to an unseen mass
- Recognition horror: a personal or safe sound is used incorrectly
- Mechanic betrayal: a familiar clip gains one altered detail
- Controlled pursuit: short route-based escape testing a learned system
- Aftermath: evidence shows a prior action caused an unseen reaction

### Cadence

- No more than one direct jump scare in a 20-minute segment
- Every loud scare needs setup and aftermath
- At least half of scares must be affected by player sound choices
- Maximum three major pursuit sequences in the full game
- Follow each major threat with a quiet room containing one interaction and one clue
- Never stack more than two high-intensity sequences without recovery

### Reveal policy

- Act I: audio, vibration, equipment response
- Act II: footprints, condensation, shadows synchronized to sound
- Act III: partial shape behind flooded glass and pressure distortion
- Act IV: symbolic acoustic absence in the Signal Cathedral
- Act V: enough shape for navigation and threat reading, still obscured

Never show a clean full-body model under neutral light. Never explain its species. Never let it deliver a villain speech.

## 11. Endings

### Dead Air

Destroy archive and array. Mara escapes without proof or certainty about Tomas. All preserved voices are erased.

### Safe Channel

Use non-human loops to isolate the entity, destroy personal archives, and transmit a narrow warning. Mara escapes with partial evidence; the sealed station continues to hum.

### The Broadcast

Transmit the full contaminated archive. Mara escapes while coastal radios begin receiving familiar voices. This is the bad ending.

### Tomas (optional personal variant)

Recover Tomas clues, maintain low contamination, isolate one voice, and refuse to replay the final tempting clip. Mara receives one coherent message that may be Tomas or a perfect imitation.

Cut any fifth secret ending before cutting polish from the main three.

## 12. Difficulty and accessibility

### Difficulty axes

Tune separately:

- Threat persistence
- Resource forgiveness
- Hint timing
- Clip credibility decay

Presets: Story, Standard, Survival. Puzzle solutions do not change.

### Audio accessibility

- Closed captions for every meaningful sound, with direction and distance
- Optional waveform compass
- Mono-compatible mix mode
- Frequency-safe mode reducing piercing/tinnitus effects
- Headphones, speakers, and night dynamic-range presets
- Separate ambience, threat, voice, UI, and stinger sliders

### Visual and motor accessibility

- Subtitle size, opacity, and speaker label settings
- Reduced flicker and camera shake
- High-contrast interactable option
- Colorblind-safe signal palette
- Hold/toggle options
- Adjustable interaction durations
- Full remapping and controller support
- No rapid button mashing

## 13. Scope budget

Target content:

- Six major station sectors plus a short exterior
- One primary AI and two lower-cost manifestations
- Eight required major puzzles
- Six to ten short supporting interactions/puzzles
- Six to eight direct encounters
- Maximum three pursuits
- Thirty-five to fifty short narrative items
- Eight custom hero props
- Five major visual set pieces

Cut order if behind:

1. Optional Tomas ending
2. Additional dormitory rooms
3. Advanced contamination variants
4. One pursuit sequence
5. Exterior exploration beyond the opening path

Never cut the recorder, mimicry, the storm station, the moral archive choice, or fair acoustic feedback.