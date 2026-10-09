# Island and cartoon ocean demo

Open `C:\Hobby\game-box` in VS Code and press **F5** with `Run gbDemo (Debug x64)`.
The pre-launch task builds dependencies and stages canonical assets from
`gbBundle`. Microsoft C/C++ (`ms-vscode.cpptools`) provides the debugger.
Stop the previous session with **Shift+F5** before rebuilding.

Hold **LMB** and drag to orbit; elevation is clamped to 10-80 degrees.
Hold **RMB** to grab terrain or sea under the cursor and pan with that point
anchored under the cursor. The grab plane stays fixed until release. RMB takes
priority when both buttons are held. Release, capture loss or focus loss ends
the drag. Sea dragging uses the mean water level to avoid wave-induced jumps.
Wheel up zooms in; wheel down zooms out around the current look-at point.
The orbit radius is limited to 35-700 m; fractional wheel input is supported.
Initial yaw/elevation/radius are 45/55 degrees and 245 m.

## Male island resident

F5 also spawns one original low-poly male resident on the foreground dry shore.
He alternates idle breathing/weight shifts and in-place walk animation while
moving at 1.2 m/s. Stops last 2-5 seconds. Navigation is built from the rendered
lowest dry terrace at Y=0.5 m, with a 1.2 m sampled footprint/margin. He does
not enter water or climb the higher 2 m terrace walls. Routes use four-neighbor
grid paths and validate every movement segment against the actual shore mesh.

Press **C** to cycle blue, green, red and ochre. Shirt and trousers change
together; boots, skin and hair keep their own colors. Holding C does not cycle
repeatedly. Missing/corrupt FBX or short/mismatched clips are checked before
the legacy asserting loader; a failure skips the resident and logs its cause.

Source, regeneration and FBX unit conventions are described in
`gbResources/island_resident/README.md`. The editable `.blend`, model, idle/walk
FBX files, palette, mask and material/shader configurations are checked in.

```powershell
./gbScripts/check-island-resident.ps1
./gbScripts/check-island-resident-runtime.ps1 -Pose idle
./gbScripts/check-island-resident-runtime.ps1 -Pose walk -Time 0.2
./gbScripts/check-island-resident-runtime.ps1 -ColorCycle
./gbScripts/check-island-resident-runtime.ps1 -Pose walk -Time 0.2 -ColorCycle
./gbScripts/check-island-resident-runtime.ps1 -Orbit
./gbScripts/check-island-resident-runtime.ps1 -Pose auto -Duration 80
```

The runtime checks require the Vulkan screenshot layer and write PPM captures
and logs under `gbDemo/output/island-check/resident-*`. Color checks compare
shirt/trouser pixels and unchanged boot/skin pixels after real C key messages.
For a close manual view, set `$env:GB_RESIDENT_INSPECT='1'` before launching
the executable from `gbDemo`. `GB_RESIDENT_POSE=idle|walk` freezes navigation;
`GB_RESIDENT_TIME` fixes clip time; `GB_RESIDENT_COLOR=0..3` chooses a palette
entry; `GB_RESIDENT_DEBUG=1` logs safe positions and CPU skin bounds. Clear these
environment variables to return to normal autonomous F5 behavior.

Windows Vulkan build, close idle/walk captures, C debounce/shared coloring,
corrupt-FBX fallback and camera orbit/pan/wheel flows were exercised. CPU smoke
checks include 3,000 simulated seconds/144 stops, invalid/stalled dt, bounded
turning across multiple waypoints, terrain seeds 1337/42/2026, narrow/disconnected
areas and routes around an impassable interior. This is a focused smoke harness,
not an engine unit-test suite. Metal PT4B vertex descriptor/shader/source
membership are supplied, but Metal/Xcode and OpenGL execution remain UNKNOWN.
An 80 s Vulkan run observed three walks/two idle stops, with safe positions.
A temporary action callback exercised resident detach, destruction (old weak
reference expired) and fresh construction using the existing navigation system;
the fresh listener completed exactly one C palette cycle. The probe was removed
before the final build. Full transition recreation remains UNKNOWN.

## Build and checks

Run from the repository root in PowerShell:

```powershell
./gbScripts/check-island-terrain.ps1
./gbScripts/check-ocean.ps1
./gbScripts/build-gbDemo.ps1 -Configuration debug-client -Platform x64
./gbScripts/check-ocean-runtime.ps1 -CaptureName ocean
git diff --check
```

The runtime check requires the Vulkan SDK screenshot layer. To launch manually:

```powershell
Push-Location gbDemo
try { & ./output/debug/x64/gb_demo.exe } finally { Pop-Location }
```

## Terrain and water

The island has eight dry terraces at 0.5, 2, 4, 6, 8, 10, 12 and 14 m and eight submerged terraces at
-2..-16 m. The seabed is -18 m; mean water level is -0.3 m.
`terrain_data::layers` stores signed labels -8..7, with -9 for empty cells.
`terrain_generator` writes logical data; `terrain_mesh_builder` consumes that
data without depending on the generation formula. The default terrain mesh
has 60,480 vertices and 20,160 triangles.

