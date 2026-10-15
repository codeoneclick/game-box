# Stylized soft island lighting

The default model is StylizedSoftIsland. F8 opens native engine UI: SSAO on/off and intensity (0–3), Cel/Soft comparison, Soft/Sunset presets, sunlight/ambient intensity, wrap, diffuse power, specular, vegetation backlight and six linear RGB color sliders. Existing exposure, saturation, contrast, tone map and fog controls remain. Preferences are runtime presentation settings, separate from terrain edit history and level saves.

The previous model used three colors with narrow smoothstep transitions near NdotL 0.22 and 0.65. Soft mode uses clamp((NdotL + wrap)/(1 + wrap)), smoothstep over 0–1, then pow(diffuse, diffusePower). Base color is multiplied by warm directional sun and cool ambient; a broad Blinn–Phong highlight and optional vegetation backlight are added in the same material pass. Cel remains available for comparison. Hard face normals and GBuffer object IDs are preserved.

| Parameter | StylizedSoftIsland | StylizedSunsetIsland |
|---|---|---|
| Sun RGB (linear) | 1, .94, .84 | 1, .71, .43 |
| Sun intensity | 1.1 | 1.25 |
| Ambient RGB | .55, .68, .82 | .48, .48, .76 |
| Ambient strength | .32 | .32 |
| Wrap | .35 | .4 |
| Diffuse power | .8 | .8 |
| Global specular | 1 | 1 |
| Vegetation backlight | .08 | .08 |

Material strengths: grass .04, wood .06, stone .10, leaves .14, plaster .07, metal .35; shared highlight exponent 24. These are editable C++ settings. Atlas buildings lack material IDs, so their shader approximates wood/leaf/neutral surfaces from color; neutral surfaces average stone and plaster. Metal needs explicit asset metadata to identify reliably and its setting is reserved. Residents use the matte plaster coefficient. Tree wind metadata identifies vegetation, with green color separating leaves from trunks.

Mobile defaults keep wrapped diffuse and ambient, halve specular and disable backlight. Environment: GB_LIGHTING_PRESET=StylizedSoftIsland / StylizedSunsetIsland / mobile; GB_LIGHTING_MODE=Cel / StylizedSoft. SSAO uses GB_SSAO=0/1 and GB_SSAO_INTENSITY=0..3.

Shared CPU/GLSL/Metal suffix: seven vec4 values (112 bytes). Terrain/ocean buffers are 608 bytes; resident/building buffers are 208. Water consumes its original prefix and its lighting/waves/foam remain unchanged. No new render passes or textures are required. The application owns settings on the game thread; no GPU work moves to workers.

Bloom's three passes are removed from the active transition. Final shaders no longer apply bloom, DOF or vignette. Older source assets and reserved post uniform fields remain inactive for layout compatibility; flags cannot re-enable those effects. HDR, tone mapping, grading and the restored ocean/skybox atmosphere remain.

SSAO algorithm, radius, samples, bias and blur are unchanged. Only ao_params.z is driven by the live intensity or zero for Off. Blob shadows are still multiplied in the AO blur composition, so disabling SSAO preserves blob shadows. Outline coverage and its crisp final composition are unchanged.

Changed lighting shaders: shader_island_terrain.vert/.frag, shader_island_resident.frag, shader_editor_building.frag; Metal counterparts mtl_ocean.h, mtl_library.metal, mtl_island_resident.h, mtl_editor_building.metal. Presentation removal is in shader_stylized_final.frag and mtl_stylized_post.metal. Settings and uniform packing are in gbDemo/sources/stylized_lighting_settings.h and stylized_lighting_uniforms.h.

Verification: standard Debug and Release builds, CPU presentation/lighting settings checks (desktop and simulated iOS), asset graph validation, native Vulkan Soft/Cel/Sunset and SSAO-off scenes, live F8 controls and clean shutdown. Metal/OpenGL were not built or executed on this Windows machine. Shader parity was inspected in source.

Performance reproduction: ./gbScripts/profile-stylized-lighting.ps1 -Repeats 2 -Seconds 8. Measures whole-frame median FPS windows, not GPU timestamps; uses the same scene and HDR/atmosphere/SSAO/outline settings in Cel and Soft, with validation/screenshots disabled and F8 closed. Results go to gbDemo/output/island-check/lighting-performance/results.csv.

Measured Windows Vulkan Release at 1280x720 (200 residents/72 trees): two eight-second runs per mode, 409 draws in both. Average of the two median FPS-window frame times: Cel 5.1603 ms, Soft 5.1820 ms, delta +0.0217 ms (~0.4%). This small difference is within measurement variation; it is not a per-pass GPU cost. Removing three bloom passes reduces the prior closed-panel default draw count from 412 to 409 and removes approximately 5.27 MiB of desktop half-resolution HDR color targets at this viewport.
Native building preview, successful placement on the top terrace, switching back to Terrain and zoom were also exercised with Vulkan validation; normal model shading, silhouette and clean shutdown were verified.
