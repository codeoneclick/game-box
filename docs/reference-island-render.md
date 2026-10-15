# Reference breakdown and implementation

The user's sunset island image is an art-direction reference. It does not provide
mesh dimensions, material maps, light intensities or camera calibration; the
following is visual interpretation rather than a reconstruction of its renderer.

| Element | Visible characteristics | Changes in this pass |
| --- | --- | --- |
| Terrace tops | Flat, green, irregular widths, large colour pixels | Earthy green tops at every inland height, independent world-pixel palette choices, small safe plateau offsets |
| Terrace walls | Thin turf cap, brown soil, grey mineral bands | Pixel cap and interrupted, staggered stone exposures; actual terrace height controls vertical mapping |
| Coast | Warm pale sand, turquoise shelf, thin foam | Sand/shore mapping retained; warmer foam and blue-to-turquoise depth absorption |
| Grass | Blocky stepped silhouette with uneven lean | Mirrored 4x8 cutouts, low 0.28-0.55m clumps instead of a uniform tall lawn, height-scaled wind and character push; all editor palettes preserved |
| Lighting | Low warm sun, cooler shadow fill | Desktop starts with the sunset preset at 17.4h; Soft/Mobile remain selectable; existing time slider drives the same sun |
| Water | Visible shallow bottom, modest facets, gold highlights | Softer facet contrast, lower absorption, blue depth, broad warm reflection lobe plus small bright glints |
| Sky/horizon | Peach sunset, sun disc, cloud masses, atmospheric fade | Procedural sun-driven sky shared with reflection fallback and scattering; sky is not fogged a second time |
| Composition | Island viewed towards the low sun | Default orbit yaw 110 degrees, pitch 20 degrees, radius 180, centred on the island; existing orbit/pan/zoom controls remain |
| Contact shadows | Directional depth cues around trees, ledges and houses | Eight screen-space sun samples from existing world-position/ID buffers; grass does not cast a solid curtain; blue shadow fill and softer outlines |
| Geometric details | Small stone groups on terrace tops and beach | Deterministic block clusters in existing vegetation batches, requiring a completely flat support neighborhood and excluding building footprints |

The explicit user choice is to retain smooth terrain contours. Pixel art is
expressed through the materials and grass silhouette, not a grid stair-step
around terrace perimeters. Terrain editing and building support continue to use
the same flat logical heights. The initial terrain and edited chunks use the
same material shader. No user level is reset, and no buildings are automatically
placed in a user's saved scene.

The newly generated demo starts with a Forester and Woodcutter on terraces 4 and
8, placed through the normal full-footprint validation/command API. This clears
only their occupied vegetation, is undoable and marks the new demo modified.
`GB_REFERENCE_COMPOSITION=0` retains an empty starting layout. Loading a user
level replaces the state normally and does not inject these buildings.

The atlas remains 256x480 RGBA with its existing manual filtered representations.
Water's wave physics, foam interaction field, buoyancy, refraction and caustic
algorithms are retained. No new fullscreen passes are introduced. Contact shadows
add up to eight short screen-space ray samples only on opaque visible geometry;
`GB_SUN_CONTACT_SHADOWS=0` disables them for comparison. The uniform ABI is unchanged
(`field_debug.w` was reserved; `.z` remains the implementation stage). The
previous painted sky remains a negligible contribution; the
procedural model supplies the visible atmosphere and water reflection fallback.

## Verification

- CPU island generation across supported resolutions/seeds, mesh limits and
  winding; four-vertex grass cards and shared wind-root metadata.
- CPU terrain editor rebuild, undo/save-load rendering roundtrip, tree/grass data
  and building-placement checks through `check-terrain-editor.ps1`.
- CPU ocean/buoyancy and desktop/mobile lighting settings checks.
- Standard Windows debug-client and release-client builds.
- Native Vulkan screenshots: day, sunset, night, close grass, live character
  interaction, terrain sculpt and wheel zoom, with clean process shutdown and
  no reported validation/compiler errors.
