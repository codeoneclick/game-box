# Terrace woodcutters

One existing resident on each of the eight initial terraces carries an axe,
finds a reachable tree on that terrace, walks to its trunk, faces it and loops
the supplied chopping animation. Trees are not removed. The remaining residents
retain their walking and stair behaviour.

Workers stop 1.5 m from the trunk centre to remain visible beneath the canopy.
The axe is 20% larger than the first version. Its exported bind pose has the head
pointing down beneath the horizontal handle. During chopping its rigid pose fits the handle through both
animated palm centres; while walking it follows the right hand. This leaves the
supplied body animation unchanged. Runtime debug output checks the two-hand grip.
The exporter centres the handle rather than the asymmetric complete mesh. Its
signed handle and blade axes define a full attachment frame: the cutting edge
faces the trunk in the resident's local space, removing the unconstrained roll
left by fitting only the two palm positions. Debug checks cover blade orientation.
The handle direction follows the fixed grip order (left palm at the butt, right
palm nearer the head), placing the head behind the character in the backswing
as requested in the marked reference. Wrist rotation cannot reverse this order.

At scene creation, terraces without a suitable tree receive an ordinary oak in
the editable level state. The startup code does not write `Main.level`; the
editor's normal Save includes those trees. If a target is deleted, the worker
searches for another reachable tree. Navigation rebuilds reset the job after
terrain edits or loading a level. Woodcutters stay on their own terrace.

## Assets and ownership

- Source model: `gbResources/axe.bbmodel`, unchanged.
- Source animation: `gbResources/character_wood_chop.fbx`, unchanged; the existing
  Mixamo retargeter maps its `chop` clip to the Woodsman rig.
- `gbScripts/export-woodcutter-axe.py` exports the model and its embedded atlas to
  `gbBundle/Meshes/woodcutter_axe.fbx` and `gbBundle/Textures/woodcutter_axe.png`.
  It corrects the source canvas metadata in memory and weights the axe to the
  resident's right hand on an identical bind rig. Attachment axes are derived
  from transformed geometry, so edited dimensions and group rotations are supported.
- Run the exporter with Blender:
  `blender --background --python-exit-code 1 --python gbScripts/export-woodcutter-axe.py -- .`
- `resident_behavior` owns CPU target selection, safe path planning and job state;
  `ces_island_resident_system` chooses clips and copies the grounded bone palette
  to the axe, then aligns its handle with both palms while chopping. Its child
  world transform is refreshed explicitly each frame.
- The axe uses the existing resident material, lighting and skinned shadow path.
  The hand cursor's surface query also recognizes its shared skin palette.

## Verification

- `gbScripts/check-island-resident.ps1`: reachable approach, chopping, facing,
  target deletion, resident navigation and canonical FBX checks. The long stroll
  fixture explicitly uses a compact island; the expanded default island made its
  old cycle-count assertion fail even without the woodcutting checks (80 stops).
  The 200-resident, eight-terrace crowd check still uses the current default island.
- `gbScripts/check-resident-stairs.ps1`: existing stair traversal checks.
- `gbScripts/check-hand-cursor-math.ps1`: motion and surface-query checks.
- `gbScripts/check-woodcutting-demo.ps1`: launches its own Vulkan demo, checks eight
  tree assignments and eight transitions to chopping, animation retargeting,
  parent transforms, validation output, frame captures and clean shutdown.
  Optional `-Executable` selects Debug; `-NoValidation` disables Vulkan validation.
- `GB_WOODCUTTER_INSPECT=1` enables a close inspection camera for that smoke run;
  normal startup retains the existing camera.

Windows Vulkan Release and Debug are checked. OpenGL and Metal are unverified.
