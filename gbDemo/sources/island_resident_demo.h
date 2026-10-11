#pragma once
#include "main_headers.h"
#include "declarations.h"
#include "terrain_data.h"
namespace game {
class ces_island_resident_system;
struct resident_demo_handles {
    std::vector<gb::shape_3d_shared_ptr> shapes;
    std::shared_ptr<ces_island_resident_system> system;
    gb::input_context_listener_shared_ptr input;
    glm::vec3 focus=glm::vec3(0);
};
resident_demo_handles create_island_resident_demo(const gb::scene_fabricator_shared_ptr&,const terrain_mesh_data&,const terrain_config&);
}
