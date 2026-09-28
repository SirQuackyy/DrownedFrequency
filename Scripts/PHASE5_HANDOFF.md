# Phase 5 acoustic depth

Open `/Game/Game/Maps/Test/M_Test_AcousticDepth`.

The route extends Phase 4's direct and remote gates with a pipe conduit, station speaker and signal-memory recovery terminal. Record the three-strike service bell at the entrance. F replays; G deploys a puck; brackets select; Delete erases; hold R records. Controller equivalents remain visible in the recorder panel. Interact with pipe/speaker stations using the standard interaction action.

The first two gates require a clean bell. Re-recording the source gives a fresh tape. The later relays accept degraded recordings. The recovery terminal explicitly permits the remembered bell key after deletion; ordinary receivers cannot silently use the ledger. The source remains available. Overdub data is implemented and tested but unavailable before Act V.

Audio is authored: seven clean recordings, fourteen separate alternate takes, and an original room-tone bed. Gameplay uses stable metadata, never microphone input or waveform analysis. The processing rules choose authored variants after 2/4 prior plays. Captions follow that selected variant, expire on their authored schedule, and distinguish private previews, pipes, speakers and remote playback.

Designer assets:
- `/Game/Game/Audio/Phase4`: category definitions, player noise tuning, safety policy and three sound classes.
- `/Game/Game/Audio/Phase5`: alternate takes, processing rules, contamination rules and a duplicable anomaly template.
- `/Game/Game/Acoustics`: source, receiver, puck, pipe input/output and speaker Blueprint wrappers.
- `/Game/Game/UI`: recorder and caption widgets. C++ delegates update presentation; widgets do not decide gameplay results.

Development commands: `acoustic.Debug.DrawStimuli 1`, `acoustic.Debug.ListRecent`, `acoustic.Debug.EmitTest Bell 900`, `acoustic.Debug.ValidateAssets`, `acoustic.Debug.DumpReceivers`. Mutation/debug command registration is excluded from Shipping.

Automated route tests exercise the real map and gameplay objects; fixture-to-fixture positioning is automated and does not claim a human walking or controller-feel review. The prior Phase 3 route tests still walk through their route with mapped keyboard and controller input.
