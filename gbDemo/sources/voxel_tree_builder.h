#pragma once
#include "terrain_data.h"

namespace game {
struct EditorTree;
// CPU-only occupied-voxel surface. No base, terrain or decorative prop geometry.
terrain_mesh_data build_voxel_tree(const EditorTree& tree, uint32_t seed);
}
