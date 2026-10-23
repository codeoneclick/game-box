# Execution ledger — docs/superpowers/plans/2026-10-23-rhythm-gestures.md

Spec and plan approved. Native execution in existing diskoteka checkout as selected; no merge/push. Preserve nine new MP3 bytes and user deletion of three old tracks. Mechanics only; rectangular deck/daylight/ocean/sky remain.
Ruling: use portable ledger and PowerShell standalone checks instead of Bash-only skill bookkeeping on Windows, as in preceding increments.
Pre-flight: musical beats remain independent of generated playable events; session resolution feeds prompts/crowd; bot outcomes feed both visuals and leaderboard; input epoch gates playback replacement/calibration; settings generation validates before publication. No shared-interface conflicts identified.
Task1 in progress: replacement catalog, measured runtime-PCM timing, source hashes and retired asset membership/staging.

Task1: nine new runtime-decoded songs, pulse-family/phase consensus, source hashes and new canonical manifest/maps generated. Old maps/resources removed; guarded staging cleanup added. Catalog RED10-vs4 -> GREEN; audio/music/asset checks pass; initial Windows Debug build passes.
Task2: typed interfaces RED -> GREEN. Wrong consumes only nearest event; duplicate/tie/overlap/expiry tests pass. Musical beat timeline remains independent; playable minimum spacing60ms permits eighths.
Task3: deterministic phrase generation RED -> GREEN (Easy/Normal/Hard, all five actions, empty masks, pulse independence). Playback settings transaction and loading-generation tests pass. Explicit Generate during an authored-map load reproduced RED; active catalog override now preserves the deliberate regeneration request.
Task4: gesture threshold/axis/Y/timestamp/epoch/cancel tests RED -> GREEN; adapter added keyboard arrows and mouse drag/release. Native cancellation inspection found the existing context synthesizes ordinary release for WM_CANCELMODE/KILLFOCUS/CAPTURECHANGED. Ruling: add narrow default-noop on_gr_cancelled + gr_cancelled notification before the retained legacy synthetic release; Rhythm listener cancels first, other consumers retain release behavior. No feeder/entity/render core contracts changed. GetCurrentMessage was mistakenly assumed to be Win32 API; compilation rejected it, and it was removed rather than inventing an API. Full owning-module rebuild required for input_context header change.
Task5: shared bot outcomes/leaderboard and temporary dance poses added. Partitioned-update/dedup/CrowdSync/pose-return checks RED -> GREEN. Local input poses use raw time; scoring remains calibrated once.
Task6: pure prompt state RED -> GREEN. Native ring/arrows/previews, compact normal HUD and four F1 pages built. No Unicode arrow glyph dependency. Debug build passed before cancellation bridge; native visual verification in progress.
Task7: pooled radial/directional stage helper RED -> GREEN. Deck geometry and daytime ocean/sky construction retained; fixed three-ring pool only. Debug build passed before cancellation bridge.
Ruling: integrated task commits deferred until the registered vertical slice passes final checks, because these source/interface/project changes span session→playback→input→UI→crowd wiring. Focused RED/GREEN evidence and final review remain required.
Native first swipe smoke initially counted pending keyboard messages as mouse events; logs showed exactly five keys plus four swipes. Harness now waits for expected key count before starting the next input batch.

Final review and regression pass:
- Independent source review found three Important issues: stale native UI captures after F1 hide/reopen, prompt/nearest-event ownership mismatch during overlapping windows, and typed events not revalidated after decoded-duration reconciliation. All three reproduced RED before their fixes. Focused prompt/music regressions and native hidden-capture checks are now GREEN; targeted re-review reports no remaining Critical/Important findings.
- UI callbacks require originating control visibility and a per-press visibility epoch for both buttons and sliders. Session prompt ownership uses the same midpoint/earlier-tie contract as judgement. Track preparation validates typed events against reconciled duration, and publication constructs the replacement session before stopping audio/replacing live session.
- Minor review items fixed: resolved command dimming, Tap speaker response, and Win32 focus-loss notification clearing demo held keys. Default-noop focus-loss/cancellation hooks preserve existing listener behavior. All owning Windows modules rebuilt; no entity/feeder/render core contracts changed.
- Native image inspection found the editor rounded-panel shader's two-pixel inset erased thin scaled prompt glyphs. Prompt bars now use existing flat-color button material; post-fix captures show the diamond/ring instead of absent glyphs. Ring segments thickened and previews moved above the current command. No shader/backend implementation changed.
- Updated demo guide and nine-song analysis report. The source manifest retains timing evidence and practical uncertainty; no independent audible-alignment claim is made.

