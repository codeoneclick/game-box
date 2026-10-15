# Randomized terrain pixels

The terrain shader now independently chooses atlas palette entries for each
0.5 metre world-space pixel. Previously the same 4 by 4 pattern repeated every
2 metres, making larger square tiles visible. The new mapping has no repeated
block borders, bevels or tile outlines. Object silhouette outlines are unchanged.

Top faces randomize both palette coordinates. Side faces randomize the horizontal
column while retaining the vertical grass cap, soil and mesa layers. Material
selection remains tied to the logical terrace ID, so editing does not change
neighbouring terraces' materials. Coordinates remain anchored in world space.

Subpixel detail fades towards the atlas's prefiltered mean using derivatives
computed before quantization. The shader still uses two atlas samples; no extra
geometry, texture assets or render passes are introduced. The six existing atlas
resolutions are retained, with the new mapping using full detail and the mean.

GLSL and Metal implementations are kept consistent. Windows debug-client and
release-client builds and native Vulkan inspection views are the verification
path; OpenGL and Apple Metal execution are not available on this machine.
