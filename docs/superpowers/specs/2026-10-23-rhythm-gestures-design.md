# Rhythm Ship: directional choreography and replacement music

## Intent and agreed scope

Replace monotonous tapping with recognizable short dance phrases made from
exactly five actions: Tap, SwipeLeft, SwipeRight, SwipeUp, SwipeDown. Follow the
user's attached directional-rhythm specification. Do not add holds, long presses,
multifinger actions, note highways, networking or a second gameplay framework.

The user explicitly selected **mechanics only** for this increment: retain the
current rectangular deck/stage geometry, daytime presentation, ocean renderer
and skybox. Add gesture-driven lights and radial beat pulses to the existing
floor without rebuilding it as a circular stage. Preserve portrait680x960,
FPS/polygon/draw-call counters, F1 gameplay debug, F8 Render Inspector, music
playback, restart, personal calibration and the leaderboard.

Replace the three removed tracks with all nine supplied Kulakovka MP3s. Preserve
the supplied source bytes and filenames; do not restore deleted music. Determine
timing from the actual decoded PCM rather than requesting BPM from the user.

## Existing implementation and approach

`rhythm_track.*` currently resolves musical beat times; `rhythm_session.*` judges
timestamped taps and emits beat events; `rhythm_ship_playback.*` owns selection,
loading and calibration. `rhythm_ship_input.*` currently taps on pointer-down.
The shared crowd system animates50 pooled dancers and the DJ; environment helpers
drive speaker/panel/strip effects. UI uses native entity controls and labels.

Extend this implementation with typed events, a pure phrase generator, a pure
gesture recognizer and a dedicated prompt view. This is the sole sensible
approach within the user's instruction to reuse timing and avoid a parallel
framework. Independent random commands would violate the choreography goal;
runtime music-to-action classification is unnecessary for this prototype.
Implementation ownership stays in gbDemo, using existing gbSound playback.
No renderer-core or platform presentation changes are required.

## Music replacement and timing evidence

All nine replacements successfully decoded using the shipped dr_mp3 wrapper:
stereo44100Hz. Read-only measurements and preliminary tempo candidates:

| Source basename | Duration, seconds | Preliminary playable BPM family |
|---|---:|---:|
| kulakovka-active-sport-618585.mp3 | 110.419592 | 160 / 80 |
| kulakovka-adrenaline-sport-620584.mp3 | 136.280816 | 139 |
| kulakovka-gym-motivation-620569.mp3 | 151.875918 | 98 / 196 |
| kulakovka-retro-music-620580.mp3 | 159.373061 | 98 / 196 |
| kulakovka-summer-party-620566.mp3 | 163.552653 | 122 |
| kulakovka-summertime-620526.mp3 | 100.310204 | 120 |
| kulakovka-synthwave-pop-620528.mp3 | 143.777959 | 96 / 192 |
| kulakovka-upbeat-energetic-rock-614902.mp3 | 108.460408 | 145 |
| kulakovka-workout-620537.mp3 | 176.587755 | 156 / 78 |

These FFT candidates are not final tempo/phase assertions. Check periodicity,
subdivisions, phase and drift across independent sections on runtime-decoded
PCM. Select a sensible pulse family, record the choice/confidence and source
hashes, and make phases manually adjustable through the existing debug offset.
No physical device-latency or sample-perfect musical alignment claim is made.
Update the offline analyzer to enumerate the new catalog, remove obsolete maps,
replace Apple resource references, and prevent build staging from retaining the
three retired tracks. Keep the original metronome as a fallback/test entry.
Default first entry is Active Sport, default difficulty Normal.

## Musical beats, events and phrases

Keep the musical pulse timeline independent of playable action density. Audio
duration, beat position, stage pulse phase and crowd idle rhythm continue to use
the musical timeline; Easy must not slow the song or stage beat. Introduce a
renderer-independent action enum and event containing local time, action,
musical beat index, phrase ID and position. Loop event IDs are globally unique.

