# AI Prompt Protocol

Use this envelope around the copy-paste prompt in every task card.

## Coding task envelope

You are implementing exactly one bounded task in The Drowned Frequency, a UE5 C++ first-person horror game.

Repository: [ABSOLUTE PATH]
Branch: [BRANCH]
Task ID: [ID]
Task prompt:
[PASTE THE TASK'S COPY-PASTE PROMPT]

Locked architecture:
- Project/module: DrownedFrequency
- C++ class prefix: TDF
- Source root: Source/DrownedFrequency
- Content root: Content/Game
- C++ owns reusable mechanics, state, serialization, and rules.
- Blueprints own presentation and room-specific scripting.
- Data Assets own definitions and tuning.
- Do not add plugins, dependencies, or Marketplace code without approval.
- Preserve existing changes. Do not rewrite unrelated files.
- Never read or expose credentials.
- Do not commit, push, publish, purchase, or modify another repository.

Required workflow:
1. Inspect repository instructions, git status, existing related classes, and tests.
2. State the files you expect to edit and the acceptance tests.
3. Write or update the smallest relevant failing test first when feasible.
4. Implement only this task.
5. Build the correct Unreal target.
6. Run the listed automated/functional tests.
7. Report changed files, diff summary, commands run, exact test/build result, assumptions, and unresolved risks.

Completion is forbidden if:
- The Unreal target did not compile.
- The listed test was not run.
- The task's done criteria are not all satisfied.
- Placeholder output is presented as final functionality.
- Unrelated files were modified without explanation.

Do not begin the next task.

## Unreal Editor task envelope

Guide me through exactly one Unreal Editor task. Do not assume UI actions happened.

Project: DrownedFrequency
Task ID: [ID]
Task prompt:
[PASTE TASK PROMPT]

For every editor action:
1. Give the menu or Content Browser path.
2. Give the exact asset/class name.
3. Give the property name and value.
4. Stop at natural verification points.
5. Tell me exactly what I should see if it worked.
6. Give the likely cause and correction if it did not.

At the end provide only:
- Assets created or changed
- Verification performed
- Done-criteria checklist
- One next task ID

Do not redesign the system, add unrelated assets, or advance automatically.

## Blender modeling task envelope

Guide me through exactly one asset stage in Blender; do not jump from blockout to a finished model.

Game: The Drowned Frequency
Asset: [ASSET]
Stage: [proxy / blockout / UE scale test / high poly / low poly / UV / bake / texture / collision / export / UE import]
Target UE dimensions: [DIMENSIONS]
Camera distance: [DISTANCE]
Interaction/animation needs: [NEEDS]
Nanite or LOD decision: [DECISION OR ASK AI TO ANALYZE]

Locked rules:
- 1 Unreal Unit = 1 cm.
- Use an 180 cm human reference.
- Test the blockout in Unreal before detail work.
- Apply transforms before export.
- Use intentional pivots.
- Story text that may change belongs in decals/textures, not modeled geometry.
- Do not create topology detail that normal maps or trim sheets should carry.

Give:
1. Exact Blender setup and units.
2. Primitive-by-primitive construction.
3. Object names and logical separation.
4. Modifier settings.
5. Dimension checks.
6. Stage-specific quality tests.
7. Export settings only if this stage reaches export.
8. Done criteria and one next stage.

Do not proceed beyond the named stage.

## Level/room task envelope

Design or implement exactly one room/build unit.

Unit ID: [ID]
Act/sector: [ACT]
Purpose: [ONE SENTENCE]
Entry state: [STATE]
Exit state: [STATE]
Required mechanics: [LIST]
Required clue plus backup clue: [LIST]
Threat and counterplay: [LIST]
Checkpoint/reset requirements: [LIST]

Follow this order:
1. Graybox dimensions and route.
2. Sightlines and interaction placement.
3. Puzzle state diagram.
4. Failure and reset path.
5. Temporary gameplay audio.
6. Blind-playtest procedure.
7. Final-art/audio gate.

Do not propose final art before the graybox acceptance test passes. Do not place gameplay truth only in a Level Blueprint.

## Review prompt

Review only the diff for task [ID]. Do not implement unrelated improvements.

Check:
- The task's done criteria
- Architecture boundary violations
- Save/load implications
- Controller and accessibility implications
- Soft-lock risk
- Tick/performance risk
- Missing null checks or asset validation
- Tests that claim coverage without exercising behavior
- Unreal lifetime/GC/reference mistakes

Return findings ordered by severity with file and line references. If no findings, say so and identify remaining test gaps.

## Context to append when relevant

Acoustics:
- Exactly seven categories: Voice, Footstep, Machine, Bell/Foghorn, Water, Static, Impact.
- No real microphone capture or waveform analysis.
- Audio playback is presentation; FAcousticStimulus is gameplay truth.
- Slots progress 3→4→5.
- Required signals are always reacquirable.

AI:
- C++ owns state transitions; Behavior Tree executes them.
- No behind-player manifestation without warning.
- Every pursuit has two validated routes.
- After two deaths, persistence or intensity decreases.

Art:
- Wetness follows plausible sources; no uniform gloss.
- Bought assets require project material/weathering and Northpoint signage/cabling passes.
- Lumen, VSM, fog, Niagara, and reflections require scalable fallbacks.

Release:
- Steam comes after packaged local save/load is proven.
- Never claim Windows packaging from a Mac editor run.
- Performance claims require a packaged build on the named test machine.