Verification completed so far:
- Windows Debug and Release builds of all six client modules: PASS. Latest Debug log: prompt-flat-build.log; latest Release: prompt-flat-release-build.log.
- check-rhythm.ps1: PASS (final-domain.log).
- check-rhythm-audio.ps1: PASS, all nine original files decoded (final-audio.log).
- check-rhythm-music.ps1: PASS, including forced phrase generation during authored-map loading and duration reconciliation regression. A redirected run was interrupted by PowerShell treating the intentional missing-file fallback stderr as NativeCommandError; rerunning normally passed. This was harness redirection behavior, not a gameplay failure.
- check-rhythm-gestures.ps1: PASS, including typed JSON parsing/rejection, ownership midpoint/tie, gesture recognition/cancellation/epochs, deterministic difficulty, shared crowd outcomes/poses, prompt state and bounded stage pool.
- register-rhythm-ship.py and check-rhythm-ship-assets.py: PASS for source/project membership (MSVC + all three Xcode targets), hashes and canonical assets.
- analyze-rhythm-mp3.py (read-only): PASS for all nine runtime PCM files; reports/overlays remain ignored output artifacts.
- Native Debug directional checks: PASS, five keyboard actions/four mouse swipes/click/cancel/restart/diagonal/F8/stale hidden F1 capture/focus loss/normal shutdown (final-directional-native.log).
- Native F1/F8 portrait smoke: PASS (final-inspector.log). Native calibration/selection/reset/cancel smoke: PASS with 24 keyboard taps (final-controls.log).

Remaining platform limits: Apple/Metal, dormant OpenGL and server builds/runtime are UNKNOWN on this Windows host. Retained island runtime previously timed out; no new island success claim. No physical audio-latency measurement or independently heard alignment. Local branch diskoteka and current checkout are retained; no merge/push requested.

## Changed files in this increment

Compared with approved-plan commit 52612000. D means removed; M means modified; A means added (including user-provided replacement MP3s).

