#pragma once
#include "terrain_data.h"
#include <array>
namespace game {
    struct resident_point {float x=0,z=0;};
    // Immutable world-space construction footprints supplied by the scene.
    struct resident_obstacle {
        resident_point center;
        float cosine=1,sine=0,half_width=0,half_depth=0;
    };
    struct resident_navigation {
        float origin_x=0,origin_z=0,cell_size=1,shore_y=.5f;
        int width=0,height=0;
        std::vector<uint8_t> walkable;
        std::vector<std::array<resident_point,3>> triangles;
        std::vector<std::vector<size_t>> cell_triangles;
        // Certified interior cells. Empty keeps the exact triangle-query path.
        std::vector<uint8_t> solid_cells;
        std::vector<resident_obstacle> obstacles;
        bool surface_supports(resident_point) const;
        bool is_safe(resident_point) const;
        bool segment_is_safe(resident_point,resident_point) const;
        std::vector<resident_point> find_path(resident_point,resident_point) const;
        std::vector<resident_point> reachable_points(resident_point) const;
        int cell_at(resident_point) const;
        resident_point center(int) const;
    };
    resident_navigation build_resident_navigation(const terrain_mesh_data&,float shore_y,float cell_size=1.f);
    resident_navigation build_resident_navigation(const std::vector<terrain_mesh_data>&,float shore_y,float cell_size=1.f);
}
