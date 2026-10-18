#pragma once
#include "terrain_data.h"
#include "terrain_lod_settings.h"
namespace game {
struct TerrainContourConstraints {
 // World XZ/radius, fourth lane reserved. Sources include building/stair/tree
 // footprints; settings never move the logical objects to suit a coarser mesh.
 std::vector<std::array<float,4>> protected_disks;
 // Exact outer node bounds. Boundary columns stay pinned for all levels.
 std::array<float,4> bounds={{-1e20f,-1e20f,1e20f,1e20f}};
};
struct TerrainLodVariant {
 struct Collapse {
  std::array<float,2> position,target;
  uint32_t vertex=0,target_vertex=0,other_neighbor=0;
  float geometric_error=0;
  float height=0;
 };
 terrain_mesh_data mesh;
 std::vector<Collapse> collapses; // Ordered offline dependencies for this transition.
 std::vector<std::array<float,2>> morph_delta_xz,morph_normal_xz;
 size_t side_triangles=0,top_triangles=0;
 float requested_percent=0,actual_percent=0,world_error=0,next_world_error=0;
};
// True only when a clockwise XZ triangle cannot flip anywhere in t=[0,1].
bool terrain_morph_triangle_valid(const std::array<std::array<float,2>,3>& start,const std::array<std::array<float,2>,3>& target);
std::vector<TerrainLodVariant> build_terrain_contour_lods(const terrain_mesh_data&,const TerrainLodSettings&,const TerrainContourConstraints&);
// Debug overlays reuse the selected terrain entity and mesh representation.
TerrainLodVariant build_terrain_contour_debug(const TerrainLodVariant&,int mode,float marker_size);
}
