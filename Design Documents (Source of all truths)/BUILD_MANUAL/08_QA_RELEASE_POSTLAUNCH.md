# The Drowned Frequency — UE5 Solo Game QA, Performance, Packaging, Playtesting, Marketing, Steam Release, and Postlaunch Manual

Target: Windows Steam, 60 FPS mid-range PC, UE5 with Lumen, Nanite, Virtual Shadow Maps, versioned saves. Steam integration arrives late.

Use this manual as a procedural checklist. Execute sections in order unless a hotfix requires the rollback/postlaunch path.

---

## 0. Operating Rules

### 0.1 Release quality gates
A build may become a Release Candidate only if all are true:

- Game is completable from New Game to credits on a clean machine.
- No open Severity 0 or Severity 1 bugs.
- No save corruption bugs open at any severity.
- No reproducible blockers in install, launch, input, graphics settings, audio, subtitles, progression, save/load, quit/relaunch.
- 60 FPS target is met or explained by documented scalability settings on the mid-range target profile.
- Packaging is reproducible from source/control branch and tagged build number.
- Versioned save upgrade tests pass from every supported prior save version.
- Store assets, trailer, capsule images, legal files, credits, and third-party licenses are verified.
- Rollback package and previous public build are retained.

### 0.2 Build naming
Use this format everywhere:

```text
DF-Win64-<semver>-<build_number>-<git_short_sha>-<channel>
```

Examples:

```text
DF-Win64-0.9.0-0042-a1b2c3d-qa
DF-Win64-1.0.0-0100-f00ba7-releasecandidate
DF-Win64-1.0.1-0112-9abc123-hotfix
```

### 0.3 Channels

| Channel | Purpose | Distribution |
|---|---|---|
| dev | Daily development smoke builds | Internal only |
| qa | Feature-complete testing | Internal/local testers |
| playtest | External private playtest | Passworded branch/build |
| rc | Release candidate | Internal + final validation |
| release | Public launch build | Steam default branch |
| hotfix | Emergency patch | Steam beta then default |

### 0.4 Required local folders

Create a release workspace outside the project folder:

```text
ReleaseWorkspace/
  Builds/
  Logs/
  Captures/
  Saves/
  Reports/
  StoreAssets/
  Licenses/
  RC/
  Rollback/
```

### 0.5 Evidence rule
Every pass produces one of:

- PASS with build ID and date.
- FAIL with bug ID.
- BLOCKED with blocker reason.
- N/A with reason.

Never mark a pass complete without evidence: screenshot, log, stat capture, packaged build path, save file, or tester report.

---

## 1. Bug Severity, Triage, and Reporting

### 1.1 Severity definitions

| Severity | Name | Definition | Release impact |
|---|---|---|---|
| S0 | Data loss/security/legal | Save corruption, irreversible progress loss, privacy/legal violation, executable flagged from included bad files | Blocks all release/publishing |
| S1 | Blocker/crash | Crash, hang, cannot launch, cannot complete critical path, missing executable, hard progression blocker | Blocks RC/release |
| S2 | Major | Serious gameplay, performance, accessibility, localization, or UX issue with workaround | Must triage before RC; ship only by explicit waiver |
| S3 | Minor | Visual/audio/UI defect, typo, rare non-blocking bug | Can ship if logged |
| S4 | Polish | Enhancement, subjective polish, low-risk improvement | Backlog |

### 1.2 Priority definitions

| Priority | Meaning |
|---|---|
| P0 | Fix immediately before anything else |
| P1 | Fix before next RC |
| P2 | Fix before launch if feasible |
| P3 | Fix postlaunch/backlog |

Severity describes impact. Priority describes scheduling.

### 1.3 Bug report template

```markdown
# Bug: <short title>

Build: DF-Win64-<version>-<build>-<sha>-<channel>
Platform: Windows <version>, CPU, GPU, RAM, driver version
Severity: S0/S1/S2/S3/S4
Priority: P0/P1/P2/P3
Area: Launch / Save / Progression / Performance / Rendering / Audio / Input / UI / Accessibility / Steam / Packaging / Other
Frequency: Always / Often / Sometimes / Once
Repro rate: <x>/<y>

## Steps to reproduce
1.
2.
3.

## Expected

## Actual

## Evidence
- Log path:
- Screenshot/video:
- Save file:
- Unreal Insights trace:

## Notes

## Fix verification
- Fixed in build:
- Verified by:
- Regression tests run:
```

### 1.4 Daily triage procedure

1. Sort new bugs by Severity, then Priority, then affected player count.
2. Confirm each S0/S1 has reproduction steps or direct owner action to gather them.
3. Reject duplicate bugs only after linking duplicate to original.
4. Require evidence for all crash, save, packaging, and performance bugs.
5. Move fixed bugs to `Ready for Verification`, not `Closed`.
6. Verify fixes on a packaged build when the bug affects packaging/runtime.
7. Reopen immediately if repro still works.

### 1.5 Copy-paste AI prompt — bug triage

```text
You are QA triage for a UE5 Windows Steam solo game called The Drowned Frequency. Classify these bugs by severity and priority using this policy:
S0=data loss/security/legal, S1=crash/blocker/cannot complete, S2=major with workaround, S3=minor, S4=polish. P0=fix now, P1=before next RC, P2=before launch if feasible, P3=backlog.
For each bug, output: ID, severity, priority, release impact, missing evidence, suggested verification test. Do not invent facts. Bugs:
<paste bugs>
```

---

## 2. Test Matrix Overview

### 2.1 Hardware tiers

| Tier | Purpose | Example target | Expected result |
|---|---|---|---|
| Min-spec | Launch and playable at Low/Medium | 4-core CPU, GTX 1060/1650-class GPU, 8-16 GB RAM, SSD | Stable, acceptable FPS with reduced settings |
| Mid-range target | Primary 60 FPS target | 6-core CPU, RTX 2060/3060-class GPU, 16 GB RAM, SSD | 60 FPS average at target settings |
| High-end | Visual validation | RTX 4070+ class, 32 GB RAM, NVMe | High/Epic stable, no rendering regressions |
| Laptop | Thermals/integrated edge | Gaming laptop or iGPU fallback if supported | Graceful settings, no bad defaults |
| Clean machine | Packaging/install validation | Fresh Windows user profile, no dev tools | Installs, launches, saves, uninstalls |

### 2.2 Windows matrix

| OS | Required pass |
|---|---|
| Windows 10 latest supported | Full smoke + progression + packaging |
| Windows 11 latest supported | Full smoke + progression + packaging |
| Non-admin Windows user | Install/launch/save permissions |
| Fresh user profile | First-run settings/save creation |

### 2.3 GPU API/settings matrix

| Area | Values |
|---|---|
| RHI | DX12 primary; DX11 only if explicitly supported |
| Resolution | 1280x720, 1920x1080, 2560x1440, ultrawide if supported |
| Display | Windowed, Borderless, Fullscreen |
| Upscaling | Native, TSR/FSR/DLSS only if implemented |
| VSync | On, Off |
| Frame cap | 30, 60, unlimited if exposed |
| Quality preset | Low, Medium, High, Epic/Custom |
| Lumen | Enabled target path; fallback settings if exposed |
| Nanite | Verify Nanite meshes render correctly |
| VSM | Verify shadow quality/perf and fallback behavior |

### 2.4 Controller/input matrix

| Device | Required tests |
|---|---|
| Keyboard + mouse | Full game completion, rebinding, sensitivity |
| Xbox controller | Menus, gameplay, prompts, disconnect/reconnect |
| PlayStation controller if supported | Prompts/mapping or graceful generic behavior |
| No controller | No input hangs, keyboard defaults usable |
| Hot-swap | Switch KBM/controller at menu and gameplay |

