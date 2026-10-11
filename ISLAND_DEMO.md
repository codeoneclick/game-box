# Island and cartoon ocean demo

Open `C:\Hobby\game-box` in VS Code and press **F5** with `Run gbDemo (Debug x64)`.
The pre-launch task builds dependencies and stages canonical assets from
`gbBundle` and `gbResources`. Microsoft C/C++ (`ms-vscode.cpptools`) provides the debugger.
Stop the previous session with **Shift+F5** before rebuilding.

Hold **LMB** and drag to orbit; elevation is clamped to 10-80 degrees.
Hold **RMB** to grab terrain or sea under the cursor and pan with that point
anchored under the cursor. The grab plane stays fixed until release. RMB takes
priority when both buttons are held. Release, capture loss or focus loss ends
the drag. Sea dragging uses the mean water level to avoid wave-induced jumps.
Wheel up zooms in; wheel down zooms out around the current look-at point.
The orbit radius is limited to 35-700 m; fractional wheel input is supported.
Initial yaw/elevation/radius are 45/55 degrees and 245 m.
The island camera clips at 0.5-1200 m; resident close inspection uses 0.1 m
for the near plane. The near plane stays small when zooming toward terrain.

## Male island resident

F5 spawns 200 residents using the final `gbResources/woodsman.bbmodel` mesh.
Its 130 authored cubes, texture and UVs are preserved: 1040 source corners,
780 quad faces / 1560 imported triangles. The older concept mesh's 500-triangle
budget does not apply to this final mesh. The new armature uses 18 canonical
named bones (19 including the imported parent), with a height of 1.9 m.
The source Blockbench side labels are remapped to anatomical/Mixamo sides;
the +Z-facing character's left bones are on +X. This prevents a 180-degree
limb-motion inversion while the visible face keeps its correct orientation.
The source arms-down neutral pose is preserved; the embedded Blockbench walk
is removed. Runtime idle/walk still come from the separate Mixamo files.
There are 25 residents on each of the eight dry terraces at Y=0.5, 2, 4, 6,
8, 10, 12 and 14 m. Each alternates idle breathing/weight shifts and in-place
walk animation while moving at 1.2 m/s. Stops last 2-5 seconds. Each terrace
has a separate shared navigation map built from its actual rendered horizontal
triangles, with a 1.2 m sampled footprint/margin. A resident keeps its assigned
map and height, so routes cannot enter water or cross into another terrace.
Four-neighbor grid routes validate every segment against that terrace's mesh.
Characters face actual movement, including route corners, and keep their last
heading while idle. Deterministic shuffled spawns have at least 2.4 m initial
spacing; independent seeds and idle timers vary walks. Movement does not include
collision avoidance between residents.

Runtime clips come from `gbResources/character_idle.fbx` and
`gbResources/character_walking.fbx`. The engine reads their 65-bone source rig
and bakes target-sized clips during loading. Humanoid mapping, reference-pose
calibration and target limb lengths support different rig proportions and
T/A poses. Horizontal root motion is removed; navigation owns world movement.
Rigid foot skin markers correct height, including boot edges that become
lowest when pitched; this does not provide foot-locking IK. Derived clips are
cached by mesh, source clip, range and profile version; source data is unchanged.

Woodsman recoloring is temporarily disabled: **C** and `GB_RESIDENT_COLOR`
leave the original texture unchanged. Missing/corrupt FBX or short/mismatched clips are checked before
the legacy asserting loader; a failure skips the resident and logs its cause.

Source, regeneration and FBX unit conventions are described in
`gbResources/woodsman/README.md`. The previous concept asset and its generation
checks remain as an archive under `gbResources/island_resident`.

```powershell
./gbScripts/check-woodsman.ps1
./gbScripts/check-mixamo-retarget.ps1
./gbScripts/check-island-resident-runtime.ps1 -Pose idle
./gbScripts/check-island-resident-runtime.ps1 -Pose walk -Time 0.2
./gbScripts/check-island-resident-runtime.ps1 -ColorCycle
./gbScripts/check-island-resident-runtime.ps1 -Pose walk -Time 0.2 -ColorCycle
./gbScripts/check-island-resident-runtime.ps1 -Orbit
./gbScripts/check-island-resident-runtime.ps1 -Pose auto -Duration 80
```

