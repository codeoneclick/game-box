# Concept island resident

Archived concept asset. The active demo now uses the final Woodsman mesh;
see `../woodsman/README.md`. These older generation checks remain available.

Editable source: `island_resident.blend` (Blender 5.2 LTS), with packed palette
images, 17-bone T-pose armature, fallback idle/walk actions and preview cameras.
The model is authored from `Low-Poly Character Turnaround Sheet (2).png`:
slim faceless male, swept brown hair, blue V-neck tunic, belt, cuffs and boots.
Geometry is 497 triangles (maximum 500 after FBX import), 1.9 m high, with feet
at local zero. Sparse hand-authored cross sections preserve the blueprint's
large head, 1.508 m arm span, narrow waist, 0.744 m hem and 0.375 m boot tops.
The V-neck, belt, cuffs and boot toes are geometry; all surfaces use flat shading.
The engine includes the armature parent and therefore reports 18 bones.

Regenerate from the repository root:

```powershell
$blender='C:/Program Files/Blender Foundation/Blender 5.2/blender.exe'
& $blender --background --python-exit-code 1 --python gbScripts/create-island-resident.py -- --output-root . --preview-dir gbDemo/output/island-check/resident-asset
& $blender --background --python-exit-code 1 gbResources/island_resident/island_resident.blend --python gbScripts/check-island-resident-assets.py -- --output-root .
./gbScripts/check-island-resident.ps1
./gbScripts/check-mixamo-retarget.ps1
./gbScripts/build-gbDemo.ps1 -Configuration debug-client -Platform x64
```

The generator writes the three FBX files into `gbBundle/Meshes` and the palette
and clothing mask into `gbBundle/Textures`. The shirt and trousers both use UV
patch zero; other patches keep skin/leather/hair fixed. One material color
controls both garment regions. Use flat shading and keep UV/weight data when
editing or exporting.

FBX export must use `apply_scale_options='FBX_SCALE_ALL'`: the mesh importer
uses `MODIFY_GEOMETRY` while the animation importer uses `ADJUST_TRANSFORMS`.
The default Blender export setting produces a 100-fold skeleton scale mismatch.
The FBX smoke probe checks both importer modes and matching bone paths.
Generated fallback animations are baked in place at 30 fps: idle is 3 s, walk
is 0.8 s. Their FBX stacks are named `Scene`; these remain authoring examples.

The demo now uses the supplied Mixamo `../character_idle.fbx` and
`../character_walking.fbx`, rather than the generated fallback clips. These
contain 65 source bones without meshes. The engine imports rig names, parents
and reference transforms, maps humanoid limbs, calibrates reference-pose
differences and bakes 18-bone target clips once while loading. Target segment
lengths and scales are preserved. Horizontal root translation is removed;
navigation moves the entity and turns it toward actual displacement. Sole
markers adjust vertical placement, without foot-locking IK. Skin bind matrices
provide reference transforms when a source FBX includes a skinned mesh; otherwise
its default skeleton must be a neutral reference pose.

`island_resident.xml` selects idle frames 0..299 and walk frames 0..30 at 30 fps
(duplicate loop endpoints omitted). Shared-engine legacy clips with matching
rig names, hierarchy and reference transforms keep their existing playback.
Automatic humanoid inference supports Mixamo and this Blender naming convention;
other humanoid names require explicit profiles through the retargeter API.
Nonhumanoid rigs cannot use this humanoid conversion path.

Blender is needed to edit/regenerate assets, not to launch the checked-in demo.
Preview PNGs and smoke/build output stay in ignored `gbDemo/output/island-check`.
