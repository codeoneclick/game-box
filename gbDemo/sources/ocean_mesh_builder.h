#pragma once
#include "terrain_data.h"
namespace game {
    class ocean_mesh_builder {
    public: static std::vector<terrain_mesh_data> build(float size,int cells_per_side);
        static terrain_mesh_data build_horizon(float inner_size,float outer_size,int segments);
    };
}
