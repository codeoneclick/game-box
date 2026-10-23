# Rhythm Ship: local MP3 playback and input calibration

## Intent and scope

Continue the existing Rhythm Ship demo with real music from the three MP3 files
the user added to `gbResources`. Determine their timing from the audio; do not
ask the user to supply BPM. Preserve the existing ocean, sky, gameplay and debug
controls. This document specifies the next implementation increment, not a
claim that MP3 playback or calibration is already implemented.

Deliver all three selectable songs, matching beat maps, playback-clock scoring,
and persistent personal input calibration. No networking, downloads, arbitrary
runtime beat detection, new renderer, or second audio/game framework.

## Existing implementation and constraints

- `gbSound/sources/rhythm_audio_transport.*` generates PCM16 metronome audio.
  Windows owns a WinMM buffer/source; Apple owns an OpenAL buffer/source on the
  existing context. Preserve this ownership and synchronous backend lifecycle.
  Do not route through the legacy asynchronous `audio_engine::play2d` path.
- `gbDemo/sources/rhythm_track.*` loads validated beat data. `rhythm_session.*`
  handles scoring and loop IDs; effects consume its resolved beat position.
- Input currently timestamps taps using the audio transport. Wall time only
  disambiguates playback wraps or supports explicitly indicated silent mode.
- Existing Windows staging already copies MP3 files from `gbResources`.
  These source assets must retain their original filenames and bytes.
- Reusable decoding/output belongs in `gbSound`; catalog, maps, calibration
  interaction and preferences belong in `gbDemo`. No core/render refactoring.
- New compiled files need MSVC and all applicable Xcode target memberships.
  Preserve existing per-source exception/RTTI overrides and C++11 compatibility.

## Approaches

The user selected MP3 after an initial WAV/OGG suggestion. Converting source
assets to another shipped format would not satisfy that choice. A Windows-only
native decoder would leave the existing Apple output path without equivalent
support. The proposed implementation choice is a small portable MP3 decoder
(dr_mp3, pinned to an immutable revision with license/provenance), feeding the
existing PCM output. This adds decoding, not another playback engine.

Whole-track decoding is the proposed builder choice for these short files.
Their decoded stereo PCM16 payloads are approximately 14–24 MB. Streaming and
an unrestricted decoded-song cache are unnecessary for this increment.

## Audio analysis already performed

Read-only analysis decoded each original MP3 with Blender 5.2 `aud`, downmixed
to mono at 11025 Hz, and used NumPy spectral flux (1024-sample window, 128-sample
hop). Tempo candidates were checked against periodic autocorrelation,
subdivision alignment and independent sections. No BPM tags were present.

| Original filename in gbResources | Selected BPM | Decoded duration, seconds | Provisional grid phase, seconds |
|---|---:|---:|---:|
| audioera-stylish-trailer-percussion-622881.mp3 | 120 | 79.542857 | 0.017 |
| ikoliks-uplifting-action-sport-energetic-rock-619094.mp3 | 107 | 119.013878 | 0.194 |
| strawberry_candy-sport-intense-metal-fast-and-strong-622134.mp3 | 185 | 138.240000 | 0.040 |

All three sources are stereo, 44100 Hz. These are selected playable pulse rates,
not claims that musical half/double-time alternatives do not exist. Audioera's
initial 150-BPM spectral candidate failed periodic/subdivision checks against
120 BPM. Ikoliks's 214-BPM candidate is the double-time grid of 107 BPM.
Strawberry Candy remains near 185 BPM across independent sections.

Source SHA-256 fingerprints at analysis time:

- Audioera: `5BEF1859C4E99E61A14382B1957C12DA721A89D78D5136F954BBFD5E1C87F96B`
- Ikoliks: `D418B4708D7EE87C65300DA6C12B1A7510D09CFAA1C86BAAA19296DB390E054F`
- Strawberry Candy: `47CA495F806EA69CE2CE140A31729A5A052FB3721EBCC695B66AFF6EBC517881`

Phase estimates are provisional, relative to Blender's decoded waveform.
Audioera's introduction emphasizes the half-beat; later sections consistently
support the listed main pulse. Before shipping maps, repeat phase/transient
alignment on PCM from the selected runtime decoder and inspect/click-overlay
multiple song sections. Record the final values, source SHA-256 and analysis
method in a reproducible report. Different MP3 delay/padding handling must not
silently become a calibration correction. No hardware latency was measured.

## Playback and loading

