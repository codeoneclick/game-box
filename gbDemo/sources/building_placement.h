#pragma once
#include "building_catalog.h"
#include "terrain_data.h"
#include <map>
#include <utility>
namespace game {
constexpr float building_clearance = .5f;
struct EditorBuilding {
  uint32_t id = 0;
  BuildingType type = BuildingType::Forester;
  std::array<float, 3> position = {{0, 0, 0}};
  float yaw = 0;
  int terrace = 1;
};
float normalized_building_yaw(float radians);
bool point_inside_building(const EditorBuilding &, float x, float z,
                           float extra = building_clearance);
bool buildings_overlap(const EditorBuilding &, const EditorBuilding &,
                       float extra = building_clearance);
// Immutable worker-produced index of the actual, non-overlapping terrain tops.
// Pass the complete installed terrain parts (including contour-smoothed
// chunks). covers receives dimensions including the caller's construction
// clearance.
class BuildingSurfaceSupport {
  struct Triangle {
    std::array<std::array<float, 2>, 3> p;
    float height;
  };
  std::vector<Triangle> triangles_;
  std::map<std::pair<int, int>, std::vector<size_t>> buckets_;
  float bucket_size_;

public:
  struct SurfaceEdge {
    std::array<float, 2> a, b;
    float height;
  };
  explicit BuildingSurfaceSupport(const std::vector<terrain_mesh_data> &parts,
                                  float bucket_size = 16.f);
  // Queries use the same actual top triangles as covers, including contours.
  bool height_at(float x, float z, float &height) const;
  std::vector<SurfaceEdge> nearby_edges(float x, float z, float radius) const;
  bool covers(float x, float z, float yaw, float half_width, float half_depth,
              float height) const;
};
} // namespace game
