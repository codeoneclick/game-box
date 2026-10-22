# Rhythm Ship: initial playable demo

Date: 2026-10-22

Status: written design approved by the user on 2026-10-22. Replace the active
island demo with Rhythm Ship, preserve the existing skybox and ocean, and retain
the old demo sources. Implementation plan requires review before execution.

## Intent and scope

Build a playable rhythm party vertical slice inside the existing Game Box C++
engine. Launch directly into a luxury liner deck with a DJ stage, 40 dancing
pixel characters, a clearly marked stationary player, audible metronome,
beat-driven effects, scoring, crowd energy and a simulated leaderboard.

The user's latest instruction to preserve skybox and ocean takes precedence
over the attachment's night-festival suggestion. Preserve existing sky textures,
water appearance, waves, horizon and atmospheric composition. Use colored stage
effects to suggest a festival without replacing or darkening the sky/water.

Remove island terrain, vegetation, residents, reefs, floating sample props and
terrain-editor controls from the new active scene. Retain their source and
assets so the existing demo remains available through an explicit development
selection. Do not delete engine modules or rename legacy public APIs.

Networking, CDN downloads, buoyancy, character movement, production music,
skeletal dancing and a general sprite framework are outside this slice.

## Repository findings

The following are existing APIs or examples, not proposed engine APIs:

| Concern | Evidence and reuse |
| --- | --- |
| Startup | `gbDemo/sources/main_game_controller.cpp` currently selects `island_demo_transition` through a main-queue operation. Replace the default selection and retain an explicit island development option. |
| Scene ownership | `island_demo_transition.*` constructs a `scene_graph` subclass with `gb::ces_entity::construct<T>()` and provides a `scene_fabricator`. Follow this lifecycle. |
| ECS | `ces_car_input_component.*`, `ces_car_boost_system.*` and `game_transition.cpp` establish STTI, component masks and explicit system registration. Keep dance state in lightweight components and process the crowd with one scene-owned system. |
| Rendering | `scene_fabricator::add_materials()`, `create_shape_3d()` and `island_demo_scene.cpp` establish configured materials and procedural mesh upload. Reuse these facade-level operations. |
| Ocean and sky | `ces_ocean_system.*`, `ocean_mesh_builder.*`, `ocean_shader_uniforms.*` and sky/water setup in `island_demo_scene.cpp` establish surfaces, horizon, foam, receivers and interaction masks. Reproduce the necessary setup in the new scene without running island generation. |
| Input | `gbCore/sources/input_context.h` exposes key and pointer listeners, with explicit listener removal. A scene-owned listener translates input into timestamped rhythm actions. |
| Sprites | `scene_fabricator.h` exposes 2D sprites. A suitable existing upright world-space billboard abstraction has not been established. Use a small upright quad implementation for this slice rather than assume 2D sprites work in the 3D pass. |
| Animation | `scene_fabricator::add_animation_sequences_3d()` and island residents demonstrate skeletal animation support. Transform-based dancing does not require it. |
| Audio | `gbSound/sources/audio_engine.h` exposes playback and playback-position queries. The Windows branches in `audio_engine.cpp` are stubs, including a position query returning zero; they cannot supply a song clock. Apple has an OpenAL implementation. |
| UI and debug | The island scene uses `create_label_2d()`, existing UI controls and `ces_action_component`; render-debug/editor controls provide patterns for a small development panel. |
| Configuration | JSON support already exists through JsonCpp, including `graphics_settings.cpp` and `configuration::open_json()`. XML configures render materials and transitions. |
| Build | `gbScripts/build-gbDemo.ps1` builds and stages the Windows demo. Visual Studio and Xcode maintain independent source/resource membership. |

Consult `ENGINE_MAP.md`, `ENGINE_FLOWS.md`, `CODE_PATTERNS.md`,
`ARCHITECTURE_ISSUES.md` and the routed ECS/render guides during implementation.
Read each backend guide before touching that backend's shader implementation.

## Selected approach

Add dedicated `rhythm_ship_scene` and `rhythm_ship_transition` classes in
`gbDemo`, with a new transition XML referencing existing compatible ocean,
composition and UI passes. Keep existing technique identifiers where reused
materials require them. Add only the materials/shaders needed for deck,
billboards and stage effects, with matching Metal implementations.

This is preferred over turning the large island/editor scene into a mode switch:
scene ownership stays explicit and terrain generation, editor jobs and resident
navigation never start. Extracting a general environment framework would add
unnecessary scope. A small scene-local setup helper may organize the reused
ocean/sky construction without changing the existing demo's behavior.

## Scene and presentation