JSON retains `beats` as musical times and supports optional `events` objects
with `time` and `action`; accept exactly the five PascalCase action names in the
attachment. Validate types, finite times, strict ordering, song bounds and a
bounded event count. Numeric beat-only maps remain supported as all-Tap maps
when explicitly used in legacy mode. A generated-choreography mode composes
typed events from musical beats and selected difficulty, never randomizes each
event independently, and remains identical for a given track/settings seed.
Authored explicit events are authoritative; runtime generation is an explicit
operation and does not silently overwrite them.

Initial phrase library includes Basic (Tap,Tap,Right,Tap), SideStep
(Left,Right,Left,Right), Jump (Tap,Tap,Up,Up), Drop (Tap,Tap,Tap,Down), Alternate
(Left,Tap,Right,Tap), PowerEnding (Left,Right,Up,Down), and the eight-action fast
phrase from the attachment. Use controlled template variation and alternating
side accents. Expose current phrase position and at most three upcoming moves.

Easy uses only Tap/Left/Right, roughly one action every one or two musical beats.
Normal uses all five, mainly quarters and occasional eighth-note phrases. Hard
uses more eighths, longer sequences and more transitions. Keep judgement windows
the same across presets; complexity/density determine difficulty. Action-enable
toggles filter templates deterministically; an empty enabled set is rejected
with a visible message and leaves the current valid map unchanged.

Choose phrase boundaries and enabled subsets at regeneration/restart. Changing
difficulty regenerates and restarts the active song without redecoding cached
PCM, clears stale gestures, score/bots/prompt state and preserves volume/offset.
This increment need not invent Intro/Drop labels or audio section detection:
the current track model has no authored sections. Generator section support can
remain a future extension.

## Judgement and latency

An input carries action and audio-timeline timestamp captured at input arrival.
Subtract personal calibration exactly once. Retain delayed miss expiration for
positive calibration, and keep beat/prompt/crowd/stage clocks unshifted.

Find the closest event by time, independent of input direction; exact ties choose
the earlier event. Compare absolute error against configurable inclusive
40/80/130ms Perfect/Great/Good defaults. Within Good, a matching action scores
100/70/30, while a different action resolves that event as Wrong with zero and
combo reset. Out-of-window input is a stray Miss and consumes no future event.
If the closest event was resolved, ignore duplicate input rather than searching
past it for a future matching direction. Automatic misses resolve every expired
pending event even without input. Each event resolves exactly once.

Replace the old >260ms cyclic-gap restriction for playable events: eighths may
have overlapping outer windows. Nearest-time ownership and stable tie-breaking
must disambiguate them; action matching must never cherry-pick a farther note.
Still reject simultaneous/duplicate or unreasonably dense event timestamps and
validate the actual loop seam. A builder-selected minimum spacing and event cap
must support the new songs' eighths. Do not invent a closing seam event.

Track Perfect/Great/Good/Wrong/Miss counts, retain earned-points accuracy and
combo behavior, and separately expose directional correctness and signed timing
error in debug. Wrong or Miss applies the current miss energy penalty.

## Gesture recognition

Space emits Tap; arrow keys emit the four directions, with platform mappings and
repeat suppression behind the existing input abstraction. Mouse-left down begins
a candidate only outside captured UI. Record position/audio time, then consume
drag/release through a device-independent recognizer. Engine screen Y increases
downward, so negative deltaY is SwipeUp.

Proposed builder defaults:24 logical pixels minimum displacement and a1.25
dominant-axis ratio. Recognize exactly one swipe at the first unambiguous
threshold crossing, using that callback's audio timestamp; suppress release Tap
afterward. A small-motion release emits one Tap using release time. Ambiguous
diagonals can become unambiguous on later motion; a large still-ambiguous release
cancels. No gesture-duration scoring or hold mechanic is added.

This changes mouse Tap from down to up so a drag cannot first score a Tap.
Personal correction remains available; calibration explicitly continues to
capture a simple tap/key press consistently and never generates choreography
events. Ignore/calibrate directional controls as appropriate while calibration
is active. Cancel pending gestures on restart, song change, UI capture/open,
loading, calibration transition, lost pointer capture/release and teardown.
Only the primary pointer participates; do not consume native UI drags as dance.

