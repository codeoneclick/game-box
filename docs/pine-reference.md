# Pine reference revision (2026-10-15)

Pine alone now follows the new three-view voxel concept. Five irregular branch
fans replace the solid symmetric plates. Broader lower decks, one-cell droop,
coarse edge notches, visible bark in the gaps, a bare lower trunk and a small
green leader give the tree a less regular silhouette. The foliage palette is
olive/forest green. There is no pedestal or ground geometry. Oak, Birch, Palm
and Thuja retain their previous geometry and colors.

The shared initial/editor builder, species IDs, yaw/scale, silhouette IDs,
wind metadata and procedural pixel shading remain in use. Shadow casters share
the resulting mesh and wind. Pine-only six-neighbor pruning removes occasional
detached clumps before the existing greedy surface mesher runs. CPU regression
reconstructs occupied cells independently from emitted mesh face crossings and
verifies that all cells connect to the trunk. The previously failing id171,
seed5 case now passes; 128 seeds are checked. Bare-trunk and asymmetric-crown
checks first failed against the old model and now pass.

The control sample is 1406 triangles at 7.7m unscaled height, with a maximum of
1556 triangles across those 128 variants. Existing mesh capacity, winding,
height-sliced wind seams, transforms and render save/load checks pass. The older
Pine492 counts in voxel-trees.md describe the superseded mesh. The Pine menu
icon (tile8) also uses open irregular decks; all other atlas tiles remain
pixel-identical.

Sources: voxel_tree_builder.cpp, generate-editor-icons.py and the generated
editor_ui_icons.png. Checks: check-terrain-editor.ps1, check-island-terrain.ps1,
standard Debug/Release builds and native Vulkan Pine inspection.

Verification completed: both standard Windows Debug/Release builds passed,
check-terrain-editor.ps1 and check-island-terrain.ps1 passed. The current seeded
forest contains 74 trees / 78,362 tree triangles (all species combined). Native
Vulkan day, sunset and live-wind Pine inspections, menu selection and actual
editor placement passed, exited cleanly and reported no validation/compiler
errors. Placement rebuilt flags0010 at revision4. Frames/logs are in
`gbDemo/output/island-check/pine-reference-final-{12,sunset,live}` and
`pine-reference-editor-smoke`; the final inspection PNG is
`pine-reference-final-12/240.png`. git diff --check passed. The connectivity
regression was confirmed failing before pruning and passing afterward, with a
read-only review confirming the previously detached seed5 clump is connected.
Metal/OpenGL were not built or visually exercised on Windows; no shaders or
backend-specific behavior were changed for this model revision.
