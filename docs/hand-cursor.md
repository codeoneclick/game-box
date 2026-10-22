# Animated hand cursor

The editor uses `gbResources/hand_cursor.bbmodel`, authored with the native
Blockbench 5.1.6 model/animation APIs. The model has 59 cuboids, 19 articulated
groups, a 32×32 nearest-filtered texture, and a 2.8-second looping `idle` animation
with eight key poses. There is no platform under the hand.

The current animation opens all fingers while retaining the original pointing
bind pose, mesh, rig and texture. The hotspot lies under the centre of the palm.
The cursor has a fixed authored world size of about 2 metres (scale 1).
Zoom changes its size on screen. A vertical ray through the animated palm XZ
finds the highest visible solid mesh surface; the palm centre maintains a minimum
0.5-metre clearance above that hit. The ocean wave sample is the fallback. Vertical
breathing is compensated at the palm, retaining wrist/finger movement and the
small horizontal stroking motion. A bounded eight-point / 0.025-second cursor
trail drives critically damped horizontal following, with roughly 0.04?0.06 s
response. Vertical following is softer. A swept vertical curtain tests the entire
upcoming segment against projected render triangles, including thin obstacles
between waypoints. The hand anticipates ascent and pauses horizontal progress
at an obstacle until it clears its top, then descends smoothly after passing it.
The palm may be higher than 0.5 m during that motion. A moving/edited object
entering the palm's existing position requires immediate clearance correction.
The palm joint is resolved by name because
FBX import also includes an armature ancestor.

The hand presents its dorsal side to the camera, palm toward the ground,
fingers away/up and slightly left, and wrist toward the bottom of the screen,
following the supplied Black & White 2 references. It has a modest 20-degree
tilt above the ground plane and turns with camera yaw. Native Idle combines
gentle wrist pitch, rise/fall and a small forward/back stroking movement;
fingers remain extended throughout. The hand remains visible while RMB is
held; RMB still cancels placement/painting. It is hidden over UI, during orbit,
outside the viewport, and when there is no surface under the pointer.
Windows/macOS cursor visibility changes are balanced and restored on window
exit or teardown.

Surface queries use the render meshes and absolute scene transforms, including
buildings, trees, stairs, residents, rocks and boats. Preview meshes, grass blades
and sky are excluded. A cached triangle BVH prunes each vertical query; mesh
identity/VBO/IBO versions invalidate it after edits, Load or LOD replacement.
Bounds cover terrain geomorph endpoints and tree wind displacement. Intersected
tree triangles evaluate the same wind function/time as their shader. Nearby
skeletal meshes refit using the current rendered bone palette, including resident
grounding. The cursor system runs at end phase order 6, after all feed updates
and before rendering at order 7. Expired weak mesh entries are periodically retired.

The hand uses the existing resident skinning shader and lighting uniform layout;
it is not a resident, navigation agent, shadow caster, or serialized level object.
It adds 708 triangles and one world-space draw when visible.

The reserved hand outline/object ID is 9000000. SSAO skips it as receiver and
occluder. Ocean compose bypasses AO using the full-resolution ID after AO
upsampling, avoiding dark fringes. Sun-shadow resolve and screen-space contact
shadows also exclude the hand; the shadow system does not create hand caster
proxies. Matching GLSL/Metal shader branches keep its regular material lighting.


## Re-export

Start a separate Blockbench instance with an isolated profile and local debug
port 9238. The normal `ELECTRON_RUN_AS_NODE=1` inherited by some tool shells must
be removed for the GUI instance. Keep user projects in their existing instance.

Run `gbScripts/invoke-hand-blockbench.mjs` with Node 22+ (the installed
Blockbench executable also supplies Node when `ELECTRON_RUN_AS_NODE=1`). It
executes `animate-hand-cursor.js` inside Blockbench against the existing model
and writes only its updated animation plus native sampled animation matrices.
Non-animation source fields remain identical. `author-hand-cursor.js` is the
original bootstrap authoring script; do not use it to revise an existing model.

Run Blender with:

