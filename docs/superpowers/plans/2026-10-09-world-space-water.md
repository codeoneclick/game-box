# World-space Water Implementation Plan

> **For agentic workers:** Use superpowers:executing-plans for sequential execution in the current approved workspace. Track steps below.

**Goal:** Replace periodic waves and camera-dependent foam with a static-XZ multiscale wave field and rendered world-space interaction distances.
**Architecture:** Configured WS geometry mask, seed/JFA/resolve fullscreen WS stages, opaque scene, water and raised foam layer, then SS composition. CPU and both shader languages share the same procedural height field.
**Tech Stack:** C++, GLSL/shaderc/Vulkan, Metal, configured material/technique XML.
**Spec:** `docs/superpowers/specs/2026-10-09-world-space-water-design.md`

## Global constraints

- Preserve uncommitted previous work and existing F5 workspace.
- No colliders, per-object foam meshes, hardware depth sampling, engine phase changes, generated or vendor edits.
- World map 320 m; proposed 1024 square is overridden to window dimensions. Distinct earlier-stage attachments, floating seeds.
- Preserve dry shore, resident, controls, optics and buoyancy. Update Metal source; report unavailable native verification.

## Review focus

- Offscreen geometry must still contribute; bypass player frustum only for mask instances.
- Floating transforms and removed sources must not leave stale footprints.
- Empty map seeds and out-of-domain sampling must not create edge rings or NaNs.
- Thin posts and rectangular corners must survive map resolution.
- Mask/JFA dependencies must never read the active output attachment.

## 1. Wave field and mesh

Files: `ocean_wave_model.{h,cpp}`, `ocean_mesh_builder.cpp`, existing ocean/terrain GLSL and Metal functions, `check_ocean.cpp`.
Interface: preserve `evaluate_ocean`/`sample_ocean`; positions keep caller XZ, normals follow final height gradient.
- [x] Add static-XZ/time regression, run failing CPU check.
- [x] Implement hash value noise, domain warp, asymmetric multiscale waves and matching GPU height functions.
- [x] Hash cell diagonals and increase deterministic jitter while checking patch seams and positive area.
- [x] Run CPU smoke and Windows build/capture before mask work.

## 2. Interaction field

Files: new interaction/seed/JFA/resolve shaders and materials/WS XML, scene/ocean-system integration, uniform wrappers, VS/Xcode metadata if needed.
Interface: mask world coordinates, nearest boundary seed with valid flag, resolved distance in meters and inside state.
- [x] Render terrain and slab-clipped rigid object geometry into top-down mask using shared-mesh child render instances.
- [x] Add clip-space fullscreen quad stages in WS order, seed occupied texels, JFA 1024..1 plus refinement and resolve.
- [x] Check empty map, thin post, rectangular crate and irregular rock field independently; compare cameras at fixed time.
- [x] Verify object transforms, replace geometry, detached/hidden source lifecycle and Vulkan validation.

## 3. Water and foam

Files: existing ocean shaders/uniforms/settings, new foam shaders/material/pass and compose shader/XML.
Interface: water/foam sample resolved world distance and boundary direction; foam color is premultiplied with coverage.
- [x] Add dense unwarped inner contour, variable mid width, domain-warped outer patches, boundary-direction/crest influence.
- [x] Add raised shared surface with matching height displacement; manually occlude against opaque position/depth.
- [x] Quantize faceted lighting and add broad cyan plane highlights, preserve reflection/refraction.
- [x] Add requested height/mask/distance/gradient/layer/width/wave-influence diagnostics.

## 4. Verification and delivery

- [x] Build Windows debug-client x64, run CPU checks and multiframe Vulkan captures.
- [x] Inspect fixed-time fields, final images at two times, dry terrace stability, default F5 scene.
- [x] Record memory/frame cost and pixel precision limitations; update ISLAND_DEMO and architectural ocean notes.
- [x] Review changes, fix important findings, run diff whitespace check and report unverified backends.

## Execution ledger

- Static-XZ regression failed against the prior implementation and passed with the
  shared warped height field. CPU smoke and buoyancy checks passed again after delivery changes.
- Windows build: `./gbScripts/build-gbDemo.ps1 -Configuration debug-client -Platform x64`.
- Vulkan captures: final water at fixed times 3/9; layers 7/8/18/19/20;
  full mask/distance and second camera; normal animated default scene.
- `check-ocean-field.py`: mask exactly camera-independent; 523 independent world-distance
  samples, maximum error 0.0940 m. Dry terrace comparison had zero changed RGB pixels.
- Temporary lifecycle fixture cleared the crate mesh at frame 60 and detached its source
  at 130. Its ROI occupied-pixel counts were 114, 0, 0 at frames 6, 80, 160.
  Initial fixture revealed in-flight Vulkan buffer destruction. Scene-owned retention of
  interaction meshes corrected it; repeat rendering validation passed.
- Temporary crate-to-rock mesh replacement built and rendered successfully.
- Temporary empty-mask fixture validates invalid seed propagation and saturated distance.
  Empty field RGB extrema were (255,255) in each channel. All fixture edits are restored before the final build; none are shipped.
- Review found no blockers and requested clearing stale nullable meshes; corrected and
  exercised above. No core synchronization or resource destruction contracts were changed.
- Measured allocation estimate is approximately 204 MiB additional color attachments
  at 1280x720. Precise GPU timing remains UNKNOWN; native Metal/OpenGL remain unverified.
- Normal shutdown retains the pre-existing command-pool validation issue. Rendering
  checks do not suppress this; logs separate it from errors during the captured flow.

Artifacts (ignored output): `gbDemo/output/island-check/world-final3.png`,
`world-default.png`, `world-replaced-rock.png`, full-field capture directories,
`world-lifecycle`, `world-empty` and `world-delivery-build.log`.

Final delivery build and default/close-view Vulkan runtime checks passed after restoring fixtures. Hidden-source toggle and transition recreation were not exercised.
