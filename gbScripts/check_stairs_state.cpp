#include "terrain_editor_state.h"
#include <cassert>
#include <cstdio>
#include <iostream>
using namespace game;
static terrain_mesh_data surface() {
    terrain_mesh_data m;
    for(int half=0;half<2;++half) {
        uint16_t base=static_cast<uint16_t>(m.vertices.size());
        float y=half?4.f:2.f,z0=half?0.f:-16.f,z1=half?16.f:0.f;
        for(auto p:std::array<std::array<float,3>,4>{{{{-16,y,z0}},{{-16,y,z1}},{{16,y,z1}},{{16,y,z0}}}})
            m.vertices.emplace_back(p,std::array<float,3>{{0,1,0}},std::array<uint8_t,4>{{255,255,255,255}});
        for(int i:{0,1,2,0,2,3})m.indices.push_back(base+i);
    }
    return m;
}
int main() {
    terrain_data data;data.config.world_size=32;data.config.island_size=24;
    data.config.water_size=48;data.config.resolution=32;data.layers.assign(1024,1);
    for(int z=16;z<32;++z)for(int x=0;x<32;++x)data.layers[z*32+x]=2;
    TerrainEditorState state;state.initialize(data);state.consume_dirty();
    state.building_surface=std::make_shared<BuildingSurfaceSupport>(std::vector<terrain_mesh_data>{surface()});
    state.building_surface_revision=state.terrain_revision;
    state.select_tool(TerrainEditorTool::Stairs);
    EditorStairs candidate;assert(state.stairs_preview(0,-1,candidate));
    const auto density=state.level.grass_density;
    assert(state.place_stairs(0,-1));assert(state.level.stairs.size()==1);
    const auto stair=state.level.stairs.front();const auto revision=state.revision;
    assert(!state.place_stairs(0,-1));assert(state.revision==revision);
    auto dirty=state.consume_dirty();assert(dirty.stairs&&dirty.grass&&!dirty.terrain);
    assert(!state.place_tree(stair.position[0],stair.position[2]));
    assert(!state.can_place_building(stair.position[0],stair.position[2]));
    assert(state.level.grass_density==density);
    assert(state.undo()&&state.level.stairs.empty());
    assert(state.redo()&&state.level.stairs.front().id==stair.id);
    assert(state.save("stairs-state.level"));
    TerrainEditorState loaded;assert(loaded.load("stairs-state.level"));
    assert(loaded.current_tool==TerrainEditorTool::Stairs&&loaded.level.stairs.size()==1);
    assert(loaded.level.stairs[0].position==stair.position&&loaded.level.stairs[0].yaw==stair.yaw);
    assert(loaded.level.next_stairs_id==state.level.next_stairs_id);
    assert(state.place_stairs(stair.position[0],stair.position[2],true));assert(state.level.stairs.empty());
    assert(state.undo()&&state.level.stairs.size()==1);
    state.select_tool(TerrainEditorTool::Terrain);state.brush_size=4;state.set_target_terrace(6);
    assert(state.paint(stair.position[0],stair.position[2]));state.end_stroke();
    assert(state.level.stairs.empty());assert(state.undo()&&state.level.stairs.size()==1);
    // Loading malformed transforms must leave the previous valid level intact.
    loaded.level.stairs[0].lower_terrace=0;assert(!loaded.save("stairs-invalid.level"));
    std::remove("stairs-state.level");
    std::cout<<"Stairs state/history/persistence checks passed\n";
}