### 2.5 Save matrix

| Scenario | Required result |
|---|---|
| New game creates save | Save appears in intended path |
| Manual save if present | Correct slot/time/location |
| Autosave | Triggered at intended checkpoints only |
| Load after quit | Correct state restored |
| Save during level transition | No corruption/softlock |
| Save version upgrade | Old saves migrate or fail gracefully |
| Missing save | New game works; no crash |
| Corrupt save | Error handling; no crash; backup preserved if applicable |
| Cloud conflict | Later/local/cloud conflict handled or documented |

### 2.6 Accessibility matrix

| Feature | Verification |
|---|---|
| Subtitles | On/off, readable, speaker labels if needed, timing correct |
| Text size | Menus and subtitles readable at 1080p and 1440p |
| Contrast | Critical UI readable over bright/dark scenes |
| Remapping | Required actions remappable or documented |
| Hold/toggle | Sprint/crouch/aim/interact where applicable |
| Photosensitivity | No unsafe flashing; intense effects reviewed |
| Audio cues | Critical audio-only information has visual support where feasible |
| Difficulty/access assists | Options function and persist if present |
| Motion sickness | Camera shake, head bob, FOV, motion blur options if present |
| Color reliance | No critical information by color alone |

---

## 3. Build Preparation and Source Hygiene

### 3.1 Pre-build checklist

1. Sync clean branch/tag intended for build.
2. Confirm project version and build number are updated.
3. Confirm save schema/version constant is updated only when needed.
4. Delete local temporary Derived Data/Intermediate only if troubleshooting; do not rely on dirty artifacts.
5. Confirm no debug cheats/dev menus are exposed in Shipping unless intentionally gated.
6. Confirm logs do not print secrets, local file paths beyond normal logs, or personal data.
7. Confirm third-party plugin licenses are captured.
8. Confirm default maps are correct.
9. Confirm crash reporter/config behavior is intentional.
10. Confirm Steam code paths are disabled/stubbed safely until Steam integration lands.

### 3.2 Unreal Editor project settings to verify

Open Unreal Editor:

```text
Edit > Project Settings
```

Verify:

- Project > Description: Project Displayed Title, Project Version, Company Name.
- Maps & Modes: Game Default Map and Editor Startup Map are correct.
- Packaging:
  - Build Configuration: Shipping for release packages.
  - Full Rebuild: enabled for RC/release.
  - For Distribution: enabled for store/release if required.
  - Include Debug Files: disabled for public release unless crash symbol strategy requires separate symbol upload.
  - Use Pak File / Io Store: intentional and consistent.
  - List of maps to include is complete.
- Platforms > Windows:
  - Targeted RHIs include DirectX 12 if using Lumen/Nanite/VSM.
  - Default RHI is DirectX 12 unless project requires otherwise.
- Engine > Rendering:
  - Lumen settings intentional.
  - Nanite enabled.
  - Virtual Shadow Maps enabled.
  - Anti-aliasing method intentional.
- Input: mappings/rebindings/default controller behavior.
- Audio: subtitles/localization/audio quality.

### 3.3 Command-line package example
Adjust paths for your machine and engine version:

```bat
"C:\Program Files\Epic Games\UE_5.x\Engine\Build\BatchFiles\RunUAT.bat" BuildCookRun ^
-project="C:\Path\To\DrownedFrequency\DrownedFrequency.uproject" ^
-noP4 ^
-platform=Win64 ^
-clientconfig=Shipping ^
-build ^
-cook ^
-allmaps ^
-stage ^
-pak ^
-iostore ^
-archive ^
-archivedirectory="C:\ReleaseWorkspace\Builds\DF-Win64-1.0.0-0100-releasecandidate" ^
-prereqs ^
-utf8output
```

Development QA build example:

```bat
"C:\Program Files\Epic Games\UE_5.x\Engine\Build\BatchFiles\RunUAT.bat" BuildCookRun ^
-project="C:\Path\To\DrownedFrequency\DrownedFrequency.uproject" ^
-noP4 ^
-platform=Win64 ^
-clientconfig=Development ^
-build ^
-cook ^
-allmaps ^
-stage ^
-pak ^
-archive ^
-archivedirectory="C:\ReleaseWorkspace\Builds\DF-Win64-0.9.0-0042-qa" ^
-utf8output
```

### 3.4 Packaging log checks
Search packaging output for:

```text
Error:
Warning:
Ensure condition failed
Unknown Cook Failure
Missing package
LogLinker: Warning
LogUObjectGlobals: Warning
Plugin
```

Fail build on:

- Any packaging error.
- Any missing required map/asset.
- Any Blueprint compile error.
- Any missing plugin dependency.
- Any unintentional editor-only content reference.

### 3.5 Copy-paste AI prompt — packaging log review

```text
You are reviewing an Unreal Engine 5 Windows Shipping packaging log for The Drowned Frequency. Identify release-blocking errors, risky warnings, missing assets/maps/plugins, Blueprint compile problems, cook failures, and suspicious editor-only references. Output a table with: log excerpt, severity, likely cause, recommended fix, whether it blocks RC. Do not treat all warnings as equal. Here is the log:
<paste log>
```

---

## 4. Smoke Test Pass

Run this on every packaged build before deeper testing.

### 4.1 Smoke test checklist

| ID | Test | Expected |
|---|---|---|
| SMK-001 | Launch executable from packaged folder | Game opens without crash |
| SMK-002 | First-run main menu | Correct version/build visible if shown; no missing UI |
| SMK-003 | Change graphics preset | Setting applies and persists after restart |
| SMK-004 | Change resolution/window mode | Applies, recoverable, persists |
| SMK-005 | Change audio volumes | Applies and persists |
| SMK-006 | Change subtitles/accessibility options | Applies and persists |
| SMK-007 | Start New Game | Loads expected first playable scene |
| SMK-008 | Play first 10 minutes | No blockers/crashes; core loop works |
| SMK-009 | Save/autosave trigger | Save created |
| SMK-010 | Quit to desktop | Clean exit |
| SMK-011 | Relaunch and Continue/Load | Correct state restored |
| SMK-012 | Controller navigation | Menus and gameplay functional |
| SMK-013 | Alt+Tab/minimize/restore | No device loss/hang/audio failure |
| SMK-014 | Credits/legal menu | Correct and readable |

### 4.2 Smoke evidence
Record:

```text
Build ID:
Machine:
OS:
GPU/driver:
Tester:
Date:
Result: PASS/FAIL/BLOCKED
Bugs filed:
Save path checked:
Log path:
```

### 4.3 Copy-paste AI prompt — smoke report summary

```text
Summarize this smoke test report for a UE5 Windows packaged build. Identify whether it can proceed to full QA, list blocking failures, and suggest next tests. Keep it concise and do not invent missing data.
Report:
<paste report>
```

---

## 5. Full Functional QA Passes

### 5.1 Critical path completion pass

Goal: complete the entire game from New Game to credits on packaged build.

Procedure:

1. Start from no existing saves.
2. Record build ID, OS, hardware, settings preset, input device.
3. Start New Game.
4. Play naturally without using editor/dev cheats.
5. At each chapter/major location, record:
   - timestamp,
   - objective state,
   - save slot/time,
   - FPS impression or measured stats if needed,
   - bugs encountered.
6. Quit and reload once per chapter/major location.
7. Complete final sequence and credits.
8. Relaunch and verify post-completion state.

Pass criteria:

- No crash/hang.
- No progression blocker.
- No irreversible save/progression loss.
- Objectives, triggers, interactions, inventory/state, audio, subtitles, UI work across reloads.

### 5.2 Chapter/level regression matrix

Create one row per chapter/level:

| Chapter/Level | Fresh entry | Load from save | Objective works | Required items | Enemies/events | Cinematics | Subtitles | Exit transition | Performance | Bugs |
|---|---|---|---|---|---|---|---|---|---|---|
| Prologue | | | | | | | | | | |
| Chapter 1 | | | | | | | | | | |
| Chapter 2 | | | | | | | | | | |
| Finale | | | | | | | | | | |

### 5.3 Interaction pass

Test every interaction type:

- Doors/locks/keys.
- Pickups/inspectables.
- Dialogue/calls/radio/audio logs.
- Puzzles.
- Inventory/equipment.
- Checkpoints/save points.
- Ladders/stairs/elevators/teleports.
- Menus/settings/pause.
- Fail states/death/retry if present.

For each:

```text
Interaction:
Location:
Input method:
Expected:
Actual:
Reload behavior:
Bug ID:
```

### 5.4 Negative/input abuse pass

Try:

- Mash interact during transitions.
- Pause during cutscenes, dialogue, death, loading, objective update.
- Alt+Tab during loading and cinematics.
- Disconnect controller mid-action.
- Change resolution during gameplay and menus.
- Spam save/load if UI permits.
- Quit immediately after autosave.
- Load oldest save after reaching later chapter.
- Use unexpected route/backtracking.

### 5.5 UI/options pass

| Area | Tests |
|---|---|
| Main menu | New/Continue/Load/Options/Credits/Quit |
| Pause menu | Resume/Options/Load/Quit confirmations |
| Graphics | Presets, resolution, window mode, VSync, cap, brightness/gamma if present |
| Audio | Master/music/SFX/dialogue sliders, mute edge cases |
| Controls | Rebind, restore default, controller prompts, sensitivity/invert |
| Accessibility | Subtitles, toggles, text/readability, camera effects |
| Save/load UI | Slots, timestamps, overwrite prompts, corrupt/missing saves |

### 5.6 Copy-paste AI prompt — test case generation

```text
Create granular UE5 packaged-build QA test cases for this game area in The Drowned Frequency. Format as a table with ID, setup, steps, expected result, severity if failed, evidence to capture. Include normal, edge, reload, and input-abuse cases. Area:
<describe area/chapter/system>
```

---

## 6. Save System, Versioning, and Steam Cloud Readiness

### 6.1 Save path documentation
Document actual save location after implementation. Common UE Windows path:

```text
%LOCALAPPDATA%\<ProjectName>\Saved\SaveGames
```

Also inspect:

```text
%LOCALAPPDATA%\<ProjectName>\Saved\Config\Windows
%LOCALAPPDATA%\<ProjectName>\Saved\Logs
```

### 6.2 Save schema/version rules

Each save should include:

- Save format version integer.
- Game build/version that wrote it.
- Player chapter/location.
- Timestamp.
- Optional checksum or validation marker.
- Migration path or explicit unsupported behavior.

Rules:

- Never silently overwrite an unsupported future-version save.
- Never delete corrupt saves without backup/quarantine.
- Save writes should be atomic where feasible: write temp, validate, then replace.
- Version migration must be tested with real old save files.

### 6.3 Save test matrix

| ID | Scenario | Steps | Expected |
|---|---|---|---|
| SAV-001 | First save creation | Delete save folder, start game, reach autosave | Save file created; no error |
| SAV-002 | Continue | Relaunch, click Continue | Latest valid save loads |
| SAV-003 | Manual load | Load specific slot | Correct state restored |
| SAV-004 | Settings persistence | Change options, relaunch | Settings persist separately/correctly |
| SAV-005 | Mid-transition quit | Quit after autosave trigger | Load returns to safe state |
| SAV-006 | Corrupt save | Modify save bytes/copy invalid file | Game handles gracefully; no crash |
| SAV-007 | Missing folder | Delete SaveGames folder | Game recreates folder |
| SAV-008 | Read-only folder | Make save folder read-only | Error/fallback graceful; no crash |
| SAV-009 | Old version migration | Place v1 save, launch new build | Migrates correctly or clear unsupported message |
| SAV-010 | Future version | Place newer-version save | Does not corrupt; warns/fails gracefully |
| SAV-011 | Full slots | Fill max slots if applicable | UI handles limit correctly |
| SAV-012 | Unicode Windows username | Test path with non-ASCII user if possible | Save/load works |

### 6.4 Manual corrupt save test

1. Back up a valid save file.
2. Copy it to a test slot.
3. Open copy in a hex editor or text editor depending format.
4. Change bytes near the middle.
5. Launch game.
6. Attempt to load corrupted slot.
7. Expected: no crash; error message or hidden invalid slot; valid slots preserved.

### 6.5 Steam Cloud planning
When Steam arrives, configure Steam Cloud only for the intended save/config files.

Recommended include patterns to confirm later:

```text
%LOCALAPPDATA%\<ProjectName>\Saved\SaveGames\*.sav
```

Usually avoid syncing:

```text
Saved\Logs\*
Saved\Crashes\*
Saved\DerivedDataCache\*
Large temporary files
Machine-specific graphics config if it causes cross-machine problems
```

### 6.6 Steam Cloud test matrix after integration

| ID | Scenario | Steps | Expected |
|---|---|---|---|
| CLD-001 | Upload | Play on Machine A, create save, exit Steam/game | Cloud sync completes |
| CLD-002 | Download | Fresh Machine B install, same account | Save appears and loads |
| CLD-003 | Conflict local newer | Make local save offline, remote older, reconnect | Steam conflict behavior understood; no corruption |
| CLD-004 | Conflict cloud newer | Cloud save newer than local | Correct player choice or Steam default behavior documented |
| CLD-005 | Delete local | Delete local save, relaunch online | Cloud redownload or intended behavior |
| CLD-006 | Offline play | Disable internet, play/save, reconnect | Sync succeeds; no lost progress |
| CLD-007 | Version migration via cloud | Old cloud save downloads to new build | Migration works |
| CLD-008 | Cross-machine settings | Different resolutions/GPU | No bad graphics settings forced if config synced |

### 6.7 Copy-paste AI prompt — save migration review

```text
You are reviewing a UE5 save-version migration plan for The Drowned Frequency. Check for data-loss risks, missing old-version coverage, Steam Cloud conflict risks, atomic-write issues, corrupt-save behavior, and test gaps. Output required fixes before RC and suggested tests. Plan:
<paste plan/code summary/test results>
```

---

## 7. Performance and Profiling Manual

Target: 60 FPS on mid-range PC at the chosen target settings.

### 7.1 Performance build types

Use packaged Development builds for detailed profiling commands when Shipping blocks console/stats. Use Shipping builds for final user-facing FPS validation.

### 7.2 Performance capture setup

For each capture record:

```text
Build ID:
Build config: Development/Shipping
Map/location:
Scenario:
Resolution:
Window mode:
Graphics preset:
RHI:
Hardware:
GPU driver:
Command line args:
Capture tools:
Average FPS:
1% low if measured:
Game thread ms:
Render thread ms:
GPU ms:
VRAM estimate:
Bugs/perf notes:
```

### 7.3 Useful launch args

```bat
DrownedFrequency.exe -log
DrownedFrequency.exe -dx12 -log
DrownedFrequency.exe -ResX=1920 -ResY=1080 -WINDOWED -log
DrownedFrequency.exe -FULLSCREEN -ResX=1920 -ResY=1080 -log
DrownedFrequency.exe -NoVSync -log
DrownedFrequency.exe -trace=cpu,gpu,frame,bookmark,loadtime,file,net -statnamedevents
```

Use `-dx11` only if the game explicitly supports it. Lumen/Nanite/VSM target path is normally DX12.

### 7.4 Unreal stat console commands
Open console in Development build and run:

