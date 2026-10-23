# Local MP3 analysis — current nine-song pack

Nine user-supplied Kulakovka MP3s replace the previous three songs. Original filenames and bytes are preserved in `gbResources`. The canonical source SHA-256 fingerprints, runtime-decoded frame counts, section phases and tempo autocorrelation evidence are in `gbBundle/Configurations/rhythm_ship/rhythm_ship_music.json`. The catalog contains these nine songs plus the original metronome. Active Sport is selected by default.

| Source | BPM | Sample frames | Duration (s) | First pulse (s) |
|---|---:|---:|---:|---:|
| kulakovka-active-sport-618585.mp3 | 160 | 4869504 | 110.419591837 | 0.011 |
| kulakovka-adrenaline-sport-620584.mp3 | 139 | 6009984 | 136.280816327 | 0.231 |
| kulakovka-gym-motivation-620569.mp3 | 98 | 6697728 | 151.875918367 | 0.021 |
| kulakovka-retro-music-620580.mp3 | 98 | 7028352 | 159.373061224 | 0.012 |
| kulakovka-summer-party-620566.mp3 | 122 | 7212672 | 163.552653061 | 0.258 |
| kulakovka-summertime-620526.mp3 | 120 | 4423680 | 100.310204082 | 0.010 |
| kulakovka-synthwave-pop-620528.mp3 | 96 | 6340608 | 143.777959184 | 0.010 |
| kulakovka-upbeat-energetic-rock-614902.mp3 | 145 | 4783104 | 108.460408163 | 0.010 |
| kulakovka-workout-620537.mp3 | 156 | 7787520 | 176.587755102 | 0.020 |

All decode to stereo PCM16 at 44100 Hz with the exact shipped dr_mp3 decoder. Decoder revision/provenance are in `gbSound/libraries/dr_mp3/PROVENANCE.txt`. Asset verification checks source hashes against the manifest and verifies target resource membership; no replacement encoding or renaming is performed.

## Evidence and reproduction

```powershell
./gbScripts/check-rhythm-audio.ps1
python3 gbScripts/analyze-rhythm-mp3.py
```

Use Python 3 with NumPy; this machine's unqualified `python` is Python 2.7. Blender 5.2's bundled Python includes NumPy and was used here. The audio checker emits runtime-decoded `<original>.mp3.pcm` files under ignored `gbDemo/output/rhythm-check`. The analyzer writes `runtime-mp3-analysis.json` and click-overlay WAV snippets for introduction/middle/ending sections. `--pcm-dir` selects another PCM directory; `--write-maps` deliberately regenerates the nine canonical maps and catalog using the manifest's selected tempo/phase.

Tempo selection compared onset autocorrelation at candidate half/main/double rates. Phase estimation downmixes runtime PCM, averages groups of four samples, then uses 1024-sample Hann windows with 128-sample hops at 11025 Hz. Positive logarithmic spectral flux in bass/mid bands locates phase peaks in independent 15-second sections. The manifest retains the section evidence rather than claiming all sections have an identical accent.

These are practical grid estimates, **not sample-accurate musical ground truth**. Analysis hops are about 11.6 ms. Intro transients, breakdowns, syncopation and half/double-time families can favor different peaks. Adrenaline Sport alternates around 11 ms and 231 ms (approximately half a 139 BPM beat); the map selects the recurring 231 ms phase. Summer Party uses the half-beat accent around 258 ms at 122 BPM. Other tracks mostly favor the near-zero phase, with section variation retained in the manifest. A fixed-tempo map does not model every fill or local tempo drift.

Overlay files are available for human listening, but audible alignment was not independently heard/recorded here, and no physical output latency was measured. Authored-map offset and separate personal tap calibration provide correction without shifting music or confusing map phase with user/device bias.

## Loop and failure behavior

Musical pulse maps contain explicit times strictly before decoded end. The generator omits final pulses when necessary to retain more than 260 ms between musical pulses across the seam. Playable phrase events independently enforce at least 60 ms cyclic spacing. Playback repeats the entire decoded file without stretching or trimming; authored endings/silence can cause a musical gap. No seamless-edit claim is made.

The strict decoder accepts ordinary framed Layer III mono/stereo, ID3v2 at the start and ID3v1 at the end. Free-format streams, changing channel/rate streams and other trailing metadata are rejected. Partial frames and declared-length truncation fail. A CBR stream cut exactly at a whole-frame boundary without a declared length cannot be identified as incomplete from framing alone; authored-duration reconciliation also checks the selected song and revalidates typed events against actual decoded duration.

Limits: 64 MiB encoded, 128 MiB decoded, 600 seconds. Only active PCM is cached; one bounded in-flight decode can coexist until its worker finishes. Missing/corrupt/mismatched music switches to a visibly labelled metronome and matching map. Device failure uses a labelled silent clock with the selected map. Native Apple output has source/project membership coverage only on this Windows host.