- `git diff --check`.

Metal counterparts are updated, but Apple Metal and OpenGL execution remain
unverified on this Windows machine.

## Remaining differences from the reference

Matching the image exactly also requires paths, stairs connecting terraces,
flowers, and further work on the relative scale of buildings and vegetation.
Contact shadows use only visible screen geometry, have finite reach and cannot
cast from offscreen objects; this is not a directional shadow-map pass. Existing
blob shadows remain. Rendered samples must therefore be judged as an art-direction
approximation rather than an identical reconstruction of the concept.

Rock decorations are derived from terrain and building data, not saved as a new
editable object type. Terrain edits invalidate their full support neighborhood,
including across render-group boundaries. Building changes invalidate 12m around
their footprint. Support checks inspect every cell, so small holes cannot leave
a cluster hovering. Rock density scales with grid spacing to bound work and
16-bit mesh capacity. CPU checks cover determinism, normals, capacity, submerged
terrain, footprint exclusion and edit/restore across group boundaries.

These rocks are decorative; they are not added to the residents' navigation
obstacles or the building-support surface. They disappear beneath a newly placed
building and return on removal when the underlying terrain is still suitable.

## Second-pass verification record

The current seeded field contains 19,175 low grass cards in two initial batches
(135,400 cards before patch thinning). Windows Debug and Release builds pass.
CPU island/editor/lighting checks and the shader/pass ABI checks pass. The rock
boundary regression was first reproduced (missing invalidation of group 10),
then passed after expanding the dependency halo; restoring the terrain restores
the original rock mesh count.

Native Vulkan day/sunset/night and live grass interaction views completed with
no reported validation/compiler errors and clean shutdown. Release screenshots
and HUD logs comparing contact shadows on/off are in
`gbDemo/output/island-check/reference-shadow-performance-{on,off}`. The last
samples were about 56-59 FPS with shadows and 57-59 without, at the same camera
and 1,080,271 submitted triangles / 407 draws. This is a focused smoke comparison,
not a controlled benchmark or a guarantee for other views/hardware.

The user explicitly confirmed eight clearly visible starting terraces with smooth
contours. Existing level files are not rewritten.

## Contact-shadow correction (2026-10-15)

Close camera views reproduced detached and repeated character silhouettes in the
previous screen-space sun pass. Eight nonuniform samples over 27.55m combined
with a 1.8m depth slab accepted unrelated foreground and left gaps between hits.
This was screen-space tracing, not a shadow map.

The pass now traces only 1.5m with 32 uniform overlapping samples, 0.025m normal
bias, a 0.09m depth limit and a 0.12m world-space proximity check. Occluders must
sit above the receiver tangent plane. Cutout grass is excluded as both receiver
and caster. Contact fades toward the range limit and has reduced opacity so it
complements existing blob shadows. GLSL and Metal use the same algorithm.
Long sun-cast silhouettes are intentionally no longer synthesized from sparse
screen data; reliable long directional shadows need a dedicated shadow-map pass.

Verification of the correction: Windows debug-client and release-client builds
passed; shader/pass asset checks and `git diff --check` passed. Native Vulkan
close-up zoom reproductions before/after, daytime zoom and Release on/off runs
exited cleanly with no reported shader/validation errors. Screenshots/logs are in
`gbDemo/output/island-check/shadow-artifact-{before,after}`,
`shadow-fixed-release` and `shadow-fixed-performance-{on,off}`. The sampled
Release overview rates overlap (about 53-57 FPS); this is a smoke comparison.
Metal was updated but not compiled or exercised; OpenGL remains unverified.

## Directional sun maps (2026-10-15)

The later user request explicitly selected full directional sun shadows.
[Directional sun shadows](directional-sun-shadows.md) supersedes the earlier
screen-space-shadow limitations and describes the actual two-map implementation,
validation, remaining limits and measured additional rendering cost.