## Prompts and normal HUD

The player's character remains the focal point. Add a small neon indicator at
the projected player anchor with a central Tap symbol or one clear required
arrow and a contracting timing ring. Build arrows/rings with native geometry or
existing sprites; do not depend on unsupported arrow font glyphs. Separate WHAT
(icon) from WHEN (ring), with a builder default lead of1.25seconds. Fade after
resolution; do not retarget the next event prematurely during a Good window.
Preview two future moves by default, configurable0–3; show only a short phrase
fragment, never the whole track or a scrolling lane. Handle offscreen anchors.

Show small judgement pop/fade and combo near the player, including distinct
WRONG feedback. Reduce the large opaque normal HUD panels: compact score/combo,
accuracy/energy, a small expandable rank view, and lightweight track controls.
Keep FPS/polygon statistics readable but unobtrusive. Move calibration detail,
development sliders and gesture controls into F1 pages so the portrait scene
remains visible. Retain accessible track selection and existing calibration.
F8 stays the independent existing inspector; game controls cannot receive its
clicks. Do not require a browser/new UI framework for runtime visuals.

## Player, crowd and stage responses

Each accepted input can trigger an immediate temporary visual move; successful
judgement strengthens it, Wrong gets distinct brief feedback. Tap bounces,
Left/Right temporarily step/lean to the side, Up jumps and Down compresses.
Always return to the component's base position/scale: no navigation or drift.
Keep procedural movement lightweight and compatible with existing billboards.

One shared choreography source drives deterministic per-bot skill, timing bias,
miss and wrong-action outcomes. Crowd dancers reuse those outcomes for their
visual action and the leaderboard instead of unrelated perfect animations.
Visual crowd settings can disable choreography without disabling scoring.
CrowdSync is the recent proportion of active dancers succeeding on shared
events (local player's outcome included when known); use a short bounded window
and update this metric once, independent of frame rate.

Preserve global musical beat pulses. Add a small reusable radial-pulse mesh pool
at the existing DJ/floor center, expanding/fading on strong beats. Use existing
solid/emissive materials and no expensive particle framework. Tap pulses
speakers, Left/Right bias/sweep strip lights, Up stretches upward accents, Down
contracts a floor pulse. CrowdSync/energy modulate strength. Disabling these
effects changes presentation only. The rectangular deck remains unchanged.

## Debug and verification

F1 exposes difficulty, five action toggles, swipe threshold, judgement windows,
prompt lead, preview count, beat indicator/crowd choreography/directional-stage
toggles, Generate Demo Beatmap and Test Tap/Left/Right/Up/Down. Test actions use
the same judge/input route with a clearly labelled debug timestamp policy;
Force Beat remains a visual-only action. Optional debug text shows expected,
actual, signed milliseconds and result. Debug setting changes that require map
regeneration validate first and restart atomically; unrelated visual sliders do
not reset the song.

Verify pure generation, typed parsing, gesture thresholds/diagonals/Y direction,
duplicate suppression, Wrong ownership, overlapping windows/ties, automatic
Miss, calibrated early/late input, loop seams, deterministic bots, bounded
CrowdSync, no dance drift and stale epoch cancellation. Update existing music,
audio, asset and native runtime checks for all nine supplied tracks, hashes and
retired-asset removal. Test mouse drags and keyboard directions in the demo,
all five visible moves, prompt timing, F1/F8 and50 dancers in680x960.

Build Windows Debug and Release; run Vulkan validation and inspect captures.
Register every new source/resource in MSVC and all applicable Xcode targets.
Retain C++11 compatibility and narrow exception/RTTI overrides. Native Apple,
OpenGL/Metal execution and independent audible alignment remain unverified on
this host and must be reported explicitly. No new engine-owned test framework.

Exact helper names, file decomposition, phrase selection seed, animation curves,
prompt dimensions, HUD placement and numerical visual/outlier settings are
builder choices within these contracts. Optional full-phrase celebration is
visual only and may be omitted; it must not introduce another score system.
