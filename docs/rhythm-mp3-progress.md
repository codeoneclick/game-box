# Execution ledger — docs/superpowers/plans/2026-10-23-rhythm-mp3.md

User approved spec and plan. Native execution continues in diskoteka checkout as previously selected; preserve the three user-added MP3s. No merge/push.
Ruling: use this portable ledger and focused PowerShell check scripts rather than Bash-only bookkeeping on Windows.
Scope addition approved by user: portrait 680x960, previous FPS/polygon HUD, previous F8 Render Inspector. Implement presentation first to validate subsequent audio UI at the required dimensions. F1 remains gameplay debug.
Pre-flight: PCM metadata -> transport format/clock -> actual map duration; loader generation -> controller session/audio publication; input raw timestamp -> calibration correction once; visuals retain raw song time. No conflicting interfaces found.
Ruling: reuse TerrainEditorUI's existing inspector with an inspector-only option, leaving terrain controls unconstructed in Rhythm Ship. This preserves the prior F8 functionality and automatic fit-to-window scale.

Presentation: F8 mappings RED (missing action) -> GREEN domain check. Debug build passed; Vulkan portrait captures confirm 680x960, existing ocean/sky and 50 dancers. FPS/polygons/draw calls consume render_statistics::last_frame(). Inspector screenshot inspected with pass list, live attachment preview, lighting controls and Save. Underlying game UI/taps are blocked while inspector is open.
Task 1: new PCM interfaces RED -> GREEN audio checker. All three originals decode stereo44100 with expected frames. Added truncated-prefix test: dr_mp3 accepted it RED; strict framing now rejects it GREEN. Played-sample clock, forced silent, bytes/stereo conversion and unit transition/rollover checks pass.
Ruling: strict Layer III framing rejects unsupported free-format/changing-format and trailing metadata beyond ID3v1/v2; whole-frame truncation without declared length is detected against authored map duration. This is narrower and explicit instead of silently accepting arbitrary damaged prefixes.
Task 2: catalog interfaces RED -> GREEN music checker. Runtime PCM analysis reproduced phase ranges and durations; canonical explicit maps generated. Seam omission: last Audioera and Strawberry beats only. SHA256 and all Apple resource memberships pass asset checker.
Task 3: bounded joined worker, latest generation, matching session/audio publication, active PCM reuse and fallback implemented. Focused switching/cache/loading-tap checks GREEN. Windows MP3 Vulkan scoring/rank/restart/normal-close run passed with 50 dancers.
Task 4: calibration interfaces RED -> GREEN; sign, count-in, duplicate, incomplete, jitter, outlier and malformed preferences checks pass. Controller calibration completion/restoration tested. Late-input expiry regression RED -> GREEN with judgement delay while visual events use raw time.
Ruling: integrated source/controller/UI slices before task commits because their exact project membership and timestamp interfaces were developed together. Preserve reviewable focused checks and final verified commit; no partially registered intermediate commits.
Ruling: delay only miss expiration for positive personal offset; otherwise corrected late taps could already be expired. Visual events continue at raw playback time. Negative corrected taps queue until their song time.
Verification in progress: final Windows Debug and Release builds passed. First F8 smoke missed frame400 within35s while Release compilation ran; frame160 confirmed working inspector and normal close. Repeat without concurrent compilation uses60s capture budget. This was a smoke timeout, not evidence of a successful full run.

Final review: fresh read-only gpt-6-astra review completed. Two Important findings: trailing garbage accepted by JsonCpp and buffered save/close failures could publish invalid preferences. Added trailing-garbage RED checks, shared bounded full-document JSON envelope reader -> GREEN; saves explicitly flush/close/check then atomically replace on Windows. Minor stale error notice reproduced RED, cleared on successful selection/save -> GREEN. No unresolved Critical/Important or deferred minors.
Ruling on review exclusions: decoder internals remain the pinned upstream dependency, not locally rewritten; audible alignment and physical latency are explicitly not claimed; Apple/OpenGL/Metal/server require other hosts; hardware rollover is covered by arithmetic checks, not days-long hardware observation. Worker teardown is exercised with real pending requests, not a deterministic blocked decoder fixture; bounded decoding and destructor join reviewed in source.
Final runtime evidence: F8 repeat completed frame400 with normal close, prior inspector visually inspected. All three MP3s played using44100Hz stereo/sample clock with real keyboard scoring/restart; Release Ikoliks and Strawberry flows passed. The dedicated native controls smoke passed selection of all three, 24 calibration keypresses (accepted about+94ms on this synthetic run), saving, reset to zero, cancel/restoration and normal close. Uses an ignored output preferences file; does not overwrite personal calibration.
Final presentation: original ocean/sky environment source restored byte-for-byte after formatter-only changes; Windows window680x960, readable bottom music/statistics panel, F1 gameplay controls and F8 full previous inspector. F8 temporarily hides/blocks gameplay HUD/touches until closed, including X close.
Final verification commands/results are completed below after the last review fixes. Branch stays diskoteka, no merge/push (the approved plan already supplies this integration choice; no additional finishing menu needed).

Final checks after review fixes:
- `check-rhythm.ps1`: passed, including calibrated expiry and malformed trailing track JSON.
- `check-rhythm-audio.ps1`: passed, original three decodes, truncated prefix rejection, format/clock arithmetic, forced silent and real output clock.
- `check-rhythm-music.ps1`: passed, catalog/maps, loading/filter/cache, calibration signs/outliers/jitter/persistence, fallback and obsolete-notice regression.
- Blender Python3 `check-rhythm-ship-assets.py`: passed,17 canonical assets, original MP3 hashes and applicable MSVC/Xcode memberships/flags.
- `build-gbDemo.ps1 -Configuration debug-client -Platform x64`: passed after final HUD layering fix (`hud-debug-build.log`).
- Same `release-client`: passed after final HUD fix (`hud-release-build.log`).
- Release real Audioera/Ikoliks/Strawberry Vulkan flows: passed,50 dancers, keyboard scoring, restart and normal close. Latest Audioera capture visually confirms readable statistics in680x960 (`hud-final-runtime.log`).
- Debug F8 real-Audioera flow with60s capture budget: passed, inspector preview/settings/source-independent overlay and normal close (`portrait-f8-runtime2.log`).
- Release forced-silent metronome + F1 debug actions: passed (`release-final-silent.log`).
- `check-rhythm-controls.ps1`: passed, real native track controls and calibration/persistence/reset/cancel (`controls-smoke.log`).
- `git diff --check`: passed. Original MP3 hashes unchanged. No engine-core, generated/staged assets or ocean/sky sources edited.

Task1: complete. Task2: complete. Task3: complete. Task4: complete. Task5: complete.
Known limits: no native Apple/OpenGL/Metal/server build/runtime; no independent audible alignment or physical latency measurement. Retained island runtime remains UNKNOWN from the earlier timed-out check. Windows Vulkan validated; existing compiler conversion warnings remain. No deferred review minors.
