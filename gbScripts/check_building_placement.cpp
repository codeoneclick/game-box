#include "terrain_editor_state.h"
#include <cassert>
#include <cmath>
#include <cstdio>
#include <fstream>
#include <iostream>
using namespace game;
static terrain_mesh_data tiles(bool hole = false, bool step = false) {
  terrain_mesh_data mesh;
  for (int z = -20; z < 20; ++z)
    for (int x = -20; x < 20; ++x) {
      if (hole && x == 0 && z == 0)
        continue;
      float y = step && x == 0 && z == 0 ? 6.f : 4.f;
      uint16_t i = uint16_t(mesh.vertices.size());
      for (auto p : std::array<std::array<float, 3>, 4>{
               {{{float(x), y, float(z)}},
                {{float(x + 1), y, float(z)}},
                {{float(x + 1), y, float(z + 1)}},
                {{float(x), y, float(z + 1)}}}})
        mesh.vertices.emplace_back(
            p, std::array<float, 3>{{0, 1, 0}},
            std::array<uint8_t, 4>{{255, 255, 255, 255}});
      for (int a : {0, 1, 2, 0, 2, 3})
        mesh.indices.push_back(i + a);
    }
  return mesh;
}
int main() {
  TerrainEditorState rotation;
  const float radians_per_degree=3.14159265358979323846f/180.f;
  const auto preview_revision=rotation.revision,geometry_revision=rotation.terrain_revision;
  rotation.set_building_rotation_degrees(37);
  assert(std::abs(rotation.building_yaw-37*radians_per_degree)<1e-6f);
  rotation.set_building_rotation_snap(15);
  assert(rotation.building_rotation_snap==15&&std::abs(rotation.building_yaw-30*radians_per_degree)<1e-6f);
  rotation.set_building_rotation_degrees(38);
  assert(std::abs(rotation.building_yaw-45*radians_per_degree)<1e-6f);
  rotation.set_building_rotation_snap(45);
  rotation.set_building_rotation_degrees(70);
  assert(std::abs(rotation.building_yaw-90*radians_per_degree)<1e-6f);
  rotation.set_building_rotation_snap(90);
  rotation.set_building_rotation_degrees(360);
  assert(rotation.building_yaw==0);
  rotation.set_building_rotation_degrees(359);
  assert(rotation.building_yaw==0);
  rotation.set_building_rotation_degrees(-80);
  assert(std::abs(rotation.building_yaw-270*radians_per_degree)<1e-6f);
  auto snapped_yaw=rotation.building_yaw;
  rotation.set_building_rotation_snap(0);
  assert(rotation.building_yaw==snapped_yaw);
  assert(std::abs(rotation.building_yaw-270*radians_per_degree)<1e-6f);
  rotation.set_building_rotation_degrees(-37);
  assert(std::abs(rotation.building_yaw-323*radians_per_degree)<1e-6f);
  auto before_rotation=rotation.building_yaw;
  rotation.set_building_rotation_degrees(std::nanf(""));
  rotation.set_building_rotation_degrees(INFINITY);
  rotation.rotate_building(INFINITY);
  rotation.set_building_rotation_snap(30);
  assert(rotation.building_yaw==before_rotation&&rotation.building_rotation_snap==0);
  rotation.set_building_rotation_snap(15);rotation.rotate_building(15*radians_per_degree);
  assert(std::abs(rotation.building_yaw-345*radians_per_degree)<1e-6f);
  before_rotation=rotation.building_yaw;rotation.set_building_rotation_snap(-1);
  assert(rotation.building_yaw==before_rotation&&rotation.building_rotation_snap==15);
  assert(rotation.revision==preview_revision&&rotation.terrain_revision==geometry_revision&&!rotation.is_dirty&&!rotation.can_undo());
  assert(normalized_building_yaw(-1e-8f) < 6.283185307179586f);
  BuildingSurfaceSupport flat({tiles()}), hole({tiles(true)}),
      step({tiles(false, true)});
  assert(flat.covers(0, 0, .37f, 4, 3, 4));
  assert(!flat.covers(19, 0, 0, 4, 3, 4));
  assert(!hole.covers(0, 0, .37f, 4, 3, 4));
  assert(!step.covers(0, 0, .37f, 4, 3, 4));
  assert(!flat.covers(0, 0, 0, 4, 3, 6));
  assert(flat.covers(0, 0, 1.57079632679f, 4, 3, 4));
  assert(!flat.covers(0, 0, 0, 21, 3, 4)); // Narrow terrace.
  EditorBuilding a, b;
  a.position = {{0, 4, 0}};
  b.position = {{0, 4, 0}};
  b.yaw = .37f;
  assert(buildings_overlap(a, b));
  b.position[0] = 30;
  assert(!buildings_overlap(a, b));
  terrain_data d;
  d.config.resolution = 40;
  d.config.world_size = 40;
  d.config.island_size = 30;
  d.config.water_size = 60;
  d.layers.assign(1600, 2);
  TerrainEditorState s;
  s.initialize(d);
  s.consume_dirty();
  assert(!s.can_place_building(0, 0));
  s.building_surface = std::make_shared<BuildingSurfaceSupport>(
      std::vector<terrain_mesh_data>{tiles()});
  s.building_surface_revision = s.terrain_revision;
  assert(s.can_place_building(0, 0));
  // With zero grass, a removed tree must still dirty its neighboring chunk.
  TerrainEditorState tree_bounds;
  tree_bounds.initialize(d);
  tree_bounds.level.grass_density.assign(d.layers.size(),0);
  tree_bounds.building_surface=s.building_surface;
  tree_bounds.building_surface_revision=tree_bounds.terrain_revision;
  assert(tree_bounds.place_tree(.5f,.5f));
  tree_bounds.consume_dirty();
  const int removed_cell=tree_bounds.cell_at(.5f,.5f);
  const int building_cell=tree_bounds.cell_at(-4.5f,-4.5f);
  assert((removed_cell%40)/16!=(building_cell%40)/16);
  auto includes_tree=[&](const TerrainEditorDirty& dirty){
    assert(dirty.trees&&dirty.buildings&&!dirty.grass);
    assert(dirty.x0<=removed_cell%40&&dirty.x1>=removed_cell%40);
    assert(dirty.z0<=removed_cell/40&&dirty.z1>=removed_cell/40);
  };
  assert(tree_bounds.place_building(-4.5f,-4.5f));
  assert(tree_bounds.level.trees.empty());includes_tree(tree_bounds.consume_dirty());
  assert(tree_bounds.undo());assert(tree_bounds.level.trees.size()==1);includes_tree(tree_bounds.consume_dirty());
  assert(tree_bounds.redo());assert(tree_bounds.level.trees.empty());includes_tree(tree_bounds.consume_dirty());
  auto rev = s.terrain_revision;
  assert(s.place_tree(0, 0));
  auto density = s.level.grass_density;
  s.set_building_rotation_degrees(37);
  assert(std::abs(s.building_yaw-37*radians_per_degree)<1e-6f);
  assert(s.place_building(0, 0));
  assert(s.level.trees.empty());
  assert(s.level.buildings.size() == 1);
  auto id = s.level.buildings[0].id;
  assert(std::abs(s.level.buildings[0].yaw-37*radians_per_degree)<1e-6f);
  assert(s.terrain_revision == rev);
  assert(!s.place_building(0, 0));
  assert(s.level.buildings.size() == 1);
  auto rejected_revision = s.revision;
  auto rejected_next = s.level.next_building_id;
  assert(!s.place_building(19, 19));
  assert(s.revision == rejected_revision &&
         s.level.next_building_id == rejected_next);
  assert(s.undo());
  assert(s.level.buildings.empty() && s.level.trees.size() == 1 &&
         s.level.grass_density == density);
  assert(s.redo());
  assert(s.level.buildings[0].id == id && s.level.trees.empty());
  s.select_tool(TerrainEditorTool::Terrain);
  s.brush_size = 1;
  s.set_target_terrace(9);
  assert(!s.paint(.5f, .5f));
  auto foundation = s;
  foundation.level.buildings[0].yaw = .78539816339f;
  assert(!foundation.paint(6.5f, -6.5f));
  s.select_tool(TerrainEditorTool::Grass);
  assert(!s.paint(.5f, .5f));
  s.select_tool(TerrainEditorTool::Buildings);
  assert(s.save("building-check.level"));
  TerrainEditorState loaded;
  assert(loaded.load("building-check.level"));
  assert(loaded.building_rotation_snap==0);
  assert(loaded.current_tool == TerrainEditorTool::Buildings &&
         loaded.level.buildings.size() == 1 &&
         loaded.level.buildings[0].yaw == s.building_yaw &&
         loaded.level.buildings[0].id == id);
  std::ifstream saved("building-check.level", std::ios::binary);
  std::string bytes((std::istreambuf_iterator<char>(saved)), {});
  saved.close();
  auto rewrite_crc = [](std::string &b) {
    uint32_t c = ~0u;
    for (size_t i = 0; i < b.size() - 4; ++i) {
      c ^= uint8_t(b[i]);
      for (int j = 0; j < 8; ++j)
        c = (c >> 1) ^ (0xedb88320u & uint32_t(-int(c & 1)));
    }
    c = ~c;
    for (int j = 0; j < 4; ++j)
      b[b.size() - 4 + j] = char(c >> (j * 8));
  };
  const auto count_offset = bytes.size() - 36;
  for (auto field : std::array<std::pair<size_t, uint32_t>, 6>{
           {{count_offset, 0xffffffffu},
            {count_offset - 4, 0},
            {count_offset + 4, 0},
            {count_offset + 8, 3},
            {count_offset + 24, 0x7fc00000u},
            {count_offset + 28, 0}}}) {
    auto bad = bytes;
    for (int j = 0; j < 4; ++j)
      bad[field.first + j] = char(field.second >> (j * 8));
    rewrite_crc(bad);
    std::ofstream f("building-bad.level", std::ios::binary);
    f.write(bad.data(), bad.size());
    f.close();
    auto before = loaded.revision;
    assert(!loaded.load("building-bad.level"));
    assert(loaded.revision == before && loaded.level.buildings.size() == 1 &&
           loaded.level.buildings[0].id == id);
  }
  // V2 Climate value 3 must migrate to Terrain, with no building data.
  auto legacy = bytes;
  legacy.erase(legacy.size() - 48, 44);
  legacy[4] = 2;
  const size_t tool_offset = legacy.size() - 36;
  legacy[tool_offset] = 3;
  rewrite_crc(legacy);
  std::ofstream v2("building-v2.level", std::ios::binary);
  v2.write(legacy.data(), legacy.size());
  v2.close();
  TerrainEditorState migrated;
  assert(migrated.load("building-v2.level"));
  assert(migrated.current_tool == TerrainEditorTool::Terrain &&
         migrated.level.buildings.empty());
  assert(!loaded.can_place_building(12, 12));
  assert(s.place_building(0, 0, true));
  assert(s.level.buildings.empty());
  assert(s.undo());
  assert(s.level.buildings[0].id == id);
  auto prior = s.terrain_revision;
  s.select_tool(TerrainEditorTool::Terrain);
  assert(s.paint(15.5f, 15.5f));
  assert(s.terrain_revision > prior && !s.can_place_building(-12, -12));
  assert(s.undo());
  assert(s.terrain_revision > prior + 1);
  s.rotate_building(-100);
  assert(s.building_yaw >= 0 && s.building_yaw < 6.28318530718f);
  std::remove("building-check.level");
  std::remove("building-bad.level");
  std::remove("building-v2.level");
  std::cout << "Building placement CPU checks passed\n";
}