| Setting (`terrain_data.h`) | Default |
|---|---:|
| Generation domain / dry island scale | 512 m / 256 m |
| Grid resolution | 512 cells per side |
| Above / below water levels | 8 / 8 |
| Terrace height / underwater width | 2 m / 6 m |
| Lowest dry terrace (`shore_height`) | 0.5 m |
| Seed | 1337 |
| Detailed water grid | 768 m, 384 cells per side (2 m spacing) |

The water grid is split into nine patches to respect 16-bit indices. A coarse
outer skirt extends to 4096 m. Four asymmetric macro/meso/micro wave components
share a domain-warped world-space height function between CPU buoyancy and GPU
displacement. Vertices keep their XZ coordinates; only height changes. Interior
vertices have deterministic jitter and independently hashed diagonals. Facet
normals and quantized lighting produce broad cyan highlights.

Before the opaque scene pass, shared-mesh render instances project terrain and
ordinary prop geometry into a fixed 320 m world-space mask. Terrain above mean
water level contributes. Objects accumulate above/below local wave-surface bits
with MAX blending; their intersection defines the waterline cross-section. Twelve
jump-flood passes resolve nearest occupied seeds, distance in meters and boundary
direction. Mask instances bypass the player frustum and follow current source
transforms, visibility and mesh replacements. Previously used meshes remain
owned until scene teardown to avoid destroying buffers while draws are in flight.
Repeated mesh replacements therefore retain their geometry for this scene's lifetime.

Dense inner foam, asymmetric variable-width mid foam and sparse domain-warped
outer patches sample this field. Wave crests and boundary direction influence
width. A shared water-mesh foam pass sits 0.04 m above water; scene position/depth
occludes it and composition blends its premultiplied color. No object colliders,
per-object foam geometry or active hardware depth sampling are used.

Separate opaque color/normal/world-position attachments still support absorption,
refraction, screen-space reflection, caustics and wetness. Reflection falls back
to a procedural sky. Buoyant crate, raft and boat use four CPU probes, damping
and substeps. The post and rocks exercise geometry-derived interaction contours.
The dry terrace remains at 0.5 m, above the configured maximum wave/foam height.

## Diagnostics

Set environment variables before launch, then remove them for normal use:

| Variable | Effect |
|---|---|
| `GB_ISLAND_LAYER_COLORS=1` | Distinct terrace tints |
| `GB_OCEAN_INSPECT=1` | Close view of floating props and shallow water |
| `GB_OCEAN_FIXED_TIME=3` | Fixed wave clock and diagnostic prop placement |
| `GB_OCEAN_DEBUG=1` | Foam visualization |
| `GB_OCEAN_DEBUG=2` | Disable refraction |
| `GB_OCEAN_DEBUG=3` | Disable reflection |
| `GB_OCEAN_DEBUG=4` | Water thickness visualization |
| `GB_OCEAN_DEBUG=5` | Encoded wave height and CPU probe output |
| `GB_OCEAN_DEBUG=6` | Linear view depth difference / outer foam distance |
| `GB_OCEAN_DEBUG=7` | Inner contact foam |
| `GB_OCEAN_DEBUG=8` | Animated outer foam and patches |
| `GB_OCEAN_DEBUG=9` | Flat triangle normals |
| `GB_OCEAN_DEBUG=10` | Fresnel reflection weight |
| `GB_OCEAN_DEBUG=11` | Refraction UV (red/green) |
| `GB_OCEAN_DEBUG=12` | Procedural caustic mask with depth attenuation |
| `GB_OCEAN_DEBUG=13/14` | Macro / meso wave height |
| `GB_OCEAN_DEBUG=15/16/17` | World mask / distance / gradient on water |
| `GB_OCEAN_DEBUG=18/19/20` | Mid foam / varying width / wave influence |
| `GB_OCEAN_FIELD_VIEW=15/16/17` | Full top-down mask / distance / gradient |

Water settings include inner/outer foam distances, noise scale/speed, sharpness,
outer and crest strengths, foam/light/shadow colors, facet lighting strength,
absorption and depth fade, refraction strength/depth, Fresnel exponent and
reflection strength, caustic scale/speed/strength/depth falloff. The legacy
`foam_width` field is the outer foam distance. Values are validated before use.
CPU/GLSL/Metal share 23 vec4 values followed by two matrices (496 bytes).
New foam settings specify mid minimum/maximum width, domain warp and height offset.

## Verification and limits

CPU compile/smoke checks cover static XZ, finite wave sampling, seams, indices,
winding, deterministic seeds, buoyancy at 30/60/144 Hz and stall substeps.
Windows debug-client x64 builds and multiframe Vulkan captures exercise the new
passes. The full mask is pixel-identical between two player cameras at fixed time.
An independent nearest-boundary oracle checked 523 distance samples with maximum
error 0.094 m. Fixed-time dry-terrace pixels remained unchanged between two wave
times. See the execution ledger for lifecycle, replacement and empty-map checks.

