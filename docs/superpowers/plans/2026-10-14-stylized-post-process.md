# Stylized island post-processing implementation

Source request: pasted spec supplied 2026-10-14, 12 ordered stages.

Baseline: WS geometry/cel, blob field, water/foam; SS SSAO+blur, ocean composition with silhouette coverage and existing atmosphere, UI composition, output. Preserve lighting bands, SSAO filtering, silhouette classification/coverage and blob geometry.

1. Audit architecture and resource formats; report baseline before edits.
2. Correct texture decode and select exactly one presentation encode through graphics_context.
3. Change base scene, water, foam and scene composite colors to RGBA16F; keep exact normal-ID/position buffers.
4. Add scene final pass with EV exposure and ACES fitted curve; apply original silhouette coverage after post-processing.
5. Analytic grading: saturation/contrast, temperature/tint, shadow/highlight balance, gentle building midtone lift.
6. Three bloom passes at half desktop/quarter mobile resolution; threshold before downsampling, separable blur, linear upsample.
7. Replace old atmospheric integration with weak distance fog; retain water-depth reconstruction.
8. Weak vignette before silhouette overlay.
9. Optional depth-aware small DOF before tone mapping; default off.
10. Authored lantern mask and HDR water/foam intensities, without geometry changes.
11. Engine-native F8 effect controls and intermediate views; UI after all scene effects.
12. Desktop/mobile startup presets, both Windows configurations, native smoke/debug comparisons and measured frame costs.

Every implemented stage gets a Windows Release build, native Vulkan validation smoke and normal shutdown check. Final Debug uses an isolated output when needed. Metal/OpenGL source compatibility is inspected, but execution on these backends is not available here.

Core changes are limited to a read-only presentation-format query and an overridable SS pass-size policy; defaults retain the existing sizing of all other transitions. GPU commits remain on the game-loop thread.
