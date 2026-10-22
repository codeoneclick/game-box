# Terrain sculpt contours and response time

## Cause

The initial island uses radial, smoothed contour geometry. Previously, editing
replaced individual 16-cell chunks with an unrelated grid contour, then covered
the disagreement using vertical boundary skirts. This moved otherwise untouched
shoreline segments and produced rectangular notches at chunk boundaries.

The visual rebuild worker also generated every changed CLOD hierarchy and resident
navigation before returning. A newer brush revision invalidated its entire result
and queued a full-island LOD refresh. Continuous painting could repeatedly delay
visible publication by several seconds.

## Implementation

`TerrainEditorRenderCache` records a clipped seed-triangle domain and continuous
contour thresholds. Sculpting applies local filtered edits relative to this seed,
preserving boundaries whose binary terrace occupancy has not changed. Protection
fades continuously through the brush fringe and composes across all thresholds.
Terrace tops retain their discrete elevations. A shared submerged height field
handles the original gap between underwater -1 and beach +1 without introducing
an unrelated shallow shelf or triangle-dependent cracks when editing level zero.

The visual worker now builds only the local mesh and a one-level exact render
preview. Terrain uploads publish before decoration uploads and navigation. Newer
input queues another local update rather than discarding completed progress or
rebuilding the entire island.

A separate CPU worker refines changed batches into CLOD and rebuilds placement
support/navigation after the stroke. Generation, settings revision and cancellation
epoch guard publication; newer per-batch sources survive cancellation or failures.
GPU uploads stay on the game-loop path. Closing the scene cancels refinement before
waiting for its future. Refinement can still take seconds, independently of visible
sculpt updates.

## Verification, Windows Vulkan, 2026-10-18

- `check-terrain-editor.ps1`: passed. Includes real seed meshes at 128 and 1024
  resolution, unchanged shoreline, winding/area, Undo/Redo, reload, empty sea and
  authored level-zero height/shared-vertex checks.
- `check-terrain-lod.ps1`: passed, including exact preview geometry/version and
  existing hierarchy, SSE, morph and hysteresis checks.
- `build-gbDemo.ps1 -Configuration release-client -Platform x64`: passed.
- `build-gbDemo.ps1 -Configuration debug-client -Platform x64`: passed using a
  separate linker PDB/output override to avoid the existing locked debug PDB.
  The resulting executable was copied to the standard debug runtime location;
  SHA256 matched.
- Native `-Sculpt -SculptUndo -OceanInspect coast -NoValidation -TechniqueSizes
  -WaterFields`: passed in Release and Debug. Fifteen visible Release publications:
  median **24.09 ms**, maximum **36.31 ms**. Ten Debug publications: median
  **227.85 ms**, maximum **241.41 ms**. These measure queued brush input to terrain
  publication, not completion of background CLOD/navigation. No full-island rebuild
  occurred during the local stroke. GraphicsSettings is restored byte-for-byte.
- Vulkan validation sculpt/Undo/Redo smoke passed during development (Release:
  median 69.64 ms, maximum 173.46 ms). Validation-layer overhead is excluded from
  the ordinary interaction measurements above.
- Final Vulkan validation `-Zoom` and `-Exercise -TechniqueSizes -WaterFields`
  smokes passed. Zoom selected the coarsest level and reduced 45,974 input
  triangles to 8,582 (5.36 times fewer), then restored finer selection on return.
  Exercise covered superseding LOD settings, F8 Save and sculpt Undo/Redo while
  background work was active.

OpenGL and Metal builds/runtime remain unverified on this Windows machine. The
changed meshing/worker code is backend-independent; this task changes no shaders.

## Level Load follow-up

The native Load flow also waited for full-island seed grass and decoration
preparation before returning any terrain. Loading the current `Main.level`
without subsequent input took 3.95–4.98 seconds in the baseline Release checks.

Load now publishes terrain through a CPU promise before seed grass generation and
decoration packing. The same worker retains exclusive ownership of the cache.
Immutable seed chunks/domains are reused only when every terrain configuration
field matches; saved edits, climate, trees and reconstruction policies are still
reapplied. A larger upload budget applies only until the loaded terrain is shown.
Load generations reject old results, including pending renderer publications.

The CPU check compares final streamed terrain/decorations with ordinary reload,
asserts terrain arrives before seed grass starts, and verifies seed reuse and
invalidation on a changed generation seed. It passed with the full editor checks.
Both Windows Release and Debug builds passed. Native Release Load via the file
dialog took **595 ms** without further input; disabling CLOD took **715 ms**.
Debug terrain publication took
**2.91–3.57 seconds** with validation disabled. The larger load-only upload budget
avoids stretching the 67 terrain batches across one batch per slow Debug frame;
the ordinary editing budget remains 2 ms.
Two consecutive loads while background processing was active passed with Vulkan
validation, XML attachment-size checks and water-field checks. Validation overhead
is excluded from the ordinary timing above. A final single-load validation smoke
with the final upload budget passed at 787 ms. Scene decoration generation/upload and
CLOD refinement intentionally finish after terrain becomes visible.

Debug verification limitation: the native check displayed the loaded terrain and
captured frame 420, but process teardown exceeded the 30-second close timeout;
the helper terminated its own process and restored settings. Clean Debug shutdown
after a full-level load is not verified. Release load, repeated-load validation,
and sculpt/Undo/Redo checks exited normally. The final sculpt regression measured
24.23 ms median and 34.06 ms maximum visible update latency.