```text
stat fps
stat unit
stat unitgraph
stat gpu
stat scenerendering
stat rhi
stat memory
stat streaming
stat virtualshadowmap
stat nanite
stat lumen
stat game
stat slate
stat audio
stat levels
```

Interpretation:

- 60 FPS frame budget: 16.67 ms.
- If Game thread exceeds 16.67 ms: gameplay/Blueprint/AI/tick issue likely.
- If Draw/Render thread exceeds 16.67 ms: scene complexity/render submission likely.
- If GPU exceeds 16.67 ms: rendering/GPU-bound issue likely.
- If hitches occur with average FPS fine: investigate streaming, shader compilation, garbage collection, save writes, level loads.

### 7.5 Profiling commands

#### Quick FPS/stat capture

```text
stat fps
stat unit
stat gpu
```

#### Start/stop stat file

```text
stat startfile
<play scenario for 60-120 seconds>
stat stopfile
```

Then open the `.ue4stats`/stats file in Unreal Session Frontend Profiler if available.

#### CPU/GPU trace for Unreal Insights
Launch with:

```bat
DrownedFrequency.exe -trace=cpu,gpu,frame,bookmark,loadtime,file -statnamedevents -log
```

Or from console if available:

```text
Trace.Start cpu,gpu,frame,bookmark,loadtime,file
<play scenario>
Trace.Stop
```

Open trace in Unreal Insights:

```text
Engine/Binaries/Win64/UnrealInsights.exe
```

#### CSV profiler

```text
csvprofile start
<play scenario for 60-120 seconds>
csvprofile stop
```

CSV files commonly appear under:

```text
<Project>/Saved/Profiling/CSV/
%LOCALAPPDATA%\<ProjectName>\Saved\Profiling\CSV\
```

#### GPU visualizer
Development/editor only:

```text
profilegpu
```

### 7.6 Rendering feature checks

#### Lumen
Commands/tools:

```text
stat lumen
r.Lumen.ScreenProbeGather 1
r.Lumen.Reflections.Allow 1
```

Editor view modes:

```text
Lit
Lumen Scene
Lumen Reflections
```

Check:

- Indirect lighting stable in dark interiors.
- Reflections do not flicker distractingly.
- No massive black cards/light leaks in critical scenes.
- Performance cost acceptable per scene.

#### Nanite
Commands/tools:

```text
stat nanite
r.Nanite.Visualize.Advanced 1
```

Editor viewport:

```text
Nanite Visualization > Triangles / Clusters / Overdraw
```

Check:

- Nanite meshes render in packaged build.
- Foliage/transparent assets use appropriate non-Nanite fallback if needed.
- No missing collision due to Nanite assumptions.
- No severe overdraw/material cost hidden by geometry efficiency.

#### Virtual Shadow Maps
Commands/tools:

```text
stat virtualshadowmap
r.Shadow.Virtual.ShowStats 1
```

Check:

- Shadow cache invalidation not excessive.
- No crawling/flickering shadows in hero scenes.
- Contact shadows/readability acceptable.
- Dynamic lights are budgeted.

### 7.7 Scalability commands

Test all presets:

```text
sg.ViewDistanceQuality 0
sg.AntiAliasingQuality 0
sg.ShadowQuality 0
sg.GlobalIlluminationQuality 0
sg.ReflectionQuality 0
sg.PostProcessQuality 0
sg.TextureQuality 0
sg.EffectsQuality 0
sg.FoliageQuality 0
sg.ShadingQuality 0
```

Then repeat for `1`, `2`, `3`.

Apply all low:

```text
sg.ViewDistanceQuality 0 | sg.AntiAliasingQuality 0 | sg.ShadowQuality 0 | sg.GlobalIlluminationQuality 0 | sg.ReflectionQuality 0 | sg.PostProcessQuality 0 | sg.TextureQuality 0 | sg.EffectsQuality 0 | sg.FoliageQuality 0 | sg.ShadingQuality 0
```

Apply high/epic depending project mapping:

```text
sg.ViewDistanceQuality 3 | sg.AntiAliasingQuality 3 | sg.ShadowQuality 3 | sg.GlobalIlluminationQuality 3 | sg.ReflectionQuality 3 | sg.PostProcessQuality 3 | sg.TextureQuality 3 | sg.EffectsQuality 3 | sg.FoliageQuality 3 | sg.ShadingQuality 3
```

### 7.8 Performance scenario matrix

| ID | Scenario | Duration | Required capture |
|---|---|---|---|
| PERF-001 | Main menu idle | 60s | FPS/unit |
| PERF-002 | First playable area idle | 60s | FPS/unit/gpu |
| PERF-003 | Dense interior with Lumen | 120s | stat gpu + Insights |
| PERF-004 | Largest vista/high Nanite scene | 120s | stat nanite + gpu |
| PERF-005 | Heavy shadow/dynamic lights scene | 120s | VSM stats + gpu |
| PERF-006 | Combat/chase/high interaction if present | 120s | unitgraph + Insights |
| PERF-007 | Puzzle/object interaction sequence | 120s | stat game/slate if UI heavy |
| PERF-008 | Level transition/loading | transition | loadtime trace |
| PERF-009 | Autosave moment | 30s before/after | unitgraph/hitch note |
| PERF-010 | Endgame/finale | 120s | FPS/unit/gpu |

### 7.9 Hitch pass

Procedure:

1. Launch packaged Development build with trace args.
2. Run `stat unitgraph`.
3. Traverse each major level at sprint/walk speed.
4. Trigger known streaming, autosave, cinematics, and large UI transitions.
5. Mark any visible hitch > 100 ms.
6. Open trace in Unreal Insights.
7. Identify whether hitch is asset load, shader compilation, GC, save write, Blueprint spike, audio load, or level streaming.

### 7.10 Performance acceptance thresholds

| Metric | Mid-range target |
|---|---|
| Average FPS | 60+ in normal play at target settings |
| Frame time | Mostly <=16.67 ms |
| Short hitches | Rare, non-gameplay-critical |
| Long hitches >250 ms | Not acceptable during gameplay unless loading screen |
| GPU-bound scenes | Must have scalable options |
| CPU-bound scenes | No runaway ticks/Blueprint loops |
| VRAM | No severe streaming pool exhaustion at target texture quality |

### 7.11 Copy-paste AI prompt — performance analysis

```text
You are analyzing UE5 performance for The Drowned Frequency, a Windows Steam game targeting 60 FPS on mid-range hardware using Lumen, Nanite, and Virtual Shadow Maps. Given these stat unit/stat gpu/Insights notes, identify whether the bottleneck is Game thread, Render thread, GPU, streaming, VSM, Nanite, Lumen, Slate/UI, audio, or save I/O. Output prioritized fixes and what to measure after each fix. Data:
<paste measurements/logs>
```

---

## 8. Accessibility Verification

### 8.1 Accessibility pass procedure

1. Start from fresh settings.
2. Inspect every menu at 1080p and 1440p.
3. Enable subtitles and play every major dialogue/cinematic/radio/audio log type.
4. Turn music to 0 and verify critical information is not music-only.
5. Turn SFX/dialogue down/up and verify sliders work.
6. Use keyboard only for menus and gameplay.
7. Use controller only for menus and gameplay.
8. Rebind common actions if supported.
9. Toggle hold/toggle settings if present.
10. Review intense VFX, flicker, lightning, glitch, emergency lights, water reflections, screen distortion.
11. Verify camera shake/head bob/motion blur/FOV options if present.
12. Verify color is not the only state indicator for puzzles/objectives.
13. Save/relaunch and confirm accessibility settings persist.

### 8.2 Subtitles checklist

