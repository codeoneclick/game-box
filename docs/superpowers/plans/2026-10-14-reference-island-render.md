# Reference island render

Priority: landscape, followed by coherent sunset lighting and water. Existing
terrain editing, flat walking/building surfaces and animated interactions must
remain functional. Preserve smooth contours unless the user chooses block geometry.

1. Terrain: earthy grass tops, warm sand, grass caps over soil and mineral bands;
   independently randomized large pixels without tile frames. Update canonical
   atlas authoring and GLSL/Metal material selection. Slightly offset generated
   upper terraces to vary terrace widths without changing the grid data format.
2. Lighting/sky: warm soft illumination with cool ambient fill. Add an inexpensive
   directional procedural sky driven by the existing sun, including a sun disc,
   warm horizon and cloud masses. Share that sky model with water's reflection
   fallback; preserve the painted cubemap as a small background contribution.
3. Water: blue depth, turquoise shallows, gentler facet contrast and a warm broad
   specular trail plus small bright glints. Keep wave physics, foam, buoyancy,
   refraction and caustic calculations intact.
4. Verify terrain CPU invariants, lighting CPU checks, Debug/Release builds and
   Vulkan screenshots at day and sunset, plus editing. Inspect screenshots and
   document remaining gaps: stairs, paths, rock scatter and exact cast shadows
   are scene/geometry features beyond shader/material matching.

No extra fullscreen passes, raymarch steps, mesh subdivisions or asset dependencies
are needed for this pass. Apple Metal source parity does not constitute an Apple
runtime check.

Status: implemented and checked on Windows Vulkan. User confirmed smooth
contours and subsequently clarified that grass requires a stepped silhouette.
The grass path now uses masked four-vertex cards, explicit side bits and updated
editor cache partitioning. See `docs/reference-island-render.md` for visual
breakdown, verification and remaining scene-composition/shadow differences.
