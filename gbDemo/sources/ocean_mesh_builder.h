#pragma once
#include "terrain_data.h"
#include <array>
namespace game {
    struct ocean_visibility {
        std::vector<uint16_t> water,foam;
        std::vector<size_t> water_nodes,foam_nodes;
        size_t tested_nodes=0;
        bool initialized=false;
    };
    // Bounds use the original vertex positions; selection never changes triangle order.
    class ocean_mesh_quadtree {
    public:
        explicit ocean_mesh_quadtree(const terrain_mesh_data& mesh);
        ocean_visibility select(const std::array<float,16>& view_projection,float min_y,float max_y,bool horizon,float foam_extent) const;
        // Keep one cache per tree, resetting it when the source mesh is rebuilt.
        // Returns true only when index lists have been rebuilt.
        bool select(const std::array<float,16>& view_projection,float min_y,float max_y,bool horizon,float foam_extent,ocean_visibility& cache) const;
    private:
        struct bounds { float min_x=0,min_z=0,max_x=0,max_z=0; };
        struct node {
            bounds box;
            std::vector<size_t> triangles;
            std::array<int,4> children={{-1,-1,-1,-1}};
        };
        std::vector<uint16_t> m_indices;
        std::vector<bounds> m_triangle_bounds;
        std::vector<std::array<double,2>> m_triangle_centroids;
        std::vector<node> m_nodes;
        int build_node(const std::vector<size_t>& triangles,int depth);
    };
    class ocean_mesh_builder {
    public: static std::vector<terrain_mesh_data> build(float size,int cells_per_side);
        static terrain_mesh_data build_horizon(float inner_size,float outer_size,int segments);
    };
}
