# Water field resolution (2026-10-18)

The Render Inspector showed actual 4096x4096 attachments even after XML was
changed to 1024x1024. `island_demo_transition::get_ws_technique_size` overrode
every `ws.water.*` configuration with 4096 before GPU attachment creation.
The inspector was reporting the real allocation, not stale graphics settings.

All WS and SS techniques now use `frame_width` and `frame_height` directly from
their XML. The viewport overrides and the island-specific fixed map overrides
have been removed, including grass interaction and both sun maps. Missing/zero
dimensions produce an error naming the technique instead of silently allocating
another size. Three legacy WS XMLs used obsolete `screen_width/screen_height`
names; those attributes now use the existing schema's `frame_*` names, preserving
their declared values. Generated configuration files were not modified.

The output technique presents the native window/swapchain, so its size is the
actual window size, not a hardcoded number or an offscreen XML allocation. Its
existing configuration schema has no frame dimensions. GraphicsSettings does
not override the XML sizes.

Sun shadow setup reads both actual attachment dimensions. Matrix snapping uses
separate X/Y texel sizes, while receiver bias and shadow-caster LOD derive their
texel sizes from the same actual maps. The GLSL/Metal receiver-plane correction
derives light-space width from the matrix, including for non-square maps.

`ces_ocean_system::set_world_dimensions` no longer replaces the interaction
resolution with 4096. Scene setup reads the actual distance attachment width,
which supplies the reciprocal texel size used by foam and distance conversion.
GLSL and Metal JFA steps derive their initial power-of-two jump from the input
texture size. The existing twelve passes remain; at 1024 the last extra passes
use one-texel refinement steps. No shader assumes a 4096 input grid.

XML changes are applied on scene/application creation after asset staging; the
current application does not recreate these attachments when an XML is edited
on disk while running. The normal build script stages gbBundle source assets.
GraphicsSettings JSON stores render settings/toggles, not attachment dimensions.

Verification: Release build, ocean CPU/buoyancy checks and Windows Vulkan smoke
`./gbScripts/check-terrain-lod-demo.ps1 -WaterFields`. The smoke compares every
actual water attachment's logged size against its source XML. Verified mask,
seed, all twelve JFA outputs and distance at 1024x1024 with clean validation.
Capture: `gbDemo/output/island-check/terrain-lod-work/demo-d36c4374b9294946bc0114356e87abf4`.
Metal/OpenGL execution is unavailable on this Windows machine.

Debug was also rebuilt through the standard all-module script using isolated
demo PDB paths to avoid the existing PDB lock. The canonical Debug executable
matches the built file by SHA256. Debug Vulkan `-WaterFields` completed normally
with clean validation: `demo-56377d058b4143d483267fad397cd370`.

A focused ordinary Release check at the unchanged starter camera measured
11.11ms frame wall time after warmup (~90 FPS). The earlier 4096 baseline was
about 17.5ms (~57 FPS), but the two captures also bracket the terrain work;
do not attribute every part of that difference solely to resolution. The
resolution assertions above are independent of timing or FPS.

## All-technique XML sizing follow-up

`check-terrain-lod-demo.ps1 -TechniqueSizes -WaterFields` now compares all WS/SS
attachments in the island transition with their source XML, including the
inspector's own hidden WS target. The original executable failed this regression
on grass interaction (XML 1024x1024, GPU 4096x4096).

Both standard all-module Release/Debug builds passed after removing overrides.
Debug again used isolated demo PDB paths; the canonical Debug executable was
updated and its SHA256 matched the linked binary. Native Vulkan validation and
all-technique size checks passed:

- Release: `demo-af3e2a44fd49415eb8febe648d0752c6`.
- Debug: `demo-922f3c0107344cc9ac415ca05f7fa858`.
- Temporary XML-only variation, without rebuilding C++:
  `demo-3708594be61a42e59209210ef5ca535b`. Grass was 768x512, far sun 1024x512,
  SSAO 640x360, and actual GPU attachments matched each value. Both source and
  staged XML were restored byte-for-byte in `finally`.

The source values remain grass 1024x1024, near/far sun 2048x2048 and SSAO
1024x768. All 53 source WS/SS XMLs have positive frame dimensions. Sun shadow CPU
checks passed, including per-axis snapping for non-square maps. `git diff
--check` passed. Metal shader parity was updated but Metal/OpenGL and unused
legacy transitions were not exercised on this Windows machine.
