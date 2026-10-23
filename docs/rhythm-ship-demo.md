# Rhythm Ship demo

The default Windows demo now opens a stationary luxury liner in the existing
ocean. The island/editor does not initialize. Ocean/sky shader and material
sources are unchanged; saved lighting/water presentation preferences are reused.
`GB_DEMO=island` selects the retained island demo.
The Windows client window is portrait **680 x 960**. The HUD displays FPS,
submitted polygons/frame (including render passes) and draw calls/frame.

Build and launch from repository root:

```powershell
./gbScripts/build-gbDemo.ps1 -Configuration debug-client -Platform x64
Push-Location gbDemo
./output/debug/x64/gb_demo.exe
Pop-Location
```

Controls: Space or LMB taps; R restarts; F1 toggles development controls;
F8 opens the previous Render Inspector with pass switches, attachment previews,
lighting/post/water settings and Save. The inspector fits the portrait window;
its mouse wheel scrolls the hovered column. Clicking
HUD/debug controls does not tap. The panel offers BPM, volume, offset, three hit
windows, energy, 30-50 dancers, stage intensity and four debug buttons. It uses
native sliders, so no text focus or virtual keyboard is needed. Invalid timing
edits retain the last valid values. Force Beat changes visuals only.

Default: Audioera at 120 BPM, 40 dancers plus DJ. A preallocated pool supports 50. The player
has a yellow ring and YOU label. Leading with a positive score adds a crown and
STAR OF THE DANCE FLOOR. Ten seeded simulated rivals update once per beat.

Use `Prev` / `Next` to select Audioera (120 BPM), Ikoliks (107 BPM),
Strawberry Candy (185 BPM), or the original metronome (128 BPM). Selection resets
score and bots; R reuses the active decoded music. Loading freezes scoring and
stale results cannot start a previous song. Missing/corrupt/mismatched audio
falls back to the metronome with its own map and a visible diagnostic.

Track catalog: `gbBundle/Configurations/rhythm_ship/rhythm_ship_catalog.json`.
The original metronome configuration remains `rhythm_ship_track.json`.
Fields: `track`, `bpm`, `offset` (seconds), `duration` (seconds), optional
`audio_file`, optional strictly ordered `beats` array. Explicit times receive the
offset. The initial windows require cyclic spacing greater than 260 ms. Defaults
generate one beat per interval; loop endpoint beats are included once. Invalid
or missing JSON logs a diagnostic and uses the default metronome.

`RhythmSession` owns renderer-independent beat events, queued timestamped taps,
judgements, score/combo/accuracy and clamped energy. Taps resolve before expiration
at their timestamp. Each beat is scored once; missed deadlines reset combo.
Windows are inclusive 40/80/130 ms, worth 100/70/30 points. Accuracy divides earned
points by possible points across resolved beats and stray misses. Debug score
override does not fabricate accuracy.

`RhythmAudioTransport` in gbSound plays local MP3s decoded by a pinned dr_mp3
revision and can generate original PCM clicks. Windows playback
uses WinMM and played-sample position, independently of frame time. An unavailable
device uses labelled SILENT CLOCK mode and `steady_clock`. Apple source initializes
the existing engine's OpenAL context and owns a synchronous PCM source/buffer with
sample-position queries. Both output paths use the PCM's channel count/rate.
Original MP3s remain under `gbResources`; build staging handles Windows copies,
and Apple target resources reference the originals. No streaming/OGG is added.

`Calibrate` starts four count-in clicks followed by 24 taps. Use Space/click in
time; a consistent run saves the median signed offset, while incomplete/noisy
runs keep the old value. Cancel restores the selected song from its start.
The manual slider adjusts -250 to +250 ms; Reset offset restores zero.
Preferences are separate in `gbResources/RhythmAudioSettings.json` (ignored by
Git). Positive means late taps and is subtracted once; visuals retain raw song
time. This measures practical tap bias, not physical device latency. Calibration
requires hearing the metronome; silent mode is useful for deterministic checks
but cannot establish an audible timing preference.

Analysis, exact sample counts, loop seam decisions, decoder limits and known
alignment uncertainty: [rhythm-ship-audio-analysis.md](rhythm-ship-audio-analysis.md).

One queried ECS system animates upright pixel-art quads from the song clock.
Eight shared costume meshes use one original nearest-filtered atlas. The DJ
uses a riser behind the booth. Ordinary mesh/material APIs draw the deck, stage,
panels, ring and crown; no renderer core changes or new lighting architecture.
Speaker/panel pulses and oscillating strips consume beat state and energy.

Development environment variables:

- `GB_RHYTHM_TRACK=0..3`: initial catalog entry (default 0).
- `GB_RHYTHM_AUDIO_SETTINGS=<path>`: alternative calibration preferences file.

- `GB_RHYTHM_SILENT=1`: force silent clock.
- `GB_RHYTHM_CROWD_COUNT=30|40|50`: initial population.
- `GB_RHYTHM_DEBUG=1`: open the debug panel on startup.
- `GB_RHYTHM_DIAGNOSTICS=1`: log clock, score, combo, energy and leader state.

Checks:

```powershell
./gbScripts/check-rhythm.ps1
./gbScripts/check-rhythm-audio.ps1
./gbScripts/check-rhythm-music.ps1
./gbScripts/check-rhythm-controls.ps1
python3 ./gbScripts/check-rhythm-ship-assets.py
./gbScripts/check-rhythm-ship-runtime.ps1 -CrowdCount 50
./gbScripts/check-rhythm-ship-runtime.ps1 -Silent -DebugPanel
```

