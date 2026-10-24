# Rhythm Ship stage prompt and anchored steps вЂ” 2026-10-24

User correction to f2adde7d: move the action prompt from the player to the front of the DJ booth; Tap must be a circle; four swipes must produce steps along the deck and return to the character's anchor; every bot uses its own 0вЂ“330 ms reaction delay.

Implemented using existing demo helpers and native sprite material; no new engine interfaces, source memberships, assets or shaders. The fixed world prompt anchor is (0, 8, -23), projected before UI update. Tap is a filled circle; arrow rotations are exact -90/90/0/180 degrees for Left/Right/Up/Down. The timing ring and previews remain with the relocated prompt. The old player-attached indicator is gone.

Step offsets use ship X/Z coordinates: Left/Right along X, Up towards the DJ (-Z), Down away (+Z). The envelope moves out over 180 ms, holds for 100 ms and returns by 550 ms. Each frame derives position from base_position plus the temporary offset; horizontal idle drift was removed. Vertical idle dance remains. A subsequent swipe replaces the temporary pose; it cannot accumulate displacement or change the anchor.

Each bot has a deterministic delay in [0, .33) seconds, fixed for the run and independent of event ID. Its outcome and movement start at event time plus that delay. Existing occasional misses/wrong actions remain. Delays beyond the Good window score as misses, reflecting rhythm skill. Shared outcomes still feed both choreography and leaderboard.

During native inspection, verified that the vendored GLM rotate() converts its angle from degrees internally. The prior prompt supplied radians, which barely rotated arrows. Converted prompt layout angles and crowd billboard yaw/lean to degrees at the transform boundary. No engine/vendored changes.

Checks:
- Gesture regression reproduced old Up/Down jump/crouch, then passed after planar steps; separate delay regression reproduced early bot outcomes, then passed with fixed nonnegative per-bot delays.
- check-rhythm-gestures.ps1: PASS, including four step directions, exact return at 550 ms, stable bounded bot delays, deterministic update partitioning, and existing gesture/prompt/stage checks.
- check-rhythm.ps1: PASS.
- Windows Debug and Release builds: PASS (stage-step-degrees-debug.log, stage-step-degrees-release.log).
- Vulkan Debug 50-character portrait runtime, scoring and normal shutdown: PASS (stage-step-degrees-runtime.log). Captures inspected for relocated circle/ring, previews and crowd layout.
- Native directional controls, cancellation, F8, stale F1 captures and focus loss: PASS (stage-step-input.log).
- check-rhythm-ship-assets.py and git diff --check: PASS.
- Independent source review and targeted degree-conversion re-review: no actionable findings.

Logs/captures are ignored under gbDemo/output/rhythm-check. Apple/Metal, dormant OpenGL and server builds/runtime remain unverified on this Windows host. Audio content/transport and ocean/skybox/stage geometry were not changed.

Final Release Vulkan 50-character runtime and Release native directional/cancellation/UI/focus-loss checks also passed (stage-step-final-release-runtime.log, stage-step-final-release-input.log), including normal shutdown.

## More forgiving rhythm timing вЂ” 2026-10-24

User reported that actions changed too quickly and were too hard to hit. Normal now schedules one command every two musical pulses, with no eighth-note bursts (0.75вЂ“1.25 seconds between actions across the nine-song pack). Easy uses one every three pulses; Hard retains its deliberate quarter/eighth challenge. Generated charts allow four musical pulses of preparation before the first action. Music BPM and the separate visual beat clock are unchanged.

Default judgement windows increased from +/-40/80/130 ms to +/-90/180/300 ms: the full accepted interval is now 600 ms. Timing sliders and validation allow manual windows up to 500 ms. Prompt lead increased from 1.25 to 2 seconds. The current icon still uses the same ownership boundaries as judgement; Normal's slower spacing allows the entire default acceptance window without a competing next action. Explicit authored event timing is preserved.

New regression checks reproduced old default-window and fast-command behavior before the changes. After changes, check-rhythm.ps1 and check-rhythm-gestures.ps1 pass, including early/late inclusive score boundaries, expiry, Normal 160 BPM spacing >=750 ms, accepted late +290 ms input with matching current prompt, preparation, and adjustable windows. check-rhythm-music.ps1 passes; its calibration check now derives the first generated event instead of assuming an immediate startup note. Source review found no actionable issues.

