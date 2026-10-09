# Cartoon Ocean Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** Deliver reference-inspired cartoon water with automatic intersection foam, wave-driven floating props and eight above-water plus eight underwater island terraces in the F5 demo.

**Architecture:** Demo-owned CPU terrain/wave/buoyancy code drives procedural meshes and matching shader uniforms. Ordered opaque-scene and water WS passes feed an SS composition pass; water reads scene color and a separate position/depth color attachment, never the active hardware depth texture. Shared engine corrections are limited to independently verified backend requirements.

**Tech Stack:** Existing C++/MSBuild, Vulkan/shaderc, configured WS/SS passes, GLSL and matching Metal functions, PowerShell CPU compile/smoke checks.

**Spec:** `docs/superpowers/specs/2026-10-08-cartoon-ocean-design.md` (approved by user).

## Global Constraints

- `layers_count` remains the above-water count; `underwater_layers_count`, default eight.
- One to eight above-water layers and zero to eight underwater layers; `layer_height`, default 2 m.
- Mean water remains at `water_y = -0.3 m`; terrace tops default to 0..14 m and -2..-16 m.
- Default `island_size = 256 m`, `world_size = 512 m`, `resolution = 512`; underwater terrace width is 6 m.
- Seabed at -18 m is background geometry, not a seventeenth terrace; initial water patch is 768 m wide.
- Mesh indices are 16-bit; each independent mesh must stay within 65,535 vertices.
- A single simulation clock and parameter set drive CPU and GPU; four directional Gerstner waves initially.
- F5, LMB orbit and RMB anchored terrain/mean-sea panning remain functional.
- Preserve generator/data/builder ownership, deferred entity construction and explicit ECS ordering.
- No external asset purchase or dependency; canonical runtime assets remain in `gbBundle`.
- Add compiled sources to applicable `.vcxproj`, filters and existing Xcode targets; do not edit generated configuration or vendored dependencies.
- Verify Windows/Vulkan; explicitly report Metal/Xcode, dormant OpenGL and server targets not exercised.

## Review Focus

1. Underwater label `-1` must not be confused with empty ocean, including off-domain reads and edited cells (Task 1).
2. A stall, reset or flat-wave configuration must not desynchronize CPU/GPU waves or destabilize floats (Tasks 2 and 5).
3. Water below a foreground object must not refract that foreground back over itself or erase opaque pixels (Tasks 3 and 4).
4. Off-screen reflection rays and invalid background samples must fall back cleanly rather than smear screen edges (Task 4).
5. Camera pan/orbit, scene recreation and float removal must not leave stale listeners, captures or weak references (Task 6).

## File responsibilities

- Modify `gbDemo/sources/terrain_data.h`, `terrain_generator.cpp`, `terrain_mesh_builder.{h,cpp}`: signed terrace data, expanded domain and seabed geometry.
- Create `gbDemo/sources/ocean_wave_model.{h,cpp}`: engine-independent settings, clock-independent displacement and inverse surface sampling.
- Create `gbDemo/sources/ocean_mesh_builder.{h,cpp}`: tiled water grid from existing `terrain_mesh_data`.
- Create `gbDemo/sources/ocean_shader_uniforms.{h,cpp}`: named custom uniform wrappers and matching packed values.
- Create `gbDemo/sources/buoyancy_simulation.{h,cpp}`: engine-independent probe forces and bounded integration.
- Create `gbDemo/sources/ces_buoyancy_component.{h,cpp}`: probe settings and per-prop runtime state.
- Create `gbDemo/sources/ces_ocean_system.{h,cpp}`: shared clock, buoyancy updates and water/receiver uniform updates before rendering.
- Create `gbDemo/sources/ocean_demo_props.{h,cpp}`: procedural rocks/wooden hulls and their configured materials.
- Modify `gbDemo/sources/island_demo_scene.{h,cpp}`, `island_demo_transition.cpp`: pass materials, new geometry and system lifetime.
- Modify `gbBundle/Shaders/shader_island_terrain.{vert,frag}`: scene-data outputs and receiver effects.
- Create `gbBundle/Shaders/shader_ocean.{vert,frag}`, `shader_ocean_compose.{vert,frag}`; modify `gbBundle/Shaders/metal/mtl_library.metal`: shader counterparts.
- Create `gbBundle/Configurations/game_objects/ocean.xml`, `ocean_prop.xml`, `materials/material.ocean.xml`, `material.ocean_prop.xml`.
- Modify `gbBundle/Configurations/game_objects/materials/material.island_terrain.xml` and `gbBundle/Configurations/transitions/transition.island_demo.xml`.
- Create `gbBundle/Configurations/techniques/ws_techniques/ws.technique.ocean.xml`, `ss_techniques/ss.technique.ocean_compose.xml`, `material/material.ss.ocean_compose.xml`, `output_techniques/output.technique.island_demo.xml`, `material/material.ss.output.island_demo.xml`; preserve opaque technique in `ws.technique.island_demo.xml`.
- Modify `gbScripts/check_island_terrain.cpp`; create `gbScripts/check_ocean.cpp`, `check-ocean.ps1`, `check-ocean-runtime.ps1`: focused mathematical and runtime smoke checks.
- Modify `gbDemo/gbDemo.vcxproj`, `gbDemo/gbDemo.vcxproj.filters`, `gbDemo/gbDemo.xcodeproj/project.pbxproj` alongside each source addition.
- Modify `ISLAND_DEMO.md`, `ENGINE_MAP.md`, `ENGINE_FLOWS.md`, `CODE_PATTERNS.md`, `ARCHITECTURE_ISSUES.md` only where verified old assumptions change.

