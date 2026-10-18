# Terrain contour SSE revision, 2026-10-17

## Audit before implementation

The existing renderer is retained. `terrain_contour_lod.*` builds nested greedy
point-to-neighbor-chord collapses offline; `terrain_lod.*` partitions existing
render groups into a quadtree and selects nodes; `terrain_lod_renderer.*` owns
cached GPU variants and reuses a pool of render entities. Scene orchestration
and epoch checks are in `island_demo_scene.cpp::prepare_editor_lod` and its update
callback. `terrain_editor_render_cache.*` rebuilds affected 16-cell logical
chunks and publishes 64-cell render groups. CPU logical terrain is unchanged.

The seed mesh (`terrain_mesh_builder.cpp`) uses radial contour rings and
independent flat top/wall faces. Edited terrain (`terrain_editor_mesh.cpp`) uses
the smooth logical contour. Render-only `terrain_mesh_optimizer.cpp` first
removes exact coplanar interiors, preserving attributes and boundaries.

1. Selection is per quadtree node, not per entire terrace. Typical selected
   starter nodes are 128m groups; children reach 32m. A terrace spans many nodes.
2. The old selector interpolates ordinal levels across a world-distance band.
   Maximum projected error only postpones that band; the individual variant's
   error does not determine selection. This is not genuine error-driven LOD.
3. Greedy importance already prefers small point-to-segment distances, but the
   runtime treats an entire node's worst error alike. Artificial boundaries,
   object footprints, orientation/topology and simultaneous-morph guards limit
   actual reduction. Requested percentage is a maximum, not a guarantee.
4. Mesh simplification runs on generation/edit/settings changes, not each frame.
   Camera movement selects cached variants; child uploads are lazy. Uploads may
   stall once when a variant is first encountered. Main geometry and immutable
   GPU buffers are not rebuilt every camera frame.
5. Indices really shrink: starter exact 43,314 triangles, distant N6/P50 26,703.
   Existing exact optimization already removed much of the original top geometry.
6. Draw calls are selected node count. Root variants keep the same count; child
   traversal can increase calls if its weighted cost is lower. Existing cost
   includes a draw overhead and parent hysteresis.
7. Current Release selection including renderer maintenance is about 0.3–0.5ms
   with 1,208 tested nodes. Recursion builds temporary child selections each
   frame even when the parent is ultimately selected.
8. Main terrain is a small share of about four million aggregate submissions.
   Grass, water, shadow/mask copies and post-processing are unaffected. A smaller
   terrain index count alone cannot establish a total-frame performance gain.
9. Every variant retains unreferenced source vertices and independent wall
   copies. That inflates CPU/GPU memory and uploads without helping rendering.
10. Existing morph deltas synchronize top/wall XZ columns. Boundary and artificial
    seam columns are locked; protected footprints stay fixed. Analytic signed
    triangle-area validation prevents intermediate flips. These safeguards must
    remain; existing CPU checks cover morphs, holes and touching contours.
11. Frustum selection uses node AABBs; actual renderer submits selected compact
    indices through `mesh_3d`. `render_statistics` counts indexed submissions,
    not original model counts, and aggregates all passes.
12. The original shared contour cap was 50%. The subsequent explicit user target
    of fivefold overview reduction authorizes extending this cap to 90%.
13. GPU and before/after camera orbit evidence will be appended after capture.
    CPU wall time is not GPU time. Popping/cracks cannot be ruled out solely by
    passing compile checks.

## Implementation approach and sequence

Improve the existing precomputed-variant strategy, without a second renderer or
new logical/chunk data. Keep the existing greedy collapse dependencies and safe
shared contour. Record the hierarchy's collapse targets/errors and compact each
variant to referenced vertices. Choose/morph using each next variant's projected
geometric error and a smooth pixel budget (near/normal/far). Retain boundary
locks and analytic morph checks. Add temporal hysteresis at the collapsed end
without changing its rendered position. Avoid allocating child selections for
obviously uneconomic subdivisions and reuse stationary selections.