The engine currently overrides WS XML dimensions with window size: captures use
1280x720 rather than the proposed 1024 square. JFA steps use actual texture size.
World texels are 0.25 x 0.444 m; the fixed 0.15625 m seed-radius correction needs
revalidation if dimensions change. Fourteen RGBA32F field attachments plus mask
and foam RGBA8 add approximately 204 MiB of color attachments at this size,
excluding depth and other allocations. GPU pass timings have not been measured.
The waterline mask assumes closed projected geometry; open meshes and overlapping
disconnected volumes above/below the same XZ position need separate acceptance
checks. Subpixel posts are limited by field resolution.
The world interaction domain is bounded; ocean beyond it has no contact foam.

Metal source and Xcode membership match the new layout and passes, but native
Metal, dormant OpenGL and server verification remain UNKNOWN on Windows.
Normal shutdown retains the existing pending-command-pool validation issue
(VUID-vkDestroyCommandPool-commandPool-00041). Terrain remains nested/star-shaped;
continuous CPU heights can differ from interpolated mesh heights. Reflection and
refraction use screen-visible data; caustics and sky reflections are artistic.

## Animated contact foam (2026-10-09)

Foam sums hashed moving world-space kernels with compact support. Its isocontour
merges and splits as centers move; the inner band's width also changes, leaving
only a fine contact core. The interaction distance field remains geometric and
camera-independent. Compact support avoids discontinuities when the kernel cell
neighborhood changes. Default foam animation rate is 0.65.

At the user's request, wave spacing/speed/amplitude and optical appearance were
restored to their values before the appearance refinement. Metaball foam remains.
Original facet normals, Fresnel, lighting contrast, reflection strength and sun
direction are restored in GLSL and Metal. Native Apple verification is unavailable.

## Contact gap correction (2026-10-09)

The prior mask represented the widest projected prop footprint in a height slab,
so foam could sit outside the actual waterline of sloped rocks and hulls. Object
mask rendering now MAX-accumulates separate bits for faces above/below the shared
local wave height. Only pixels with both bits seed the distance field. Terrain
continues to supply fully occupied pixels. Seed, distance and full-map diagnostics
all decode the same occupancy in GLSL and Metal; no new passes are needed.

Metaballs previously shrank the contact band to 0.12 m, below field texel size.
Its minimum width is now 0.3 m, with a solid 0.2 m core and a 0.25 m raster margin
on the animated outer edge. The first antialiased contact pixel remains blended;
this is not a detached water gap. Outer foam keeps its merging/splitting motion.
Wave and optical settings remain at the restored values requested by the user.

Contact verification: Windows debug-client x64 build and multiframe Vulkan
captures passed at fixed times 3 and 7. The static-rock edge regression failed
before and passed after the correction (solid foam within the first two exposed
pixels, allowing edge antialiasing). Its animated ROI changed 2785 pixels.
The full mask is camera-independent; 527 distance samples have maximum error
0.094 m. CPU and buoyancy smoke checks and diff whitespace checks passed.
Known shutdown command-pool warnings remain; native Metal/OpenGL are unverified.

## Locally excited chaotic waves (2026-10-09)

The shared CPU/GLSL/Metal height model now combines each directional component
with a counter-travelling transverse component. Smooth advected phase noise
modulates the local instantaneous speed/direction. Height varies along each crest
through spatial energy envelopes, rather than one fixed amplitude per wave.
Local excitation packets rise smoothly and decay exponentially. Their staggered
4.5..8.5 s forcing periods differ between components; advected noise prevents
exactly repeated wave shapes. Periods do not vary spatially, avoiding progressively
compressed envelopes during long runs.

A smooth tanh bound keeps total deviation within 1.09 times summed amplitudes,
maintaining the existing dry-terrace clearance. The low-poly mesh, lighting,
reflection/refraction, sun and metaball foam settings are unchanged. Buoyancy,
receivers and the waterline interaction mask all sample the same height model.
This is an analytic forced-wave field, not a volumetric fluid solver.

A local-excitation smoke regression failed on the previous travelling-wave model
and passes now, including when travel speed is zero. Checks cover deterministic
sampling, temporal continuity, finite normals, height/terrace limits through 300 s,
and buoyancy at 30/60/144 Hz against a 120 Hz reference. All pass.

Final Windows debug-client x64 build, fixed-time and default/animated Vulkan
captures passed. The dry-terrace ROI is pixel-identical to the previous model;
contact remains attached at the newly exposed rock edge (the moving waterline
changes its screen position). A 32-frame preview is saved at
 gbDemo/output/island-check/chaotic-waves-motion.gif. Known shutdown command-pool
warnings remain. Native Metal/OpenGL verification is unavailable.

The coarser low-poly appearance refinement was reverted at the user's request.
The original 384-cell mesh and unquantized facet lighting are restored; locally
excited chaotic waves and all contact-foam corrections remain.
