# Island silhouette outlines

The island compose pass outlines object-ID boundaries only. It no longer
compares face normals or plane distances. One ID covers the whole resident,
building or tree; each logical terrace has one ID shared by its top and cliff.
Grass, sky, ocean and untagged props are excluded. Visible grass pixels suppress
neighboring outlines so the terrain contour does not draw through the grass.
The color remains display RGB 64/64/64.

The outline uses a .5 screen-pixel half-width and analytic pixel coverage.
Across two selected objects this produces a one-pixel-wide antialiased band,
half the previous two fully covered pixels. Fractional pixel phases and corners
use boundary distance and maximum coverage, without extra sampling passes.

The existing normal attachment stores normal RGB and positive identity alpha.
It uses the existing RGBA32Float format (125), preserving exact IDs and SSAO
normals without another render pass or additional draw calls. This increases
the attachment's storage from 4 to 16 bytes per pixel. Integer texel reads avoid
mixing adjacent identities; procedural identities are rounded before storage.

Procedural IDs travel through unused vertex channels: 7 bits in normal.w,
7 bits in negative tangent.z and 8 bits in color.a. Lighting colors, normal XYZ,
wind weight/phase, root UVs and vertex layout are preserved. The single packed
pattern that resembles untagged opaque geometry is reserved. Trees use a
thread-safe compact registry keyed by their logical save IDs; residents and
buildings use their existing fragment uniform buffers.

Verification: Windows Release and isolated Debug builds, engine shaderc compile,
11 compose semantic fixtures, editor and terrain CPU checks (including seam
IDs, packing and save/load), Vulkan screenshots and normal teardown. The
Vulkan validation log is empty. Matching Metal sources are updated, but Apple
and OpenGL execution remain unverified.