Expose actual top/side/base/active counts, draws, transitions, CPU time and
actual reduction coloring through the existing terrain UI/material uniforms.
Persist pixel budgets/debug mode with existing graphics settings. Measure
baseline and revised Release on the same camera/settings, including Vulkan GPU
timestamps using the existing temporary core probe; restore instrumentation and
rebuild ordinary executables afterwards. Test close/far/orbit, bounded error,
crease preservation, morph continuity, index reduction, holes and boundaries.

## Implemented behavior

The offline greedy collapse ranks a vertex by its perpendicular distance to the
neighbor chord. Topology, protected footprints, node seams, sharp silhouette
points and analytic simultaneous-morph validity still constrain removal. The
old independent-set restriction was redundant with analytic validation and
artificially capped removal per transition; dependency-resolved successive
collapses now reach the requested reduction where the shape permits it.
Records contain source position, target, vertex/neighbor dependency IDs, height
and geometric error. Each variant stores its cumulative maximum contour error.

SSE is `world_error * abs(projection[1][1]) * viewport_height/2 / distance`.
Distance is to each local node's nearest AABB point, rather than island center.
The allowed pixel error smoothly blends .5 / 1.5 / 5px using
`t = distance / (128*cell_size + distance)`, with the normal budget at t=.5.
These three budgets are editable under F8 -> ws.base and persist through Save.

Runtime inverts a continuous conservative error curve over the prepared states.
Adjacent equal-error states receive small additive threshold spacing bounded by
2% of the final error, rather than skipped morphs or multiplicatively growing
distance rings. The target is 80% of the pixel budget, allowing smoothstep
interpolation within each fractional phase. Continuous play hysteresis has
opposite directional bounds and vanishes at completed endpoints. A code review
found overlapping/plateau-band popping in the first revision; explicit plateau
and close-error regressions now pass with the continuous-curve fix.

Top and wall vertices continue to resolve through the same XZ-column collapse
graph. Heights are fixed. Boundary/footprint locks are retained. Conservative
nonintersection and analytic triangle-area checks apply to the entire morph,
including transitive collapse targets. Source clipping roundoff is snapped to
a shared binary sub-millimetre grid (at most 0.000489m per X/Z axis) so that false
seams do not pin nearly duplicate points. Former palette UV gradients, unused by
the current world-space terrain material, are normalized only on render copies.
Logical terrain, collision, vegetation and placement data remain unchanged.

Exact coplanar cleanup is performed before contour reduction and again at the
final state, when no further morph correspondence is needed. Referenced vertices
and matching deltas are compacted after building all state correspondences.
Full unused source-vertex copies are no longer uploaded for every variant.
There is no per-frame mesh regeneration. Precomputed GPU variants remain cached;
newly visited child variants can upload once on first use. A stationary view
skips selection and material/visibility churn altogether. Moving views traverse
one output buffer instead of allocating child selections recursively.

The F8 snapshot now includes source/active vertices and triangles, separate
side/top counts, draw calls, changed groups, LOD histogram, CPU selection time,
upload/cache counts and projected error/budget. GPU timing is obtained from the
profiling capture, not mislabeled CPU wall time. Debug modes show actual contour
reduction, wireframe, error/budget color, collapse arrows (red source, green
target, yellow link), and active contour vertex markers. Overlays reuse the
selected terrain entities and are lazily cached only when requested; their extra
triangles are included in the debug submission count.

## Measured results and limitations

All values below precede the separate 2026-10-18 reduction of water-field maps
from 4096 to 1024. That later optimization must not be credited to terrain LOD.

| Terrain geometry | Original render input | Revised distant view |
|---|---:|---:|
| Submitted triangles | 43,314 | 8,690 |
| Side triangles | 16,846 | 2,539 |
| Top triangles | 26,468 | 6,151 |
| Vertices | 52,226 | 11,876 |
| Terrain draw calls | 67 | 67 |