| Check | Pass criteria |
|---|---|
| All spoken critical dialogue subtitled | No missing key lines |
| Timing | Appears before/with audio, remains readable |
| Speaker clarity | Speaker label or context clear where needed |
| Contrast | Readable over bright/dark backgrounds |
| Size | Readable from couch distance at 1080p if target includes controller play |
| No overlap | Does not overlap prompts/objectives badly |
| Persistence | Setting persists after restart |

### 8.3 Photosensitivity review

Flag scenes with:

- Flashing lights.
- Rapid full-screen color changes.
- Strobing emissive materials.
- Glitch effects.
- Lightning/electrical bursts.
- High-contrast flicker during horror sequences.

Mitigations:

- Reduce frequency/intensity.
- Add effect intensity toggle.
- Add content warning if still present.
- Avoid sustained flashes in risky frequency bands.

### 8.4 Copy-paste AI prompt — accessibility audit

```text
Audit this accessibility feature list and QA results for The Drowned Frequency. Identify missing verification, launch risks, and specific tests for subtitles, input remapping, motion sickness, photosensitivity, contrast, audio-only cues, and settings persistence. Output must separate blockers from recommended improvements.
Data:
<paste feature list/results>
```

---

## 9. Clean-Machine Packaging and Install Validation

### 9.1 Clean machine definition
A clean machine/profile has:

- No Unreal Editor installed.
- No Visual Studio required to run the game.
- No project files or source tree.
- Fresh Windows user profile if possible.
- Standard user account test, not only admin.
- No existing game saves/config unless intentionally testing migration.

### 9.2 Clean-machine test procedure

1. Copy packaged archive or installer to clean machine.
2. Extract/install to a normal user folder such as:

```text
C:\Games\DrownedFrequency
```

3. Launch executable as standard user.
4. If prerequisites are required, verify installer includes/runs them or game gives clear failure.
5. Start New Game.
6. Reach first save.
7. Quit.
8. Verify logs/save/config are written to user-writable location, not install directory unless intentional.
9. Relaunch and Continue.
10. Uninstall/delete game folder.
11. Verify save retention/deletion behavior matches intent.

### 9.3 Dependency checks

Fail if:

- Game requires editor DLLs.
- Game requires Visual Studio installed.
- Missing plugin runtime DLLs.
- Missing DirectX/VC runtime prerequisites without installer/clear requirement.
- Game writes saves only to protected install directory.
- Launch path contains spaces/unicode and breaks.

### 9.4 Path tests

Install/extract to:

```text
C:\Games\DrownedFrequency
C:\Games\The Drowned Frequency
C:\Users\<User>\Desktop\The Drowned Frequency
C:\测试\DrownedFrequency
```

Expected: launch works or unsupported path is documented. Unicode path support is preferred.

### 9.5 Antivirus/smart screen sanity

- Scan packaged folder with local antivirus.
- Confirm no unexpected executables/scripts are shipped.
- Confirm crash reporter/prereq installers are expected.
- Keep code signing plan if applicable.

### 9.6 Copy-paste AI prompt — clean-machine report review

```text
Review this clean-machine install test for a UE5 Windows packaged game. Identify whether the build is safe for RC, what dependency or permission risks remain, and which failures block release. Report:
<paste report>
```

---

## 10. Playtesting Program

### 10.1 Playtest phases

| Phase | Size | Goal | Build channel |
|---|---:|---|---|
| Internal sanity | 1-3 | Find obvious blockers | qa |
| Guided playtest | 3-5 | Observe comprehension/core loop | playtest |
| Blind playtest | 5-15 | Real player progression/friction | playtest |
| RC validation | 2-5 | Confirm fixes/no regressions | rc |

### 10.2 Tester instructions

Give testers:

- Build ID.
- Install instructions.
- Known issues list.
- What to avoid spoiling publicly.
- How to report bugs.
- Where saves/logs are located.
- Expected session length.
- Survey link/document.
- Consent/permission for recording if applicable.

### 10.3 Blind playtest rules

Do not explain puzzle solutions or controls beyond what a real player sees. Observe:

- Where they hesitate.
- Where they misread objective/UI.
- Where they get lost.
- Where fear/tension lands or fails.
- Where performance dips.
- Whether they understand saving.
- Whether accessibility/settings are discovered.

### 10.4 Playtest observation template

```markdown
# Playtest Observation

Build:
Tester alias:
Date:
Hardware:
Input:
Session length:
Prior genre familiarity:

## Timeline notes
00:00 -
05:00 -
10:00 -

## Completion/progression
- Furthest point reached:
- Blockers:
- Hints needed:

## Comprehension
- Objective clarity:
- Controls clarity:
- Puzzle clarity:
- Story clarity:

## UX/friction

## Bugs observed

## Performance notes

## Accessibility notes

## Best moments

## Worst moments

## Follow-up questions
```

### 10.5 Post-playtest survey

Ask:

1. Did you know what to do most of the time? Where not?
2. Did controls feel comfortable?
3. Did anything feel unfair, broken, or confusing?
4. Did you experience crashes, freezes, or severe stutters?
5. Did saving/loading behave as expected?
6. Did subtitles/text/audio work for you?
7. What was the strongest moment?
8. What was the weakest moment?
9. Would you keep playing/buy/recommend? Why?
10. Hardware specs and graphics settings used.

### 10.6 Playtest analysis procedure

1. Group notes by theme: blocker, confusion, pacing, controls, performance, accessibility, narrative, bugs.
2. Count frequency per issue.
3. Separate bugs from subjective feedback.
4. Mark release blockers.
5. Convert top issues into tickets with evidence.
6. Run regression tests after fixes.

### 10.7 Copy-paste AI prompt — playtest synthesis

```text
Synthesize these playtest notes for The Drowned Frequency. Group issues by theme, count repeated problems, separate bugs from design feedback, identify release blockers, and propose the smallest actionable fixes. Do not overfit to one tester unless the issue is severe. Notes:
<paste notes>
```

---

## 11. Marketing and Store Asset Production

### 11.1 Asset inventory
Create and track:

| Asset | Purpose | Status | Notes |
|---|---|---|---|
| Game logo | Capsules/trailer/end card | | |
| Key art | Capsules/store/social | | |
| Steam capsule set | Store | | |
| Screenshots | Store | | |
| Trailer | Store/social | | |
| Short clips/GIFs | Social/press | | |
| Description short/long | Store | | |
| Feature bullets | Store | | |
| Press kit | Media/creators | | |
| Credits/legal | Game/store | | |
| Content warnings | Store/game if needed | | |

### 11.2 Trailer sequence
Recommended launch trailer structure for solo atmospheric game:

1. 0-3s: Immediate hook image/audio; game title mood implied, not logo-only.
2. 3-8s: Premise line or visual mystery.
3. 8-20s: Exploration and atmosphere.
4. 20-35s: Core mechanics/interactions/puzzles/threats.
5. 35-50s: Escalation, stakes, standout moments.
6. 50-60s: Review quote/feature beats if available, otherwise strongest montage.
7. Final 3-5s: Title, release timing, Steam wishlist/available now.

Rules:

- Show real gameplay/captured in-engine footage.
- Avoid long black fades.
- Avoid UI clutter unless UI is part of gameplay.
- Capture at stable high settings but do not misrepresent gameplay.
- Include captions/subtitles for any spoken lines.
- Export store/social variants.

### 11.3 Trailer capture checklist

| Shot type | Need |
|---|---|
| Establishing mood shot | 2-3 options |
| Player interaction | 3-5 shots |
| Puzzle/system shot | 2-4 shots |
| Threat/tension shot | 3-5 shots |
| Lumen/Nanite visual showcase | 2-4 shots |
| Story/mystery beat | 2-3 shots |
| End card | 1 |

