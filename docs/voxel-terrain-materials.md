# Voxel terrain materials

Terraces are numbered from the coast upwards: all submerged terraces and seabed use limestone; 1 sand; 2 shoreline; 3–5 grass; 6 lime grass; 7 dry grass; 8–16 mesa clay.

No block geometry is added. Rounded terrace contours, flat surfaces, edit commands, pathfinding and outlines are preserved. Material selection uses each triangle's constant logical terrace ID, so the old interpolated palette cannot produce wrong surrounding materials after editing. Existing climate colors do not tint these fixed terrace materials.

The authored RGBA PNG atlas `gbBundle/Textures/terrain_voxel_atlas.png` contains separate top and side faces. Grass sides have a jagged green cap over soil, shoreline blends sandy patches and grass, mesa sides have horizontal clay strata. Textures repeat every 2 metres in world coordinates instead of stretching across meshes. Side height follows the actual terrace height; the coastal height and layer height are passed in previously unused `extra.zw` channels. Ocean uniform ABI and water behavior remain unchanged.

The Vulkan PNG path provides one GPU mip. Six manually filtered resolutions (32,16,8,4,2,1) are stored in the same atlas; derivatives choose and blend adjacent levels in linear light. Borders do not overlap. Layout is 256x480, seven columns with 36-pixel stride, six LOD rows with 80-pixel stride, top at row+1 and side at row+41. All pixels are opaque RGBA, matching the loader's upload size. The atlas occupies about 480 KiB uncompressed and needs no extra draw calls.

Regenerate deterministically with `python gbScripts/generate-terrain-voxel-atlas.py` (Pillow required; Python 2.7/3 supported). Color palettes and fringe/strata patterns are in that script. The reference image is not modified.

Material XML adds sampler index 2, read as GLSL sampler_03 / Metal texture(2). Texture orientation accounts for the PNG loader's vertical row flip. Three Apple demo resource targets include the atlas; matching Metal shader source is updated. Vulkan builds/runtime are verified on Windows; Metal and dormant OpenGL are not executed.
Verification completed: standard debug-client and release-client x64 builds, check-island-terrain.ps1 CPU checks, engine shaderc compilation, atlas dimensions/RGBA checks, native Vulkan sculpting on a lower terrace followed by zoom (correct mesa/grass assignment and smooth contour), clean exit 0, and git diff --check.
