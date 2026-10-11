# Game Box core profiling probes

Audit artifacts, Windows Vulkan only. These do not change the normal engine build.
`core-probe.py` temporarily instruments existing source and saves exact original
bytes, including pre-existing uncommitted changes. Removal verifies both hashes
and refuses to overwrite a source file changed since installation. Run sequentially
in a workspace where nobody edits these sources during a capture.
The current installer snapshots 11 files. Always remove it and rebuild before
using the ordinary demo; restoring source alone does not replace the executable.

Baseline:

```powershell
./gbScripts/build-gbDemo.ps1 -Configuration release-client -Platform x64
./gbScripts/check-island-performance.ps1 -Configuration release -Residents 200 -Samples 5 -Label baseline-release
```

CPU scopes, GPU timestamps, raw frame times and submitted geometry:

```powershell
python gbScripts/performance/core-probe.py install
./gbScripts/build-gbDemo.ps1 -Configuration release-client -Platform x64
$env:GB_PROFILE_CORE='1'
./gbScripts/check-island-performance.ps1 -Configuration release -Residents 200 -Samples 5 -Label probe-release
```

Optional controlled comparisons while the probe is installed:

```powershell
$env:GB_VULKAN_VALIDATION='0' # Runtime override; normal Release defaults off, Debug on.
./gbScripts/check-island-performance.ps1 -Configuration release -Residents 200 -Samples 5 -Label no-validation
$env:GB_PROBE_WIDTH='640'; $env:GB_PROBE_HEIGHT='360'
./gbScripts/check-island-performance.ps1 -Configuration release -Residents 200 -Samples 5 -Label half-resolution
# Set these only for separate, named controls, after restoring default resolution:
# $env:VK_LOADER_LAYERS_DISABLE='~implicit~'
# $env:GB_PROBE_HUD_FREEZE='1'
# Use -SteadySeconds 30 for a longer capture after the first report.
```

Restore the caller's previous environment values afterwards. Loader-forced layers
and external overlays are not disabled by `GB_VULKAN_VALIDATION`; inspect
`modules.json`. `VK_INSTANCE_LAYERS` is cleared by the benchmark harness, then restored.

Finish by restoring the original sources and rebuilding the ordinary executable:

```powershell
python gbScripts/performance/core-probe.py remove
./gbScripts/build-gbDemo.ps1 -Configuration release-client -Platform x64
python gbScripts/performance/summarize-core-profile.py gbDemo/output/island-check/perf-probe-release-200
git diff --check
```

Captures and snapshots live under ignored `gbDemo/output/island-check/`.
The harness defaults to Debug for compatibility and supports 0–200 residents.
It closes only the process it launched. It records process/private memory,
per-thread CPU consumption between first and final report, loaded module names,
and the existing 120-frame system aggregates. Process CPU seconds divided by
elapsed seconds is an equivalent number of busy logical cores, not main-thread time.

GPU query pools belong to swapchain images. Results are read after the renderer's
existing image fence, without `VK_QUERY_RESULT_WAIT_BIT`. There are no added waits,
submissions or barriers. Timestamps are recorded at bottom-of-pipe boundaries;
their intervals are elapsed queue execution intervals, not hardware occupancy or
pure shader ALU time. Total excludes presentation and host waits. Query pools are
destroyed after the existing teardown device-idle wait. Timestamp support,
valid-bit wrap and device timestamp period are handled explicitly.

Warmup is 60 rendered frames. Aggregates cover 120 frames each; percentile values
are computed only from raw `PROBE_FRAME`/`PROBE_GPU` samples, never from block means.
The existing system logger omits block means at or below 0.05 ms. Collected system
means for partly omitted categories are therefore lower bounds; GPU/pass scopes
do not apply this filtering.
Instrumentation and log output have overhead: compare the ordinary Release run
with a probe run using identical validation settings. GPU timing is unavailable
for dormant OpenGL/Metal paths. This is a focused performance harness, not an
engine-owned automated test suite or a profiler for every possible game scene.

`PROBE_SYSTEM_SPIKE` logs system phases longer than 5 ms using the existing feeder
timer. They include OS preemption and waits; they do not establish pure CPU compute
cost. `PROBE_UPLOAD` records individual staging flush times and the probe frame
number. The parser correlates those with the next frame-begin interval. HUD-freeze
updates its text once, then stops rebuilding it; draw calls and passes remain.

`GB_PROBE_NATIVE_STRESS=1` is a correctness control, never a performance capture.
On frames 60-65 it allocates 1,100 actual descriptor sets per frame and exercises
multiple submitted VBO/IBO snapshots in one frame. This checks pool/uniform chunk
growth and preservation of earlier recorded data under Vulkan validation.
Leave it unset for benchmarks. Environment flags are cached outside per-draw work.
Discard overlapping demo captures and captures from older probe versions that
looked up this flag on every draw. Run all comparisons sequentially with the same
validation, resolution, population and loader-layer settings.
