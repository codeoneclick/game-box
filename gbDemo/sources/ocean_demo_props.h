#pragma once
#include "terrain_data.h"
#include "buoyancy_simulation.h"
namespace game {
    struct ocean_demo_prop {
        terrain_mesh_data mesh;
        ocean_vec3 position;
        float yaw=0;
        bool floating=false;
        buoyancy_settings buoyancy;
        std::vector<buoyancy_probe> probes;
    };
    std::vector<ocean_demo_prop> build_ocean_demo_props(const ocean_settings&);
}
