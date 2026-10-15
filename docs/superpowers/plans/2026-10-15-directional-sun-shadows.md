# Directional sun shadows implementation plan

**Goal:** Replace screen-space cast-shadow approximations with stable, visible sun shadows on the island.

**Architecture:** Two orthographic depth maps (near 96m, far 800m, 2048 pixels each), snapped to light texels. Scene-specific caster proxies share the visible meshes and bone palettes, reproduce tree wind, and are culled in light space independently of the view camera. An HDR screen pass resolves PCF shadows before tone mapping. A default-preserving WS size hook is the only core API addition.

**Constraints:** Eight smooth terraces; editor/save behavior unchanged; no bloom/DOF/vignette; retain ambient light. Windows Vulkan verified, matching Metal implementation with Apple verification explicitly unavailable. No generated/vendor edits. No changes to render traversal, platform synchronization or resource threading.

**Review focus:** offscreen casters, animated bones/wind alignment, cascade transitions and texel snapping, terrain/building removal and mesh retirement, night and sunrise behavior.

- [x] CPU checks for orthographic projection, shadow direction/length, subtexel stability, vertical light and bounds. Observe failure before adding math.
- [x] Implement fixed WS sizes, caster proxies and shared-pose uniforms; register source/build metadata.
- [x] Add depth shader/materials/passes and HDR PCF resolve with cascade blending; update Metal and asset metadata.
- [x] Build Debug/Release; native close/overview, day/sunset/night, orbit/zoom and editing checks, validation/teardown, performance comparison.
- [x] Review implementation and record limitations and evidence.

Execution record: implemented inline. CPU projection checks failed for the missing
header, then passed. Both Windows configurations built. Native Vulkan sunset,
close-up, day/night, edit/orbit/zoom and on/off runs passed validation and teardown.
Review fixes: linear UNORM depth, combined Metal skin block, matching Metal depth
attachment, first-frame uniform setup. Receiver-plane PCF corrected low-sun acne.
Full details and performance cost: docs/directional-sun-shadows.md.

Final review additionally found SS scalar/matrix copying before the end phase and
array omission in bulk setters. Resolve publication moved to feed, casters retain
those exact matrices, and skin arrays upload explicitly through the existing
adapter. Final animated close/edit/pan/orbit/day/night native checks repeated
successfully after both fixes; no Important review findings remain.
