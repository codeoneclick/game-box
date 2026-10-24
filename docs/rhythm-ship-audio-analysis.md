# Audio accents and Bluetooth timing

The nine user-supplied Kulakovka MP3 files are unchanged. Source hashes, decoded frame counts and descriptive tempo analysis remain in `gbBundle/Configurations/rhythm_ship/rhythm_ship_music.json`. Active Sport is the default; Adrenaline Sport is entry 1. Runtime PCM is stereo PCM16 at 44100 Hz, decoded by the pinned dr_mp3 implementation.

## What places an action

The previous fixed-tempo phase grid and repeated choreography templates did not represent the track's actual accents. All nine canonical music maps now use `"choreography":"accents"` and measured source candidates, each with time, action and strength. Informational BPM and the older phase audit do not place playable actions.

`gbScripts/rhythm_audio_accents.py` computes positive logarithmic spectral flux in bass, mid and high bands. Analysis uses 512-sample Hann windows at approximately 11025 Hz with 64-sample hops (about 5.8 ms), adaptive thresholds and local peaks. Each candidate is refined around the spectral peak with a 1 ms energy-rise hop. Silence rejection and 130 ms nonmaximum suppression remove quiet/duplicate candidates. This refinement matters: a centred spectral window can peak before the audible attack.

Energy contrast before/after an attack suggests a rise or reentry; timbre and stereo changes suggest directional accents. Neighbouring ordinary drum attacks become Tap groups. These are acoustic heuristics, not semantic ground truth about drops, and action balance is not forced. Maps retain per-candidate energy/timbre evidence and source provenance.

The C++ generator selects stronger candidates, filters disabled directions, limits bursts and leaves recovery time around swipes. It preserves every selected source timestamp and action; no random seed, BPM quantization or repeating action table changes them. This permits irregular Tap groups and rests. Explicit authored `events` remain authoritative. Metronome/legacy maps keep their compatibility path.

## Shared clock and Bluetooth

Previously personal calibration shifted judgement input only. Prompts and dancers still followed raw endpoint time, so calibration could not align the visible action with delayed Bluetooth sound.

Now song time is endpoint time minus the saved AV offset. Prompts, local movement, crowd and scoring use that same timeline. Input is corrected once at the playback boundary. A changed offset restarts playback/chart state and clears pending inputs. Calibration itself deliberately uses raw time.

A scratch Windows WASAPI loopback probe detected 13/13 known clicks from 0.5 to 6.5 seconds. WinMM position matched the Windows Bluetooth endpoint onset within +1.485 to +1.577 ms (mean 1.514 ms), with residual drift 0.0077 ms/s. The roughly 221 ms endpoint startup delay was already followed by the sample counter. This supports leaving the transport implementation unchanged. Evidence is ignored under `gbDemo/output/rhythm-check/clock-probe`.

**Endpoint loopback does not measure the physical speaker's acoustic Bluetooth delay.** No independent physical listening/recording verification was performed. Use F1 Calibration: four slow count-in clicks, then 24 taps to the heard clicks, spaced 1.5 seconds apart. The saved median includes device and tapping bias and shifts the entire visual/scoring timeline. Manual AV sync allows -600 to +600 ms. Default remains zero; no guessed Bluetooth offset is inserted.

## Reproduction and checks

Use Python 3 with NumPy (this machine's bare `python` is Python 2.7; Blender 5.2's bundled Python was used).

```powershell
./gbScripts/check-rhythm-audio.ps1
python3 gbScripts/check-rhythm-accents.py
python3 gbScripts/analyze-rhythm-mp3.py --write-maps
./gbScripts/check-rhythm-music.ps1
./gbScripts/check-rhythm-gestures.ps1
./gbScripts/check-rhythm-sync.ps1
python3 gbScripts/check-rhythm-ship-assets.py
```

The audio checker emits runtime-decoded PCM under ignored `gbDemo/output/rhythm-check`. `--pcm-dir` selects another directory. `--write-maps` deliberately updates all nine source maps from these PCM files. Without it, the analyzer only writes ignored evidence/overlays. Historical grid overlays diagnose phase families; they do not validate current playable accents.

Synthetic waveform checks verify arbitrary known attacks within 25 ms, silence without generated commands, and matching time shifts after shifting the PCM. Native C++ checks verify exact selected-event identity against source candidates across all nine maps, compatibility, difficulty/masks and publication failure recovery. Sync checks cover signed AV offsets through +/-600 ms, a shared prompt/pose/judge timeline, epoch resets, calibration and version-1 preferences. These compile checks are not an engine-owned unit test suite or proof of perceptual musical quality.

## Loop and failure behavior

Musical pulses are measured strong attacks with cyclic spacing over 260 ms. Playable events are independently validated, including loop seams. Playback repeats the full original file without stretching/trimming; authored silence/endings can leave a gap. No seamless-edit claim is made.

Decoder limits remain 64 MiB encoded, 128 MiB decoded, 600 seconds. Only active PCM is cached; one bounded in-flight decode may coexist. Missing/corrupt/mismatched music uses the labelled metronome and matching map; device failure uses a labelled silent clock. Failed chart publication disables scoring and ends loading while retaining decoded PCM so corrected action settings can recover. Apple/Metal and dormant OpenGL are not verified by this Windows Vulkan work.
