# Five voxel trees

The procedural models are Pine, Oak, Birch, Palm and Thuja, with no ground platforms, soil blocks, grass patches or pedestal geometry. All roots sit on the selected terrace. Pine has tiered foliage, Oak broad lobes, Birch pale bark with dark marks, Palm a curved segmented trunk and drooping block fronds, Thuja a narrow tapered crown.

The common CPU voxel_tree_builder produces occupied 0.35m cells, culls interfaces between occupied cells and merges exposed faces. Vertex RGB describes wood/bark/species foliage. Near bark/leaf pixel detail is projected from the undeformed positions after vertex-stage wind sampling and fades with screen derivatives at distance. It remains attached while trees sway.

Existing procedural forest placement still stays on terraces3–7, with conifers above and broadleaf species below; all five species are seeded deterministically. The same builder serves initial scene trees and editor trees, including after load. Saved species IDs0–3 remain Pine/Oak/Birch/Palm; Thuja is4. GBLVv3 fields and checksum are unchanged; validation accepts the new type. Older save formats remain readable. Thuja placement, undo/redo and save/load are CPU checked.

Trees remain batched. Internal voxel faces are absent, individual meshes fit16-bit indices, and the initial forest splits a batch if it would overflow. Outline IDs, blob shadows, root grounding, yaw/scale and per-tree chaotic wind are preserved. The native Trees menu now has five buttons and corresponding voxel icons; atlas slots8,9,10,11,15 retain all other UI assets.

Wind-aware meshing keeps vertical edges split at voxel heights, and flutter is shared by all vertices of one tree rather than varying spatially across a merged face. This prevents animation cracks at greedy-face/material T junctions, including Birch bark markings.

Source: gbDemo/sources/voxel_tree_builder.h/.cpp; editor delegate terrain_editor_mesh.cpp; seed integration terrain_mesh_builder.cpp; state/save limits terrain_editor_state.h/.cpp; shader_island_terrain.vert/.frag and Metal terrain functions in mtl_library.metal. New C++ source is registered in the Windows project and all three Apple demo targets. Menu and icons are terrain_editor_ui.cpp and gbScripts/generate-editor-icons.py.

Checks: ./gbScripts/check-terrain-editor.ps1 and ./gbScripts/check-island-terrain.ps1; standard Debug/Release build scripts. CPU checks cover five species, winding, finite positions, normals, grounding, transforms, internal interfaces, deterministic generation, wind metadata and save roundtrip. Native inspection uses GB_TREES_INSPECT=pine/oak/birch/palm/thuja. Metal/OpenGL execution remains unverified on Windows.

Final verification: standard debug-client/release-client x64 builds passed. CPU mesh and save checks passed. All five models were inspected in native Vulkan; live wind, five-button menu, Thuja selection and placement were exercised, with clean exit0 and no validation errors. Metal was updated in source only.

Maximum triangles over128 seed/id variants after wind-safe subdivision: Pine492, Oak1200, Birch584, Palm1112, Thuja880. Current seeded forest contains83 trees /61274 triangles, within10–20 trees per terrace. The preceding model set used approximately11762 tree triangles; the new silhouettes deliberately add geometry, while hidden faces and horizontal merging limit the cost. These counts do not constitute a per-pass GPU timing measurement.