Capture notes:

- Use packaged or standalone build where possible.
- Disable debug overlays.
- Use stable resolution/framerate.
- Avoid spoilers unless trailer strategy allows.
- Keep raw captures organized by build/date/location.

### 11.4 Steam screenshot sequence
Use 6-10 screenshots. Suggested order:

1. Strongest visual hook.
2. Core exploration/gameplay.
3. Main interaction/puzzle mechanic.
4. Atmospheric environment variety.
5. Tension/threat/story moment.
6. UI-light feature shot.
7. Accessibility/readable subtitle shot if beneficial.
8. Late-game visual without major spoiler.

Rules:

- No editor viewport shots.
- No debug text/stat overlays.
- No misleading supersampling beyond what players can reasonably see.
- Avoid screenshots that all look identical/dark.
- Check readability at Steam thumbnail size.

### 11.5 Store copy structure

Short description:

```text
A concise 1-2 sentence hook explaining genre, fantasy, and standout mechanic/mood.
```

Long description:

```markdown
## Enter the signal
<premise paragraph>

## What you do
- Explore <spaces/theme>
- Solve <type> puzzles
- Uncover <mystery/story>
- Survive/experience <threat/tension if applicable>

## Built for atmosphere
<visual/audio/technical mood>

## Content note
<if applicable>
```

Feature bullet rules:

- Concrete beats, not vague praise.
- Avoid unsupported claims like “revolutionary”.
- Mention single-player, controller support, subtitles, Steam Cloud only after implemented.

### 11.6 Press kit contents

```text
PressKit/
  README.txt
  Factsheet.pdf or .md
  Logos/
  KeyArt/
  Screenshots/
  TrailerLinks.txt
  GIFs/
  AboutDeveloper.txt
  Contact.txt
  CreditsAndLicenses.txt
```

Factsheet:

- Title.
- Developer/publisher.
- Platforms.
- Release date/window.
- Price if set.
- Website/store link.
- Genre.
- Description.
- Features.
- Trailer link.
- Contact.

### 11.7 Copy-paste AI prompt — store page critique

```text
Critique this Steam store copy and asset plan for The Drowned Frequency. Check clarity, genre expectations, misleading claims, missing player-facing features, trailer/screenshot order, accessibility/content warning needs, and wishlist conversion. Output specific edits, not generic marketing advice. Copy/assets:
<paste store copy and asset list>
```

---

## 12. Legal, License, Credits, and Compliance Checks

### 12.1 Third-party inventory
Track every external asset/tool/plugin:

| Item | Type | Source URL/vendor | License | Proof file | In-game credit required? | Store credit required? | Redistribution allowed? | Notes |
|---|---|---|---|---|---|---|---|---|

Include:

- Marketplace assets.
- Fonts.
- Music/SFX.
- Plugins.
- Code libraries.
- Textures/materials/models.
- Icons/UI assets.
- Trailer music/fonts/assets.
- AI-generated assets if used, with provenance and rights notes.

### 12.2 License verification procedure

1. List all non-original assets.
2. Locate license terms for each.
3. Save proof PDF/text/screenshot in `ReleaseWorkspace/Licenses`.
4. Confirm commercial use allowed.
5. Confirm redistribution in packaged game allowed.
6. Confirm attribution requirements.
7. Confirm modification requirements.
8. Confirm no asset has incompatible license.
9. Confirm trailer/store use is allowed, not only in-game use.
10. Confirm credits include required names/notices.

### 12.3 Credits checklist

- Developer name/studio.
- Contractors/collaborators.
- Voice actors if any.
- Music/SFX attribution.
- Fonts.
- Plugins/middleware.
- Marketplace assets requiring credit.
- Special thanks.
- Unreal Engine trademark/required notices if applicable.
- Open-source notices.

### 12.4 Privacy/data checklist

Confirm whether game collects or transmits any data:

- Analytics.
- Crash reports.
- Telemetry.
- Steam stats/achievements.
- Player identifiers.
- Logs containing usernames/system paths.

If any data leaves the player machine, prepare privacy policy and store disclosure. If no data leaves except platform services, document that.

### 12.5 Content rating/content warning checklist

Review for:

- Violence/gore.
- Horror/intense fear.
- Language.
- Substance references.
- Self-harm/suicide themes.
- Flashing lights/photosensitivity.
- Mature themes.

Store page and in-game warnings should be accurate without spoiling more than necessary.

### 12.6 Copy-paste AI prompt — license audit

```text
Audit this third-party asset/license inventory for a commercial Steam release of The Drowned Frequency. Identify missing license proof, attribution requirements, redistribution risks, trailer/store usage risks, incompatible licenses, and credits/legal text gaps. Do not provide legal advice; provide practical release blockers to investigate. Inventory:
<paste inventory>
```

---

## 13. Steam Release Preparation

Steam integration comes late, so separate pre-Steam readiness from Steam-specific validation.

### 13.1 Pre-Steam readiness
Before Steam SDK is integrated:

- Game launches outside Steam.
- Save path documented.
- Version/build visible in logs or menu.
- No hard dependency on Steam for single-player launch unless intentional later.
- Placeholder achievements/stats code disabled safely.
- Store assets prepared.
- Legal and content warning text prepared.

### 13.2 Steamworks setup checklist after access/integration

Verify in Steamworks:

- App name and capsule assets.
- Store page copy and screenshots.
- Trailer uploaded/processed.
- System requirements.
- Supported languages.
- Controller support flags.
- Single-player tags/genre tags.
- Mature content survey.
- Release date/wishlist visibility.
- Pricing and packages.
- Depots for Windows build.
- Default branch and beta branches.
- Steam Cloud paths.
- Achievements/stats if implemented.
- Community assets.
- Review build upload status.

### 13.3 Steam SDK functional test matrix

| ID | Test | Expected |
|---|---|---|
| STM-001 | Launch from Steam client | Game launches |
| STM-002 | Launch executable directly | Intended behavior documented; no crash |
| STM-003 | Steam overlay | Opens/closes without input lock if enabled |
| STM-004 | Controller via Steam Input | Correct prompts/mapping or documented behavior |
| STM-005 | Steam Cloud upload/download | Saves sync correctly |
| STM-006 | Achievements if present | Unlock once, persist, no debug spam |
| STM-007 | Offline mode | Game launches/saves if intended |
| STM-008 | Branch switching | Saves survive or migration works |
| STM-009 | Uninstall/reinstall | Saves/cloud behavior as expected |
| STM-010 | Steam Deck if unsupported/supported | Label honestly; basic launch if claiming support |

### 13.4 Steam build upload concept
Use SteamPipe/SteamCMD only when ready. Keep local manifest/build scripts versioned but do not upload until release authority is explicit.

Typical content prep:

```text
steamworks_sdk/tools/ContentBuilder/content/<app_id>/WindowsNoEditor/...
steamworks_sdk/tools/ContentBuilder/scripts/app_build_<app_id>.vdf
steamworks_sdk/tools/ContentBuilder/scripts/depot_build_<depot_id>.vdf
```

Validation before upload:

- Correct executable path.
- Correct depot content root.
- No source files, debug captures, licenses-only working docs, personal files.
- Prereqs included if needed.
- Build ID matches packaged game.

### 13.5 Steam store final review

Before pressing release:

- Store page visible in preview and reads correctly.
- Trailer plays with correct thumbnail.
- Capsules not cropped badly.
- Screenshots are ordered correctly.
- Tags match actual game.
- System requirements match tested hardware.
- Content warnings accurate.
- Release date/time correct.
- Price/discount correct.
- Cloud/achievements/controller claims only if verified.
- Legal name/tax/banking Steamworks setup complete.

### 13.6 Copy-paste AI prompt — Steam readiness review

