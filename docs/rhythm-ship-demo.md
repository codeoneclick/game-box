п»ї# Rhythm Ship demo

The default demo is a stationary ship with directional rhythm gameplay. The rectangular deck, daylight, ocean renderer and skybox are retained. Windows uses a **680 x 960 portrait** client area, with FPS, submitted polygons/frame and draw calls/frame in the bottom HUD. `GB_DEMO=island` selects the retained island.

## Build and controls

```powershell
./gbScripts/build-gbDemo.ps1 -Configuration debug-client -Platform x64
Push-Location gbDemo
./output/debug/x64/gb_demo.exe
Pop-Location
```

- **Space / small left click:** Tap. **Arrow keys / mouse swipe:** Left, Right, Up, Down. There are no holds. Release the key before repeating it.
- Mouse gestures begin outside UI controls. A swipe fires once on crossing 24 pixels with dominant axis at least 1.25 times the other axis. Up means decreasing screen Y. Small releases produce Tap; ambiguous diagonals cancel. Swipes use audio time at threshold crossing; taps use release time.
- **R:** restart selected music using cached decoded PCM. **Prev / Next:** select nine Kulakovka tracks or the original metronome (entries 0 through 9).
- **F1:** gameplay settings. **F8:** original Render Inspector, including pass switches, attachment previews, water/light/post settings and Save. Scroll the hovered column with the mouse wheel.

Loading, restarts, calibration changes, capture cancellation and F8 invalidate pending gestures. UI clicks do not become gameplay taps. Hidden/reopened F1 controls invalidate their captured interaction.

## Gameplay and presentation

The compact indicator above the local player shows the current command and follows the player during directional steps; its cyan ring contracts towards the beat, then the icon flashes white on the target. Smaller icons above it preview the next two commands (configurable 0вЂ“3). A filled circle means Tap; arrows indicate direction. Resolved commands dim, and judgement feedback fades separately. Command ownership changes at the nearest-event midpoint, with exact ties going to the earlier event, independently of hit-window expiry. Ring progress starts from the actual cue start rather than assuming its entire two-second lead was visible. The yellow player ring and YOU label identify the player.

MP3 actions are selected from measured attacks and changes in the recording's energy and timbre. Their exact source timestamps are preserved: no repeating action template or fixed BPM interval places these commands. Short Tap groups and longer rests follow available accents; difficulty filters strength and density and leaves recovery time around swipes. The initial 1.5 seconds allow preparation. Easy disables Up/Down. Legacy maps and the metronome retain their synthetic fallback. Prompt lead defaults to two seconds. Direction suggestions use energy, timbre and stereo changes; this is a heuristic transcription, not a semantic identification of every musical drop.

One event resolves once. Correct actions within inclusive **В±90/180/300 ms** windows earn Perfect/Great/Good and **100/70/30** points. The default full acceptance interval is up to 600 ms; in dense series the nearest event owns overlapping windows. F1 Timing sliders permit windows up to 500 ms. A different direction produces **Wrong**, consumes the nearest event and resets combo; expired events produce Miss. Repeating an already resolved nearest event cannot skip ahead. Accuracy is earned points divided by possible points across judgements. Positive AV calibration is subtracted once from raw endpoint time for prompts, player movement, crowd and judgement. The audio itself continues unchanged. Changing this offset restarts the chart and clears pending inputs.

Swipes produce one step along the deck: Left/Right along ship X, Up towards the DJ booth (-Z), Down away from it (+Z). Each step reaches its accent in 100 ms, settles for 80 ms and returns by 550 ms; offsets always derive from the character's anchor, so movement cannot accumulate. Tap's small vertical bounce peaks in 60 ms and returns by 240 ms, keeping tap series distinct. The 30вЂ“50 character crowd uses a stable individual reaction delay from 0 to 330 ms, with occasional wrong actions and misses. Shared delayed outcomes drive poses and leaderboard; delays beyond the Good window count as misses. CrowdSync represents recent coordination. Directional strips/panels, Tap speaker pulses and a fixed three-slot floor-ring pool react without creating per-beat entities. The DJ retains an idle beat animation.

## F1 pages

| Page | Controls |
|---|---|
| Gameplay | Easy/Normal/Hard, five action toggles, Refresh accents, five action test buttons, swipe threshold, crowd count, energy, Force Beat, Become Rank #1 |
| Timing | Perfect/Great/Good windows, authored map offset, BPM for generated pulse maps |
| Visuals | Indicator/crowd/directional stage toggles, volume, stage intensity, prompt lead, preview count |
| Calibration | Start, Cancel, Reset offset, manual AV sync offset |

All-disabled action settings and invalid timing edits retain the last valid state. Difficulty/action changes validate a generated candidate before restarting music and resetting score. Explicit authored charts retain their actions until Refresh accents is deliberately requested. Action test buttons judge at current audio time, so an off-time test can result in Miss. Force Beat changes presentation only.

