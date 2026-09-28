# Phase 4–6 acceptance record

Verified on 2026-09-28 with Unreal Engine 5.8, Windows Development builds, RTX 3060.

| Gate | Result | Evidence under project `Saved/` |
|---|---|---|
| Phase 4 full editor regression | 34 passed, 0 failed | `Tests/Phase4_Flow2` |
| Phase 5 full editor regression | 44 passed, 0 failed | `Tests/Phase5_A25` |
| Final editor regression | 54 passed, 0 failed | `Tests/Phase6_Final` |
| Final authored identity audit | 9 maps, 36 distinct persistent actors, 2 sectors; no invalid/duplicate IDs | `Logs/phase6_asset_audit.log` |
| Final Unreal asset validation | 0 errors, 0 warnings | `Logs/Phase6DataValidation.log` |
| Windows compile/cook/package | Successful | `Logs/Phase6Package.log` |
| Packaged acoustic, checkpoint, death, save/load and progression flows | 5 passed | `Tests/Phase6PackageFlow` |
| Separate-process write | Passed | `Tests/Phase6PackageWrite` |
| Separate-process cross-map load | Passed | `Tests/Phase6PackageRead` |
| Final rendered room/caption/audio capture | Passed and images inspected | `Tests/Phase6PackageRendered` |

The final editor result includes 49 clean successes and 5 successes with warnings: deliberately missing death-fallback files and unavailable Epic online services. No project assertion failed. The combined packaged run initially exposed a shared checkpoint fixture ID; tests now use unique identities and relative write counts. A PowerShell array-expansion issue in the rendered runner was corrected to select only the named capture test. Final packaged results above are from the corrected runner and final assets.

The fresh-process test starts in the input gym, reads a save written by an earlier exited process, opens the save-test map, waits for Intro, and verifies location/rotation, health, key inventory, recorded signal, objective, solved door, puck identity and released controls. Tests use isolated QA slots and do not overwrite normal player slots.

Final mixer output: 48 kHz stereo PCM, 25.69 seconds, peak -17.11 dBFS, RMS -34.83 dBFS, zero clipped samples. Original source audio comprises 22 WAVs; all manifest SHA-256 hashes matched. Gameplay uses authored metadata; no microphone or runtime waveform analysis is involved.

Final captures and mixer output are inside `Builds/Phase6/Windows/Horror/Saved/Screenshots` and `Builds/Phase6/Windows/Horror/Saved/AudioQA`. Visual review caught excessive even fill in the first save-room pass. The final pass uses focused cool pools, restrained amber control-area light and dark upper recesses; floors, routes and controls remain readable. The caption backdrop sizes to its text above the recorder HUD.

Cleanup preserved all design documents, original audio, source, assets and useful build/test tools. Two superseded retry wrappers were removed. Source/config/script whitespace was cleaned, script syntax checked, and no source/audio temporary or backup files remained. Generated builds, caches, reports and local saves are excluded from Git. Existing documentation whitespace was left intact. No broad refactor was needed.

Scope ends at master Phase 6, including foundation task 20's streaming cross-check. Final models/materials, Listener AI, production menus and the later difficulty/accessibility systems remain in their scheduled phases. Automated checks do not replace a human controller-feel review or a performance benchmark.
