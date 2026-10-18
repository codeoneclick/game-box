# Water optics and F8 controls (2026-10-18)

Reflection/refraction were still in `shader_ocean`, and underwater caustics were
still in `shader_island_terrain`. They were not standalone techniques, so the
pass-only F8 inspector exposed neither their names nor independent controls.

The visual loss had two contributing factors: the water's world-space pixel
footprint blended up to 95% of the environment reflection into uniform horizon
airlight, and the original fine caustic pattern became difficult to resolve at
the enlarged island's viewing scale. The distance-based horizon blend remains,
but pixel footprint no longer erases the skybox reflection. The existing SSR
march/fallback and wave geometry are retained. Caustic scale changes from .8 to
.3 (broader shapes), strength from .5 to 1; refraction strength changes from .008
to .016. The underwater depth falloff and foreground-rejection guard remain.

F8 -> **ws.ocean** now exposes **Reflection**, **Refraction**, **Caustics**, each
with On/Off and 0-3x intensity. Save persists the six corresponding fields in
`postProcess` in `gbResources/GraphicsSettings.json`, and scene startup loads
them. Old files remain supported. The UI changes preferences; scene commands
pass intensity multipliers to the ocean system. The system updates existing
uniform buffers for water and submerged receivers, without changing physical
wave settings, buoyancy, meshes, render-pass composition or attachment sizes.

Reflection Off removes the Fresnel baseline as well as SSR/skybox contribution.
Refraction Off skips distortion/rejection sampling, keeping bottom transmission.
Caustics Off skips their lighting calculation on submerged receivers. Caustics
execute in ws.base; water transmits the lit bottom. Existing ws.ocean preview
shows the final combined water, rather than pretending each shader operation
has a separate GPU buffer or triangle submission count.

GLSL and Metal implementations were updated together. Metal water and foam UVs
now project their world position instead of dividing fragment coordinates by
the scene input texture size, which was invalid when XML pass sizes differed.
Apple/OpenGL execution was not available on Windows.

## Verification

- Both all-module Release/Debug demo builds passed. Debug used the existing
  isolated PDB override to avoid the pre-existing PDB lock; the canonical Debug
  executable was updated and its SHA256 matched the linked file.
- `check-graphics-settings.ps1` passed, including round-trip of six water
  preferences and atomic rejection of a nonboolean water toggle.
- `check-ocean.ps1` passed (ocean and buoyancy CPU checks).
- `check-terrain-lod-demo.ps1 -WaterOptics -TechniqueSizes -WaterFields` passed on
  Release: `demo-51bf5facb92b4be19d42e1634f3f7880`.
  It opens F8, selects ws.ocean, clicks each toggle independently, verifies Save,
  and compares fixed-camera/time water pixels with the all-enabled frame.
  In the 220x320 water crop, Reflection changed 70400 pixels, Refraction 47895,
  and Caustics 18090. All XML-to-GPU attachment checks and Vulkan validation
  passed. The helper restores the original JSON byte-for-byte on exit.
- The same native check passed in Debug:
  `demo-2d3da6b11bce466380f963dcbb2da407`, including F8 toggles/Save, per-effect
  rendered-pixel differences, water/other technique XML sizes and clean Vulkan
  validation. Both demo executables are updated. `git diff --check` passed.
- Initial UI capture: `demo-f7691cd21588464ba42488fa94b0585d/120.png` in
  `gbDemo/output/island-check/terrain-lod-work` (before final refraction tuning).

The optional `GB_OCEAN_INSPECT=coast` debug camera locates the real shallow
terrace, so this verification continues working after island resizing.
