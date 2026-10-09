# Cartoon ocean and sixteen island terraces

Status: approved and implemented; Windows Vulkan verified. Platform and lifecycle verification limits are documented in ISLAND_DEMO.md.

## Outcome

Replace the flat water in the default island demo with an animated cartoon ocean.
Match the water characteristics of Adam Homoki's reference: saturated deep blue,
turquoise shallows, broken white foam, sun glitter, reflections, refraction,
underwater caustics and wet shoreline surfaces. The primary interaction requirements
are automatic foam at object intersections and objects floating on the rendered waves.

Reference: https://80.lv/articles/cartoon-water-shader-in-ue4
Attached visual reference: `C:/Users/sserh/Downloads/adam-homoki-screenshot-ocean.jpg`.
This is an original implementation for Game Box, not a port of proprietary UE assets.
The reference informs water appearance and behavior; recreating its house, trees,
waterfalls and rainbow is outside this feature.

F5 continues to build and launch the island. LMB orbit and RMB anchored panning remain.
The island has eight distinct terraces above mean water and eight below it.

## Existing architecture and constraints

- `gbDemo/sources/island_demo_scene.cpp` creates procedural terrain and a two-triangle
  water plane, using the same opaque terrain material for both.
- `terrain_data.h` stores mutable cell layers as `int8_t`, with `-1` currently
  indicating empty ocean. `terrain_generator.cpp` and `terrain_mesh_builder.cpp`
  currently validate only one to eight land layers.
- Terrain meshing consumes cell data and extracts nested, origin-containing,
  star-shaped contours. Keep this boundary; the builder must not regenerate contours
  using the generator's seed or analytic formula.
- Mesh indices are 16-bit; each independent mesh must stay within 65,535 vertices.
- `game_transition.cpp` registers named WS/SS attachment resources. Materials can
  sample preceding pass outputs using existing texture configuration.
- `ces_shader_uniforms_component` supplies named vertex/fragment uniform buffers;
  Metal and Vulkan need matching layouts. Application systems must update these
  before rendering, not introduce effects inside `ces_render_system`.
- Vulkan pools hardware depth attachments by dimensions in
  `vk_render_pass_descriptor.cpp`. They remain in depth-attachment layout after
  rendering. Do not sample this shared attachment while writing it in a water pass.
- The existing float RGBA attachment format `125` can store world position and
  linear view depth for later sampling, as distinct color resources.
- The engine has Box2D integration, not an established general 3D rigid-body API.
  Floating props use a dedicated demo simulation rather than invented 3D physics calls.

## Terrain representation

Keep `layers_count` as the count of above-water terraces; add
`underwater_layers_count`, default eight. Support one to eight above-water layers
and zero to eight underwater layers. Both use `layer_height`, default 2 m.

Cell labels are physical terrace identifiers:

- `0..7`: above-water tops at `label * layer_height`, defaults 0, 2, ... 14 m.
- `-1..-8`: underwater tops at `label * layer_height`, defaults -2, -4, ... -16 m.
- A named empty-cell sentinel below all valid labels, e.g. `-9`, replaces the old
  ocean sentinel. Out-of-domain reads return the same sentinel. Do not conflate
  the shallowest submerged terrace with empty ocean.

Mean water remains at `water_y = -0.3 m`. Counts describe terrace tops relative
to mean water; animated crests may wash over the lowest above-water terrace.
Validate every enabled above-water height is above mean water and every enabled
underwater height below it. Debug coloring distinguishes all sixteen bands.

Preserve the existing island footprint by separating island generation size from
total terrain domain: default `island_size = 256 m`, `world_size = 512 m`,
`resolution = 512`. This retains approximately 1 m cells and leaves space for
eight outward-growing underwater rings. Above-water contours keep their existing
seeded shape. Underwater contour radii expand from the shoreline with bounded
seeded variation and a positive minimum gap; default terrace width is 6 m.
All contours must fit the domain and preserve nesting after smoothing.

Build sixteen top surfaces and their connecting walls, with upward top normals
and outward wall normals. Limit segment count or split meshes before exceeding
the existing index capacity. Continue rejecting disconnected cells, holes and
non-star-shaped input explicitly. No silent loss of edited terrain.

Add a seabed at -18 m beyond the deepest terrace. It is background geometry,
not a seventeenth terrace. Terrain joins this base without coincident top faces.
Use sandy/rocky colors for submerged surfaces rather than underwater grass.
The initial water patch is 768 m wide; camera clipping and background must avoid
an exposed water edge in the demo's tested orbit/pan range.

## Ocean surface and shared wave model

Create a subdivided grid instead of the two-triangle plane. Split it into patches
if needed for the index limit. Use four directional Gerstner waves initially,
plus shader-only small-scale ripples. Parameters include direction, wavelength,
amplitude, steepness and speed. Expose palette, foam width, absorption,
refraction strength and wave parameters in a documented configuration struct.

A single simulation clock and parameter set drive CPU and GPU. Compute analytic
displacement and tangents/normals; account for horizontal Gerstner displacement
when sampling height at a world X/Z point, using bounded inverse iteration.
Cap steepness to avoid self-intersection and keep crest height appropriate to
the 2 m terraces. CPU sampling returns height and normal, not a separate
approximate sine animation. Pause/reset produces consistent CPU/GPU state.

Camera panning continues to anchor on static terrain or mean sea level so the
camera does not inherit wave bobbing. It does not grab decorative floating props.

## Render passes

Use configured passes and application material registration:

