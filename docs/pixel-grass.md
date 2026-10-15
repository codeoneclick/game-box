# Pixel-art grass

Grass has a staircase silhouette, rather than a triangular outline with square
colours. Each blade is a full-height card with a four-column/eight-row cutout:
two rows per step, two columns wide, narrowing to one column at the top.
The pattern is mirrored per root for variation. It uses four vertices and two
triangles instead of five vertices and three triangles, in both the initial
island and editor-generated grass. The seed partitioning cache uses the same
four-vertex layout.

The left/right coordinate is explicitly stored as 0/255 in grass vertex colour
alpha, which is otherwise replaced by the shader's .5 outline sentinel. This
avoids deriving the side from half-precision root coordinates: those coordinates
can round enough to collapse a narrow card far from the world origin. Wind still
uses the shared root UV and the side bit does not change the GPU vertex layout.

Grass uses colour cells in blade-local rest-pose coordinates. The
fragment shader samples the existing root-to-tip colour gradient at each
cell's vertical centre and applies one of three brightness levels. This keeps
the green, dry and snow palettes while replacing the continuous gradient with
larger, clearly separated patches.

Wind and character interaction still sample the shared root coordinates before
the vertex shader projects the fragment coordinates onto the blade. Consequently
the pixels follow the bending blade rather than sliding across it. Subpixel
colour detail fades with screen derivatives to reduce distant shimmer. The
silhouette mask discards fragments before G-buffer writes, so the transparent
part of the card cannot occlude the terrain. Instance count and render passes
are unchanged; fewer triangles and batches are needed for the seed field.

Implemented in `shader_island_terrain.vert`, `shader_island_terrain.frag` and the
corresponding functions in `Shaders/metal/mtl_library.metal`. The existing grass
marker restricts the effect to grass, including grass created by the editor.

Verification: Windows debug-client and release-client builds; native Vulkan
grass and resident-interaction inspection views at frames 120 and 240 with live
wind, validation enabled and clean application shutdown. Apple Metal and the
OpenGL backend have not been built or exercised on this Windows machine.
