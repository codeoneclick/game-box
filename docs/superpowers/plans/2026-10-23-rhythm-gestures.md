# Rhythm Ship Directional Choreography Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** Replace Tap-only play with musical directional phrases, character-centred timing prompts and coordinated dance/stage responses across the nine replacement tracks.

**Architecture:** Extend the current track/session/playback classes, keeping musical beats separate from playable typed events. Pure phrase, gesture, bot-outcome and pose helpers supply the existing input adapter, crowd ECS system and native UI. Existing gbSound PCM decoding/output and its authoritative clock remain in place.

**Tech Stack:** C++11-compatible demo code, existing ECS/gbUI/JsonCpp, existing WinMM/OpenAL and pinned dr_mp3; Python 3/NumPy for offline music analysis; MSVC/Xcode project metadata.

**Spec:** `docs/superpowers/specs/2026-10-23-rhythm-gestures-design.md` (approved).

## Global Constraints

- Exactly five actions: Tap, SwipeLeft, SwipeRight, SwipeUp, SwipeDown. No holds, long presses, multifinger actions, note highways, networking or a second gameplay framework.
- Mechanics only: retain current rectangular deck/stage geometry, daytime presentation, ocean renderer and skybox. Add gesture effects and radial pulses without rebuilding the stage.
- Preserve portrait 680x960, FPS/polygon/draw-call counters, F1 gameplay debug, F8 Render Inspector, playback, restart, calibration and leaderboard.
- Preserve all nine new MP3 filenames/bytes; do not restore deleted music. Keep the metronome fallback. Default first song Active Sport, default difficulty Normal.
- Musical beat/visual time stays raw. Subtract personal calibration exactly once from input and retain positive-offset miss-expiration delay.
- Same inclusive 40/80/130 ms judgement windows across difficulties; points 100/70/30, Wrong/Miss zero and combo reset.
- Every new source/resource needs applicable MSVC and all three Xcode target memberships; retain narrow exception/RTTI overrides and C++11 compatibility.
- Continue native execution in the current diskoteka checkout as previously selected. Preserve user resource changes. No merge/push.

## Review Focus

- A swipe starting before a restart/selection/loading/calibration/F8 transition must not resolve an event in the new epoch (Task 4).
- Overlapping eighth-note windows must choose by time, not seek a farther event matching the supplied direction (Task 2).
- Changing action toggles or difficulty must validate before replacing the current map; an empty set preserves the last valid session (Task 3).
- Pointer focus/capture loss must not become an unintended release Tap; keyboard repeat and ambiguous drags cannot double-score (Task 4).
- A large update interval or bot timing offset must not duplicate scores/CrowdSync or make bot results depend on frame rate (Task 5).

## File structure and shared contracts

- Extend `gbDemo/sources/rhythm_track.*`: musical beats plus optional authored typed events, strict JSON validation and event resolution.
- Add `rhythm_action.h`: the five-action enum, names and timestamped input/event value types with C++11-compatible constructors.
- Add `rhythm_choreography.h/.cpp`: difficulty/settings and deterministic phrase generation, independent of renderer/audio APIs.
- Extend `rhythm_session.*`: typed judgement, separate musical beat and action-resolution streams, preview queries and counters.
- Add `rhythm_gesture.h/.cpp`: pure primary-pointer state machine. Extend `rhythm_ship_input.*` and `rhythm_key_action.h` for adapter mappings, gating and cancellation.
- Extend `rhythm_ship_playback.*`: source-versus-generated track, settings transaction, input route and monotonic epoch.
- Add `rhythm_crowd_choreography.h/.cpp` and `rhythm_dance_pose.h/.cpp`: deterministic outcomes, bounded sync metric and temporary visual poses. Extend `fake_leaderboard.*`, `ces_dance_crowd_system.*`, `ces_dance_character_component.*` to consume them.
- Add `rhythm_prompt_model.h/.cpp` and `rhythm_prompt_view.h/.cpp`: pure presentation state and native geometry/labels. Refactor `rhythm_ship_ui.*` into compact normal HUD plus F1 pages; wire from `rhythm_ship_scene.*`.
- Extend `rhythm_ship_environment.*` for pooled radial/gesture effects using existing materials; keep deck and ocean/sky construction unchanged.
- Add `rhythm_stage_state.h/.cpp` for portable effect values and fixed-pool pulse lifetime; environment code consumes this state.
- Update `gbBundle/Configurations/rhythm_ship/`, music analysis/report, registration/staging scripts, standalone checkers and native runtime/control scripts.