Use a roughly 40 by 85 metre deck with a visible simple hull above water, side
railings, upper structure, elevated stage, booth, speaker towers and short stairs.
The roughly 28 by 34 metre dance floor provides separated positions for 40
characters. Hull geometry participates in the existing water interaction mask;
the ocean renderer itself remains unchanged. The ship stays fixed at origin.

A camera near 50 degrees FOV looks down diagonally from behind the crowd toward
the player and DJ. Slow, small camera sway must keep the player and stage visible.
Keep enough ocean around the ship to make the setting legible.

Make an original small pixel character atlas procedurally as a source PNG asset,
with several generic outfits and palette variants. Use nearest filtering and
atlas padding to avoid blurred pixels and bleeding. Characters remain upright,
rotate about world Y toward the camera, and use alpha cutout/depth testing to
avoid transparent sorting dependencies. Share mesh/material/atlas resources;
do not allocate one texture or independent system per character.

Use a single crowd ECS system with per-character base position, phase, amplitude
and speed variation. Evaluate bounce, sway and lean from song beat phase rather
than accumulating animation time. Keep feet at or above deck height. Include
the DJ in the same lightweight animation pattern with a separate stage position.

Mark the local player with a bright floor ring, overhead YOU label and distinct
palette. When the player leads, show a crown/star marker and strengthen the ring.

Fake concert illumination with emissive/unlit colored geometry, pulsing panels,
speaker scale and oscillating light strips. Existing deferred light APIs need
not be added to the active pipeline. Stage effects consume beat events and
energy; they do not participate in score calculation.

## Rhythm domain and timeline

Keep renderer-independent `RhythmTrack`, `RhythmSession` and `RhythmScore`
logic in `gbDemo`, separate from input and scene entities. These names describe
new types to implement, not APIs already provided by the engine.

Track JSON contains track identifier, BPM (default 128), beat offset in seconds,
duration, optional audio filename, and an optional explicit beat array. Constant
BPM tracks generate beats starting one interval after the offset. Explicit beats
must be finite, strictly increasing and inside the track duration. Validate
positive BPM, duration and ordered hit windows. Invalid configuration reports a
clear diagnostic and uses the bundled default metronome track.

Separate song start, current transport position and beat timeline from normal
game time. The session reads transport seconds and advances every crossed beat,
including after a slow frame. Expose beat index/time events or an equivalent
observable state; the rhythm domain must not own pointers to scene entities.

Timestamp taps when the input listener receives them, using the same transport
as the session. Consume queued actions in timestamp order before expiring beat
windows. Ignore keyboard auto-repeat until release. Mouse/touch input follows
the same tap-action boundary; clicks on debug controls do not count as taps.

For a tap, find the nearest expected beat, then check whether it is already
resolved. Each expected beat can resolve exactly once. A duplicate tap produces
no score, no combo gain and no extra energy loss. A tap outside all hit windows
is a MISS and resets combo; it does not consume a future beat. An unhit beat
becomes a MISS once its Good window closes, exactly once.

| Absolute error | Result | Score | Energy |
| --- | --- | --- | --- |
| At most 40 ms | PERFECT | 100 | +0.025 |
| At most 80 ms | GREAT | 70 | +0.015 |
| At most 130 ms | GOOD | 30 | +0.005 |
| Outside window / expired beat | MISS | 0 | -0.020 |

Successful hits increment combo; misses reset it. Track max combo, total score
and energy clamped to [0,1], initially 0.35. Accuracy is earned points divided by
100 times (resolved expected beats plus out-of-window stray taps), displayed as
a percentage; before any judgement display 0%. Show the latest judgement briefly.

Loop the bundled metronome track. Keep a monotonically increasing session
position and assign distinct beat IDs across loops; score/combo continue across
loops. Explicit Restart resets transport, queued input, beat resolution, score,
energy, bots and transient visuals together.

## Audio transport

Introduce a small transport boundary that exposes start, stop, restart, volume
and playback seconds. Scene/gameplay logic uses this boundary, never native
audio calls. Do not use frame count or accumulated visual delta as the clock.

For Windows, add a narrowly scoped reusable PCM output transport in `gbSound`,
using Windows audio output and its played-sample position for generated PCM
metronome playback. Keep generated audio buffers alive until playback has stopped
and the device has released them. Generate clicks from the resolved beat timeline
(including explicit beats when present); changing BPM or offset restarts the
session. Handle playback position rollover and loop boundaries without time
moving backward.

On Apple, use existing `audio_engine` playback/position queries for a bundled
generated metronome WAV. Observe initialization before starting the session and
unwrap looping playback position. Both implementations share the transport
contract. Neither requires changing renderer or input internals.

On device/file failure, enter an explicitly labelled SILENT CLOCK mode using
`std::chrono::steady_clock`. Preserve gameplay without pretending there is audio
synchronization. A future supported music file is selected through JSON; decoding
OGG on Windows is outside this first slice and must be reported as a limitation.
The config retains an optional audio filename for the later track pipeline.

