# Original island resident

Editable source: `island_resident.blend` (Blender 5.2 LTS), with packed palette
images, 17-bone armature, idle/walk actions and a front preview camera.
The model is procedurally authored for this demo; no external character asset
was imported. Geometry is 2,012 triangles, 1.9 m high, with feet at local zero.
The engine includes the armature parent and therefore reports 18 bones.

Regenerate from the repository root:

```powershell
$blender='C:/Program Files/Blender Foundation/Blender 5.2/blender.exe'
& $blender --background --python-exit-code 1 --python gbScripts/create-island-resident.py -- --output-root . --preview-dir gbDemo/output/island-check/resident-asset
& $blender --background --python-exit-code 1 gbResources/island_resident/island_resident.blend --python gbScripts/check-island-resident-assets.py -- --output-root .
./gbScripts/check-island-resident.ps1
./gbScripts/build-gbDemo.ps1 -Configuration debug-client -Platform x64
```

The generator writes the three FBX files into `gbBundle/Meshes` and the palette
and clothing mask into `gbBundle/Textures`. The shirt and trousers both use UV
patch zero; other patches keep skin/leather/hair/eyes fixed. One material color
controls both garment regions. Use flat shading and keep UV/weight data when
editing or exporting.

FBX export must use `apply_scale_options='FBX_SCALE_ALL'`: the mesh importer
uses `MODIFY_GEOMETRY` while the animation importer uses `ADJUST_TRANSFORMS`.
The default Blender export setting produces a 100-fold skeleton scale mismatch.
The FBX smoke probe checks both importer modes and matching bone paths.
Animations are baked in place at 30 fps: idle is 3 s, walk is 0.8 s. Exported
FBX stacks are named `Scene`; runtime configuration assigns idle/walk names.

Blender is needed to edit/regenerate assets, not to launch the checked-in demo.
Preview PNGs and smoke/build output stay in ignored `gbDemo/output/island-check`.
