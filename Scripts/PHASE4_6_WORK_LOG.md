# Phases 4–6 implementation record

## Authorized scope

Implement Phase 4 acoustic spine, Phase 5 acoustic depth, Phase 6 local saves in master build order. Improve dark-room visibility without losing horror ambience. Generate authored test audio. After validation, inspect workspace for safe cleanup, commit and push to origin. Preserve documents and all useful game content. No new art production or unrelated refactor.

## Source-of-truth decisions

- `00_MASTER_BUILD_ORDER.md` overrides stale Anomaly category examples in `03_ACOUSTIC_SYSTEM.md`: exactly Voice, Footstep, Machine, Bell, Water, Static, Impact.
- Anomaly is a source type. No runtime mic capture, audio analysis, or inferred waveform puzzle matching.
- Acoustic events use one `UTDFAcousticWorldSubsystem`. C++ owns rules; Data Assets own tuning; Blueprint wrappers/widgets own presentation.
- A04 references signals and the stimulus bus before A05/A10/A11. Their minimum shared contracts were introduced for A04; each later task receives its own behavior tests. Similarly A07 requires a ledger hook before A14.
- Recorder component enforces sequential 3→4→5 unlocks. Non-puzzle authored signals remain valid recordings. Puzzle eligibility is separate from recording validity.
- Runtime payload structs contain stable asset IDs and GUIDs, never persistent actor pointers.
- Original offline synthesized WAVs live in `ContentSource/Audio/Phase4`; provenance and objective sound-file QA accompany them. Gameplay never reads their waveform.

## Progress

- A01 tags: build + automation passed.
- A02 types: build + automation passed. UE requires a hidden zero sentinel for the tier enum; playable tiers remain 3, 4, 5.
- A03 Data Asset: build + automation passed. PrimaryAssetId text import corrected to `Type:Name`; all eight WAVs and seven anomaly assets imported and validated.
- A04 source and A05 signal: build + automation passed.
- A06 slots: build + automation passed.
- A07–A14 core behavior: builds and automation passed. Dedicated tests cover authored playback, remote puck identity/origin, bounded event bus, all player-noise categories, receiver rejection/consumption, and explicit recovery policy.
- Phase 4 integration: native input operator, three reusable pucks, delegate-driven recorder widget, keyboard/controller actions and Blueprint wrappers authored. The new map is `M_Test_AcousticSpine`; earlier rooms receive visibility fill and 0.7-stop exposure lift.
- Full editor suite `Phase4_Flow2`: 34 passed (31 success, 3 success with external warnings), 0 failed. Includes both acoustic mapped-input flows and prior player/interaction regression tests. The new test exposed and fixed null-world teardown in persistent ID/receiver cleanup.
- Phase 4 complete: Windows Development package built; 2 packaged mapped-input routes passed; final DX12 render/audio capture passed; older-room visibility capture passed. Actual packaged stereo mix: 48 kHz, 36.48 s, peak −17.12 dBFS, RMS −35.7 dBFS, zero clipped samples. Screenshots copied to `Saved/Phase4Preview`. The initial HUD mapping timing issue was found visually and fixed with the mapping-rebuilt delegate.
- Phase 5: A15–A22 compiled and their automation checks passed: authored alternate takes, deterministic degradation, Act V overdub gating, bounded pipe/speaker routing, authored captions, presentation helpers, debug tools, and serialized acoustic payloads. A23 templates and caption widgets were authored and validated. A24 full-route PIE test passed, including private caption/no-event, all four routes, and delete/recovery. A25 complete: 44 editor tests passed (41 success + 3 external warnings), DataValidation 0 errors/0 warnings, Windows package built, full-route packaged test and real DX12 capture passed. Actual engine mix 36.48 s, peak -17.22 dBFS, RMS -35.47 dBFS, zero clipped samples.
- Phase 6: stable-ID cross-check reuses existing registry; 1.1 schema disk round-trip passed. 1.2 write path compiled; test exposed an inverted boolean check on successful file replacement, now corrected. The load task requires the sector contract before 4.1–4.3, so its minimum shared contracts are introduced as a dependency, with full streaming behavior validated at those numbered steps.
- Final cleanup, commit, push pending.

Per-step logs and machine-readable reports are under `Saved/Logs/Phase4_*` and `Saved/Tests/Phase4_*`. Earlier Phase 2/3 work is preserved. No commits or pushes yet.


- Phase 6: 1.2 write path and 1.3 streamed load now pass their integration tests. A Python editor setter/save-current-level issue was corrected: streamed references are force-saved with the owning world and checked after reopening. Save/load restores player, signal, inventory and opened door after unloading its sector.
- Phase 6: 1.4 migrations passed; schema V2 adds puck/checkpoint recovery metadata. 2.1 checkpoint overlap, duplicate suppression and restore passed. 2.2 death/respawn with saved puzzle and profile health passed. 3.1 graph tests pass for valid graphs, duplicate IDs, missing edges and cycles. 3.2 objective/UI integration and 4.x streaming edge-case validation are in progress.
- Phase 6 final editor regression: 54 passed (49 clean, 5 with warnings), 0 failed. Warnings were expected missing fallback files and unavailable Epic online services. New coverage includes manual/chapter death fallback, objective-to-checkpoint persistence, cached sector state, missing actors, invalid sectors, coalescing and timeout recovery.
- Final authored-asset audit: 9 maps, 36 distinct persistent actor GUIDs, 2 valid sectors; no duplicate IDs. Unreal DataValidation: 0 errors, 0 warnings.
- Windows Development package compiled and cooked successfully. The combined packaged run exposed reuse of a test-only checkpoint ID between fixtures; unique fixture IDs and relative write-count assertions corrected the isolation problem. All 5 packaged acoustic/save/checkpoint/death/progression flows now pass.
- Cleanup: removed two superseded Phase 6 retry wrappers, normalized source/config/script end-of-file whitespace, verified all 39 Python scripts parse and all 22 original WAV hashes match their manifests. No temporary/reject/backup files were found in source/audio/script folders. Existing source-of-truth documents were preserved, including their original whitespace. No broad refactor was warranted; local binaries/caches/saves remain ignored.
- Phase 6 complete: separate packaged processes wrote, quit, restarted in another map, and restored all required player/acoustic/key/objective/puzzle/sector state. Final packaged suite: 5 gameplay flows + write + read + rendered capture all passed. Final engine mix: 48 kHz stereo, 25.69 s, peak -17.11 dBFS, RMS -34.83 dBFS, zero clipping.
- Visual review corrected overly even fill in the new save rooms with focused cool practical pools and restrained amber accents. Floors and controls remain readable; caption backdrop no longer overlaps the recorder panel. Final asset audit and DataValidation passed again after the changes. See PHASE4_6_QA.md and PHASE6_HANDOFF.md for evidence and controls.