## Energy, events and simulated players

Each real beat triggers speaker/booth pulses and updates 10 seeded bots with
skills in [0.65,0.98]. Keep bot simulation independent of entities and networking.
Bots use the same score values; the local player and bots sort by score, with
deterministic tie order and local player winning ties. Show top five and the
local player's current rank even when outside the top five. Require a positive
local score before displaying STAR OF THE DANCE FLOOR.

Energy scales stage brightness, pulse strength and crowd dance amplitude/speed
smoothly, with extra light strips/brief flashes near full energy. Preserve
readability of characters and rhythm UI. A synthetic Force Beat affects visuals
only: it must not advance the timeline or award scores to player or bots.

## HUD, controls and development panel

The HUD shows score, combo, max combo, accuracy, latest judgement, energy bar
and percentage, BPM, beat pulse, top five, local rank and leader status.

Controls: Space or left mouse button taps; R restarts; F1 toggles the panel.
Suppress gameplay keyboard actions while a panel text field captures input.

Use existing UI controls for BPM, volume, beat offset, ordered Perfect/Great/Good
windows, energy, crowd count (30-50) and stage intensity. Apply BPM/offset changes
by restarting transport and timeline together. Explicit-beat tracks keep their
authored timeline and do not expose an ineffective BPM edit. Other settings apply
without rebuilding the scene. Retain a pool of 50 characters and change visibility
and active count instead of destroying render resources during an update.

Buttons: Restart Track, Force Beat, Set Energy 100%, Become Rank #1. The last
button is a labelled development override setting local score above the current
bot maximum; it is cleared by restart. Debug edits are not persisted by default.

## Ownership, lifetime and file boundaries

Planned new groups under existing source directories:

- `gbDemo/sources/rhythm_ship_scene.*`, `rhythm_ship_transition.*`: scene lifecycle.
- `rhythm_track.*`, `rhythm_session.*`, `fake_leaderboard.*`: plain gameplay logic.
- `ces_dance_character_component.*`, `ces_dance_crowd_system.*`: crowd ECS state/update.
- `rhythm_ship_environment.*`, `rhythm_ship_ui.*`, `rhythm_ship_input.*`: scene helpers.
- `gbSound/sources/rhythm_audio_transport.*` and platform implementations: audio clock/output.
- `gbBundle`: track JSON, generated original atlas/metronome, transition/material/shader assets.
- `gbScripts`: focused rhythm logic/audio/config checks and runtime smoke support.

Modify `main_game_controller.*` for selection and applicable Visual Studio,
filters and Xcode metadata for sources/assets. Exact filenames may be consolidated
where a small helper does not justify an additional translation unit.

Construct derived entities through `construct<T>()`, assign system order against
the actual transition registrations, and update rhythm before crowd/effects and
rendering. Remove input listeners and stop transport on scene teardown. Avoid
strong callback cycles. Allocate static/shared meshes once; retain any replaced
GPU resources according to established safe lifetime patterns. GPU commits stay
on the existing resource-accessor/game-loop path.

## Validation and acceptance

Build incrementally after scene/environment, crowd/effects, and rhythm/UI
integration using:

```powershell
./gbScripts/build-gbDemo.ps1 -Configuration debug-client -Platform x64
```

Add focused standalone checks for rhythm boundary cases: exact hit thresholds,
offsets, explicit beats, duplicate taps, missed beats, delayed frames, event
catch-up, looping, restart, energy clamps and leaderboard ranking/ties. Include
transport monotonicity and audio failure fallback checks. These are targeted
checks, not a claim that the repository has an engine-wide unit test suite.

Validate JSON/XML/assets, shader bindings and source/resource project membership.
Run the Windows Vulkan demo and inspect captures showing hull/deck, preserved
ocean/sky, stage, 40 sharp pixel characters, player marker, readable HUD, beat
effects and leader change. Exercise Space/mouse input, debug UI exclusion,
restart, expired misses, count 50, audible metronome and normal shutdown. Record
audio clock progression and inspect validation/runtime diagnostics. Sound
audibility still requires an audio-capable local session.

Run `git diff --check` and inspect new files with `git status --short`. Build all
directly affected available targets. Report Metal/OpenGL/Apple/server as unverified
unless actually built and exercised; source parity is not runtime verification.
Do not claim completion if required build or smoke checks fail.

## Limits and next step

This slice uses primitive ship geometry, procedural pixel art, transform dancing,
fake concert lights and simulated competitors. The unchanged sky may remain
daytime. Windows production track decoding and latency calibration are not yet
provided. Recommend a real audio-track pipeline with playback latency calibration
next; do not implement networking or CDN support as part of this task.
