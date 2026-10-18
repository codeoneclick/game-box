#pragma once
#include "terrain_data.h"
#include <cstddef>
namespace game {
struct TerrainOptimizeConstraints {
 std::vector<uint16_t> locked_vertices;
 size_t vertex_limit=60000;
};
struct TerrainOptimizeResult {
 std::vector<terrain_mesh_data> parts;
 size_t input_triangles=0,output_triangles=0,fallback_regions=0;
};
// Exact rendering-only retopology. Every boundary edge and nonconstant field
// remains fixed; CPU source surfaces used by gameplay/grass are never changed.
TerrainOptimizeResult optimize_terrain_mesh(const terrain_mesh_data&,const TerrainOptimizeConstraints&);
}
