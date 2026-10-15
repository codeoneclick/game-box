# Stylized island presentation

The island renders linear HDR scene color, applies exposure/tone mapping, grading and optional weak distance fog, then composes engine-native UI. Bloom, depth of field (DOF), and vignette have been removed from the active island pipeline and final shaders. Their legacy settings and uniform fields remain for compatibility and cannot re-enable the removed effects.

## Current pass order

Geometry/cel + blob field + HDR water/foam ? SSAO/blur ? HDR ocean composition + original silhouette coverage in alpha ? scene final (distance fog, exposure/tone map, grading, original silhouette ink) ? UI composition ? one display encode.

There are five screen-space techniques: `ss.ao`, `ss.ao.blur`, `ss.ocean_compose`, `ss.stylized_final`, and `ss.island_ui`. The three bloom extraction/blur techniques are absent from the transition, so their targets and fullscreen draws are not created or executed. Their standalone source assets and unused Metal entry points remain for compatibility. Final-material slots 2/3 bind the existing scene color as reserved placeholders; positions and water retain slots 4/5. The final shaders never sample the reserved slots.

SSAO retains its existing kernel, depth/normal inputs and blur. Silhouette-ID coverage, blob-shadow geometry and water waves, interaction masks, reflection/refraction and caustics retain their existing algorithms. Original silhouette ink is applied after scene presentation, preserving the 64/255 display gray. UI stays sharp and is composed after scene presentation and every debug view.

## Linear and HDR storage

Active scene-color targets use format **115 / RGBA16Float** in `ws.technique.island_demo.xml`, `ws.technique.ocean.xml`, `ws.technique.ocean_foam.xml`, `ss.technique.ocean_compose.xml` and `ss.technique.stylized_final.xml`. Exact object IDs and positions retain **125 / RGBA32Float**. UI targets remain LDR.

Ordinary PNGs upload as UNORM. Resident/building color textures, sky and editor display colors decode sRGB explicitly before linear calculations. Data masks are never gamma-corrected. Procedural terrain/water palettes retain their engine linear convention.

`graphics_context::is_output_srgb()` queries the actual Vulkan swapchain or Metal drawable convention; legacy GL requires manual output encoding. The island output shader encodes for the latter/UNORM fallback. `enableHDR=false` clamps scene values before final processing without reallocating resources during a frame.

## Active controls

