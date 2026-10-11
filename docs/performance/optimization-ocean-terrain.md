# Ocean and terrain optimization review

Constraint: image quality must not degrade. Preserve full resolution, all JFA/SSAO/blur/SSR samples, wave formulas, jitter/triangulation/tessellation, refraction rejection, shoreline/contact foam, and moved buoyant-prop updates. This report separates exact changes from proposals that need a larger lifetime/attachment design. Full build/runtime/image verification is owned by the root task.

## R08-R15 disposition

| ID | Status and safe boundary |
|---|---|
| R08 | All mask/seed/12 JFA/distance passes retained every frame. Dirty/static field reuse is unsafe without tracking moving buoyant masks, visibility, mesh swaps, terrain edits, and settings. `ces_ocean_system.cpp:43` copies interaction transforms every frame; `island_demo_scene.cpp:294` creates all field quads. Static land and moving masks currently share one target. |
| R09 | Window dimensions retained (`game_transition.cpp:87,114`). Resolution policy is not changed: half-resolution would violate the requested quality constraint unless proven equivalent, which it has not been. |
| R10 | Separate JFA targets retained. XML consumer search shows each JFA consumed only by the next iteration and jfa11 by distance. This supports a future two-target lifetime design, but current descriptor owns raw images/views/memory and resource GUIDs expose snapshots. A generic opt-in alias API, retained native owner, format/dimension checks, per-pass WAR/WAW dependencies, and historical/debug consumer semantics are needed. Do not hard-code island GUIDs into engine allocation. |
| R11 | Depth/load/store retained. False depth-compare flag does not mean unused depth: ws.base clears/writes depth, ws.ocean/ws.ocean.foam load it. Current depth is pooled by dimensions. Field/SSAO materials disable depth/stencil but removing their attachments needs an explicit pass policy and matching backend pipeline compatibility. Color store remains necessary wherever next pass samples. |
| R12 | Implemented exact local optimization: evaluate direction cos/sin once before four-ring loop (`shader_ss_ssao.frag:26`, `mtl_ssao.metal:36`), preserving identical angle and UV expression, 32 samples, radius, and accumulation order in GLSL/Metal. Compiler may already hoist it; no measured benefit claimed. |
| R13 | 25-tap bilateral blur retained, including weights, full resolution, and accumulation order. Separable filtering is not equivalent to current 2D joint bilateral filter. Precision changes are excluded. |
| R14 | SSR 32-step march plus four-step refinement retained. Quality/distance tiers excluded. Refraction check continues evaluating displaced ocean height for sampled scene geometry; replacing it with the mean plane can incorrectly pull dry geometry underwater. |
| R15 | Implemented exact local optimization: base ocean evaluates procedural foam only for debug IDs 1,7,8,18,19,20; field sample only for IDs15,16,17 (`shader_ocean.frag:139`, `mtl_ocean.h:146`). These values only influence those debug outputs; actual visible foam is rendered by separate foam pass. Matching Metal path updated. Separate foam pass, offset and coverage remain untouched. |

## Dependency evidence

`gbBundle/Shaders/shader_ocean.frag:117` compares sampled opaque positive linear depth with ocean fragment depth to reject foreground intersections. `:124` checks displaced ocean height before accepting refracted scene coordinates. `:139` computes foam layers, but subsequent uses occur exclusively in listed debug branches. `shader_ocean_foam.frag:100` rejects foreground by the same sampled depth before deriving independent foam coverage with `foam_shape.w` offset. `mtl_ocean.h` contains corresponding Metal functions. `material.ocean.xml` and `material.ocean_foam.xml` plus WS technique XML establish depth/load/blend state; those states remain unchanged.

`vk_render_pass_descriptor.cpp:155` always creates/attaches depth and `:302` pools by dimensions. False `is_depth_compare_mode_enabled` means clear rather than omit. Removing depth based on this flag would break ws.base and change subsequent ocean/foam behavior. Pooled depth clear/store in fullscreen passes is a valid optimization target only once pass semantics are explicit. No public pass interface or render_pipeline change is required by the bounded shader/CPU work.

## Terrain and ocean geometry/update findings

Terrain is generated and meshed once during scene construction (`island_demo_scene.cpp:260-263`). Default 16 layers and 320 contour segments produce 60,480 independent vertices/indices; independent triangles deliberately preserve separate wall/top normals. Consolidating vertices, changing contour smoothing, merging facets, or reducing segments could alter appearance and is excluded. The builder now reserves exact bounded output capacity (`terrain_mesh_builder.cpp:136`) while leaving emitted data and validation rules unchanged.

Ocean builds a 384x384 grid as nine 128-cell patches plus a 384-segment horizon (`island_demo_scene.cpp:302-304`). Each full patch has 16,641 vertices and 98,304 indices. Existing jitter uses global grid coordinates, making seams identical; randomized diagonal hash changes facet appearance and must remain. Surface displacement is shader-driven; no per-frame terrain/ocean vertex regeneration was found. Builder now reserves known vertex/index/patch capacities (`ocean_mesh_builder.cpp:10,15,44`) to eliminate growth copies during load without changing tessellation, seams, triangle ordering, or shader work.

`ces_ocean_system.cpp:49` now compares source/mask mesh identity first, running retention search and geometry assignment only on replacement. Retained old meshes survive until teardown, null replacement behavior is preserved, and transform/visibility updates still happen every frame. This reduces redundant CPU ownership/list work; it is not a GPU wave optimization. CPU buoyancy sampling remains necessary for gameplay transforms.

Terrain/water upload helper computes explicit bounds (`island_demo_scene.cpp:201-229`), including water displacement margin. Root owns scene edits. Box culling transforms only min/max in legacy core code, so rotated objects use sphere bounds in scene. Do not claim ocean/terrain culling optimality without camera-edge captures and actual visible patch counts. Ocean/terrain use distinct materials per shape and all 23 ocean uniform vec4s plus matrices are rewritten on both shader stages every frame; core binding caching belongs to the root's descriptor/material work. Avoid altering immutable settings or frame timing merely to reduce these writes.

## Verification required

Root should run `gbScripts/check-island-terrain.ps1`, `gbScripts/check-ocean.ps1`, the Windows demo build, fixed-time identical-camera image comparisons for normal output and relevant debug modes, underwater/refraction/shoreline camera states, and moving buoyant props. Capture GPU timestamps before/after and report descriptor/geometry allocations separately from image correctness. Metal/OpenGL source parity does not establish a verified Apple/GL target. No GPU speedup or image equivalence is asserted by this static report.

Scoped `git diff --check` after edits exited 0 (line-ending normalization warnings only). No build/runtime/profiler commands were run by this agent; root task owns those checks. No pass API, attachment, resolution, format, wave, tessellation, sample-count, blur-weight or SSR edits were made. The visible foam shader was not edited by this optimization.
