# The Drowned Frequency — Production Roadmap

## Planning assumptions

- Solo developer, focused full time
- Windows PC first
- Marketplace assets used for generic environment content
- Contractors optional for creature animation, key art, trailer, final mix, and voice
- Scope remains at 4–6 hours
- Total target: 47 focused weeks
- Realistic solo calendar with interruptions: 12–18 months

Do not treat the week number as sacred. Treat every exit criterion as sacred.

## Phase 0 — Preproduction (3 weeks)

### Outcomes

- One-page pitch locked
- Master GDD reviewed and cut
- Reference board for each sector
- First-pass critical path map
- Technical spike for authored recording
- Source control and build conventions established

### Backlog

1. Create UE C++ project and Git LFS repo.
2. Implement one anomaly capture and one receiver.
3. Build a graybox hallway and safe room.
4. Test spatial sound, captions, and Lumen baseline.
5. Produce one Windows packaged build.

### Exit criteria

- Project packages on a clean Windows environment
- Recorder illusion works without runtime microphone capture
- A room reaches representative lighting without missing the frame target catastrophically
- Scope sheet lists every sector, required puzzle, direct encounter, and custom hero asset

## Phase 1 — Vertical slice (8 weeks)

Follow `02_VERTICAL_SLICE.md`.

### Exit criteria

- 20–25 minutes start to finish
- One complete record/project/misdirect/mask loop
- Save/load and controller support
- Representative final art and audio in three spaces
- External blind playtests pass the stated criteria
- Performance profile recorded from packaged build

### Greenlight decision

If the slice fails:

- Confusing recording: reduce categories or add receiver feedback
- Unfair AI: simplify scoring and expose stronger tells
- Boring stealth: shorten encounter and increase route choice
- Weak fear: improve consequence and delayed repetition before adding jumps
- Poor performance: cut light, fog, and reflection complexity before lowering gameplay density

Do not begin five chapters while hoping the mechanic becomes fun later.

## Phase 2 — Core production (20 weeks)

### Sprint A — Campaign skeleton (4 weeks)

- Graybox every critical-path room
- Establish streamed sector boundaries
- Implement chapter and objective subsystem
- Implement all shortcuts and checkpoint locations
- Complete a start-to-finish no-art walkthrough

Exit: the entire game is traversable with placeholder gates and ending trigger.

### Sprint B — Systems complete (4 weeks)

- Finish five signal slots and categories
- Add contamination and cleansing
- Add speaker/pipe routing
- Complete power and pump state systems
- Finish Listener states, credibility memory, and director guard rules

Exit: no new reusable gameplay system is planned after this sprint.

### Sprint C — Acts I and II (4 weeks)

- Implement Landfall and Playback puzzles
- Add first two hub transformations
- Add required clue chain
- Add final-quality opening storm approach where feasible
- Run blind tests through Act II

Exit: opening 90–120 minutes is content complete in gameplay.

### Sprint D — Acts III and IV (4 weeks)

- Implement flooded lower grid
- Add Drowned Technician zone controller
- Build Signal Cathedral
- Implement personal-recording choices and partial reveal
- Run blind tests through Act IV

Exit: all required puzzle families and manifestations are represented.

### Sprint E — Act V and endings (4 weeks)

- Implement final signal assembly
- Build tower and evacuation route
- Add three main endings
- Add Tomas variant only if schedule is healthy
- Complete start-to-finish save migration test

Exit: game is playable from title screen to credits.

## Phase 3 — Alpha (6 weeks)

Definition: feature complete, not pretty-complete.

### Work

- Replace remaining blocking placeholders that affect readability
- Finish all required narration and documents
- Complete save/version migration path
- Resolve progression blockers
- Lock puzzle solutions and encounter routes
- Validate accessibility metadata for every critical sound
- Conduct at least five full external runs

### Exit criteria

- No known progression blocker
- No missing required clue or audio caption
- All endings reachable from valid saves
- Every chapter reloads correctly
- Full controller-only completion exists
- Temporary assets are listed explicitly

## Phase 4 — Beta (6 weeks)

Definition: content complete; tune, optimize, and repair.

### Work

- Final environment and hero-prop pass
- Lighting consistency and navigation readability
- Final creature presentation and animation cleanup
- Audio mix and dynamic-range presets
- Performance optimization by measured hotspot
- Steam achievements and cloud saves
- Trailer capture and store screenshots from packaged build
- Wider external test pool

### Exit criteria

