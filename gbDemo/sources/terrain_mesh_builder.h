#pragma once
#include "terrain_data.h"

namespace game
{
    class terrain_mesh_builder
    {
    public:
        // Prototype topology: all layer masks must be nested and star-shaped
        // about the origin. Disconnected islands and holes require triangulation.
        static terrain_mesh_data build(const terrain_data& data);
        static terrain_mesh_data build_water(const terrain_config& config);
        static terrain_mesh_data build_seabed(const terrain_config& config);
    };
}