## Task 1: Sixteen terraces and the submerged seabed

**Interfaces:** Keep `terrain_generator::generate(const terrain_config&) -> terrain_data` and `terrain_mesh_builder::build(const terrain_data&) -> terrain_mesh_data`. Add `terrain_data::empty_layer = -9`, `terrain_config::underwater_layers_count`, `island_size`, `underwater_terrace_width`; add `terrain_mesh_builder::build_seabed(const terrain_config&) -> terrain_mesh_data`. Water-plane helper remains until Task 3 replaces its scene use.

- [ ] Extend `check_island_terrain.cpp` with `check_sixteen_terraces`: defaults are 8/8; each label -8..7 occurs; every top is at label times 2 m; exactly eight top heights lie on either side of -0.3 m. Assert off-domain reads equal `empty_layer`, not -1.
- [ ] Add `check_submerged_data_edits`: changing a submerged cell changes builder output or is explicitly rejected for unsupported topology; an empty hole inside an occupied ring fails. Keep reproducibility, normal/winding and malformed-data checks using the new sentinel.
- [ ] Run `./gbScripts/check-island-terrain.ps1`; expect compile/assert failure on the absent representation.
- [ ] Implement defaults and validation in `terrain_data.h` and `terrain_generator.cpp`. Generate dry contours using `island_size`; generate outward submerged rings with 6 m nominal gaps and constrained variation. Empty cells use -9 even when underwater count is zero. Reject insufficient domain, nonfinite sizes, above/below-water violations and unsupported counts.
- [ ] Update meshing in `terrain_mesh_builder.cpp` to iterate all enabled signed layers. Reuse occupancy/contour validation, maintain distinct top/wall normals, use submerged sand/rock colors, connect the deepest rim to -18 m. Use at most 320 segments for the 16-layer single mesh so the independent triangle representation stays under the index bound. Build a separate seabed outside the deepest contour without a coincident -16 m surface.
- [ ] Exercise 1/0, 8/0, 8/8 counts, resolutions 32/64/512/1024, seeds 0/1/42/1337/max, negative counts, missing rings and narrow contours. Low-resolution unsupported contours must produce a documented exception, never corrupted geometry. Defaults and normal resolutions must succeed.
- [ ] Run CPU check and standard Windows build; launch flat-water intermediate scene and confirm original island footprint plus visible submerged stairs. Commit only Task 1 paths: `feat: add eight submerged island terraces`.

## Task 2: Shared Gerstner model and water mesh

**Interfaces:** `ocean_wave_model.h` defines POD `ocean_vec3`, `ocean_wave` (direction X/Z, wavelength, amplitude, steepness, speed), `ocean_settings` (four waves, mean level and visual settings), `ocean_surface_sample` (position, normal), `evaluate_ocean(const ocean_settings&, float rest_x, float rest_z, double seconds) -> ocean_surface_sample`, `sample_ocean(const ocean_settings&, float world_x, float world_z, double seconds) -> ocean_surface_sample`. `ocean_mesh_builder::build(float size, int cells_per_side) -> std::vector<terrain_mesh_data>` returns 16-bit patches.

