# Final Woodsman resident

Source: `../woodsman.bbmodel`. The embedded Blockbench walk is removed;
all other source fields, including cube geometry, neutral pose, UVs and the
embedded texture, are preserved. No decimation, welding or silhouette edits.
130 cubes export to 1040 source vertices, 780 quads and 1560 engine triangles.
The previous 500-triangle concept budget is superseded by the final mesh.

Deliverables:

- `../woodsman.fbx`: rigged, weighted mesh without animation stacks.
- `woodsman.blend`: editable rig and mesh, with the original PNG packed.
- `../../gbBundle/Meshes/woodsman.fbx`: identical runtime FBX.
- `../../gbBundle/Textures/woodsman.png`: original embedded PNG bytes.
- `export.json`: counts, canonical bone names and source/texture hashes.
- `expected_vertices.txt`: all source corners after engine coordinate conversion.
- `expected_uv_corners.txt`: all 3120 face corners with their original UVs.

The source arms-down pose is retained as the bind pose. Game Box's humanoid
retargeter calibrates it against the separate Mixamo source reference pose;
it does not require changing the mesh into a T-pose. The existing
`character_idle.fbx` and `character_walking.fbx` are used in the island demo.
Uploading this mesh to the Mixamo service is not part of the verification.

## Skeleton and skin

18 bones use the requested naming:

```text
root
  hips
    spine
      chest
        neck
          head
        upper_arm_l -> lower_arm_l -> hand_l
        upper_arm_r -> lower_arm_r -> hand_r
    upper_leg_l -> lower_leg_l -> foot_l
    upper_leg_r -> lower_leg_r -> foot_r
```

Game Box includes the `WoodsmanRig` armature parent, so it reports 19 nodes.
The mesh uses normalized one/two-bone weights. Adjacent cubes share weight
fields across wrists, elbows, knees and ankles; the torso accessories share
the hips/spine/chest field. Geometry and UVs remain unchanged.
Foot calibration preserves the target neutral orientation: differing
ankle-to-toe proportions do not add a permanent boot pitch. Source foot
rotation deltas still transfer normally, including the idle's turned-out stance.
Ground contact samples the entire rigid foot skin because a tilted boot edge
above the neutral sole can become the lowest point. This is height correction,
not foot-locking IK. Horizontal clip motion is removed; terrace navigation
continues to move and turn all 200 residents.
The resident renderer also corrects contact after pose interpolation and
idle/walk blending, using shared deduplicated rigid-foot points. It offsets
the uploaded bone palette vertically without changing source animation data,
the mixer's pose, joint bend directions or navigation's terrace height.

## Coordinates and export

Height is 1.9 m, feet at local Y=0; Game Box uses Y-up and character front +Z.
Blockbench's 47-unit height gives a scale of `1.9 / 47` m per unit.
Source `(x,y,z)` becomes engine `(-x,y,-z)` through a proper 180-degree yaw.
Blender coordinates are `(-x,z,y)` with Z-up; FBX exports Y-up, forward -Z,
meters, no leaf bones and no baked animations. The source BB group side labels
are opposite to anatomical/Mixamo names. Export remaps source `right_*` to
canonical `*_l` and source `left_*` to `*_r`, including their vertex weights.
For the +Z-facing runtime mesh, left bones occupy +X and right bones -X.
This keeps the skeleton's inferred forward direction aligned with the face;
otherwise limb motion yaws 180 degrees and bends knees/elbows backward.
Cube UV mapping follows Blockbench's
[UVToLocal implementation](https://github.com/JannisX11/blockbench/blob/master/js/outliner/types/cube.js).

Run from the repository root with Blender 5.2 installed:

```powershell
& 'C:/Program Files/Blender Foundation/Blender 5.2/blender.exe' --background --python-exit-code 1 --python gbScripts/export-woodsman.py -- --root .
./gbScripts/check-woodsman.ps1
./gbScripts/check-mixamo-retarget.ps1
./gbScripts/build-gbDemo.ps1 -Configuration debug-client -Platform x64
./gbScripts/check-island-resident-runtime.ps1 -Pose walk -Time 0.25
./gbScripts/check-island-resident-runtime.ps1 -Pose idle -ColorCycle
./gbScripts/check-island-resident-runtime.ps1 -Pose auto -Duration 20
```

The CPU FBX probe compares all source and UV corners, hierarchy, skin weights, bounds,
animation absence and texture bytes. The retarget probe samples both clips,
checks Y-up orientation, elbow/knee bend signs relative to the body's facing
direction, and all skinned vertices against the ground plane.
Vulkan runtime probes inspect the actual interpolated pose and crowd state.
Native OpenGL/Metal execution and Mixamo website upload remain unverified.

## Temporary recolor disable

The material uses a black `woodsman_no_recolor.png` mask and white tint.
`clothing_customization_enabled=false` in `island_resident_demo.cpp` disables
the C listener and `GB_RESIDENT_COLOR` override. A future re-enable requires
an authored mask for this final texture as well as enabling the input flag;
the original PNG should remain intact.
