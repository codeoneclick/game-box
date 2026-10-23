# Rhythm Ship demo

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

The compact indicator above the player's upper body shows the current command; its cyan ring contracts towards the beat. Smaller icons above it preview the next two commands (configurable 0–3). A diamond means Tap; arrows indicate direction. Resolved commands dim, and judgement feedback fades separately. Command ownership changes at the nearest-event midpoint, with exact ties going to the earlier event. The yellow player ring and YOU label identify the player.

Events form deterministic repeated musical phrases. Easy uses Tap/Left/Right with rests. Normal adds Up/Down and occasional eighth-note phrases. Hard uses more eighths and direction changes. Generated events and musical pulses are separate: higher difficulty does not accelerate the audio or stage beat clock.

One event resolves once. Correct actions within inclusive **40/80/130 ms** windows earn Perfect/Great/Good and **100/70/30** points. A different direction produces **Wrong**, consumes the nearest event and resets combo; expired events produce Miss. Repeating an already resolved nearest event cannot skip ahead. Accuracy is earned points divided by possible points across judgements. Positive personal calibration is subtracted from input time once; music and visuals keep raw song time.

Temporary player poses follow each action and return to the original position. The 30–50 character crowd uses deterministic timing bias, occasional wrong actions and misses. Shared outcomes drive both poses and leaderboard; CrowdSync represents recent coordination. Directional strips/panels, Tap speaker pulses and a fixed three-slot floor-ring pool react without creating per-beat entities. The DJ retains an idle beat animation.

## F1 pages

| Page | Controls |
|---|---|
| Gameplay | Easy/Normal/Hard, five action toggles, Generate Beatmap, five action test buttons, swipe threshold, crowd count, energy, Force Beat, Become Rank #1 |
| Timing | Perfect/Great/Good windows, authored map offset, BPM for generated pulse maps |
| Visuals | Indicator/crowd/directional stage toggles, volume, stage intensity, prompt lead, preview count |
| Calibration | Start, Cancel, Reset offset, manual personal offset |

All-disabled action settings and invalid timing edits retain the last valid state. Difficulty/action changes validate a generated candidate before restarting music and resetting score. Explicit authored charts retain their actions until Generate Beatmap is deliberately requested. Action test buttons judge at current audio time, so an off-time test can result in Miss. Force Beat changes presentation only.

Calibration plays four count-in clicks, then collects 24 Space/click taps. A consistent run saves the median signed bias; noisy/incomplete runs retain the previous value. Cancel restores the selected song from its start. The manual range is -250 to +250 ms; positive means late taps. Preferences live separately in ignored `gbResources/RhythmAudioSettings.json`. This is practical tap bias, not physical device latency.

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

`audio_file` optionally names a local MP3 basename. Times are seconds; offset applies once to both timelines. Musical pulses must be ordered with cyclic spacing greater than 260 ms; playable events allow 60 ms, including the loop seam. Events must be finite, ordered and inside decoded duration, with at most 60,000 entries. Action spelling is exact. Explicit `events` override generation. Without events, `"choreography":"phrases"` requests phrase generation; absent or `"legacy"` preserves Tap-only beat compatibility. Missing `beats` generates the legacy BPM pulse. Invalid resources produce a diagnostic/fallback.

`RhythmAudioTransport` in gbSound decodes with pinned dr_mp3 and plays PCM through WinMM on Windows, using played-sample position. Apple source owns an OpenAL PCM source/buffer on the existing audio context; it does not use asynchronous `audio_engine::play2d`. Only active PCM is cached. Missing/corrupt/mismatched music uses the metronome and its matching map. Device failure uses a labelled SILENT CLOCK with the selected map. No network music or streaming is involved.

## Code ownership and verification

Application rules stay in `gbDemo/sources/rhythm_*`: typed session/judgement, phrase generation, gestures, playback, prompt model/view, crowd outcomes, poses and stage state. `ces_dance_crowd_system` applies shared pose state through existing ECS. The native gbUI HUD uses existing sprite/label materials. Registration covers MSVC and all three Xcode targets through `gbScripts/register-rhythm-ship.py`. Engine input additions are default no-op cancellation/focus-loss notifications; Win32 sends them while retaining legacy synthetic release and modifier cleanup.

```powershell
./gbScripts/check-rhythm.ps1
./gbScripts/check-rhythm-audio.ps1
./gbScripts/check-rhythm-music.ps1
./gbScripts/check-rhythm-gestures.ps1
./gbScripts/check-rhythm-directional-runtime.ps1
./gbScripts/check-rhythm-controls.ps1
python3 gbScripts/check-rhythm-ship-assets.py
./gbScripts/check-rhythm-ship-runtime.ps1 -CrowdCount 50
./gbScripts/check-rhythm-ship-runtime.ps1 -Silent -DebugPanel
```

Use a real Python 3 (this machine's `python` is Python 2.7). Build staging copies canonical assets and removes exact retired staged basenames when absent from source. Logs/captures are ignored under `gbDemo/output/rhythm-check`. Environment options: `GB_RHYTHM_TRACK=0..9`, `GB_RHYTHM_CROWD_COUNT=30..50`, `GB_RHYTHM_DEBUG=1`, `GB_RHYTHM_DIAGNOSTICS=1`, `GB_RHYTHM_SILENT=1`, and `GB_RHYTHM_AUDIO_SETTINGS=<path>`.

Current verification evidence and changed-file groups: [rhythm-gestures-progress.md](rhythm-gestures-progress.md). This Windows host does not verify Apple/Metal, dormant OpenGL or server. Audible alignment has not been independently heard/recorded. Retained island smoke previously timed out at 45 seconds; its runtime remains UNKNOWN. Art/geometry are placeholders; there is no networking, buoyancy or ship movement.