The runtime checks require the Vulkan screenshot layer and write PPM captures
and logs under `gbDemo/output/island-check/resident-*`. Color checks compare
the fixed character image before and after real C key messages, excluding the FPS HUD.
For a close manual view, set `$env:GB_RESIDENT_INSPECT='1'` before launching
the executable from `gbDemo`. `GB_RESIDENT_POSE=idle|walk` freezes navigation;
`GB_RESIDENT_TIME` fixes clip time; `GB_RESIDENT_COLOR` is temporarily ignored;
`GB_RESIDENT_DEBUG=1` logs safe positions and CPU skin bounds. Clear these
environment variables to return to normal autonomous F5 behavior.

The previous concept asset's Windows Vulkan build, close idle/walk captures, C debounce/shared coloring,
corrupt-FBX fallback and camera orbit/pan/wheel flows were exercised. CPU smoke
checks include 3,000 simulated seconds/144 stops, invalid/stalled dt, bounded
facing actual movement across multiple waypoints, terrain seeds 1337/42/2026, narrow/disconnected
areas and routes around an impassable interior. This is a focused smoke harness,
not an engine unit-test suite. Metal PT4B vertex descriptor/shader/source
membership are supplied, but Metal/Xcode and OpenGL execution remain UNKNOWN.
An 80 s Vulkan run observed three walks/two idle stops, with safe positions.
With the concept mesh and Mixamo clips, a fresh 65 s Vulkan run observed two
walks and one idle stop, safe positions and zero movement-facing mismatches.
CPU retarget checks exercise 31 phases per clip, immutable source poses, target
lengths/scales, T/A-pose calibration, in-place hips, sole grounding and invalid
rig/pose rejection. Runtime interpolation's observed lowest vertex was -1.1 mm
(the smoke threshold is -3 mm); grounding does not claim exact foot locking.
A temporary action callback exercised resident detach, destruction (old weak
reference expired) and fresh construction using the existing navigation system;
the fresh listener completed exactly one C palette cycle. The probe was removed
before the final build. Full transition recreation remains UNKNOWN.

For the final Woodsman asset, the Windows debug-client x64 build, exact source
and UV-corner FBX check, both Mixamo retarget probes, Vulkan walk at 0.25 s,
idle at 0.5 s with C input, and autonomous 200-resident run were exercised.
The source walk is absent and C leaves the fixed character image unchanged.
The opposite-facing source/target rig regression verifies that body alignment
yaws around Y without flipping the character vertically. The full skin probe
checks every vertex at 31 phases per clip; runtime skin bounds allow up to
3 mm of interpolation/packed-weight error. Elbow/knee bend signs are compared
to the source animation relative to each body's facing direction.
Existing command-pool validation
messages at Vulkan shutdown remain. OpenGL/Metal runtime remains UNKNOWN.
The final pose upload also grounds shared rigid-foot contact points after
interpolation and idle/walk blending; the mixer's source pose and navigation
height remain unchanged. This avoids boot penetration during clip transitions.

Crowd CPU checks simulate all 200 residents for 60 seconds and require every
resident to move, with all steps remaining safe on their assigned terrace.
The default seed produced 635 walks. Shared FBX assets and derived clips are
loaded once; each resident owns its skeleton pose, mixer, material, route and
timer. One ECS system updates the crowd with eight navigation maps. Runtime
debug reports verify 25 residents per terrace, fixed heights and safe positions;
the capture harness also rejects population or terrace changes.

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


## Performance HUD

The top-right HUD uses the engine's existing `gb::label` UI entities and
`shader_ss_compose_ui`. It shows FPS averaged over at least half a second,
submitted triangles (`Polygons/frame`), and draw calls for the last completed
frame. Text refreshes four times per second. Counts include every geometry
pass, UI, fullscreen effects and output; they are not unique scene polygons
or pixels surviving depth testing. Draw counters reset at the pipeline frame
boundary and are recorded beside actual OpenGL/Vulkan/Metal draw submission.
`GB_HUD_DEBUG=1` also prints the HUD values to stdout for diagnostics.

