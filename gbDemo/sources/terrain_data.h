#pragma once

#include <array>
#include <cstdint>
#include <vector>

namespace game
{
    struct terrain_config
    {
        float world_size = 512.f;
        float island_size = 256.f;
        int resolution = 512;
        int layers_count = 8;
        int underwater_layers_count = 8;
        float underwater_terrace_width = 6.f;
        float layer_height = 2.f;
        float shore_height = .5f;
        uint32_t seed = 1337;
        float water_y = -.3f;
        float water_size = 768.f;
        bool debug_layer_colors = false;

        void validate() const;
    };

    // Signed label times layer_height gives top height, except label 0 uses shore_height.
    // No GPU state or generation formula is stored in this mutable data.
    struct terrain_data
    {
        static constexpr int empty_layer = -9;
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