- 60 FPS target met on the minimum-spec test machine at the intended preset
- Save/load survives full campaign and cloud conflict testing
- No severity-one defects
- All critical audio is understandable with captions and waveform options
- Store page materials represent actual gameplay

## Phase 5 — Release candidate (4 weeks)

### Work

- Freeze content
- Fix only release-blocking and high-severity defects
- Test clean install and upgrade install
- Verify every achievement and ending
- Verify credits and third-party licenses
- Test low disk, no network, alt-tab, controller disconnect, and corrupted-settings recovery
- Prepare day-one support notes and known issues

### Exit criteria

- Shipping build passes clean-machine checklist
- No severity-one defect
- Rollback build archived
- Store package, capsules, trailer, warnings, and system requirements ready
- Release branch and version tag created

## Weekly operating rhythm

### Monday

Choose one playable outcome for the week. Maximum five backlog items.

### Tuesday–Thursday

Implement and test in packaged or standalone builds as soon as the feature crosses a level boundary, touches saves, or changes streaming.

### Friday

- Produce a playable build
- Run a 15–30 minute smoke route
- Log frame-time, crashes, blockers, and scope changes
- Tag meaningful milestones

If a week ends without a playable build, the following Monday starts by restoring build health before new work.

## Definition of done

A gameplay feature is done only when:

- It works with keyboard/mouse and controller
- It survives save/load
- It has final or intentionally tracked placeholder feedback
- It exposes accessibility information when audio-critical
- It has failure/reset behavior
- It works in a packaged build
- It has no known progression blocker

A room is done only when:

- Its purpose is clear
- The route is readable
- Puzzle and threat states reset safely
- Required sounds are reacquirable
- Lighting supports navigation
- Collision has been tested
- Its frame-time cost is measured

## Asset procurement plan

### Buy early

- Primary industrial modular kit
- Utility prop kit
- Coastal cliff/rock assets
- General foley and weather libraries

### Commission or create after slice greenlight

- Recorder hero prop
- Listener silhouette/animation adaptation
- Signal Cathedral machinery
- Key art and store capsule
- Final voice performances

### Delay until beta

- Trailer editing
- Marketing screenshots
- Final Steam capsule variants
- Nonessential cosmetic props

## Contractor briefs

Every contractor brief includes:

- Exact deliverable and file formats
- UE scale, skeleton, and naming requirements
- Poly/material/texture budget
- Commercial-use rights and source-file delivery
- Revision count
- In-engine reference images

Never outsource an undefined “make it scary” task. That way lies invoices and sorrow.

## QA matrix

Test each chapter across:

- Keyboard/mouse and controller
- Story, Standard, and Survival threat settings
- Captions/waveform on and off
- New game and loaded checkpoint
- Medium and low scalability

Prioritize combinations around changed systems instead of brute-forcing every permutation after every edit.

## Risk register

### Recorder becomes menu-heavy

Warning: testers spend more time trimming/managing than moving.

Response: remove trimming, cap slots at five, auto-select clean capture windows.

### AI is perceived as random

Warning: players cannot explain detection.

Response: simplify stimulus score, strengthen orientation/audio tells, log causes on death during testing.

### Environment becomes repetitive

Warning: screenshots from different wings look interchangeable.

Response: enforce sector identity through geometry, palette, acoustic bed, hazard, and hero prop—not more generic clutter.

### UE atmosphere breaks performance

Warning: wet corridors, VSM, fog, and Lumen compound.

Response: profile the vertical slice, cap shadowed lights, reduce volumetric coverage, create reflection/GI scalability before content expansion.

### Scope expands through bespoke rooms

Warning: each new puzzle invents a new mechanic or custom art set.

Response: every puzzle must use an existing family and every room must serve two purposes. Cut rooms that cannot.

## Release content checklist

- Main menu, settings, credits, warnings
- Three main endings
- 4–6 hour critical path
- Eight required major puzzles
- One primary AI plus two manifestations
- Full remapping/controller support
- Captions, waveform option, reduced flicker/shake
- Versioned local saves and tested cloud saves
- Achievements kept modest and non-grindy
- Trailer and screenshots showing real acoustic gameplay

## Immediate first session (90 minutes)

1. Create a blank First Person C++ UE project named `DrownedFrequency`.
2. Initialize Git LFS before importing assets.
3. Create the folder structure from `03_TECHNICAL_DESIGN.md`.
4. Graybox the safe radio room, dorm corridor, and relay vestibule.
5. Package an empty traversal build for Windows.

The first victory is not a monster or cinematic. It is a reproducible packaged build with source control.