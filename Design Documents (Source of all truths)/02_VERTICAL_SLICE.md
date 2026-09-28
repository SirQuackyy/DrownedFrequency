# The Drowned Frequency — Vertical Slice

## Purpose

Build a 20–25 minute standalone slice proving the final game’s core promise before full production.

This is not a teaser made only of pretty lighting. It must test the recorder, one puzzle chain, one systemic threat, one narrative turn, save/load, controller input, packaged performance, and accessibility feedback.

## Slice premise

Mara reaches the station’s dormitory sector. The generator-hall door requires two mechanical rhythms. She records a clock and a generator thump, uses an acoustic puck to misdirect the Listener, crosses a shallow-water crawlspace under wave noise, and discovers the station replaying her first recording from the wrong room.

## Playable spaces

1. Safe radio room
2. Dormitory corridor
3. Two bunk rooms
4. Laundry room
5. Flooded maintenance crawlspace
6. Generator relay vestibule
7. Generator hall overlook

Use modular graybox geometry first. Only the final vertical-slice pass receives representative art.

## Minute-by-minute flow

### 0:00–2:00 — Orientation

- Load from main menu into safe radio room
- Teach move, interact, crouch, and recorder selection
- Show clipboard objective: restore the generator-hall relay
- Player hears a regular wall-clock tick in the dormitory

Success test: player understands the objective without a voice-over explanation.

### 2:00–5:00 — First capture

- Record clock tick
- Replay locally at relay
- First latch opens
- Playback creates an AI hearing stimulus
- Listener crosses a distant perpendicular hall without attacking

Success test: player understands that the same action solved a problem and attracted attention.

### 5:00–8:00 — Remote projection

- Find/recharge one acoustic puck
- Project recorded footsteps or clock into laundry
- Listener investigates the puck
- Player crosses behind it or uses a maintenance recess

Success test: player predicts the AI response before committing.

### 8:00–12:00 — Masked traversal

- Relay requests a heavy machine thump
- Enter shallow-water crawlspace
- Splash noise is dangerous
- Exterior wave impacts create safe movement windows
- Capture generator thump through a wall opening
- Something taps back through the pipe

Success test: audio rhythm produces a navigation decision, not just atmosphere.

### 12:00–16:00 — Mimicry turn

- Return route is altered by one closed fire door
- Clock recording plays from an unused bunk room
- Waveform compass marks it as contaminated/unsourced
- Opening the room is optional; inside is only a wet impression and stopped clock

Success test: player distrusts the sound while understanding the game did not cheat.

### 16:00–20:00 — Combined pressure

- Replay generator thump at second relay
- Operate lever during correct rhythm
- Existing puck loop loses credibility
- Listener searches probable route instead of the puck center
- Player masks final crossing under machinery startup

Success test: repeated playback visibly changes enemy behavior.

### 20:00–25:00 — Reward and hook

- Enter generator overlook
- Power restoration transforms lighting, ambience, and distant station state
- Autosave fires
- A speaker plays Mara’s clock clip, followed by a voice matching her breathing
- End card only after player regains control and approaches the window

Success test: slice ends on curiosity, not merely a scream.

## Required systems

### Must be real

- First-person movement and final input framework
- Interaction component
- Authored anomaly recording
- Three clip slots
- Local playback
- One reusable puck
- Sound stimulus scoring
- Listener patrol/investigate/search/chase states
- One acoustic receiver puzzle
- One masking-volume/rhythm puzzle
- Clip repetition credibility
- One contamination event
- Checkpoint save/load
- Subtitles and optional waveform direction
- Development packaged build

### May be faked for slice

- Final inventory presentation
- Full station streaming graph
- Final creature mesh
- Full encounter director
- Complete options menu styling
- Steam integration
- Final narrative voice acting

Faking means bounded authored logic behind the expected interface—not editing a trailer to imply nonexistent gameplay.

## Graybox layout rules

- Main route length: roughly 90–120 seconds without obstacles
- Every critical intersection has a unique silhouette
- At least two routes around the Listener encounter
- One clearly safe room
- One dead-end optional scare room
- No corridor longer than needed to hide streaming or build a specific sound event
- Use sightline breaks every 8–15 meters indoors

## Art target

The slice needs three visual identities:

- Safe radio room: warm practical lamp, dry materials, dampened sound
- Dormitory: cold emergency lighting, wet concrete, fabric motion, long reflections
- Generator overlook: industrial scale, strong moving shadows, steam and vibration

Representative UE features:

- Lumen power-state transition
- Nanite architecture and machinery where appropriate
- Virtual Shadow Maps after profiling
- Exponential Height Fog plus sparse local/Niagara mist
- Wet material instances with restrained reflection
- MetaSound ambience driven by power and threat state
- One recorder post-process effect

## Audio target

Required authored layers:

- Storm exterior transmission
- Room-specific ventilation/hum beds
- Clock source: clean, projected, and mimicked variants
- Generator source: muffled, clean, relay playback, startup
- Listener cable clicks, breath, steps, search cue, chase cue
- Surface-dependent player footsteps
- Wave-surge masking cue
- Silence transition before mimic event

Do not use continuous music. One low tonal layer may appear at peak search state.

## Playtest questions

Ask testers to play before explaining anything, then answer:

1. What did the recorder let you do?
2. Why did the creature investigate or find you?
3. When did you feel safest, and was that safety real?
4. Did any audio clue become inaccessible or confusing without headphones?
5. Would you voluntarily play the next 30 minutes?

Instrument locally:

- Time to first recording
- Relay attempts
- Deaths and cause
- Hint tier shown
- Puck placements
- Time spent stuck
- Frame-time spikes by room

## Pass/fail criteria

Pass only if:

- Four of five testers correctly describe the tool/risk loop
- Four of five understand their detection cause after failure
- Median completion is 18–30 minutes
- No tester is soft-locked
- At least four want to continue
- Packaged build maintains the chosen 60 FPS target on the minimum-spec test machine with no repeated severe traversal hitch
- Save/load restores puzzle, clip, puck, and AI state coherently

## Build order

### Week 1 — Core interaction

- Create C++ project and source control
- Implement movement, Enhanced Input, interaction trace
- Build seven-room graybox
- Produce packaged smoke build

Exit: player can traverse and operate the relay in a packaged build.

### Week 2 — Recording

- Implement anomaly data asset and recorder component
- Capture/select/replay one clock signal
- Add receiver compatibility and feedback

Exit: clock recording opens latch and survives save/load.

### Week 3 — Threat

- Implement sound event structure and Listener states
- Add footsteps and local playback stimuli
- Build patrol and two authored route options

Exit: threat consistently investigates the source that generated the stimulus.

### Week 4 — Projection and masking

- Add puck placement and loop
- Add wave masking windows and water footstep loudness
- Add repeated-signature credibility

Exit: encounter supports two viable player solutions.

### Week 5 — Complete gameplay

- Add generator capture, second relay, contamination event, checkpoint
- Add death/retry and anti-soft-lock resets
- Run five internal full completions

Exit: graybox slice is start-to-finish complete.

### Weeks 6–7 — Representative art and audio

- Replace hero spaces with consistent modular kit
- Build lighting states, wet materials, particles, MetaSounds
- Add placeholder/final hybrid voice and creature presentation

Exit: slice resembles the intended shipped game.

### Week 8 — QA and decision

- Profile and optimize packaged build
- Run controller-only and accessibility passes
- Run at least five blind external tests
- Record findings and make the greenlight decision

Exit: all pass/fail criteria have evidence.