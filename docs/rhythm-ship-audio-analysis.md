# Local MP3 analysis

The source MP3 files in `gbResources` remain byte-for-byte unchanged. The catalog
contains three music entries and the original metronome. Audioera is selected
by default; no Internet access is used by the game.

| Source | BPM | Sample frames | Duration (s) | First pulse (s) |
|---|---:|---:|---:|---:|
| audioera-stylish-trailer-percussion-622881.mp3 | 120 | 3507840 | 79.542857143 | 0.017 |
| ikoliks-uplifting-action-sport-energetic-rock-619094.mp3 | 107 | 5248512 | 119.013877551 | 0.194 |
| strawberry_candy-sport-intense-metal-fast-and-strong-622134.mp3 | 185 | 6096384 | 138.240000000 | 0.040 |

All are stereo PCM16 at 44100 Hz after decoding with the exact shipped decoder.
Decoder provenance and SHA-256 are in `gbSound/libraries/dr_mp3/PROVENANCE.txt`;
source MP3 SHA-256 fingerprints are recorded in the approved design and checked
by `gbScripts/check-rhythm-ship-assets.py`.

## Evidence and reproduction

Run `./gbScripts/check-rhythm-audio.ps1` to decode all originals and emit ignored
`gbDemo/output/rhythm-check/<original>.mp3.pcm` files. Then run Python 3 with NumPy:

```
python gbScripts/analyze-rhythm-mp3.py
```

Use a genuine Python 3 interpreter: the machine's unqualified `python` currently
points to Python 2.7. Blender 5.2's bundled Python 3 includes NumPy and was used
for this analysis. `--write-maps` deliberately regenerates the three canonical
maps and catalog; omit it for read-only verification.

The analyzer emits `runtime-mp3-analysis.json` and click-overlay WAV snippets
from each introduction, middle and ending under the ignored output directory.
It downmixes the runtime-decoded PCM, averages groups of four samples, then uses
1024-sample Hann windows with 128-sample hops at 11025 Hz. Positive logarithmic
spectral flux in bass/mid bands locates pulse phase in independent 15-second
sections. The preceding independent Blender decoder/tempo-candidate analysis
agreed on duration and selected tempo family. Audioera's 150-BPM spectral peak
failed periodic/subdivision checks against 120 BPM; Ikoliks's 214 BPM is the
double-time grid of 107 BPM.

Runtime PCM phase peaks in later sections:

- Audioera: 16–20 ms. Its introduction emphasizes the offbeat at about 257 ms;
  the map keeps the stable 120-BPM main grid at 17 ms.
- Ikoliks: 189–197 ms; map phase 194 ms.
- Strawberry Candy: 36–44 ms; map phase 40 ms.

Analysis resolution is approximately 11.6 ms; these are measured practical
grid estimates, not sample-accurate musical ground truth. Half/double-time
choices and syncopation remain musical decisions. The generated overlays are
available for listening; audible alignment was not independently heard or
recorded in this environment. No physical output-device latency was measured.
Debug authored-map offset and separate personal tap calibration allow practical
correction without conflating song coordinates with device/user bias.

## Loop and failure behavior

Maps contain explicit times strictly before the actual decoded end. One final
beat is omitted for Audioera (79.517 s) and Strawberry Candy (138.202162162 s)
to retain more than 260 ms between adjacent scoring windows across the seam.
Ikoliks needs no omission. Whole-file playback repeats without stretching;
authored endings/silence can cause a musical gap. No seamless-edit claim is made.

The strict decoder accepts ordinary framed Layer III mono/stereo, ID3v2 at the
start and ID3v1 at the end. Free-format streams, changing channel/rate streams,
and other trailing metadata are rejected explicitly. Partial MP3 frames and
declared-length truncation fail. A CBR stream cut exactly at a whole-frame
boundary without a declared length cannot be identified as incomplete by framing
alone; the selected song's authored duration check rejects this mismatch.
Limits: 64 MiB encoded, 128 MiB decoded, 600 seconds. Only the active PCM is
cached; one in-flight bounded decode can coexist until its worker finishes.

Missing/corrupt/mismatched music switches to a visibly labelled metronome with
its matching map. Device failure uses an explicitly labelled silent clock with
the selected song map. Native Apple output has source/membership coverage only
on this Windows host.
