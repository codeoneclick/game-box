#pragma once

#include <array>
#include <cstdint>
#include <vector>

namespace game
{
    struct terrain_config
    {
        float world_size = 256.f;
        int resolution = 256;
        int layers_count = 8;
        float layer_height = 2.f;
        uint32_t seed = 1337;
        float water_y = -.3f;
        float water_size = 400.f;
        bool debug_layer_colors = false;

        void validate() const;
    };

    // Cell centres span X/Z; -1 is ocean, 0..layers_count-1 is land.
    // No GPU state or generation formula is stored in this mutable data.
    struct terrain_data
    {
        terrain_config config;
        std::vector<int8_t> layers;
        int layer_at(int x, int z) const;
    };

    struct terrain_vertex
    {
        std::array<float, 3> position;
        std::array<float, 3> normal;
        std::array<uint8_t, 4> color;
    };

    struct terrain_mesh_data
    {
        std::vector<terrain_vertex> vertices;
        std::vector<uint16_t> indices;
    };
}
