#pragma once
#include "terrain_editor_state.h"
namespace game {
constexpr int terrain_editor_chunk_cells=16;
struct EditorChunkMesh {terrain_mesh_data terrain;std::vector<terrain_mesh_data> grass;};
struct EditorSeedSample {float x,z,value;};
using EditorSeedTriangle=std::array<EditorSeedSample,3>;
EditorChunkMesh build_editor_chunk(const TerrainEditorLevel&,int chunk_x,int chunk_z,bool with_grass=true);
EditorChunkMesh build_editor_seed_chunk(const TerrainEditorLevel&,int chunk_x,int chunk_z,const std::vector<EditorSeedTriangle>&,const terrain_data& seed);
std::vector<terrain_mesh_data> build_editor_grass(const TerrainEditorLevel&,const terrain_mesh_data& surface);
terrain_mesh_data build_editor_tree(const EditorTree&,uint32_t seed);
terrain_mesh_data build_editor_rocks(const TerrainEditorLevel&,float x0,float z0,float x1,float z1);
}