Windows Debug and Release builds pass (relaxed-timing-debug-build.log, relaxed-timing-release-build.log). Asset/project checks pass. Native runtime results are recorded below. Logs remain ignored output artifacts. Apple/Metal, OpenGL and server remain unverified.

Native Vulkan Debug and Release runs with 50 characters, advancing PCM clock, keyboard scoring, 680x960 captures and normal shutdown: PASS (relaxed-timing-debug-runtime.log, relaxed-timing-release-runtime.log). Debug silent F1/F8 inspector flow: PASS (relaxed-timing-inspector.log). Final git diff --check: PASS.

## Musical alignment and playable phrase rhythms вЂ” 2026-10-24

User identified Adrenaline Sport as the worst mismatch and clarified that action sequences may be irregular (Tap/Tap/Tap, a rest, then Up), with reproduction of the music more important than a fixed command interval.

Diagnosed and corrected three separate causes:
- Visual command switching used the Good expiry deadline. Expanding it to 300 ms shifted the visual transition between musical subdivisions. Visual ownership now uses only nearest-event midpoints, matching judgement selection, independently of tolerance. The ring normalizes from the actual cue/ownership start, so it resets fully and contracts on the target instead of appearing already mostly closed. A 120 ms white icon pulse marks the target audio time.
- Adrenaline's 139 BPM grid had selected a 231 ms half-beat spectral accent. Low-frequency analysis of original runtime-decoded PCM supports the 11вЂ“40 ms kick family; the selected map now starts at 11 ms, 220 ms earlier. The manifest retains original mixed-band sections and the new bass audit. Canonical analyzer selected-grid bass strength is about 1.152 vs 0.848 on its half-beat counterpart. This is signal evidence for the phase family, not independent audible confirmation or sample-perfect kick annotation. Other tracks' phases and original MP3 bytes are preserved.
- Tap animation had peaked 275 ms after the input. Its accent now peaks in 60 ms and returns by 240 ms, so consecutive taps remain distinct. Step accent reaches its displacement in 100 ms, holds 80 ms and still returns to the anchor by 550 ms. Bot reaction delays remain unchanged.

Normal now uses designed beat-offset patterns with Tap series and rests within a 12-beat phrase. Intro: Tap at offsets 0/1/2, Up at 7, then a trailing rest before the next phrase. The gap before Up is about 2.16 s on Adrenaline Sport. Direction changes have breathing room; Easy stretches patterns; Hard retains its quarter/eighth challenge. These are patterns on the measured musical grid, not automatic transcription of every instrument/fill. Wider acceptance windows remain; nearest ownership divides overlapping windows in rapid series.

Verification:
- RED then GREEN for independence from Good expiry, full ring restart/target convergence and target pulse; phrase series/rests; quick Tap/step response; Adrenaline phase fixture.
- check-rhythm-gestures.ps1, check-rhythm.ps1 and check-rhythm-music.ps1: PASS. The catalog check verifies every generated Normal action is exactly on a resolved musical beat for all nine songs and the metronome.
- Canonical PCM analyzer rerun for all nine songs; selected-phase evidence matches manifest phases. Review found a phase-search variable shadowing bug in the new audit report; reproduced RED, renamed search variable and verified all nine selected-phase reports GREEN. It never affected runtime map generation.
- Windows Debug/Release builds PASS (music-sync-debug-build.log, music-sync-release-build.log). Debug Adrenaline with 50 dancers, running PCM clock, scoring, portrait multiframe capture and normal close PASS (music-sync-adrenaline-debug.log); image inspected. Native directional/cancellation/F8/F1 capture/focus-loss checks PASS (music-sync-directional.log).
- Asset/project/hash checks PASS. Independent source review plus targeted audit-fix review: no remaining Important findings.

Output reports, overlay WAVs and logs remain ignored under gbDemo/output/rhythm-check. Audible alignment on the user's output device remains unconfirmed; no audio loopback/listening measurement was available. Apple/Metal, OpenGL and server remain unverified.

