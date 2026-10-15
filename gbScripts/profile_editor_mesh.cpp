#include "terrain_editor_render_cache.h"
#include "terrain_generator.h"
#include "terrain_mesh_builder.h"
#include <iostream>
#include <chrono>
int main(){
 game::terrain_config config;auto generated=game::terrain_generator::generate(config);auto surface=game::terrain_mesh_builder::build(generated);auto grass=game::terrain_mesh_builder::build_grass(surface,config);
 auto state=std::make_shared<game::TerrainEditorState>();state->initialize(generated);state->consume_dirty();game::TerrainEditorRenderCache cache(state);cache.initialize(surface,grass);auto batches=cache.rebuild({});auto p=cache.last_profile();std::cout<<"Seed merge batches="<<p.batches<<" vertices="<<p.vertices<<" merge_ms="<<p.batch_ms<<"\n";
 state->current_tool=game::TerrainEditorTool::Terrain;state->brush_size=20;state->strength=1;
 for(int pass=0;pass<5;++pass){state->set_target_terrace(7-pass);state->begin_stroke();state->paint(12,2);state->end_stroke();auto changes=cache.rebuild(state->consume_dirty());p=cache.last_profile();std::cout<<"Edit "<<pass<<" chunks="<<p.chunks<<" mesh_ms="<<p.mesh_ms<<" merge_ms="<<p.batch_ms<<" batches="<<p.batches<<" vertices="<<p.vertices<<"\n";}
}