```text
Review this Steam release checklist/status for The Drowned Frequency. Identify claims that are not yet verified, Steam Cloud/save risks, depot/build risks, store page risks, and blockers before release. Output a go/no-go recommendation with required fixes. Status:
<paste status>
```

---

## 14. Release Candidate Procedure

### 14.1 RC creation steps

1. Freeze content except approved blocker fixes.
2. Update version/build number.
3. Package Shipping build with full rebuild.
4. Archive build with exact build ID.
5. Save packaging log.
6. Create checksum file if desired:

```bat
certutil -hashfile DrownedFrequency.exe SHA256
```

7. Run smoke test.
8. Run clean-machine install test.
9. Run full critical path completion.
10. Run save/version/cloud readiness tests.
11. Run performance target pass.
12. Run accessibility pass.
13. Run legal/license/store asset review.
14. Triage all findings.
15. If any S0/S1 or unwaived critical release risk: reject RC.
16. If accepted: tag as RC and move to release approval.

### 14.2 RC acceptance checklist

| Gate | Required result | Status |
|---|---|---|
| Packaging | Shipping package succeeds without blocking warnings | |
| Smoke | Pass on Win10/Win11 target machines | |
| Clean machine | Launch/save/load as standard user | |
| Critical path | Game completable | |
| Save/versioning | No corruption; migration passes | |
| Steam Cloud | Pass after integration, or not claimed/enabled | |
| Performance | 60 FPS target validated or settings documented | |
| Accessibility | Required features verified | |
| Legal/licenses | No unknown rights | |
| Store assets | Final and accurate | |
| Rollback | Previous known-good build retained | |
| Bug status | No S0/S1; S2 waived/fixed | |

### 14.3 RC rejection rules
Reject RC if:

- Any reproducible crash/blocker.
- Any save corruption/progress loss.
- Critical path incomplete.
- Clean machine cannot launch.
- Packaged build differs materially from tested build.
- Store/legal claims are false or unverified.
- Steam Cloud corrupts/misplaces saves.
- Performance falls far below target without acceptable settings fallback.

### 14.4 Copy-paste AI prompt — RC go/no-go

```text
You are release manager for The Drowned Frequency. Given this RC checklist, bug list, performance report, save/cloud report, clean-machine report, and legal/store status, decide GO / NO-GO / GO WITH WAIVERS. List blockers, required fixes, allowed waivers, and exact verification needed after fixes. Do not invent missing evidence.
Data:
<paste RC packet>
```

---

## 15. Rollback Plan

### 15.1 Rollback prerequisites
Before every public release/hotfix:

- Keep previous public build package.
- Keep previous Steam depot/build ID.
- Keep previous save schema documentation.
- Know whether new build migrates saves irreversibly.
- If save migration is irreversible, create backup/migration safeguard before release.
- Prepare announcement template for rollback/hotfix if needed.

### 15.2 Rollback triggers
Rollback immediately or hotfix through beta if:

- Widespread launch crash.
- Save corruption/progression loss.
- Major Steam Cloud conflict deleting/overwriting saves.
- Public build missing executable/content.
- Severe performance regression for large player segment.
- Legal/compliance takedown issue.

### 15.3 Rollback procedure

1. Stop further promotion of bad build.
2. Confirm issue is in public build and not isolated setup.
3. Preserve logs/reports/save files from affected users.
4. Switch Steam default branch back to previous known-good build if public impact is severe.
5. Post concise player notice if needed:
   - issue acknowledged,
   - rollback/hotfix status,
   - save safety instructions,
   - where to send logs/saves.
6. Reproduce internally.
7. Fix on hotfix branch.
8. Package hotfix build.
9. Run targeted regression + smoke + save/cloud tests.
10. Upload to beta branch first if possible.
11. Promote to default after validation.
12. Keep incident notes for postmortem.

### 15.4 Save rollback risk handling
If a bad build wrote a newer save version:

- Do not downgrade blindly if old build cannot read new saves.
- Provide backup restore instructions.
- Consider hotfix forward instead of rollback.
- If migration created backups, direct users to backup location.
- Never instruct users to delete saves as first step.

### 15.5 Copy-paste AI prompt — rollback decision

```text
Given this public release incident for The Drowned Frequency, decide whether to rollback, hotfix, or hold. Consider severity, player impact, save-version compatibility, Steam Cloud risk, and validation time. Output immediate actions, player-facing message draft, and verification checklist. Incident data:
<paste incident reports>
```

---

## 16. Postlaunch Operations

### 16.1 Launch day monitoring
Track for first 24-72 hours:

- Launch crashes.
- Save/load issues.
- Steam Cloud conflicts.
- Performance complaints by hardware/GPU.
- Input/controller issues.
- Progression blockers.
- Negative reviews citing the same bug.
- Forum/Discord/email bug reports.
- Refund reason patterns if available.

### 16.2 Player bug report request template

```text
Thanks for reporting this. If you can, please send:
- What happened and what you expected
- Steps to reproduce
- Your Windows version, CPU, GPU, RAM, and GPU driver
- Whether you use keyboard/mouse or controller
- The game build number shown in-game or in the log
- Your log file from: %LOCALAPPDATA%\<ProjectName>\Saved\Logs
- Your save file if the issue involves progress/loading: %LOCALAPPDATA%\<ProjectName>\Saved\SaveGames
Please back up saves before deleting or moving anything.
```

### 16.3 Hotfix process

1. Reproduce issue or collect enough evidence.
2. Assign severity/priority.
3. Fix on hotfix branch.
4. Increment patch version/build number.
5. Package Shipping hotfix.
6. Run targeted test for bug.
7. Run smoke test.
8. Run save migration/backward compatibility test if touched.
9. Run clean-machine launch if packaging changed.
10. Upload to beta branch if possible.
11. Validate Steam Cloud/launch.
12. Promote to default.
13. Publish patch notes.

### 16.4 Patch notes template

```markdown
# The Drowned Frequency Patch <version>

## Fixes
- Fixed <specific issue>.
- Fixed <specific issue>.

## Improvements
- Improved <specific area>.

## Known issues
- <issue and workaround, if any>.

If you encounter save/load issues, please back up your saves before troubleshooting and send logs from `%LOCALAPPDATA%\<ProjectName>\Saved\Logs`.
```

### 16.5 Postlaunch backlog classification

| Category | Examples | Action |
|---|---|---|
| Emergency | Crash/save loss/blocker | Hotfix immediately |
| High-impact | Frequent progression/performance/input bug | Patch soon |
| Quality | Polish, UX friction, balance | Batch patch |
| Content | New features/content | Roadmap only after stabilization |

### 16.6 Copy-paste AI prompt — postlaunch report clustering

```text
Cluster these postlaunch player reports for The Drowned Frequency. Identify duplicate issues, likely root causes, severity, affected hardware/settings, required evidence, and recommended hotfix order. Preserve exact user claims and do not assume unreported facts. Reports:
<paste reports>
```

---

## 17. Master Task Breakdown

Use these as small task cards. Each card has test/evidence and AI prompt.

### TASK-001 — Create build ID/version policy

Steps:
1. Define semver/build number/git SHA format.
2. Decide where build ID appears: log, main menu, pause menu, crash reports.
3. Verify packaged build displays/logs correct ID.

Test:
- Launch packaged build and capture build ID from UI/log.

AI prompt:

```text
Review this build ID/version policy for The Drowned Frequency. Check whether it supports QA, Steam releases, hotfixes, save migration, and rollback. Suggest exact improvements.
<policy>
```

### TASK-002 — Package first QA build

Steps:
1. Set configuration to Development or Shipping as needed.
2. Run BuildCookRun.
3. Save full log.
4. Search for errors/warnings.
5. Archive build.

