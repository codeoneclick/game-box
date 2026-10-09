# World-space low-poly water and interaction foam

Status: approved and implemented; see the implementation plan execution ledger.

## Intent and acceptance

The user's attached 35-section specification is the visual source of truth.
Replace the current regular directional waves and screen-depth-only foam with
irregular world-space waves and geometry-derived interaction distances.
Preserve the existing island, resident, orbit/RMB pan/wheel zoom, buoyancy,
reflection/refraction, caustics, and F5 entry point. Preserve the dry-shore fix.
Do not use object colliders, per-object foam rings, random triangle colors,
tiled wave/normal textures, or screen depth as the primary foam source.

Acceptance requires an identifiable rectangular contact contour around a crate,
an irregular rock contour, a thin post contour, complete coastline foam,
variable asymmetric width, large detached patches, and wave-driven animation.
The interaction field must remain unchanged when only the player camera moves.
Waves must exhibit macro/meso/micro scales without an obvious repeated motif.

## Verified integration constraints

- `ces_render_system.cpp::on_feed_end()` executes ordered WS techniques.
  `render_pipeline.cpp::on_draw_end()` executes SS techniques afterward.
  A conventional SS JFA before the existing WS water would read the wrong frame.
- WS techniques support configured float color attachments, sampled by later
  materials. Format 115 is RGBA16F; format 125 is RGBA32F on Vulkan.
- `grab_visible_entities()` uses the player camera for ordinary box/radius
  culling. Geometry with bounding mode `e_none` bypasses those frustum tests.
- Application shaders can supply their own top-down projection while using
  the established model matrix. No engine-owned orthographic-pass API is needed.
- The opaque pass exports SceneColor and world position/positive linear view
  depth in separate color attachments. Preserve these for reflection/refraction
  and final occlusion; do not sample active hardware depth.
- New source files require MSBuild and existing Xcode target membership.
  Asset source is `gbBundle`, not staged Windows resources.

## Recommended architecture

Use configured WS techniques throughout the interaction/JFA/water sequence.
Fullscreen processing uses one application-owned quad rendered directly into
clip space, with bounding mode `e_none`. This respects the existing WS ordering
and avoids altering the renderer's frame phases.

Alternatives considered: extend the renderer to interleave arbitrary WS/SS
passes (unnecessary shared-engine risk); generate object footprints/distances
on CPU (does not satisfy the rendered-geometry requirement). Neither is selected.

Frame order:

1. `ws.water_interaction`: orthographic XZ geometry mask.
2. `ws.water_seed`: mark land/object boundary seeds.
3. JFA WS stages: jumps 512, 256, 128, 64, 32, 16, 8, 4, 2, 1.
4. Extra jump-1 refinement and distance resolve.
5. Existing opaque `ws.base` (clears the main camera depth).
6. Existing `ws.ocean`: faceted water, reflection/refraction.
7. `ws.ocean_foam`: displaced foam layer, 0.04 m above water.
8. Existing composition, extended for premultiplied foam; output gamma.

All stages sample a distinct earlier attachment, never their own render target.
Prototype JFA uses separately configured stage textures rather than native
ping-pong resource aliasing. Measure its memory/time before optimizing reuse.
Depth tests/writes are disabled for field processing. Main scene depth is
cleared after those passes so pooled depth attachments cannot affect the scene.

## Irregular wave field

Keep XZ positions static. Replace horizontal Gerstner displacement and its
inverse solve with direct world-XZ height sampling, shared by CPU/GPU.
Use three noncommensurate scales: macro approximately 24 m, meso approximately
8 m, micro approximately 2.5 m. Macro dominates; meso is about 0.35 and micro
about 0.08 of its amplitude. Low-frequency procedural domain warp changes local
orientation, phase, and envelope; asymmetric crest shaping breaks sinusoidal
symmetry. Hash/value-noise functions and gradients must match CPU/GLSL/Metal.

Expose octave heights and gradients for diagnostics and foam. Buoyancy uses
the same final height and normal at the same clock time. Remove the old inverse
iteration from receiver and refraction water-height queries. Continuous CPU
height versus rasterized triangle interpolation remains an explicit limitation.

Use deterministic interior jitter near 25% of nominal 2 m spacing, global
vertex IDs across patch seams, and independently hashed cell diagonals.
Keep horizon boundary vertices shared and validate positive triangle area.
Facet normals come from displaced-position derivatives. Lighting uses 4-8
levels plus broad cyan plane highlights; remove the narrow realistic specular
and fine glitter as dominant highlights. Depth color transitions through
turquoise, cyan-blue, saturated deep blue with weak world-space variation.

## Interaction geometry and domain

Initial map: fixed world-centered 320 m square, 1024x1024 (0.3125 m/texel).
It covers the current 256 m island and props. Outside the map, return no
interaction; horizon water remains valid. A larger/recentered map is future
work, not silently clamped edge foam. Start inner width around 0.3 m because
0.15 m is subtexel at this resolution; assess 2048 quality only after measuring
the configured multi-stage texture cost.