- **Tone mapping:** EV exposure `2^exposure`, then Narkowicz's ACES fitted curve with linear input/output. This approximates ACES and does not implement the full ACES color-management pipeline. Source/license: [author's MIT/CC0 fit](https://knarkowicz.wordpress.com/2016/01/06/aces-filmic-tone-mapping-curve/).
- **Grading:** saturation/contrast around linear 0.18, temperature/tint, cool shadows and warm highlights. Building IDs receive a restrained toe/midtone lift and neutral stone/plaster balance; acid greens are slightly desaturated. Cel-band thresholds remain unchanged.
- **Fog:** optional weak distance ramp from `fogStart` to `fogEnd`, with light cyan linear color. Reconstructs sea intersection where the position buffer has no opaque surface. Existing ocean horizon scattering remains active independently.
- **SSAO:** enabled by default, intensity **1.2**, sanitized to **0..3**. These controls adjust the existing AO contribution without replacing its algorithm.

Both desktop and mobile presets enable HDR, tone mapping, grading, fog and SSAO. They default to exposure **0.15 EV**, saturation **1.12**, contrast **1.05**, temperature **0.015**, tint **0**, shadow tint **(0.97, 1.015, 1.035)**, highlight tint **(1.035, 1.015, 0.98)**, building midtone lift **0.08**, fog start/end/strength **450 / 6000 / 0.08**, and fog linear RGB **(0.42, 0.66, 0.78)**. The former desktop/mobile bloom dimensions no longer affect allocations or execution.

On Windows, **F8** opens the native gbUI presentation panel. Its visible bounds block terrain strokes and camera pointer controls. Closing it restores ordinary terrain/camera input. Presentation preferences do not modify terrain/history or level data.

Views: **Final (0)**, **SceneHDR (1)**, **ToneMapped (4)** and **ColorGraded (5)**. SceneHDR uses `c/(1+c)` solely to display values above one. ToneMapped and ColorGraded expose their stage outputs before silhouette ink. Retired BloomBrightPass/BloomResult names and numeric IDs 2/3 sanitize to Final.

Startup presets: `GB_POST_PRESET=StylizedIsland` / `StylizedIslandMobile` (aliases `desktop` / `mobile`; iOS/tvOS default to mobile). Active switches: `GB_POST_HDR`, `GB_POST_TONEMAP`, `GB_POST_GRADE`, `GB_POST_FOG`, **`GB_SSAO`**. Numeric overrides: `GB_POST_EXPOSURE`, `GB_POST_SATURATION`, `GB_POST_CONTRAST`, `GB_POST_FOG_STRENGTH`, **`GB_SSAO_INTENSITY`**. `GB_POST_VIEW` selects the active IDs or names above. `GB_POST_STAGE` remains a development gate, default **12**; earlier stages are diagnostic gates and not historical pipeline snapshots.

Legacy `GB_POST_BLOOM`, `GB_POST_DOF` and `GB_POST_VIGNETTE` cannot enable those effects. Their numeric settings remain parsed/sanitized for compatibility but have no active visual effect. The existing thirteen post-uniform names and **304-byte** layout stay unchanged; retired activation fields are zero.

## Ownership and checks

Settings/uniforms live in `gbDemo/sources/stylized_post_process*`; integration in `island_demo_scene.*` / `island_demo_transition.*`; controls in `terrain_editor_ui.*`. Canonical configs and shaders live in `gbBundle`, including the matching Metal final shader. Generated/staged Windows assets are rebuilt by the build script.

`python gbScripts/check_stylized_post_assets.py` checks active pass dependencies, HDR formats, preserved sampler slots, removal of final effect sampling, shader pairs, uniform order and RGBA emission bindings. `./gbScripts/check-stylized-post-process.ps1` and `-Mobile` check presets, finite/range sanitization, retired flags/views, SSAO environment controls and EV conversion.

Backend build/runtime verification must be reported with the integrating change. Earlier implementation smokes and the measurements below do not verify this removal. Metal and dormant OpenGL runtime behavior remain **UNKNOWN** on this Windows machine.

## Current cost model

The final presentation stage adds one full-resolution fullscreen draw. Three reduced bloom draws and their target allocations have been removed. Fog retains depth/water samples and ray reconstruction when enabled. Tone mapping and grading are fused ALU work in final. There are no DOF taps or vignette/bloom operations in the active final shader. No new timing measurements were made for this removal.

These measurements describe the former bloom/DOF/vignette pipeline. They are retained as historical data only and do not measure the current pipeline or establish its performance. The profiling script and old cases below also describe that former pipeline.

## Historical measurements before effect removal

Windows Vulkan Release, 1280x720, default camera, 200 residents/72 trees, native editor UI visible and debug panel closed. Validation and screenshot layers disabled. Two runs of ten seconds per case; discard the first eight HUD samples, then use median of the 0.5-second FPS windows. All cases submit 412 draws. Baseline retains the allocated HDR targets and four new configured draws with effects disabled. These measurements do not isolate the intrinsic cost of those allocations/draws or individual GPU pass time.

| Case | Mean of window medians, ms | Delta vs disabled effects, ms |
|---|---:|---:|
| none | 5.395 | +0.000 |
| tone | 5.386 | -0.009 |
| grade | 5.613 | +0.218 |
| bloom | 5.413 | +0.018 |
| fog | 5.436 | +0.041 |
| vignette | 5.390 | -0.006 |
| dof | 5.424 | +0.028 |
| full | 5.681 | +0.286 |
| mobile | 5.467 | +0.072 |

Differences below roughly 0.15 ms are within the run-to-run variation in this small sample; they are not proof of zero GPU cost. Desktop full preset is approximately 176.0 FPS, compared with 185.3 FPS for disabled effects. Optional DOF has a scene/focus-dependent cost; the default focus keeps the center sharp, so this is not a worst-case blur benchmark. Mobile removes three quarters of the bloom pixel work, even where overall frame time is limited elsewhere.

At 1280x720, changed HDR color targets add about 14.06 MiB over their former 8-bit colors; final adds 7.03 MiB; desktop bloom adds 5.27 MiB, mobile bloom 1.32 MiB. Total color-target increment: about 26.37 MiB desktop / 22.41 MiB mobile, excluding backend allocation alignment. Geometry, normal-ID and position buffers are unchanged by this task.

Reproduce: `./gbScripts/profile-stylized-post-process.ps1 -Repeats 2 -Seconds 10`. Raw CSV and logs are under `gbDemo/output/island-check/post-performance`. The script restores environment overrides and closes only its own test processes.