Common value types in namespace `game`:

- `RhythmAction`: the five actions, no extra gameplay action values.
- `RhythmInput { double time; RhythmAction action; }`.
- `RhythmEvent { double time; RhythmAction action; uint64_t beat_index; uint32_t phrase_id; unsigned phrase_step; }` stores local-song time; runtime wrappers supply globally unique loop IDs.
- `RhythmResolution` contains global event ID, expected event, judgement, input action/time, signed timing error and `has_input`; automatic Miss sets `has_input=false`.
- `RhythmScheduledEvent { uint64_t id; RhythmEvent event; }` wraps a local event in a loop-specific identity/time for previews and consumers.
- `RhythmChoreographySettings`: Easy/Normal/Hard, enabled action mask and deterministic seed.
- `RhythmPresentationSettings`: lead=1.25 seconds, preview_count=2 bounded 0..3, beat/crowd/stage toggles and swipe_threshold=24 logical pixels.

### Task 1: Replace the music pack and verify timing

**Files:** `gbScripts/analyze-rhythm-mp3.py`, `check_rhythm_audio.cpp`, `check_rhythm_music.cpp`, `check-rhythm-ship-assets.py`, `register-rhythm-ship.py`, `build-gbDemo.ps1`; canonical catalog/maps; `gbDemo/gbDemo.xcodeproj/project.pbxproj`; `docs/rhythm-ship-audio-analysis.md`.

**Interfaces:** Retain existing catalog and `decode_rhythm_mp3` interfaces. Add catalog analysis metadata containing original basename, SHA-256, final sample count/rate/channels, selected BPM/phase and confidence/provenance. Runtime map timing remains seconds from the shipped decoder's PCM origin.

- [ ] Change music checks to require nine supplied MP3 entries plus metronome, unique IDs, source hashes, matching decoded duration and no retired catalog/project references. Check staging omits each retired basename when it is absent from source. Run audio/music/asset checkers and record expected failures against the old three-song catalog.
- [ ] Generalize offline analysis to read an explicit catalog/manifest rather than three hardcoded files. Use runtime-decoder PCM; compare tempo-family periodicity, subdivision alignment, independent section phase and drift. Finalize the preliminary families from the spec, generate bounded musical beat maps and record measured phases/uncertainty and any seam omissions. Emit optional click-overlay evidence without modifying MP3s.
- [ ] Replace the old three maps/catalog references with nine new ones and the metronome. Update source resource registration and remove old PBX IDs in every applicable Resources phase. In Windows staging, remove only the three explicitly retired staged basenames when absent from both source roots; verify each absolute target lies directly under the intended staging directory before deletion.
- [ ] Run `check-rhythm-audio.ps1`, `check-rhythm-music.ps1` and Python 3 `check-rhythm-ship-assets.py`; expected all pass with nine real decodes and unchanged source hashes. Run the standard Debug build and confirm staged assets. Commit the music replacement and checker changes, including user-authorized old deletions/new assets.

### Task 2: Typed event model and nearest-time judgement

**Files:** New `rhythm_action.h`; `rhythm_track.*`, `rhythm_session.*`, `gbScripts/check_rhythm.cpp`, `check-rhythm.ps1`; registration/project metadata.

**Interfaces:** Add `resolve_rhythm_events(const RhythmTrack&) -> vector<RhythmEvent>`, `RhythmSession::enqueue_input(RhythmInput)`, `resolutions() const -> const vector<RhythmResolution>&`, `upcoming(double raw_time, size_t count) const -> vector<RhythmScheduledEvent>`, `last_resolution(RhythmResolution& out) const -> bool`. Preserve `enqueue_tap` as a compatibility wrapper, `beat_events()` and musical `beat_position()`. Add per-judgement counts and directional-correctness/debug timing data.