Calibration plays four count-in clicks, then collects 24 Space/click taps at 1.5-second intervals. Tap to the heard clicks, including when using Bluetooth. A consistent run saves the median signed bias; noisy/incomplete runs retain the previous value. Cancel restores the selected song from its start. The manual AV sync range is -600 to +600 ms; positive values delay visual cues to the heard sound. Preferences live separately in ignored `gbResources/RhythmAudioSettings.json`, with an initial offset of zero. This measures combined device and tapping bias; physical Bluetooth latency cannot be inferred from the Windows endpoint sample position alone.

## Beatmaps and audio

Canonical catalog: `gbBundle/Configurations/rhythm_ship/rhythm_ship_catalog.json`. Source fingerprints, decoded frame counts and measured timing: `rhythm_ship_music.json` in the same directory. New MP3 filenames and bytes are preserved in `gbResources`; the old three-song resources are retired. Default: Active Sport, 160 BPM. Measurement notes: [rhythm-ship-audio-analysis.md](rhythm-ship-audio-analysis.md).

An authored chart can separate musical pulse times from typed playable events:

```json
{
  "track": "example", "bpm": 120, "duration": 8, "offset": 0,
  "beats": [0.5, 1, 1.5, 2, 2.5, 3, 3.5, 4],
  "events": [
    {"time": 1, "action": "Tap"},
    {"time": 1.5, "action": "SwipeLeft"},
    {"time": 2, "action": "SwipeRight"},
    {"time": 2.25, "action": "SwipeUp"},
    {"time": 2.5, "action": "SwipeDown"}
  ]
}
```

`audio_file` optionally names a local MP3 basename. Times are seconds; offset applies once to both timelines. Musical pulses must be ordered with cyclic spacing greater than 260 ms; playable events allow 60 ms, including the loop seam. Events must be finite, ordered and inside decoded duration, with at most 60,000 entries. Action spelling is exact. Explicit `events` override generation. Without events, `"choreography":"accents"` selects from a required `accents` array of `{ "time": 1.83, "action": "Tap", "strength": 0.8 }` entries. Strength is finite and in [0,1]. `"choreography":"phrases"` requests the legacy phrase generator; absent or `"legacy"` preserves Tap-only beat compatibility. Missing `beats` generates the legacy BPM pulse. Invalid resources produce a diagnostic/fallback.

`RhythmAudioTransport` in gbSound decodes with pinned dr_mp3 and plays PCM through WinMM on Windows, using played-sample position. Apple source owns an OpenAL PCM source/buffer on the existing audio context; it does not use asynchronous `audio_engine::play2d`. Only active PCM is cached. Missing/corrupt/mismatched music uses the metronome and its matching map. Device failure uses a labelled SILENT CLOCK with the selected map. No network music or streaming is involved.

## Code ownership and verification

Application rules stay in `gbDemo/sources/rhythm_*`: typed session/judgement, accent selection and legacy phrase generation, gestures, playback, prompt model/view, crowd outcomes, poses and stage state. `ces_dance_crowd_system` applies shared pose state through existing ECS. The native gbUI HUD uses existing sprite/label materials. Registration covers MSVC and all three Xcode targets through `gbScripts/register-rhythm-ship.py`. Engine input additions are default no-op cancellation/focus-loss notifications; Win32 sends them while retaining legacy synthetic release and modifier cleanup.

```powershell
./gbScripts/check-rhythm.ps1
./gbScripts/check-rhythm-audio.ps1
./gbScripts/check-rhythm-music.ps1
./gbScripts/check-rhythm-gestures.ps1
./gbScripts/check-rhythm-sync.ps1
python3 gbScripts/check-rhythm-accents.py
./gbScripts/check-rhythm-directional-runtime.ps1
./gbScripts/check-rhythm-controls.ps1
python3 gbScripts/check-rhythm-ship-assets.py
./gbScripts/check-rhythm-ship-runtime.ps1 -CrowdCount 50
# After check-rhythm-music.ps1 exports generated input evidence:
./gbScripts/check-rhythm-ship-runtime.ps1 -CrowdCount 50 -Track 1 -InputChart gbDemo/output/rhythm-check/playable-adrenaline_sport.json
./gbScripts/check-rhythm-ship-runtime.ps1 -Silent -DebugPanel
```

Use a real Python 3 (this machine's `python` is Python 2.7). Build staging copies canonical assets and removes exact retired staged basenames when absent from source. Logs/captures are ignored under `gbDemo/output/rhythm-check`. Environment options: `GB_RHYTHM_TRACK=0..9`, `GB_RHYTHM_CROWD_COUNT=30..50`, `GB_RHYTHM_DEBUG=1`, `GB_RHYTHM_DIAGNOSTICS=1`, `GB_RHYTHM_SILENT=1`, and `GB_RHYTHM_AUDIO_SETTINGS=<path>`.

Gesture upgrade evidence and changed-file groups: [rhythm-gestures-progress.md](rhythm-gestures-progress.md). The subsequent DJ prompt/anchored-step update is recorded in [rhythm-stage-steps.md](rhythm-stage-steps.md). This Windows host does not verify Apple/Metal, dormant OpenGL or server. Audible alignment has not been independently heard/recorded. Retained island smoke previously timed out at 45 seconds; its runtime remains UNKNOWN. Art/geometry are placeholders; there is no networking, buoyancy or ship movement.
