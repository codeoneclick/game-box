# Coarse pixel art and solar-time control

Terrain's authored color cells are now8x8 atlas texels instead of2x2, making their world-space squares four times larger (0.5m on a2m block). The same atlas dimensions/layout and six linear-light filtered resolutions are preserved. Bark/leaf procedural pixels use6 cells/metre instead of24, with four distinct shade levels and stronger contrast. Pixel detail stays attached to the rest pose and fades at distance rather than shimmering.

The native top toolbar has a Time of day slider immediately after Load. Range0–24h, one-decimal readout, 0h/24h labels; saved status moves after the panel. Drag begins inside UI hit bounds and cannot sculpt terrain or start a camera drag. UI refresh synchronizes changes made through presets.

StylizedLightingSettings::timeOfDay defaults10h; Soft/Mobile defaults10h and Sunset17h. The sun follows a normalized daily arc with sunrise6h, zenith12h, sunset18h and identical0h/24h positions. Direct sunlight fades smoothly around the horizon; configured ambient remains. The control changes logical directional lighting and water highlights, not the already painted skybox textures, blob-shadow shape or terrain state. Time is a runtime presentation preference, not auto-advancing game time or a level-save field.

Terrain, trees, residents, buildings, water and horizon scattering all consume the same direction. Existing sun.w object IDs and outline-enable flags are preserved. Soft sun intensity and water highlights are multiplied by daylight visibility. Cel keeps its daytime band colors but blends to its shadow fill at night; its wet glint also fades. No GPU uniform layout or extra passes are added: light_options.w carries visibility in the reserved field. Environment override: GB_TIME_OF_DAY=0..24.

Checks: CPU normalized-direction tests across0–24h, sunrise/noon/sunset/endpoints, invalid-value handling and environment parsing, including simulated mobile defaults; standard Debug/Release builds; native Vulkan frames at0/6/12/18/24h and live slider endpoints/noon; no validation errors and clean shutdown. Metal shader counterparts are updated but not built/run on Windows.
The slider snaps its final1% to exact0/24h and clamps captured drags beyond either track end. Native screenshots confirm0.0h/24.0h/noon and unchanged Saved status (no terrain edit).