This is 79.94% fewer triangles, or 4.98 times less geometry, with the whole island
visible. The base is the real input before this revision's exact cleanup, not an
artificially inflated current LOD0. Exact cleanup also helps the nearest state;
the ratio between two already optimized states is naturally smaller. Near and
far views have different frustum visibility, so their raw totals alone are not
used as proof of savings. The native smoke asserts distant output <=20.5% of
the input and separately checks local detail restoration during zoom.

| Same starter camera, Release | Previous LOD, cap 50% | Revised, cap 90% |
|---|---:|---:|
| Terrain triangles | 37,029 | 11,039 |
| Terrain draw calls | 67 | 67 |
| Terrain GPU elapsed time | ~0.160ms | ~0.126ms |
| Total GPU frame elapsed | ~17.55ms | ~17.52ms |
| Editor update, stationary | ~0.497ms | ~0.032ms |
| Frame wall time including GPU waits | ~17.66ms | ~17.54ms |
| FPS from frame wall time | ~56.6 | ~57.0 |

The full-frame difference is small and should not be sold as a meaningful FPS
gain: the scene remains GPU-bound. Distance-field/JFA water passes consume most
of the GPU budget; grass alone submits about 2.8 million triangles, and water,
SSAO, shadow maps and other submissions remain. Terrain GPU time improved by
about 21% and stationary editor work by about 94%, but terrain was only a small
fraction of the original total. Moving-camera selection/maintenance median is
about .36ms among nonzero samples in the validation capture; first-use child
GPU upload can still spike to ~18ms through the engine's synchronous upload path.
This revision does not claim to eliminate that remaining upload limitation.

GPU timestamps use the existing temporary core probe, extended to nest terrain
draw intervals inside ws.base. Sum the terrain mean per call times draws/frame;
the displayed per-call mean is not a per-frame number. Identical probes were used
before/after; timestamp probes can perturb execution. Sources are restored with
byte/hash checks and ordinary executables rebuilt after capture.

Evidence under ignored `gbDemo/output/island-check/terrain-lod-work`:

- Baseline profiling: `demo-52d6b2ad245b4b0ab1a9dcb3ee6ab66c`.
- Revised profiling: `demo-0248af8fb5e04804b9f2ec1bb3ec012e`.
- Zoom/5x assertion: `demo-3afe40c45cef42319eff5dc84e6d1b37`.
- Native density + orbit: `demo-5d0e11feabce4880abbedcc290baa97d`.
- F8 budgets/debug/Save: `demo-b7e319245d2b42b1baff29ef2bdb8e2f`.
- Collapse target overlay: `demo-b6d78facfc2a4394ba933412656c906f`.

CPU checks cover a sharp silhouette, aggressive smooth-contour reduction,
holes/touching contours, fixed heights, analytic/sample morph orientation,
compaction/overlays, SSE response to FOV/importance, monotonic index reduction,
pixel budgets, hysteresis and plateau continuity. Windows Vulkan native runs
exercise close/far/orbit and debug controls without reported validation errors.
No cracks were observed in inspected captures; this is not a claim that every
possible authored level has been exhaustively tested. Metal implementations
were updated but Apple/OpenGL builds and execution remain unverified.

Primary implementation files: `terrain_contour_lod.*`, `terrain_lod.*`,
`terrain_lod_renderer.*`, `terrain_lod_settings.h`, `island_demo_scene.cpp`,
`ocean_shader_uniforms.*`, `graphics_settings.cpp`, `terrain_editor_ui.cpp`,
`shader_island_terrain.frag`, `metal/mtl_library.metal`; matching checks are in
`gbScripts/check_terrain_*`, `check_graphics_settings.cpp` and
`check-terrain-lod-demo.ps1`. Existing modules, chunk ownership, source mesh
representation and render entity pool are retained.
