#pragma once
#include "terrain_contour_lod.h"
#include <memory>
namespace game {
struct TerrainLodVersion {uint64_t load_generation=0,terrain_revision=0,settings_revision=0;};
inline bool terrain_lod_version_current(const TerrainLodVersion&a,const TerrainLodVersion&b){return a.load_generation==b.load_generation&&a.terrain_revision==b.terrain_revision&&a.settings_revision==b.settings_revision;}
struct TerrainLodNode {
 std::array<float,3> minimum={{0,0,0}},maximum={{0,0,0}};
 std::array<int,4> children={{-1,-1,-1,-1}};
 int parent=-1,depth=0;
 std::vector<std::shared_ptr<const TerrainLodVariant>> levels;
};
struct TerrainLodGeneration {TerrainLodVersion version;std::vector<TerrainLodNode> nodes;std::shared_ptr<const terrain_mesh_data> source;uint64_t signature=0;float cell_size=1;size_t source_triangles=0,source_top=0,source_sides=0;double build_ms=0;};
struct TerrainLodView {
 std::array<float,16> view_projection={};
 std::array<float,3> camera={{0,0,0}};
 float projection_scale=1;
 float near_error_px=.5f,normal_error_px=1.5f,far_error_px=5.f;
};
struct TerrainLodItem {int node=0,level=0;float morph=0;bool operator==(const TerrainLodItem&b)const{return node==b.node&&level==b.level&&morph==b.morph;}};
struct TerrainLodSelection {std::vector<TerrainLodItem> items;size_t tested=0,culled=0,triangles=0;};
TerrainLodGeneration build_terrain_lod_generation(const terrain_mesh_data&,const TerrainLodSettings&,const TerrainContourConstraints&,float cell_size,const TerrainLodVersion&);
TerrainLodGeneration build_terrain_lod_preview(terrain_mesh_data,float cell_size,const TerrainLodVersion&);
TerrainLodSelection select_terrain_lod(const TerrainLodGeneration&,const TerrainLodView&,const TerrainLodSelection* previous);
float terrain_lod_allowed_error(const TerrainLodView&,float distance,float cell_size);
}