Test:
- Launch executable from archive folder.

AI prompt:

```text
Review this UE5 BuildCookRun result for a Windows packaged QA build. Identify blockers and risky warnings.
<log>
```

### TASK-003 — Run smoke test

Steps:
1. Execute SMK-001 through SMK-014.
2. Capture logs/screenshots.
3. File bugs.

Test:
- All smoke rows PASS or bugs filed.

AI prompt:

```text
Evaluate this smoke test result and decide whether full QA can proceed.
<smoke report>
```

### TASK-004 — Build functional test matrix by chapter

Steps:
1. List all chapters/maps/objective chains.
2. Add required interactions and transitions.
3. Add reload tests per chapter.

Test:
- Every playable area has at least one fresh-entry and load-from-save test.

AI prompt:

```text
Turn this chapter list into a granular functional QA matrix with reload, objective, interaction, and edge-case tests.
<chapter list>
```

### TASK-005 — Complete critical path pass

Steps:
1. Start with no saves.
2. Play through to credits.
3. Quit/reload per chapter.
4. Log all blockers.

Test:
- Credits reached and post-completion state reloads.

AI prompt:

```text
Analyze this critical path QA log. Identify blockers, recurring friction, missing evidence, and regression tests.
<critical path log>
```

### TASK-006 — Save system validation

Steps:
1. Run SAV-001 through SAV-012.
2. Preserve sample saves for each version.
3. Verify corrupt/missing/future saves.

Test:
- No crash/corruption; migration documented.

AI prompt:

```text
Review these save tests for data-loss risk and missing migration cases.
<save test results>
```

### TASK-007 — Performance benchmark pass

Steps:
1. Define target graphics preset/resolution.
2. Capture stat unit/stat gpu in PERF-001 through PERF-010.
3. Capture Insights trace for worst scenes.
4. Identify bottlenecks.

Test:
- Mid-range target meets 60 FPS or has documented scalable settings.

AI prompt:

```text
Analyze these UE5 performance captures for Lumen/Nanite/VSM bottlenecks and prioritize fixes.
<perf data>
```

### TASK-008 — Hitch and loading pass

Steps:
1. Launch with trace args.
2. Traverse streaming boundaries.
3. Trigger autosaves and cinematics.
4. Mark hitches.
5. Inspect in Insights.

Test:
- No severe gameplay hitches unresolved.

AI prompt:

```text
Analyze these hitch notes and Unreal Insights observations. Identify likely causes and next measurements.
<hitch data>
```

### TASK-009 — Accessibility pass

Steps:
1. Run accessibility matrix.
2. Verify subtitles/settings persistence.
3. Review intense flashing/motion effects.

Test:
- Required accessibility claims are true and persistent.

AI prompt:

```text
Audit these accessibility QA results for launch blockers and missing tests.
<accessibility results>
```

### TASK-010 — Clean-machine package validation

Steps:
1. Install/extract on clean Windows profile.
2. Launch as standard user.
3. Save/load/relaunch.
4. Test paths with spaces and unicode if possible.

Test:
- No editor/dev dependency; saves/logs in writable location.

AI prompt:

```text
Review this clean-machine validation report and identify release blockers.
<report>
```

### TASK-011 — Playtest plan and execution

Steps:
1. Recruit phase-appropriate testers.
2. Provide instructions and known issues.
3. Observe or collect blind reports.
4. Synthesize patterns.

Test:
- Top repeated issues converted to tickets.

AI prompt:

```text
Synthesize these playtest notes into actionable bugs/design fixes with severity and frequency.
<notes>
```

### TASK-012 — Store asset production

Steps:
1. Capture trailer footage and screenshots from clean build.
2. Assemble trailer sequence.
3. Export capsules/screenshots/trailer.
4. Review at thumbnail sizes.

Test:
- Assets match actual game and contain no debug overlays/spoilers beyond plan.

AI prompt:

```text
Critique this trailer/screenshot/store asset sequence for clarity, conversion, and accuracy.
<asset plan>
```

### TASK-013 — Legal/license audit

Steps:
1. Inventory all third-party assets.
2. Save license proof.
3. Verify commercial redistribution and attribution.
4. Update credits.

Test:
- No unknown-license asset remains in release build/assets.

AI prompt:

```text
Audit this license inventory for missing proof, attribution, and redistribution risks.
<inventory>
```

### TASK-014 — Steam integration validation

Steps:
1. Integrate Steam SDK/features.
2. Validate launch/overlay/cloud/achievements if present.
3. Test offline/branch switching.

Test:
- Every Steam store claim passes functional test.

AI prompt:

```text
Review this Steam integration test result for launch, overlay, cloud, achievements, offline mode, and branch risks.
<results>
```

### TASK-015 — Release Candidate gate

Steps:
1. Package Shipping RC.
2. Run all RC gates.
3. Triage bugs.
4. Decide GO/NO-GO/WAIVER.

Test:
- RC checklist complete; no S0/S1.

AI prompt:

```text
Make a GO/NO-GO release recommendation from this RC packet. Include blockers, waivers, and exact verification required.
<RC packet>
```

### TASK-016 — Rollback rehearsal

Steps:
1. Identify previous known-good build.
2. Confirm save compatibility.
3. Draft rollback notice.
4. Simulate branch/build switch procedure without public action.

Test:
- Rollback can be executed quickly and safely.

AI prompt:

```text
Review this rollback plan for save compatibility, Steam branch risk, player messaging, and missing safeguards.
<rollback plan>
```

### TASK-017 — Postlaunch monitoring setup

Steps:
1. Define report intake channels.
2. Prepare bug report template.
3. Prepare patch notes template.
4. Define hotfix gates.

Test:
- First player report can be classified and routed within minutes.

AI prompt:

```text
Cluster and prioritize these postlaunch reports into hotfix, patch, and backlog groups.
<reports>
```

---

## 18. Final Pre-Release Checklist

```text
[ ] Build ID/version finalized
[ ] Shipping package created from release branch/tag
[ ] Packaging log reviewed
[ ] Smoke test passed
[ ] Clean-machine test passed
[ ] Critical path completed
[ ] Save tests passed
[ ] Save migration/version tests passed
[ ] Steam Cloud tests passed or feature not claimed/enabled
[ ] Performance target validated
[ ] Lumen/Nanite/VSM scenes profiled
[ ] Accessibility pass completed
[ ] Controller/input pass completed
[ ] Store trailer final
[ ] Screenshots/capsules final
[ ] Store copy final
[ ] Legal/license inventory complete
[ ] Credits complete
[ ] Content warnings accurate
[ ] Steam setup complete after integration
[ ] Rollback build retained
[ ] Hotfix process ready
[ ] No open S0/S1 bugs
[ ] S2 waivers approved/documented
[ ] Release decision recorded
```

---

## 19. Minimal GO/NO-GO Packet Template

```markdown
# The Drowned Frequency Release Decision Packet

Build ID:
Date:
Decision: GO / NO-GO / GO WITH WAIVERS
Decision owner:

## Build/package
- Package path:
- Packaging log:
- Checksum:

## QA
- Smoke:
- Critical path:
- Clean machine:
- Save/versioning:
- Steam/Cloud:
- Accessibility:
- Controller/input:

## Performance
- Target hardware:
- Target settings:
- Average FPS / worst scenes:
- Known perf risks:

## Bugs
- Open S0:
- Open S1:
- Open S2:
- Waivers:

## Store/marketing
- Trailer:
- Screenshots:
- Capsules:
- Store copy:

## Legal
- License inventory:
- Credits:
- Content warnings:

## Rollback
- Previous known-good build:
- Save compatibility:
- Rollback instructions verified:

## Final notes
```