- [ ] Create `check_ocean.cpp` and `check-ocean.ps1` following the standalone MSVC pattern in `check-island-terrain.ps1`. Assert zero amplitude gives mean level, unit upward normal and no horizontal displacement; nonzero waves give finite unit normals and position matching evaluation after inverse sampling within 1e-3 m on default waves.
- [ ] Assert samples change with time, fixed time is deterministic, invalid/nonfinite wave settings are rejected, and steepness bounds prevent folded surfaces. Check every grid patch for valid indices and upward rest winding, including patch seams with equal positions.
- [ ] Run `./gbScripts/check-ocean.ps1`; expect compilation failure before new code exists.
- [ ] Implement analytic Gerstner displacement/tangents in `ocean_wave_model.cpp`, with normalized directions and bounded horizontal inverse iteration (8 iterations, residual threshold 1e-3 m). Use identical angular phase conventions in the eventual shaders.
- [ ] Implement `ocean_mesh_builder` with a 384-cell, 768 m grid split into patches of at most 128 cells per side; all use global rest X/Z coordinates so seams share displacement. Avoid the terrain helper's duplicated vertices for this grid.
- [ ] Add project membership; run the CPU check and Windows build. Commit Task 2 paths: `feat: add shared ocean wave model and grid`.

## Task 3: Scene-data, water and composition passes

**Interfaces:** Opaque `ws.base`, order 0, writes `.attachment_01` color, `.attachment_02` encoded normal/validity and `.attachment_03` world XYZ/positive linear view depth. Invalid background has validity/depth zero. New `ws.ocean`, order 1, writes color/coverage for composition. `ss.ocean_compose` writes `.attachment_01`; new island output material samples `ss.ocean_compose.attachment_01` using existing `shader_ss_output`. Introduce `ces_ocean_system(const ocean_settings&)`, `add_surface(const gb::ces_entity_shared_ptr&)`, `add_receiver(const gb::ces_entity_shared_ptr&)`: weak references only, shared clock and uniform updates initially; Task 5 adds buoyancy queries.

- [ ] Add a runtime check in `check-ocean-runtime.ps1` that launches the demo with Vulkan validation, captures multiple frames and fails on new shader/descriptor/layout errors. Save outputs under the existing ignored island-check directory. Preserve/report known command-pool shutdown diagnostics independently.
- [ ] Define uniform wrappers in `ocean_shader_uniforms`: `ocean_vertex_uniforms` and `ocean_fragment_uniforms`, registered on entities using `ces_shader_uniforms_component`. Use custom vertex buffer index 2 and fragment buffer index 0 for Metal. Follow `game_objects_custom_uniforms` declarations/implementations; each declared GLSL uniform corresponds to its named wrapper member and std140-compatible packed representation.
- [ ] Establish one packed settings layout: four `vec4` wave descriptors (direction X/Z, wavelength, amplitude), four `vec4` descriptors (steepness, speed, zero, zero), `vec4` time/mean level data; fragment data additionally carries camera position, view/projection matrices and vec4 palette/foam/absorption settings. Explicitly test offsets/sizes and match Metal definitions.
- [ ] Create/register `ces_ocean_system` at order 6 and implement its `on_feed` clock/uniform path; empty `on_feed_start`/`on_feed_end` preserve feeder phase order. Clamp accepted dt to [0,0.25] s, reject nonfinite dt, and feed the same resulting clock to every surface/receiver. Give runtime checks a fixed-time diagnostic setting so animated shader captures can be compared before buoyancy exists.
- [ ] Configure the two WS passes and composition resources, keeping dimensions consistent. Use `125` for sampled scene-position/depth data. Ocean samples only opaque color/data; never `ws.base.depth`. Set water depth writes off, retain opaque occlusion, and clear water coverage to zero.
- [ ] Verify actual depth load behavior: Vulkan already shares and loads same-sized depth; Metal currently clears depth, so add scene-data-based water fragment rejection for foreground occlusion rather than introducing a new global depth-sharing policy. Inspect dormant GL branch before changing any shared wrapper.
- [ ] Implement a flat-color ocean shader and composition first, with matching Metal entry points. Register scene materials separately; remove the terrain material from water. Reference `output.technique.island_demo.xml` from the island transition. Add a configured sphere intersecting the waterline using `scene_fabricator::create_shape_3d("ocean_prop.xml", "sphere")`, transformed into a rock for input-independent occlusion checks; Task 5 extends this prop setup.
- [ ] Run build and runtime check; inspect a rock crossing the waterline and foreground terrain. Assert foreground pixels are unchanged by water, invalid scene-data pixels never become phantom terrain, and no pass samples its own color output. Commit Task 3 paths: `feat: compose ocean with opaque scene data`.