- [ ] Write checks for all five JSON names; malformed/unknown actions, duplicate/nonfinite/out-of-bounds/too-dense events; legacy beat-only Tap behavior; unchanged musical beat positions. Typed Left at the correct time must score only for Left, wrong Right scores zero/Wrong and consumes that event once. Run domain checker and observe missing interfaces/behavior before implementation.
- [ ] Implement strict parsing with the existing bounded full-document JSON helper. `events` is authoritative authored data; absent events retains legacy Tap resolution until explicit phrase generation. Use a 60 ms minimum playable-event/seam spacing and 60000-event cap; preserve the existing conservative musical-beat validation where independent from playable events.
- [ ] Implement closest-time ownership across loop boundaries, including already-resolved nearest events; exact ties go earlier. Add event-window overlap support without direction-based searching. Miss deadlines run without input and every event resolves once; out-of-window inputs produce stray Miss without consuming future events. Reset preserves score/debug semantics of current restart.
- [ ] Add assertions: events at 1.0/1.2 seconds with different actions, input at 1.09 chooses the first even if the second direction matches; tie at 1.1 chooses earlier; duplicate input at a resolved nearest event cannot score the next one; ±40/80/130 ms inclusive boundaries, automatic Miss, loop seam and positive/negative calibrated timestamps. Run domain/audio/music checks; expected pass. Register/build and commit this independently checkable domain slice.

### Task 3: Phrase generation and atomic playback settings

**Files:** New `rhythm_choreography.*`; `rhythm_ship_playback.*`, `rhythm_track.*` mode metadata, focused checks and standalone compile lists.

**Interfaces:** `generate_rhythm_choreography(const RhythmTrack& source, const RhythmChoreographySettings&, RhythmTrack& out, string& error) -> bool`; `RhythmShipPlayback::apply_choreography(settings, bool force_authored=false) -> bool`; `input(RhythmInput)`; `epoch() const -> uint64_t`. Keep source maps in catalog separate from generated active session maps. `tap(double)` remains a calibration/legacy wrapper.

- [ ] Test deterministic identical output for the same source/settings; all seven named phrase templates; Easy excludes Up/Down and is sparser; Normal/Hard use all enabled directions, Hard has more eighths and transitions. Assert empty masks reject without modifying output, musical beats/duration remain identical, and authored events remain authoritative unless explicit Generate is requested. Establish RED in a dedicated `check_rhythm_gestures.cpp` driven by `check-rhythm-gestures.ps1`.
- [ ] Compose phrases by stable track/settings seed, never independent per-event randomness. Easy alternates one/two-beat slots; Normal inserts an eight-action phrase every four phrases; Hard alternates quarter/eighth phrases and longer paired phrases. Skip a final incomplete subdivision or seam event that violates Task 2's minimum spacing rather than stretching time. New shipped tracks explicitly opt into generated mode; legacy mode remains available to existing deterministic checks.
- [ ] On valid settings change, generate/validate before publishing, reuse cached active PCM, reset session/bots/prompt/input epoch atomically and retain volume/calibration. Loading stores desired settings for the latest result; calibration delays gameplay-map publication and ignores directional taps. Increment epoch on selection/restart/loading/calibration transitions and effective map replacement.
- [ ] Test generated-mode fallback, rapid select→settings→calibration→cancel, old decode results, failed empty-mask transaction preserving the current score/map/epoch, and one-time calibration correction. Run gesture/domain/music checks, Debug build and restart/cache smoke. Commit the generation/playback slice.

### Task 4: Mouse gestures, keyboard actions and cancellation

**Files:** New `rhythm_gesture.*`; `rhythm_ship_input.*`, `rhythm_key_action.h`, scene input wiring; gesture checker and native controls smoke.

**Interfaces:** `RhythmGesture::begin(x,y,audio_time,epoch)`, `move(x,y,audio_time,epoch, RhythmInput& out) -> bool`, `end(...) -> bool`, `cancel()`. Threshold/rate-independent logic consumes logical screen coordinates and a configurable threshold; `RhythmShipInput::cancel_gesture()` provides lifecycle cancellation.