Final Release Adrenaline 50-character runtime and Debug silent F1/F8 inspector flow PASS (music-sync-adrenaline-release.log, music-sync-inspector.log), including normal shutdown. Final git diff --check PASS.

## 2026-10-24: PCM accents and a shared Bluetooth AV timeline

Supersedes the previous BPM-template and input-only calibration descriptions above. All nine MP3 charts now select from measured PCM attacks, energy rises/reentries and timbre/stereo changes. Source timestamps/actions are preserved through difficulty/action filtering; no repeating phrase table places MP3 commands. Stronger accents receive priority with burst/recovery limits. Directions are heuristic suggestions; semantic drop recognition and perceptual quality are not proven. Original MP3 hashes remain unchanged.

Bluetooth diagnosis found that calibration previously corrected scoring only. Prompts, local movement, crowd and scoring now share endpoint time minus the same saved AV offset. Manual range is +/-600 ms. Calibration uses four count-in pulses and 24 heard-click taps at 1.5-second spacing; no guessed device offset is installed. Changing offset resets the chart/input epoch. Failed chart publication now ends loading, disables scoring and retains PCM for recovery after settings corrections. Input-driven dancers no longer add unrelated idle motion.

A scratch WASAPI loopback probe measured 13/13 clicks: WinMM position versus Windows endpoint onset +1.485..1.577 ms, mean 1.514 ms; residual drift 0.0077 ms/s. Startup delay was already followed by the sample clock. Physical Bluetooth acoustic delay is UNKNOWN and requires calibration on the user's device. No transport/core/backend implementation change was justified. See [audio analysis](rhythm-ship-audio-analysis.md) for methodology and limitations.

Checks: synthetic known-attack/silence/shifted-PCM regression, native C++ domain/gesture/music/sync checks and asset/hash/project checks PASS. Music checks compare every generated action directly with source candidates across all nine songs, independently of BPM. RED/GREEN regressions cover shared timing and failed-publication recovery. Independent final source review and targeted recovery re-review found no remaining Important findings.

Windows Vulkan Debug/Release builds PASS (`accents-av-final-debug-build.log`, `accents-av-final-release-build.log`). Final Adrenaline native Debug/Release with 50 dancers, typed keyboard scoring, portrait captures and normal shutdown PASS (`accents-final-native.log`, `accents-final-release-native.log`). The old Tap-only smoke failed on the directional introduction; the harness now accepts a generated `-InputChart` and sends its actual actions. F1/F8 inspector and directional/cancellation flows PASS (`accents-final-panels-native.log`, `accents-final-directional-native.log`); fresh Adrenaline capture inspected. Output logs/captures remain ignored under `gbDemo/output/rhythm-check`. Apple/Metal, dormant OpenGL and server remain unverified.

Final full native calibration flow PASS (`accents-final-controls-native.log`): track switching, 24 taps, save, reset, cancel and normal shutdown. Final `git diff --check` PASS.

## 2026-10-24: action indicator above the player

Moved the existing action prompt from the DJ booth to the local player's projected head position. It follows the current shared dance pose and initially used 120 screen pixels of clearance so feedback/phrase/combo text stays above the character. The projection uses the current pose because the scene action callback precedes the crowd ECS transform update. Existing screen bounds and prompt rendering are retained.

Verification: Windows Vulkan debug-client and release-client x64 builds PASS; Adrenaline native run with 50 dancers and typed chart input PASS (scoring, portrait captures, normal shutdown). Fresh 160-frame capture inspected for player alignment and text clearance. `git diff --check` PASS. Apple/Metal and OpenGL remain unverified.

### Screenshot adjustment

Moved the prompt down 75 screen pixels (45 pixels above the projected head) to match the marked reference. Main action icon scale is 2.4 times its previous size; the timing ring now keeps sufficient minimum radius around it. Phrase/combo labels sit to the right so lowering the prompt does not put them over the player. Preview sizes remain unchanged.

Verification: Debug/Release Windows Vulkan builds PASS; native Adrenaline flow PASS; fresh capture inspected against the requested lower placement. git diff --check PASS. Other backends remain unverified.