Decode only the selected song into immutable interleaved PCM16 with explicit
channel count, sample rate and frame count. Support mono/stereo safely; reject
unsupported formats, impossible sizes and decode failures with a diagnostic.
Bound decoded allocation and song duration using validated metadata/frame
limits. Keep decoder implementation in one translation unit.

CPU decoding runs off the game loop. A bounded loader owns its worker and
request generation; repeated selection replaces pending work, stale results
cannot start playback, and teardown joins/cancels safely. Audio backend calls
remain on the game-loop path. Cache only the active decoded song for fast R
restart. During replacement release inactive PCM before retaining the next
song; one in-flight decode may coexist briefly with the active buffer.

Expose loading and failure states. While loading, freeze scoring and ignore
game taps. Publish the matching decoded duration, map, reset session and audio
start together. Never score a new map against old playback. Default to Audioera
on first launch; selection changes reset score, combo, bots and song time.

Use actual decoded frame count/sample rate for loop duration. Configure WinMM
and OpenAL with the PCM's real channels/rate. Position conversion must account
for frames versus interleaved samples, bytes and device-returned time units,
including counter rollover. Preserve played-audio position as the main clock.
Loop the complete decoded song without stretching; there is no extra seam beat.
Restart resets epoch and scoring. Decoder delay/padding behavior is documented;
do not promise seamless musical looping of files with authored endings.

Missing/corrupt MP3 falls back visibly to the existing metronome and its matching
map/duration. If audio output itself fails, use explicit silent mode with the
same selected timeline. Errors must not leave an old title/map paired with
fallback audio. Existing force-silent diagnostics remain supported.

## Catalog and beat maps

Store the three entries and maps under canonical `gbBundle` configuration,
referencing original MP3 basenames staged from `gbResources`. Display readable
titles, selected BPM and playback/loading state. Provide previous/next track
controls. Include the existing metronome as an explicit fallback/test entry.
Register the source music assets for applicable Apple bundles as well.

Generate offline explicit beat times in decoded-song coordinates, beginning at
the verified first pulse and ending strictly before decoded duration. The
existing generated-grid convention starts at `offset + period`, so do not
mistake a measured first-beat phase for its offset parameter. Validate actual
resolved intervals, including the loop seam. Omit tail beats if necessary to
preserve the existing non-overlapping judgement-window contract; document any
omitted beat. Repeated-loop IDs remain unique. Intro/outro silence need no
invented beats. Keep debug map offset correction available and separate from
personal calibration. These shipped maps need no runtime tempo estimation.

## Personal input calibration

Add a calibration action, saved signed offset in milliseconds, manual adjustment
and reset. Use the existing metronome transport, four count-in pulses and 24
measured taps. Capture each tap at input arrival; pair once with the expected
pulse in a bounded range. Use robust median signed error, reject outliers and
incomplete/high-jitter runs, and show an explanatory retry state. Calibration
taps never affect game score. Exit restores the selected song from its start.

Define positive offset as the user's taps arriving late relative to playback.
Scoring uses `input_audio_seconds - offset_seconds`; audio, visual pulses and
crowd animation retain unmodified song time. Timestamp and apply the offset
exactly once. Preserve negative early values, validate finite saved/manual data,
and bound adjustment to +/-250 ms. Save separately from track-authored timing
and graphics settings using the existing preference-path pattern. A rejected
run leaves the previous saved value intact. The result measures practical
tap-to-playback bias; it is not a physical device-latency measurement.

## Verification and builder decisions

Use focused portable checks for decoder format/frame counts, malformed/truncated
input, map bounds and seam spacing, source-rate/stereo position conversion,
counter/loop rollover, calibration sign/outliers/rejection, and stale loading
results/teardown. Keep existing rhythm/audio checks passing and extend asset
membership checks for music/catalog/configuration.

Build Windows debug-client and release-client with `gbScripts/build-gbDemo.ps1`.
Exercise each real MP3, switching while loading, restart, missing/corrupt file
fallback, forced silent mode, calibration persistence and normal shutdown in the
Vulkan demo. Verify source hashes unchanged and run `git diff --check`.
Inspect recorded output/click alignment where possible; playback-clock movement
alone does not establish audible alignment. Native Apple/OpenGL/Metal validation
remains UNKNOWN on this Windows host and must be reported explicitly.

Exact decoder revision, worker implementation, control placement and numerical
outlier thresholds are builder choices, subject to these ownership, timing and
acceptance contracts. They do not require additional user design questions.