- [ ] Write checks for threshold 24, dominant-axis ratio 1.25, screen-up negative Y, four directions, small-motion release Tap, large ambiguous diagonal cancellation, later unambiguous movement, one swipe/no release Tap, stale epoch cancellation and repeat suppression. Run gesture checker; expected new recognizer failures.
- [ ] Implement pure recognizer. Timestamp Swipe at the first qualifying move callback, Tap at release, never at pointer-down. Ignore secondary pointers and drags begun over UI; cancel if capture/gating changes before completion. Map desktop arrows/Space behind the existing platform adapter with Windows virtual codes and macOS physical keycodes.
- [ ] Route all typed inputs through playback once. Calibration accepts only Tap via the same consistent click/key policy; gameplay directional debug tests bypass no scoring rules. Cancel on epoch change, loading, F1/F8 capture, calibration, release outside viewport and teardown. Inspect `input_context_win32.cpp`'s synthetic releases on capture loss: the adapter must distinguish cancellation through native capture/focus validity before emitting; keep native queries in the platform input adapter, never in session/generator code. Do not alter global input semantics merely for this scene.
- [ ] Extend native controls smoke to post real mouse drags in every direction, small clicks, UI slider drags and restart-mid-drag; assert one typed input, correct timestamps/directions, no post-restart/cancel score and no click-through under F8. Run gesture/domain/music checks and Debug build. Commit input slice.

### Task 5: Shared bot choreography, CrowdSync and temporary dance poses

**Files:** New `rhythm_crowd_choreography.*`, `rhythm_dance_pose.*`; `fake_leaderboard.*`, `ces_dance_crowd_system.*`, `ces_dance_character_component.*`, scene wiring; gesture/domain checkers.

**Interfaces:** `RhythmCrowdChoreography::restart(uint32_t seed)`, `update(double raw_time, const RhythmSession&, int active_count)`, `local_resolution(const RhythmResolution&)`, `sync() const -> double`; `dancer_state(size_t index, double raw_time) const` exposes action, start, judgement and intensity. `rhythm_dance_pose(RhythmAction, double age, float strength) -> RhythmDancePose`, where pose contains temporary offset/scale/lean values. `FakeLeaderboard::on_outcome(size_t bot, uint64_t event_id, RhythmJudgement)` receives actual resolved bot outcomes instead of rolling a separate score RNG.

- [ ] Test identical seeded outcomes across different update partitions, missed/late/wrong bots, duplicate event delivery, active-count changes and bounded 0..1 sync with one contribution per participant/event. Test poses for all five actions and exact return to base after a bounded 0.45-second animation. Establish RED in gesture checker.
- [ ] Simulate 49 lightweight crowd bots plus local player; the first ten bots supply named leaderboard rows. Use stable per-event/per-bot skill, timing bias/jitter and error decisions; each outcome's timing/direction determines its score and displayed move. Preserve the DJ as the musical-pulse performer. Keep recent sync records bounded to the last eight events, use active participants at event creation and update local contribution once when available.
- [ ] Apply temporary poses through the existing crowd system's one ECS query, preserving billboard facing and base position/scale. Use raw song time for animation; immediate local input response can be strengthened/corrected by the corresponding judgement. Disabling crowd choreography leaves score/outcomes intact and restores idle dance; restart clears old poses and sync.
- [ ] Run gesture/domain/music checks; run Debug demo with 50 dancers, observe five distinct moves, intentional bot imperfections and no positional drift after repeated phrases. Register new sources and commit this slice.

### Task 6: Character-centred prompts, compact HUD and complete F1 controls

**Files:** New `rhythm_prompt_model.*`, `rhythm_prompt_view.*`; `rhythm_ship_ui.*`, `rhythm_ship_scene.*`; native UI configuration only if required; prompt/gesture checks and runtime/control smoke.

**Interfaces:** `rhythm_prompt_state(session, raw_time, lead_seconds, preview_count) -> current event/ring progress/preview/feedback`; `RhythmPromptView::update(state, projected_anchor, viewport)`; UI accepts shared gameplay/presentation settings and callbacks rather than owning score/generation logic.

