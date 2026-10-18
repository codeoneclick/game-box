# Terrain LOD performance regression investigation (2026-10-17)

The reported 120-to-30 FPS drop was investigated with the current saved graphics
settings, fixed camera, 1280x720 viewport, fixed waves/resident pose and Release.
Performance runs disabled Vulkan validation; separate interaction runs enabled it.
All native helpers operated on their own process and restored GraphicsSettings.

## Confirmed causes and fixes

1. The new LOD/wireframe controls performed UI writes every frame even with
   unchanged settings. Button text, disabled state and selected colors route
   through label font setters, which recreate font meshes/GPU resources; slider
   fill size also rewrote geometry. Profiling measured 18.4-19.1ms per frame in
   TerrainEditorUI::refresh. Cache persistent toggle/disabled states and skip
   unchanged fill sizes/positions/visibility. UI work is now approximately 0.03ms.
2. Ordered CLOD morph intervals had doubled their distance for each level.
   Increasing N could move the coarsest level beyond the useful camera range.
   The first correction shared [D, 2D], D=max(32*cell_size, max_error*projection_scale).
   N changes the number of intermediate steps rather than exponentially moving
   the far cutoff. The follow-up below corrects its too-short spatial range.
   The nearest shape stays exact; transitions remain continuous,
   and the entire band is conservative for the maximum projected error.
3. The aggregate HUD counter sums triangle submissions across all render passes,
   including grass, water, masks and shadows. Terrain alone is a small part of
   this scene. A terrain-only Tris label is now visible under Wireframe and
   refreshes at most four times per second, only rewriting changed text.

## Measurements

| Current starter camera | Frame time | Approximate FPS | UI time |
|---|---:|---:|---:|
| Before fix, LOD disabled | 32.4-32.7ms | 31 | not separately sampled |
| Before fix, LOD enabled | 31.7-32.1ms | 31 | 18.4-19.1ms |
| After UI fix | 17.76-17.80ms | 56 | 0.03ms |
| After both fixes | 17.45-17.76ms | 56-57 | 0.03ms |

| Geometry, fixed camera | Triangles |
|---|---:|
| Aggregate frame, N=1/P=0 | 4,070,351 |
| Aggregate frame, N=6/P=50 after fix | 4,036,796 |
| Terrain-only label, N=1 | 43,276 |
| Terrain-only label, N=5/P=50 | 26,978 |

Terrain fell by 37.7%, but aggregate submissions fell by only about 0.8%.
Triangle count alone therefore does not predict frame time. A diagnostic run
with grass omitted reduced aggregate submissions to 1,268,622 but ran around
62 FPS, so remaining frame time is not proportional to the grass triangle count
either. Grass density, water, shadows and saved visual settings were not reduced
as a performance fix. These measurements confirm the regression fix, not a
reproduction or guarantee of the earlier 120 FPS configuration.

## Verification

- Release build through build-gbDemo.ps1.
- CPU CLOD regression: the old selector failed the coarsest-distance check;
  current selector passes for 2/5/8 total levels plus continuous-boundary tests.
- Graphics settings checks remain green.
- Vulkan interaction smoke: N1/N8/N5, percentage, dragging past track, F8 Save,
  sculpt, Undo/Redo and Wireframe On/Off/On. Validation clean, normal exits.
- Fixed-camera performance logs and captures are in the ignored
  gbDemo/output/island-check/terrain-lod-work directory (perf-*.log).
- Debug and final checks are recorded in the same workspace.
- OpenGL/Metal runtime was not exercised for these UI/selection changes.

Reproduce the steady-camera benchmark without validation overhead:

```powershell
./gbScripts/check-terrain-lod-demo.ps1 -Profile -TerrainLodMode 1
./gbScripts/check-terrain-lod-demo.ps1 -Profile -TerrainLodMode 0
```

The helper captures frame 360 after warmup and records PROFILE system timings,
editor input/work/UI time, HUD FPS, aggregate triangles and draw calls. Run the
cases sequentially without a simultaneous build or second game instance.
For correctness checks use -Exercise or -Wireframe instead; these enable Vulkan
validation. GraphicsSettings is restored after each helper run.

Final Debug verification rebuilt all modules using isolated gbDemo compiler/linker
PDB paths (the normal PDB has a known locking issue), then updated the canonical
Debug executable from that build. Hashes of the copied and tested executables
match. Debug native Profile completed with normal exit and no runtime errors.
Its unoptimized /Od configuration took about 95ms/frame on this machine; UI work
was about 0.08ms and editor/CLOD work about 2.1ms, with most time in the renderer
(~57ms), ocean (~10ms) and shadow/resident/animation systems. This is a separate
configuration, not the Release before/after benchmark above. Release and Debug
runs therefore must not be mixed when comparing a previous FPS measurement.

## Follow-up: apparently static contours during zoom

Native reproduction confirmed that the initial 64..128m band exhausted the
levels before the enlarged island's ordinary camera range. At the starter camera
63 of 67 visible blocks selected LOD 5; zoom mostly changed frustum visibility.
This was a selection-range defect, not missing mesh generation.

The current band is [D, 6D], D=max(128*cell_size,
max_error*projection_scale). For cell_size=2m and six levels, nominal boundaries
are 256 / 512 / 768 / 1024 / 1280 / 1536m, measured to each block's nearest AABB
point. Higher projected error can move an individual block's boundaries farther
away. Morphing remains continuous, N does not move the endpoints, and parent and
child nodes use the same base scale. Seam and footprint safety guards remain.

New CPU regression failed against the old selector: "island camera already
saturates at coarsest LOD". It now verifies progressive geometry reduction across
ordinary island zoom distances, exact nearby geometry, independent near/far
selection, and reaching the last level. Existing contour and morph checks pass.

Native `check-terrain-lod-demo.ps1 -Zoom` captures four wireframe stages and checks
their selection histograms with validation enabled. Observed N=6/P=50:

| Camera stage | Selection | Visible terrain triangles |
|---|---|---:|
| Starter | LOD 0..4 | 37,029 |
| Close | LOD 0 dominates; contour reduction at most 10% | 23,377 |
| Distant | all 67 selected blocks at LOD 5; up to 50% | 26,703 |
| Return toward island | mixed LOD 0..3 plus exact aliases | 33,065 |

Counts across different cameras also include frustum culling and must not be
interpreted as fixed-view savings. LOD 5 aliases on exact/no-error blocks do not
mean a nearby contour was simplified. The previous fixed-camera FPS table above
belongs to the earlier selector; no new FPS improvement is claimed here.
