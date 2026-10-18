# Ocean visibility

The island keeps the original 384-cell water grid and horizon triangles. Each
original mesh has a CPU quadtree over triangle bounds. Selection compacts visible
indices in their original order and shares the original VBO between the water
and foam draws. It changes neither topology nor wave, foam or lighting shaders.
Draw calls remain bounded by the original nine patches and horizon; quadtree
leaves do not become separate entities or draw calls.

Bounds include shader wave displacement and the foam height offset. Far-plane
rejection is omitted for water geometry that can use the shader's depth clamp
beyond world extent 720, including large loaded water grids. Foam uses normal
clip depth. Invalid projection data conservatively retains geometry.

Foam support includes the interaction field and its warp margin. Outside-support
triangles are removed only if their conservative screen bounds cannot overlap
possible nonzero foam. Zero-writing occluders are retained because this pass
overwrites its target without depth testing or blending. Broad future foam
settings that could make the field's sentinel distance produce foam disable
support pruning.

Camera projection changes trigger selection and index updates. Stationary cameras
reuse the previous selection. Small camera movements that select the same tree
nodes also reuse the index lists. Intersecting leaves retain their boundary
triangles as a group, trading a little extra geometry for inexpensive CPU work.
Dynamic IBOs use existing backend upload handling,
including Vulkan's per-frame snapshots. Level water-size/height changes rebuild
the trees and retain replaced meshes through the existing retirement grace.

Diagnostics:

- `GB_OCEAN_CULLING=0`: original full index lists and coarse entity box culling.
- `GB_OCEAN_CULL_LOG=1`: selected triangle counts, tested nodes and selection CPU time.
- `GB_OCEAN_FIXED_TIME=3`: deterministic waves for image comparisons.
- `./gbScripts/check-ocean.ps1`: CPU geometry, visibility and buoyancy checks.

Windows Vulkan is exercised locally. OpenGL and Metal execution remain unverified.

## Local verification (2026-10-16)

Release captures at 1280x720, fixed ocean time 3, zero residents, identical
graphics settings and camera: default, low-horizon and close views each had zero
different RGB pixels with culling enabled versus disabled, excluding the changing
statistics HUD. The original renderer also matched the new disabled path.

In the default view, submitted water triangles fell from 352,256 to 187,327;
foam fell from 229,376 to 146,944. Other views depend on visible surface area.

A separate Release run with 200 residents and eight seconds of continuous mouse
orbit measured seven moving-camera windows per mode: mean frame interval 8.095 ms
without fine culling, 8.269 ms with it. Selection averaged 0.624 ms per camera
update (maximum 1.550 ms); unchanged camera matrices skip selection entirely.
The measured benefit is reduced submitted geometry, not a demonstrated FPS gain
on this machine. Timing runs used no screenshot layer and no concurrent builds.