- [ ] Test bounded monotonic ring contraction, lead 1.25, preview 0..3, current event retained through its late Good window, resolved fade, Wrong/Miss feedback, loop transition, offscreen/behind-camera anchor and restart clearing. Establish RED in gesture checker.
- [ ] Build arrows/Tap target and ring from shared native meshes/sprites, not Unicode arrow font glyphs. Use one prominent current icon, contracting ring, two dim preview icons and a short phrase fragment. Feedback lasts about 0.45 seconds and remains near the projected player; clamp/fade offscreen without giant panels. Cache meshes and update transforms/colors without per-frame resource creation.
- [ ] Replace large normal HUD panels with compact score/combo/accuracy/energy, expandable leaderboard and lightweight track buttons. Retain readable statistics. Move detailed calibration and development controls into F1 pages. Add difficulty, five toggles, swipe threshold, three windows, lead, preview count and beat/crowd/stage toggles, Generate and Test for every action. Test buttons inject the chosen action at current audio time through the standard playback route; expose signed timing/expected/input/result only in debug. Keep Force Beat visual-only.
- [ ] Test native page navigation, song/calibration controls, invalid empty action mask, generation/restart, debug actions and F8 capture isolation. Inspect 680x960 captures with scene visible and prompts readable at 50 dancers. Run checks and Debug build; commit UI slice.

### Task 7: Directional stage effects and final integrated verification

**Files:** New `rhythm_stage_state.h/.cpp`; `rhythm_ship_environment.*`, scene wiring, runtime/control checks; demo/analysis docs and `docs/rhythm-gestures-progress.md` ledger.

**Interfaces:** `rhythm_stage_state(RhythmAction, double action_age, double beat_age, double energy, double sync, float intensity, bool directional_enabled) -> RhythmStageFrame` supplies bounded sweep, vertical emphasis, floor contraction and glow values. `RhythmStagePulses::trigger(double)` reuses a three-slot ring; `sample(size_t slot, double now)` returns active/scale/opacity with a one-second lifetime. Environment update consumes this portable state and raw musical beat time. Musical pulses and action reactions are separate; visual settings do not change event judgement.

- [ ] Add focused effect-state checks proving direction-dependent sweep/expansion/contraction, bounded intensity, disabled effects leaving game state unchanged, and pulse pool reuse after restart. Establish RED for the effect-state helper before implementation.
- [ ] Add a fixed pool of three radial pulse meshes using existing solid/emissive materials at the DJ/floor centre. Strong musical beats expand/fade them on the unchanged rectangular floor. Tap pulses speakers; Left/Right bias existing strips; Up adds vertical emphasis; Down contracts floor emphasis. Combine CrowdSync/energy without replacing existing lights/ocean/sky or introducing a new shader/pass unless the existing material is demonstrably insufficient.
- [ ] Run `check-rhythm.ps1`, `check-rhythm-audio.ps1`, `check-rhythm-music.ps1`, `check-rhythm-gestures.ps1`, Python 3 asset/project/hash checks, `check-rhythm-controls.ps1`, and both standard Debug/Release builds. Expected all exit successfully; retain meaningful failure logs and native checks instead of describing an engine unit-test suite.
- [ ] Run Windows Vulkan validation with every new track, all five keyboard/mouse actions, Wrong/Miss/duplicate/overlap cases, all difficulties, restart/loading/cancel, calibration, F1/F8 and 50 dancers in 680x960. Inspect prompt/move/stage captures and confirm old staged music cannot play. Record any audible-alignment and native-platform limitations accurately.
- [ ] Use one fresh whole-change reviewer under the retained native execution method. Fix material findings with reproducing checks and rerun affected verification. Record every deviation/ruling and actual completion evidence in the ledger; update demo controls/JSON example/visual-system documentation. Run `git diff --check`, inspect status, verify source hashes and commit intended files. Keep branch diskoteka; no merge/push.

## Self-review and execution notes

Coverage: Task 1 supplies valid replacement audio/maps; Tasks 2/3 own event/phrase/scoring contracts; Task 4 owns timestamp/cancellation; Task 5 owns shared outcomes/poses; Task 6 owns prompts and HUD/debug; Task 7 owns effects, integration and documentation. The five Review Focus conditions have explicit checks in their owning tasks. Musical beats remain independent of generated density and source maps remain independent of the active session. No circular deck/night-lighting work is included. Optional full-phrase celebration and authored section analysis remain outside this increment.

Before implementation, invoke executing-plans, verify the retained checkout preference, read the current root/ECS/render guides and canonical declarations/implementations, and record pre-existing old-track deletions/new-track additions. Track RED/GREEN/build/smoke evidence and rulings in the ledger so work resumes correctly after compaction.