## Task 4: Cartoon appearance, automatic foam and receiver effects

**Interfaces:** Vertex shader reproduces Task 2 Gerstner equations using Task 3 packed wave/time data. Fragment sampler order: scene color `sampler_01`, scene position/depth `sampler_02`, scene normal/validity `sampler_03`; keep GLSL converter's existing bindings. Receiver shaders use the same wave/time parameters. `ocean_settings` exposes deep/shallow/foam colors, foam width, absorption, refraction strength, sun direction and highlight strength.

- [ ] Extend runtime captures to fixed simulation times and lower orbit angles. Record diagnostic variants for foam-only, scene-data, refraction-off and reflection-off through debug settings rather than changing permanent palette values.
- [ ] Implement wave displacement and analytic macro normals; add shader-only fine ripples. Compare rendered marker points to CPU wave samples at fixed time, targeting <=0.02 m vertical difference. Use direct CPU/GPU projected marker captures, not only matching source text.
- [ ] Implement depth-thickness absorption, deep blue/turquoise shallow colors, Fresnel blending, directional sun highlights and sparse glitter. Clamp invalid/extreme offsets; only refract to scene-data samples behind the water surface, preventing foreground leakage.
- [ ] Add bounded screen-space reflection traversal (32 steps and 4 refinement steps); use sky color fallback for invalid/off-screen rays and fade at screen boundaries. Test orbit across screen edges without reflected edge smearing.
- [ ] Implement animated intersection foam from water-to-opaque depth/position gap. Use a broken white noise-shaped band with finite width, wave modulation and antialiasing. Test a translated/rotated arbitrary opaque rock: foam follows its new waterline without new per-object masks. Do not add hardcoded radial foam around each demo prop.
- [ ] Extend terrain and prop receiver shading with world-projected animated caustics below local water height, attenuated by depth and orientation, and waterline wetness darkening/highlights. Reserve per-prop wetness state for Task 5. Keep dry high terraces unchanged.
- [ ] Implement matching Metal shader functions; validate uniforms and sampler order. Run build and runtime check, inspect over-time captures against the reference's color/foam/glitter character. Confirm submerged geometry remains readable at shallow depth. Commit Task 4 paths: `feat: add stylized ocean shading and interaction foam`.

## Task 5: Stable buoyancy and demonstration props

**Interfaces:** `buoyancy_simulation.h` defines `buoyancy_probe` (local position and volume), `buoyancy_settings` (mass, inertia, damping, draft), `buoyancy_state` (pose, vertical/angular velocities and wetness), `step_buoyancy(const ocean_settings&, const buoyancy_settings&, const std::vector<buoyancy_probe>&, double time, float dt, buoyancy_state&)`. ECS component owns these values. `ces_ocean_system` owns the shared simulation clock and uses queried component entities, updating pose and ocean/receiver uniforms in `on_feed` before render order 7.