- M docs/rhythm-ship-audio-analysis.md
- M docs/rhythm-ship-demo.md
- D gbBundle/Configurations/rhythm_ship/rhythm_ship_audioera.json
- M gbBundle/Configurations/rhythm_ship/rhythm_ship_catalog.json
- D gbBundle/Configurations/rhythm_ship/rhythm_ship_ikoliks.json
- D gbBundle/Configurations/rhythm_ship/rhythm_ship_strawberry.json
- M gbCore/sources/input_context.cpp
- M gbCore/sources/input_context.h
- M gbCore/sources/input_context_win32.cpp
- M gbDemo/gbDemo.vcxproj
- M gbDemo/gbDemo.vcxproj.filters
- M gbDemo/gbDemo.xcodeproj/project.pbxproj
- M gbDemo/sources/ces_dance_crowd_system.cpp
- M gbDemo/sources/ces_dance_crowd_system.h
- M gbDemo/sources/fake_leaderboard.cpp
- M gbDemo/sources/fake_leaderboard.h
- M gbDemo/sources/rhythm_key_action.h
- M gbDemo/sources/rhythm_session.cpp
- M gbDemo/sources/rhythm_session.h
- M gbDemo/sources/rhythm_ship_environment.cpp
- M gbDemo/sources/rhythm_ship_environment.h
- M gbDemo/sources/rhythm_ship_input.cpp
- M gbDemo/sources/rhythm_ship_input.h
- M gbDemo/sources/rhythm_ship_playback.cpp
- M gbDemo/sources/rhythm_ship_playback.h
- M gbDemo/sources/rhythm_ship_scene.cpp
- M gbDemo/sources/rhythm_ship_ui.cpp
- M gbDemo/sources/rhythm_ship_ui.h
- M gbDemo/sources/rhythm_track.cpp
- M gbDemo/sources/rhythm_track.h
- M gbDemo/sources/rhythm_track_catalog.cpp
- D gbResources/audioera-stylish-trailer-percussion-622881.mp3
- D gbResources/ikoliks-uplifting-action-sport-energetic-rock-619094.mp3
- D gbResources/strawberry_candy-sport-intense-metal-fast-and-strong-622134.mp3
- M gbScripts/analyze-rhythm-mp3.py
- M gbScripts/build-gbDemo.ps1
- M gbScripts/check-rhythm-controls.ps1
- M gbScripts/check-rhythm-music.ps1
- M gbScripts/check-rhythm-ship-assets.py
- M gbScripts/check-rhythm-ship-runtime.ps1
- M gbScripts/check_rhythm.cpp
- M gbScripts/check_rhythm_audio.cpp
- M gbScripts/check_rhythm_music.cpp
- M gbScripts/register-rhythm-ship.py
- A docs/rhythm-gestures-progress.md
- A gbBundle/Configurations/rhythm_ship/rhythm_ship_active_sport.json
- A gbBundle/Configurations/rhythm_ship/rhythm_ship_adrenaline_sport.json
- A gbBundle/Configurations/rhythm_ship/rhythm_ship_gym_motivation.json
- A gbBundle/Configurations/rhythm_ship/rhythm_ship_music.json
- A gbBundle/Configurations/rhythm_ship/rhythm_ship_retro_music.json
- A gbBundle/Configurations/rhythm_ship/rhythm_ship_summer_party.json
- A gbBundle/Configurations/rhythm_ship/rhythm_ship_summertime.json
- A gbBundle/Configurations/rhythm_ship/rhythm_ship_synthwave_pop.json
- A gbBundle/Configurations/rhythm_ship/rhythm_ship_upbeat_energetic_rock.json
- A gbBundle/Configurations/rhythm_ship/rhythm_ship_workout.json
- A gbDemo/sources/rhythm_action.h
- A gbDemo/sources/rhythm_choreography.cpp
- A gbDemo/sources/rhythm_choreography.h
- A gbDemo/sources/rhythm_crowd_choreography.cpp
- A gbDemo/sources/rhythm_crowd_choreography.h
- A gbDemo/sources/rhythm_dance_pose.cpp
- A gbDemo/sources/rhythm_dance_pose.h
- A gbDemo/sources/rhythm_gesture.cpp
- A gbDemo/sources/rhythm_gesture.h
- A gbDemo/sources/rhythm_prompt_model.cpp
- A gbDemo/sources/rhythm_prompt_model.h
- A gbDemo/sources/rhythm_prompt_view.cpp
- A gbDemo/sources/rhythm_prompt_view.h
- A gbDemo/sources/rhythm_stage_state.cpp
- A gbDemo/sources/rhythm_stage_state.h
- A gbResources/kulakovka-active-sport-618585.mp3
- A gbResources/kulakovka-adrenaline-sport-620584.mp3
- A gbResources/kulakovka-gym-motivation-620569.mp3
- A gbResources/kulakovka-retro-music-620580.mp3
- A gbResources/kulakovka-summer-party-620566.mp3
- A gbResources/kulakovka-summertime-620526.mp3
- A gbResources/kulakovka-synthwave-pop-620528.mp3
- A gbResources/kulakovka-upbeat-energetic-rock-614902.mp3
- A gbResources/kulakovka-workout-620537.mp3
- A gbScripts/check-rhythm-directional-runtime.ps1
- A gbScripts/check-rhythm-gestures.ps1
- A gbScripts/check_rhythm_gestures.cpp

Final verification completed:
- Native Vulkan Debug runtime for every music entry 0 through 8: PASS, each with 50 characters, running PCM clock, keyboard scoring, FPS/polygon statistics, 680x960 multiframe captures and normal shutdown. Logs/captures: runtime-debug-50-False-False-track0..8-f8False.
- Native Vulkan Release track8 with 50 characters: PASS (final-release-runtime.log); Release directional/cancellation/UI-capture/focus-loss flow: PASS (final-release-directional.log).
- Latest flat-color prompt compiled in both Debug and Release; native image inspected after the change. Final asset/project/hash/retired-resource check PASS; git diff --check PASS.
- Scope complete for approved mechanics-only upgrade. Recommended follow-up: mobile/touch gesture adapter using the same pure recognizer, then richer sprite dance animation. Neither is implemented here.
