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