- [ ] Extend `check_ocean.cpp`: a four-probe object released above flat water settles within 0.02 m of its expected draft after 30 s; height/roll/pitch stay finite. A moving wave tilts probes consistently with sampled heights. Use 30/60/144 Hz and a 0.25 s frame stall, asserting bounded pose and convergence without divergent energy.
- [ ] Assert reset restores clock/pose consistency; wetness rises on immersion, decays after emergence and remains in [0,1]. Reject nonpositive/nonfinite mass or inertia, empty/nonfinite probes, and negative/nonfinite dt; zero dt is a no-op. Run check expecting failure before integration exists.
- [ ] Implement multi-probe restoring forces, torques and damping in `buoyancy_simulation.cpp`. Use <=1/120 s substeps and cap processed frame time at 0.25 s; any discarded wall time is discarded for both simulation clock and shader time. Keep horizontal placement/yaw independent, and never substitute sinusoidal pose animation for force integration.
- [ ] Add `ces_buoyancy_component` with required STTI and extend the existing `ces_ocean_system` with an explicit mask query; keep its single registration at order 6 (existing light system does not depend on ocean). Integrate float state using the previous/current shared clock, then set all wave and receiver uniforms. Supply camera matrices after recomputation, avoiding stale camera position.
- [ ] Implement `ocean_demo_props::populate(const gb::scene_graph_shared_ptr&, const ocean_settings&)` using canonical configured shape/material construction. Add rocks and at least three wooden floating props, including a simple hull, with probe volumes/draft fitted to their geometry. Position them in visible shallow water; attach ocean-data/receiver uniform wrappers before first render.
- [ ] Hook system/props into island scene lifecycle. Add project membership, run CPU/build/runtime checks. Verify floats appear on the rendered surface at fixed and running times, foam/reflections follow their pose, and removal does not retain strong ECS references. Commit Task 5 paths: `feat: add wave driven buoyancy demo props`.

## Task 6: Integration verification, visual tuning and documentation

**Interfaces:** Existing F5 launch/build task and `orbit_camera_input` remain the user entry points. CPU checks, runtime captures and documented settings become the review evidence for the complete feature.

- [ ] Extend runtime smoke to LMB orbit, RMB land/mean-water pan, release/capture loss and scene recreation. Confirm camera transform does not bob with the wave clock and previous listeners stop receiving events after recreation.
- [ ] Tune the initial view and prop positions for visible shoreline foam, submerged terraces, caustics and floating motion. Keep current orbit defaults unless visibility evidence requires a documented adjustment. Extend far clip only as needed for the larger water domain; preserve depth precision and avoid exposing the finite patch edge in the tested view range.
- [ ] Run `./gbScripts/check-island-terrain.ps1`, `./gbScripts/check-ocean.ps1`, standard Windows build and runtime capture check once on final code. Check grid seams, underwater stair visibility, moving-object foam, reflection fallback and foreground refraction rejection. Repeat only for new changes/failures.
- [ ] Update `ISLAND_DEMO.md` with settings, 8+8 labels/heights, ocean interactions, diagnostic switches, controls and evidenced limitations. Update relevant architecture documents to replace obsolete flat-water/eight-layer/single-pass assumptions.
- [ ] Inspect all new Visual Studio/Xcode memberships and Metal shader names. Run `git diff --check` and `git status --short`; report exactly which backends/targets were not built.
- [ ] Obtain whole-change review using the execution workflow's reviewer, resolve concrete findings, and reverify affected checks. Commit final tuning/documentation separately: `docs: document cartoon ocean verification and limits`.

## Completion evidence

The final report must distinguish mathematical CPU checks, Windows compilation,
Vulkan runtime/capture verification and unverified Apple/OpenGL targets. Include
representative captures with the water features visible and disclose screen-space
reflection/foam limitations and remaining shutdown validation diagnostics.
No success claim is justified by shader source presence alone.

## Execution outcome (2026-10-08)

Tasks 1-6 delivered in the approved F5 workspace. CPU checks, Windows build,
Vulkan captures, input frame comparisons and independent whole-change review
passed. Two review findings were fixed and rechecked: rejecting dry refraction
candidates and convergence of accepted steep waves. Inverse sampling uses up
 to 64 steps with early exit, replacing the planned fixed eight steps.

Implementation adjustments: procedural prop meshes are built by
`build_ocean_demo_props` and instantiated in the scene; the shared ocean system
owns their integration. Explicit PTNTC bounds, rotating radius culling and a
coarse horizon skirt use existing engine contracts. Initial target moved to
(-24,6,24) to expose the props in the first frame. No core renderer changes.

The checklist above is the original approved plan, not a claim that every
proposed test was performed. Interactive pause/reset is not exposed; fresh
state is created with the scene. Arbitrary runtime prop removal and transition
recreation were not exercised. Metal/OpenGL/server are unverified. Per-task
commits were consolidated into the working changes; no repository commit is
required to run the approved result with F5. See ISLAND_DEMO.md for evidence.
