#pragma once
#include "terrain_data.h"
#include <cstdint>
namespace game {
class BuildingSurfaceSupport;
struct EditorBuilding;
constexpr int stairs_step_count=6;
constexpr float stairs_width=3.6f;
constexpr float stairs_run=4.f;
struct EditorStairs {
    uint32_t id=0;
    std::array<float,3> position={{0,0,0}}; // Flight center, at lower terrace height.
    float yaw=0; // Local +Z points uphill; radians, same convention as buildings.
    int lower_terrace=1; // Upper terrace is lower_terrace+1.
};
bool point_inside_stairs(const EditorStairs&,float x,float z,float margin=0.f);
bool stairs_overlap(const EditorStairs&,const EditorStairs&,float margin=.15f);
bool stairs_building_overlap(const EditorStairs&,const EditorBuilding&,float margin=.2f);
bool stairs_ray_hit(const EditorStairs&,const terrain_config&,const std::array<float,3>& origin,
                    const std::array<float,3>& direction,float& distance);
terrain_mesh_data build_stairs_mesh(const EditorStairs&,const terrain_config&);
bool find_stairs_site(const BuildingSurfaceSupport&,const terrain_config&,float x,float z,
                      EditorStairs& result,float snap_distance=5.f);
bool stairs_supported(const BuildingSurfaceSupport&,const terrain_config&,const EditorStairs&);
}