```powershell
& 'C:/Program Files/Blender Foundation/Blender 5.2/blender.exe' --background --python-exit-code 1 --python gbScripts/export-hand-cursor.py -- . --animation-only
```

Animation-only export preserves the base FBX bytes and updates the Idle FBX,
plus a preview/report under `gbDemo/output/hand-cursor`.
It compares all 85 sampled poses against the native
Blockbench matrices. The normal demo build stages canonical `gbBundle` assets.

## Checks

```powershell
node gbScripts/check-hand-cursor.mjs
./gbScripts/check-hand-cursor-math.ps1
./gbScripts/check-terrain-lod-demo.ps1 -HandCursor
./gbScripts/check-hand-cursor-native.ps1
```

The native cursor check operates on its own game window and restores the prior
pointer position and foreground window. `-HandCursor` uses synthetic mouse input,
checks nonstatic Idle frames, minimum palm clearance, constant world scale, live
terrain/building/tree/resident intersections and shrinking screen size on zoom-out; captures
terrain/water/UI plus two zoom positions, and checks Vulkan validation output.

Windows Release and Debug builds and Windows Vulkan runtime are the available
verification targets. OpenGL, Metal, and native macOS cursor behavior require
separate platform verification.

Current fixed-size / surface-following revision (2026-10-18):

- CPU checks cover topmost triangles, gaps, transformed rays, animated refits,
  terrain morphs, moving tree crowns and spatial pruning.
- Release build and Vulkan validation passed all six captures in
  `demo-c0416aad4323433db822eedf5f94c45d`. Live probes matched terrain, building,
  tree and posed resident surfaces. Every logged palm clearance was 0.5 metres
  at scale 1.
- Debug build passed with the existing isolated linker/PDB override; the staged
  executable hash matches that output. Debug Vulkan smoke (validation disabled)
  passed all six captures in `demo-92a6f47fe0c34568b937fbcc787a97d0`, including
  constant world scale, 0.5 m clearance, RMB visibility, live mesh probes and
  a smaller projected hand after zoom-out.
- Native Windows test confirmed the hand remains visible during RMB, the system
  cursor stays hidden over terrain/ocean, returns over UI/outside the viewport,
  and the owned process closes normally.
- Mesh and texture are unchanged: base FBX SHA256 remains
  `246331d7906b18333ab48107c31c7848044b1cb2b62c67afc48b44d9ba4cdc4f`.

Independent review found no remaining important issue. `git diff --check` passed.
OpenGL/Metal and macOS native cursor behavior remain unverified.

Spring following / lighting exclusion revision (2026-10-19):

- Motion checks pass at 30/60/144 Hz, covering cliff ascent/descent, rapid waypoint
  changes, bounded following, and a sub-millimetre step crossing a sharp edge.
- Swept BVH checks include thin geometry between endpoints and interpolation
  across a sloped triangle, alongside posed/morphed/wind-deformed surfaces.
- Windows Release and Debug builds passed. Debug used the existing private PDB
  override and its staged executable SHA256 matches the linker output.
- Release Vulkan validation and six close-camera captures passed in
  `demo-e8233ad12be24a5f9b009d8e6055f78d`; Debug smoke without validation passed
  six captures in `demo-ddc205add7864e2f84b8fb4751c24a2b`. These cover terrain,
  ocean, UI hiding, RMB visibility, scale 1 and minimum 0.5 m palm clearance.
- Native cursor/normal process close passed after the test helper explicitly
  activated its own window; Windows initially denied ordinary foreground transfer.
- Independent review found no remaining important issue. Mesh/texture hashes are
  unchanged. OpenGL and Metal/macOS execution remain unverified.

Follow-up tuning: horizontal and vertical spring frequencies doubled (48/32),
waypoint retention halved to 0.05 s. A CPU response check confirms the new pose
at 0.1 s matches the former pose at 0.2 s on both axes. Obstacle clearance remains
covered at 30/60/144 Hz.

2026-10-20 tuning: halved following delay again. Spring frequencies are now
96/64 and waypoint retention is 0.025 s. Response checks compare the new
0.05-second pose with the previous 0.1-second pose; clearance remains 0.5 m.