```powershell
./gbScripts/check-render-statistics.ps1
```

Windows debug-client x64 build, Vulkan capture and resident crowd runtime
were verified. OpenGL and Metal compilation/runtime remain unverified.

## Resident crowd performance

The 200-resident scene uses conservative cached terrain coverage: cells fully
inside a terrace avoid triangle searches, while boundary cells retain the exact
footprint and segment checks. Animation accesses bones by ID, composes affine
TRS matrices directly, and caches inverse bind matrices until rebinding. Rendering
reads uniform maps by reference; the original copying getter remains available.

Measured on the same machine with Windows debug-client x64, Vulkan validation
enabled, 1280x720, 200 residents and three 120-frame samples: feeder CPU time
fell from 76.7 ms to 36.9 ms (about 2.1x). Terrace navigation fell from roughly
24 ms to 3 ms and animation from 25 ms to 8 ms. Rendering remains the largest
cost at roughly 21 ms. These figures are machine-specific, not an FPS guarantee.
The final steady-clock frame average was 36.9 ms (about 27 FPS); the original
76.3 ms game-loop average rounded each frame to integer milliseconds.

Repeat the benchmark without screenshot capture:

```powershell
./gbScripts/check-island-performance.ps1 -Residents 200 -Samples 3 -Label crowd
./gbScripts/check-island-performance.ps1 -Residents 0 -Samples 3 -Label empty
```

Reports and `summary.json` are written under `gbDemo/output/island-check/perf-*`.
`GB_PROFILE_SYSTEMS=1` enables per-system/phase CPU timings after 60 warmup
frames, averaged over 120 frames. `GB_RESIDENT_COUNT` overrides the default 200
for diagnostics (range 0 through 200). Profiling is disabled during normal F5
execution. Run one benchmark at a time without background compilation.
The shared animation/render changes are verified on Windows Vulkan; OpenGL
and Metal remain unverified.

## Screen-space ambient occlusion

SSAO is enabled by default in the F5 island scene. The existing `ss.ao`
material and GLSL/Metal shaders now use valid G-buffer world positions and
normals, a camera-projected 3 m radius, an angular bias of 0.06, and 32 samples.
Island normals are decoded from [0,1]; the legacy raw-normal mode remains
available through the shared uniforms wrapper. View/projection uniforms follow
camera changes every frame, including orbit, pan and zoom.

The `ss.ao.blur` pass uses a 5x5 depth/normal bilateral filter. The resulting
visibility modulates opaque scene color before water and premultiplied foam
composition, keeping the ocean appearance intact. Water reflection/refraction
still sample the original opaque scene buffer. This is screen-space contact
shading: offscreen or hidden geometry cannot contribute. GL 2 keeps the raw AO
mask because its shader path has no `textureSize`; that backend is unverified.

Diagnostic environment variables can be set in `.vscode/launch.json`:

- `GB_SSAO=0`: neutral AO for comparison (the passes remain allocated).
- `GB_SSAO_DEBUG=1`: show the filtered AO mask; white means unoccluded.
- Existing `GB_OCEAN_FIELD_VIEW` takes precedence over the AO mask view.

Build and repeat the fixed-time Vulkan capture comparison:

```powershell
./gbScripts/build-gbDemo.ps1 -Configuration debug-client -Platform x64
./gbScripts/check-ssao-runtime.ps1
```

The image check requires Python with Pillow. Captures are saved under
`gbDemo/output/island-check/ssao-on`, `ssao-off` and `ssao-mask`. It checks visible
terrace contact darkening, a white background, unchanged open-water pixels,
no AO brightening, and identical fixed-scene frames 80 and 160.

Windows debug-client x64 compilation and Vulkan rendering were verified at the
default, ocean inspection and resident inspection camera distances, including
orbit and mouse-wheel zoom changes. Metal source and all three Xcode resource targets were updated;
native Metal and OpenGL builds/runtime remain UNKNOWN. The existing Vulkan
command-pool destruction validation messages still occur during shutdown.