1. **Opaque scene**: terrain, seabed, rocks and floating props write scene color,
   normal and world-position/linear-depth attachments. An explicit validity marker
   distinguishes background from geometry.
2. **Ocean surface**: render displaced water into separate attachments. Sample
   only completed opaque scene color/data textures. Use shared hardware depth
   loading where already supported to reject water hidden behind opaque objects;
   it is never sampled as a water texture. Preserve opaque depth needed for composition.
3. **Composition**: combine opaque scene and water coverage into the output, then
   use the existing presentation path. Opaque foreground must remain opaque.

Validate existing color-read/write dependencies, attachment formats and depth
load behavior before implementation. If a backend correction is necessary, isolate
it to the existing wrapper and its consumers. Do not redesign presentation,
resource workers or ECS scheduling to add water.

Reflection uses screen-space ray traversal against the opaque scene data, with
a sky/environment fallback for off-screen or missed rays. Apply Fresnel blending,
wave-normal distortion and bounded traversal. Reflections cannot contain objects
absent from screen-space data; this is an explicit limitation, not full planar
reflection. No cubemap API is assumed without verifying existing resource support.

Refraction samples opaque color with wave-normal offsets and depth/validity checks
to prevent foreground leakage. Water thickness controls absorption and a shallow
turquoise to deep blue gradient. Add sun highlights and sparse, temporally stable
sparkles; avoid a uniformly noisy white surface.

## Object interaction

**Foam:** reconstruct the gap between water and opaque geometry from the scene
data. Shape an animated, broken white band near intersections. It responds to
current object transforms without per-object hand-painted foam rings. Provide
separate shoreline and moving-object tuning only where required by the same method.
Depth-based foam cannot see occluded geometry and has view-dependent coverage;
do not claim distance-field quality or interaction with invisible objects.
Every water-interacting object participating in the opaque scene pass receives
this behavior, rather than only hardcoded demo props.

**Caustics:** modify submerged terrain and object materials with animated projected
caustic patterns, attenuated by depth and surface orientation. Caustics belong on
the receiving geometry, not solely on the water surface. The shallow submerged
terraces provide a visible demonstration.

**Wetness:** receiving materials darken and gain highlights near the wave-driven
waterline. Track a damped wetness amount on moving props to avoid instant drying;
terrain uses a narrow animated shoreline zone. Surfaces well above reach stay dry.

**Buoyancy:** add a demo-owned component describing local probe positions, immersion,
mass/inertia approximation, vertical/angular velocities and damping. An explicitly
registered demo system applies buoyant forces at multiple probes and updates
vertical displacement, pitch and roll before draw uniforms are read. Use bounded
substeps to remain stable across frame times. Yaw and horizontal placement remain
controlled independently; this version does not implement collision dynamics,
boat controls or general-purpose hydrodynamics. Floating motion must arise from
sampling waves and integration rather than a prescribed bobbing animation.

## Demonstration and ownership

Include shoreline rocks and at least three distinct floating wooden props in the
initial view, including a simple boat/hull. Place them where intersection foam,
roll/pitch and reflections can be seen. Use original procedural geometry and
repository assets; no external asset purchase or dependency is needed.

- `gbDemo`: terrain changes, ocean settings/wave sampling, buoyancy components and
  system, prop construction, scene/pass orchestration, custom uniform declarations.
- `gbBundle`: water/receiver/composition shaders, textures if needed, material and
  technique XML; matching Metal functions.
- `gbScripts`: focused CPU checks for terrain and wave/buoyancy invariants.
- `gbCore`: only narrowly evidenced render-wrapper corrections if existing APIs
  cannot safely express pass composition; no ocean-specific gameplay rules.

Add every new compiled source to applicable Visual Studio and existing Xcode
targets. Update `ISLAND_DEMO.md` and architecture notes whose old assumptions
about eight levels, empty ocean or a single pass become false.

## Acceptance and verification

1. CPU compile/smoke checks demonstrate all eight positive/zero and eight negative
   tops, valid indices/winding, nested nonzero terrace widths, reproducible seeds,
   and rejection of malformed data. Update previous `-1 means ocean` checks.
2. Wave checks cover finite displacement/normals, CPU/GPU parameter layout,
   inverse height sampling and zero-amplitude flat water. A float settles stably
   on flat water and follows wave height/tilt without divergent velocities.
3. Standard `build-gbDemo.ps1 -Configuration debug-client -Platform x64` succeeds.
4. Inspect Vulkan frames over time: wave silhouette/normal motion, deep/shallow
   color, visible refracted terraces, sun glitter, reflection change under orbit,
   caustics on receivers, shoreline wetness and moving object foam.
5. Exercise floats and confirm CPU probe locations match rendered water. Verify
   foam responds to transformed opaque objects, not static painted contours.
6. Exercise LMB orbit, RMB land/sea pan, release and focus/capture cancellation.
7. Check shader compiler and Vulkan validation output, including multi-frame
   rendering and close. Report existing shutdown command-pool validation messages
   separately; do not obscure new attachment or synchronization failures.
8. Run `git diff --check` and inspect new source/project membership. Metal/Xcode,
   dormant OpenGL and server runs remain unverified unless actually exercised.

Tune from the initial strategy view and a lower orbit angle. Match the reference
water's visual character; a screenshot of a flat tinted plane or decorative foam
alone is not acceptance. Distance fields, full planar reflection, waterfalls,
terrain editing, navigation and full 3D collision physics are outside this design.