Use Python 3; this machine's `python` resolves to Python 2.7. The bundled Blender
5.2 Python executable was used for asset generation/checks. Original art/metronome
can be regenerated with `gbScripts/create-rhythm-ship-assets.py`. Build staging
includes WAV and MP3 assets. Runtime logs/captures are in ignored
`gbDemo/output/rhythm-check/`.

Verification recorded after final fixes: Windows Debug and Release builds;
focused rhythm/audio checks; asset/project membership/compiler-flag checks;
Vulkan 50-character Release PCM run and Debug silent run, real keyboard scoring,
debug leader/energy buttons, rank gain/loss, multiframe captures and normal
shutdown. Independent review's seven Important findings were addressed in one
fix pass; details are in `rhythm-ship-progress.md`. A running playback
clock was measured; audio audibility has not been independently heard/recorded.

An additional retained-island check (`GB_DEMO=island`,
`check-ocean-runtime.ps1 -CaptureName rhythm-retained-island -TimeoutSeconds 45`)
initialized island residents/presentation but did not finish frame capture within
45 seconds; normal shutdown was not verified in that check. Its runtime remains
UNKNOWN; no island source or engine-core files were changed.

Apple/Metal and dormant OpenGL builds/runtime remain UNKNOWN on this Windows
machine; project membership and Metal source are provided. Server is unverified.
The existing sky remains daytime. Art, ship geometry and effects are placeholders;
there is no networking, buoyancy or movement. Real local MP3 playback and
calibration are now implemented; current verification is recorded in
`rhythm-mp3-progress.md`. Multiplayer synchronization remains future work.

## Files changed

- `.gitignore`
- `docs/superpowers/specs/2026-10-22-rhythm-ship-design.md`
- `gbDemo/gbDemo.vcxproj`
- `gbDemo/gbDemo.vcxproj.filters`
- `gbDemo/gbDemo.xcodeproj/project.pbxproj`
- `gbDemo/sources/main_game_controller.cpp`
- `gbScripts/build-gbDemo.ps1`
- `gbSound/gb_sound.vcxproj`
- `gbSound/gb_sound.vcxproj.filters`
- `gbSound/gb_sound.xcodeproj/project.pbxproj`
- `docs/rhythm-ship-demo.md`
- `docs/rhythm-ship-progress.md`
- `docs/superpowers/plans/2026-10-22-rhythm-ship.md`
- `gbBundle/Configurations/game_objects/materials/material.rhythm_ship_character.xml`
- `gbBundle/Configurations/game_objects/materials/material.rhythm_ship_solid.xml`
- `gbBundle/Configurations/game_objects/rhythm_ship_character.xml`
- `gbBundle/Configurations/game_objects/rhythm_ship_solid.xml`
- `gbBundle/Configurations/rhythm_ship/rhythm_ship_track.json`
- `gbBundle/Configurations/transitions/transition.rhythm_ship.xml`
- `gbBundle/Shaders/metal/mtl_rhythm_ship.metal`
- `gbBundle/Shaders/shader_rhythm_ship_character.frag`
- `gbBundle/Shaders/shader_rhythm_ship_character.vert`
- `gbBundle/Shaders/shader_rhythm_ship_solid.frag`
- `gbBundle/Shaders/shader_rhythm_ship_solid.vert`
- `gbBundle/Sounds/rhythm_ship_metronome.wav`
- `gbBundle/Textures/rhythm_ship_characters.png`
- `gbDemo/sources/ces_dance_character_component.cpp`
- `gbDemo/sources/ces_dance_character_component.h`
- `gbDemo/sources/ces_dance_crowd_system.cpp`
- `gbDemo/sources/ces_dance_crowd_system.h`
- `gbDemo/sources/fake_leaderboard.cpp`
- `gbDemo/sources/fake_leaderboard.h`
- `gbDemo/sources/rhythm_key_action.h`
- `gbDemo/sources/rhythm_session.cpp`
- `gbDemo/sources/rhythm_session.h`
- `gbDemo/sources/rhythm_ship_environment.cpp`
- `gbDemo/sources/rhythm_ship_environment.h`
- `gbDemo/sources/rhythm_ship_input.cpp`
- `gbDemo/sources/rhythm_ship_input.h`
- `gbDemo/sources/rhythm_ship_scene.cpp`
- `gbDemo/sources/rhythm_ship_scene.h`
- `gbDemo/sources/rhythm_ship_transition.cpp`
- `gbDemo/sources/rhythm_ship_transition.h`
- `gbDemo/sources/rhythm_ship_ui.cpp`
- `gbDemo/sources/rhythm_ship_ui.h`
- `gbDemo/sources/rhythm_track.cpp`
- `gbDemo/sources/rhythm_track.h`
- `gbScripts/check-rhythm-audio.ps1`
- `gbScripts/check-rhythm-ship-assets.py`
- `gbScripts/check-rhythm-ship-runtime.ps1`
- `gbScripts/check-rhythm.ps1`
- `gbScripts/check_rhythm.cpp`
- `gbScripts/check_rhythm_audio.cpp`
- `gbScripts/create-rhythm-ship-assets.py`
- `gbScripts/register-rhythm-ship.py`
- `gbSound/sources/rhythm_audio_transport.cpp`
- `gbSound/sources/rhythm_audio_transport.h`
