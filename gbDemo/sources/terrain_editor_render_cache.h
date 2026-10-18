#pragma once
#include "terrain_editor_mesh.h"
#include <memory>
#include <string>
#include <set>
#include <map>
namespace game {
struct TerrainLodGeneration;
struct EditorRenderBatch {enum Kind {Terrain,Grass,Trees};Kind kind=Terrain;std::string key;terrain_mesh_data mesh;std::shared_ptr<const TerrainLodGeneration> lod;};
struct EditorCacheProfile {double seed_ms=0,mesh_ms=0,batch_ms=0;size_t chunks=0,batches=0,vertices=0;};
// CPU ownership only. The scene uploads returned batches on the game-loop path.
class TerrainEditorRenderCache {
 std::shared_ptr<TerrainEditorState> state_;
 std::map<int,EditorChunkMesh> chunks_;
 terrain_mesh_data seed_terrain_;
 std::map<int,std::vector<size_t>> seed_top_triangles_;
 std::map<int,std::vector<EditorSeedTriangle>> seed_domain_;
 terrain_data seed_field_;
 std::set<int> canonical_;
 std::map<std::pair<int,int>,terrain_mesh_data> skirts_;
 std::map<std::pair<int,int>,size_t> part_counts_;
 bool initial_flush_=true;
 uint64_t load_generation_=0;
 std::function<std::pair<terrain_mesh_data,std::vector<terrain_mesh_data>>(const terrain_config&)> seed_builder_;
 EditorCacheProfile profile_;
 int chunk_count() const;
 void preserve_seed_palette(terrain_mesh_data&,int chunk) const;
public:
 explicit TerrainEditorRenderCache(std::shared_ptr<TerrainEditorState> state):state_(std::move(state)){}
 void set_state(std::shared_ptr<TerrainEditorState> state){state_=std::move(state);}
 void set_seed_builder(std::function<std::pair<terrain_mesh_data,std::vector<terrain_mesh_data>>(const terrain_config&)> builder){seed_builder_=std::move(builder);}
 const EditorCacheProfile& last_profile()const{return profile_;}
 void initialize(const terrain_mesh_data&,const std::vector<terrain_mesh_data>&);
 std::vector<EditorRenderBatch> rebuild(TerrainEditorDirty,bool optimize=true);
 std::vector<terrain_mesh_data> terrain_parts() const;
 std::vector<std::array<float,4>> active_tree_blobs() const;
};
}
