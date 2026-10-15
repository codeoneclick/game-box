# Soft island lighting implementation plan

**Goal:** Implement the supplied wrapped diffuse specification and remove Bloom, Vignette and DOF from the active pipeline.
**Architecture:** Keep existing WS material passes and append the same seven vec4 lighting fields to their fragment uniform buffers. Application-owned presentation settings are updated on the game thread; native F8 controls edit those settings. Preserve SSAO kernel, silhouettes, blob shadows, water and horizon scattering.
**Spec:** User attachment a2e7595b-8822-48d8-8d4f-62748f9aecc8/Pasted text.txt; direct instruction removes Bloom/Vignette/DOF.

- [x] Implement validated day/sunset/mobile lighting settings, shared material strengths and CPU/GLSL/Metal uniform layout.
- [x] Implement wrapped diffuse, cool ambient, soft specular and vegetation backlight in terrain/resident/building shaders; keep Cel comparison. Preserve hard face normals and all GBuffer IDs.
- [x] Remove three bloom passes and unused final effects, retain HDR tone/grading/fog and horizon atmosphere.
- [x] Add native F8 SSAO toggle/intensity and lighting/preset/color controls; keep camera/editor input capture.
- [x] Verify preset validation, graph/ABI, Debug and Release builds, native scenes and UI changes, teardown and relative performance. Report Apple/GL unavailable.

Review focus: zero light/spec strength; grazing/opposite normals; preview tint and emission; AO off restoring unoccluded surfaces; UI bounds on 1280x720; existing wave/horizon behavior.

