#pragma once

#include <array>
#include <cstdint>
#include <vector>

namespace game
{
    struct terrain_config
    {
        float world_size = 2048.f;
        float island_size = 1792.f * .67f;
        int resolution = 1024;
        int layers_count = 8;
        int underwater_layers_count = 8;
        float underwater_terrace_width = 6.f * .67f;
        float layer_height = 2.f;
        float shore_height = .5f;
        uint32_t seed = 1337;
        float water_y = -.3f;
        float water_size = 2048.f;
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
        // Grass: shared world-XZ root; all four vertices sample the same field.
        // Grass color alpha encodes left/right UV (0/255), not opacity or outline ID.
        // Other procedural geometry leaves both values at zero.
        std::array<float, 2> texcoord = {{0.f, 0.f}};
        // Packed into tangent.xy, with tangent.z=1 identifying grass.
        std::array<float, 2> grass_bend = {{0.f,0.f}};
        // Tree bend weight/phase, packed into tangent.xy with tangent.w=1.
        std::array<float, 2> tree_bend = {{0.f,0.f}};
        // Editor terrain stores level/climate lookup UVs, never final vertex RGB.
        std::array<float, 2> editor_palette = {{-1.f,-1.f}};
        // Per-face/per-object silhouette identity; grass keeps the zero default.
        uint32_t outline_id = 0;
        bool stone_texture = false; // Pixel stone on static slabs; no vegetation bending.

        terrain_vertex() = default;
        // Some Apple demo targets still use C++11, where default member
        // initializers prevent aggregate initialization with three fields.
        terrain_vertex(const std::array<float, 3>& p, const std::array<float, 3>& n,
                       const std::array<uint8_t, 4>& tint) : position(p), normal(n), color(tint) {}
    };

    struct terrain_mesh_data
    {
        std::vector<terrain_vertex> vertices;
        std::vector<uint16_t> indices;
    };
    struct terrain_tree {
        std::array<float,3> position;
        int terrace=0;
        bool conifer=false;
        uint8_t species=0; // Stable TreeType value, without coupling terrain data to editor headers.
        float height=0;
        float yaw=0;
        terrain_mesh_data mesh;
    };
}