Static terrain contributes the footprint whose height reaches the water level.
The existing signed terrace representation is available, but prefer one
rendered-terrain path with a clip above/below water to avoid a separate contour
representation diverging from the actual mesh.

Ordinary rigid opaque object meshes are projected top-down and clipped to a
small slab around the water plane. Clip triangles against the slab in the GPU
pipeline (including interpolated world-height fragment rejection), not whole
objects by center height. The slab includes the wave envelope. Terrain and
object mask use different height acceptance so high land stays solid while
elevated object geometry outside the slab does not cast a footprint.

Application-owned mask render instances share source meshes and follow source
absolute transforms, but have only interaction materials and no player-camera
culling. The existing ocean receiver registration is the integration point for
current terrain/props; any replacement mesh receives the same mask path.
They are geometry render instances, not manually authored interaction shapes.
Hidden/detached sources are removed from the mask and corresponding render
instances. Animated/skinned arbitrary meshes are not claimed supported by a
rigid PTNTC mask shader; adding their vertex deformation is separate work.

## Distance field and foam

Seed boundary texels from the binary mask; encode valid flag and nearest seed
coordinates in floating attachments, not sRGB. JFA compares candidates in
world meters. Resolve unsigned exterior distance, inside-mask state, and
nearest boundary direction. Boundary gradients use neighboring world-map
texels; never screen-space derivatives to infer an XZ normal.
Half-float seed coordinate precision must be checked against a CPU oracle;
upgrade attachment precision if it exceeds one map texel.

Inner foam samples unwarped distance and stays dense, bright, nearly continuous.
Mid foam uses variable width, approximately 0.3-1.4 m, shaped by low-frequency
world noise, local crest height, and boundary-normal/wave-direction dot product.
Outer foam forms sparse 2-12 m lobes/patches with rotated multiscale world noise
and bounded coordinate warp. Warp strength decreases toward the inner contour.
Detached patches favor the wave-facing side instead of a uniform ring.

One shared foam surface follows the same water vertex displacement plus the
small height offset. Flat normals and quantized white/cyan shading distinguish
foam from water. Compare foam fragment depth to opaque SceneDepth to avoid
drawing through land/props. Preserve the existing maximum-wave-reach wetness
guard so the lowest dry terrace does not acquire moving dark patches.

## Ownership and files

Implementation remains in `gbDemo`: wave helpers/settings, interaction/JFA
uniform wrappers, mask render-instance lifecycle, mesh generation, scene and
ocean-system integration. Existing `gbBundle` material/technique/transition
assets gain mask, seed, JFA, resolve and foam shaders in GLSL and Metal.
Update CPU uniform layout and every receiver consistently. No generated,
vendored, renderer synchronization, feeder, or entity-core edits are planned.

## Diagnostics and verification

Expose macro/meso/final height, interaction mask, distance in meters, boundary
gradient, inner/mid/outer/final foam, width and wave influence. Retain the
existing optics diagnostic modes or document any deliberate mode remapping.

Implementation milestones follow the user's order: wave/triangulation/color;
mask terrain/objects; JFA correctness; inner/mid/outer foam and animation;
raised foam surface; final visual balance. Build after each major stage.

CPU smoke checks cover deterministic waves/gradients, fixed XZ, finite outputs,
patch seams and areas, buoyancy, and a small exact-distance oracle for JFA.
Vulkan captures cover crate/rock/post/shore, moving/rotated/replaced geometry,
camera changes at fixed wave time, two animation times, and dry terrace
stability. Inspect distance field separately before judging final foam.
Measure frame cost and texture allocation; performance is UNKNOWN until run.

Use `build-gbDemo.ps1 -Configuration debug-client -Platform x64`, targeted CPU
checks, capture smoke tests, and `git diff --check`. Metal/Xcode and dormant
OpenGL remain unverified on this Windows machine. Report the known shutdown
command-pool validation issue separately from runtime errors.

## Review points and limits

Approve the proposed GPU mask/JFA/raised-foam architecture before product-code
implementation. Requirements supplied by the user are preserved; map size,
format precision, slab bounds, and visual coefficients are prototype defaults
to be verified rather than promises of pixel-perfect silhouettes.
Map resolution limits thin contacts, JFA is approximate, slab footprints are
not exact intersections of arbitrary overhanging shapes, and screen-space
reflection/refraction still cannot reflect unseen geometry. These limitations
must not be confused with camera dependence of the interaction field itself.

## Implementation rulings

- Continue in the existing F5 workspace and preserve prior uncommitted work.
- WS dimensions are overridden by the engine to window size (1280x720 here).
  Use actual texture dimensions for JFA; keep the world domain fixed at 320 m.
- Seed/JFA/distance attachments use RGBA32F (format 125), not the proposed half floats.
- WS GUIDs use dots because render-system underscore stripping otherwise breaks lookup.
- Object slab is mean -3.5..+1.1 m to include closed rock bottoms in this scene.
- Explicitly propagate source absolute transforms and nullable meshes to mask instances.
- Retain previously used interaction meshes until scene teardown for in-flight GPU safety.
- Native Metal/OpenGL and GPU timestamps are unavailable; do not infer verification.
