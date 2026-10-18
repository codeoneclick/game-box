#include "terrain_editor_state.h"
#include "ocean_wave_model.h"
#include <algorithm>
#include <cctype>
#include <chrono>
#include <cmath>
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <iostream>
#include <limits>
#include <set>
#include <stdexcept>
#ifdef _WIN32
#define NOMINMAX
#include <Windows.h>
#endif
namespace game {
namespace {
void unite(TerrainEditorDirty &a, const TerrainEditorDirty &b) {
  if (b.empty())
    return;
  if (a.empty()) {
    a = b;
    return;
  }
  a.x0 = std::min(a.x0, b.x0);
  a.z0 = std::min(a.z0, b.z0);
  a.x1 = std::max(a.x1, b.x1);
  a.z1 = std::max(a.z1, b.z1);
  a.terrain |= b.terrain;
  a.grass |= b.grass;
  a.trees |= b.trees;
  a.climate |= b.climate;
  a.buildings |= b.buildings;
  a.stairs |= b.stairs;
}
EditorCellValue cell(const TerrainEditorLevel &l, size_t i) {
  EditorCellValue v;
  v.level = l.terrain.layers[i];
  v.density = l.grass_density[i];
  v.grass = l.grass_type[i];
  v.climate = l.climate[i];
  return v;
}
void put(TerrainEditorLevel &l, size_t i, const EditorCellValue &v) {
  l.terrain.layers[i] = v.level;
  l.grass_density[i] = v.density;
  l.grass_type[i] = v.grass;
  l.climate[i] = v.climate;
}
bool same(const EditorCellValue &a, const EditorCellValue &b) {
  return a.level == b.level && a.density == b.density && a.grass == b.grass &&
         a.climate == b.climate;
}
size_t chunk_policy_count(const terrain_config &c) {
  const size_t side = static_cast<size_t>((c.resolution + 15) / 16);
  return side * side;
}
void mark_remeshing(TerrainEditorLevel &level,
                    const TerrainEditorDirty &dirty) {
  const int n = level.terrain.config.resolution, side = (n + 15) / 16;
  const size_t count = chunk_policy_count(level.terrain.config);
  if (level.terrain_remeshed.size() != count)
    level.terrain_remeshed.assign(count, 0);
  if (level.grass_remeshed.size() != count)
    level.grass_remeshed.assign(count, 0);
  if (dirty.empty() || (!dirty.terrain && !dirty.grass))
    return;
  const int x0 = std::max(0, dirty.x0 - terrain_editor_rebuild_halo) / 16,
            z0 = std::max(0, dirty.z0 - terrain_editor_rebuild_halo) / 16;
  const int x1 = std::min(n - 1, dirty.x1 + terrain_editor_rebuild_halo) / 16,
            z1 = std::min(n - 1, dirty.z1 + terrain_editor_rebuild_halo) / 16;
  for (int z = z0; z <= z1; ++z)
    for (int x = x0; x <= x1; ++x) {
      const size_t i = static_cast<size_t>(z) * side + x;
      if (dirty.terrain)
        level.terrain_remeshed[i] = 1;
      if (dirty.terrain || dirty.grass)
        level.grass_remeshed[i] = 1;
    }
}
void require(bool b, const char *message) {
  if (!b)
    throw std::runtime_error(message);
}
float level_zero_clearance() {
  // CPU, GLSL and Metal bound the wave sum with limit*tanh(raw/limit).
  // Default demo amplitudes therefore guarantee |height-mean| < .73466m.
  static const float clearance = []() {
    ocean_settings ocean;
    float amplitudes = 0;
    for (const auto &wave : ocean.waves)
      amplitudes += wave.amplitude;
    return std::max(.001f, 1.09f * amplitudes) + .08f;
  }();
  return clearance;
}
void valid_config(const terrain_config &c) {
  require(c.resolution >= 2 && c.resolution <= 2048, "Invalid resolution");
  require(c.layers_count >= 1 && c.layers_count <= 16 &&
              c.underwater_layers_count == 8,
          "Invalid terrace count");
  require(std::isfinite(c.world_size) && c.world_size > 0 &&
              c.world_size / c.resolution >= .0001f && c.world_size <= 100000 &&
              std::isfinite(c.island_size) && c.island_size > 0 &&
              c.island_size <= c.world_size,
          "Invalid world size");
  require(std::isfinite(c.layer_height) && c.layer_height > 0 &&
              c.layer_height <= 1000 && std::isfinite(c.shore_height) &&
              c.shore_height >= 0 && c.shore_height < c.layer_height,
          "Invalid terrace height");
  require(std::isfinite(c.water_y) && std::abs(c.water_y) <= 100000 &&
              c.water_y - level_zero_clearance() > -c.layer_height &&
              c.water_y < c.shore_height && std::isfinite(c.water_size) &&
              c.water_size >= c.world_size && c.water_size <= 1000000 &&
              std::isfinite(c.underwater_terrace_width) &&
              c.underwater_terrace_width > 0,
          "Invalid water configuration");
}
TerrainEditorDirty stairs_dirty(const EditorStairs& stairs,const terrain_config& config){
 TerrainEditorDirty d;const float cell=config.world_size/config.resolution;
 const float radius=(stairs_width+stairs_run)*.5f+cell*terrain_editor_rebuild_halo;
 auto index=[&](float v){return std::max(0,std::min(config.resolution-1,int(std::floor((v/config.world_size+.5f)*config.resolution))));};
 d.x0=index(stairs.position[0]-radius);d.x1=index(stairs.position[0]+radius);
 d.z0=index(stairs.position[2]-radius);d.z1=index(stairs.position[2]+radius);
 d.stairs=d.grass=true;return d;
}
void valid_level(const TerrainEditorLevel &l) {
  valid_config(l.terrain.config);
  size_t n = size_t(l.terrain.config.resolution) * l.terrain.config.resolution;
  require(l.terrain.layers.size() == n && l.grass_density.size() == n &&
              l.grass_type.size() == n && l.climate.size() == n,
          "Invalid map sizes");
  for (size_t i = 0; i < n; ++i)
    require(l.terrain.layers[i] >= -9 && l.terrain.layers[i] <= 16 &&
                l.grass_type[i] < 3 && l.climate[i] < 7,
            "Invalid map value");
  require(l.trees.size() <= 1000000 && l.next_tree_id > 0,
          "Invalid tree count");
  require(l.terrain_remeshed.size() == chunk_policy_count(l.terrain.config) &&
              l.grass_remeshed.size() == chunk_policy_count(l.terrain.config),
          "Invalid reconstruction policy sizes");
  for (auto value : l.terrain_remeshed)
    require(value <= 1, "Invalid terrain reconstruction policy");
  for (auto value : l.grass_remeshed)
    require(value <= 1, "Invalid grass reconstruction policy");
  std::set<uint32_t> ids;
  for (const auto &t : l.trees) {
    require(t.id > 0 && t.id < l.next_tree_id && ids.insert(t.id).second &&
                int(t.type) >= 0 && int(t.type) < tree_species_count,
            "Invalid tree identity");
    for (float p : t.position)
      require(std::isfinite(p) && std::abs(p) <= 1000000,
              "Invalid tree position");
    require(std::abs(t.position[0]) <= l.terrain.config.world_size * .5f &&
                std::abs(t.position[2]) <= l.terrain.config.world_size * .5f &&
                std::isfinite(t.yaw) && std::isfinite(t.scale) && t.scale > 0 &&
                t.scale <= 100,
            "Invalid tree transform");
  }
  require(l.buildings.size() <= 1000000 && l.next_building_id > 0,
          "Invalid building count");
  ids.clear();
  for (const auto &b : l.buildings) {
    require(b.id > 0 && b.id < l.next_building_id && ids.insert(b.id).second &&
                int(b.type) >= 0 && int(b.type) < 3,
            "Invalid building identity");
    for (auto v : b.position)
      require(std::isfinite(v) && std::abs(v) <= 1000000,
              "Invalid building position");
    require(b.terrace > 0 && b.terrace <= 16 && std::isfinite(b.yaw) &&
                b.yaw >= 0 && b.yaw < 6.283185307179586f &&
                std::abs(b.position[0]) <= l.terrain.config.world_size * .5f &&
                std::abs(b.position[2]) <= l.terrain.config.world_size * .5f &&
                std::abs(b.position[1] -
                         editor_level_height(l.terrain.config, b.terrace)) <
                    .0001f,
            "Invalid building transform");
  }
  require(l.stairs.size()<=100000 && l.next_stairs_id>0,"Invalid stair count");
  ids.clear();
  std::map<std::pair<int,int>,std::vector<const EditorStairs*>> occupied_stairs;
  for(const auto& stair:l.stairs){
    require(stair.id>0&&stair.id<l.next_stairs_id&&ids.insert(stair.id).second,"Invalid stair identity");
    for(float v:stair.position)require(std::isfinite(v)&&std::abs(v)<=1000000,"Invalid stair position");
    require(stair.lower_terrace>=1&&stair.lower_terrace<16&&std::isfinite(stair.yaw)&&stair.yaw>=0&&stair.yaw<6.283185307179586f&&
      std::abs(stair.position[0])<=l.terrain.config.world_size*.5f&&std::abs(stair.position[2])<=l.terrain.config.world_size*.5f&&
      std::abs(stair.position[1]-editor_level_height(l.terrain.config,stair.lower_terrace))<.0001f,"Invalid stair transform");
    int sx=int(std::floor(stair.position[0]/8)),sz=int(std::floor(stair.position[2]/8));
    for(int z=sz-1;z<=sz+1;++z)for(int x=sx-1;x<=sx+1;++x){auto found=occupied_stairs.find({x,z});if(found!=occupied_stairs.end())
      for(const auto* other:found->second)require(!stairs_overlap(stair,*other),"Overlapping stairs");}
    occupied_stairs[{sx,sz}].push_back(&stair);
  }
}
FILE *open_file(const std::string &path, bool write) {
#ifdef _WIN32
  int n = MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, path.c_str(), -1,
                              nullptr, 0);
  if (n <= 0)
    return nullptr;
  std::vector<wchar_t> wide(n);
  MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, path.c_str(), -1,
                      wide.data(), n);
  return _wfopen(wide.data(), write ? L"wb" : L"rb");
#else
  return std::fopen(path.c_str(), write ? "wb" : "rb");
#endif
}
bool replace_file(const std::string &temporary, const std::string &path) {
#ifdef _WIN32
  auto wide = [](const std::string &s) {
    int n = MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, s.c_str(), -1,
                                nullptr, 0);
    std::vector<wchar_t> result(n > 0 ? n : 1, 0);
    if (n > 0)
      MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, s.c_str(), -1,
                          result.data(), n);
    return result;
  };
  auto from = wide(temporary), to = wide(path);
  return MoveFileExW(from.data(), to.data(),
                     MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH) != 0;
#else
  return std::rename(temporary.c_str(), path.c_str()) == 0;
#endif
}
void remove_temporary(const std::string &path) {
#ifdef _WIN32
  int n = MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, path.c_str(), -1,
                              nullptr, 0);
  if (n <= 0)
    return;
  std::vector<wchar_t> wide(n);
  MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, path.c_str(), -1,
                      wide.data(), n);
  DeleteFileW(wide.data());
#else
  std::remove(path.c_str());
#endif
}
uint32_t crc(const std::vector<uint8_t> &bytes, size_t count) {
  uint32_t c = ~0u;
  for (size_t i = 0; i < count; ++i) {
    c ^= bytes[i];
    for (int b = 0; b < 8; ++b)
      c = (c >> 1) ^ (0xedb88320u & uint32_t(-int(c & 1)));
  }
  return ~c;
}
struct Writer {
  // GBLV v2: explicit little-endian 32-bit values, IEEE float bits, raw maps.
  // Header/config/maps/trees/editor selections are followed by payload CRC32.
  // No host struct padding, native enum width or GPU data enters the format.
  std::vector<uint8_t> b;
  void u(uint32_t n) {
    for (int i = 0; i < 4; ++i)
      b.push_back(uint8_t(n >> (i * 8)));
  }
  void f(float v) {
    uint32_t n;
    std::memcpy(&n, &v, 4);
    u(n);
  }
  void bytes(const std::vector<uint8_t> &v) {
    b.insert(b.end(), v.begin(), v.end());
  }
};
struct Reader {
  const std::vector<uint8_t> &b;
  size_t p = 0;
  uint32_t u() {
    require(p + 4 <= b.size(), "Truncated level");
    uint32_t n = 0;
    for (int i = 0; i < 4; ++i)
      n |= uint32_t(b[p++]) << (8 * i);
    return n;
  }
  float f() {
    auto n = u();
    float v;
    std::memcpy(&v, &n, 4);
    return v;
  }
  std::vector<uint8_t> bytes(size_t n) {
    require(n <= b.size() - p, "Truncated map");
    std::vector<uint8_t> v(b.begin() + p, b.begin() + p + n);
    p += n;
    return v;
  }
};
} // namespace
const TerrainToolUIConfig &terrain_tool_ui_config(TerrainEditorTool tool) {
  static const TerrainToolUIConfig table[] = {
      {true, true, false, true, false, false, 0, "Terrain Tool"},
      {true, true, true, false, true, false, 3, "Grass Tool"},
      {false, false, false, false, false, true, 0, "Trees"}};
  int i = int(tool);
  static const TerrainToolUIConfig buildings = {
      false, false, false, false, false, false, 0, "Buildings"};
  static const TerrainToolUIConfig stairs = {false,false,false,false,false,false,0,"Stairs"};
  if(tool==TerrainEditorTool::Stairs)return stairs;
  return tool == TerrainEditorTool::Buildings ? buildings
                                              : table[i >= 0 && i < 3 ? i : 0];
}
float editor_level_height(const terrain_config &c, int l) {
  return l == 0   ? c.water_y - level_zero_clearance()
         : l == 1 ? c.shore_height
         : l > 1  ? (l - 1) * c.layer_height
                  : l * c.layer_height;
}
std::array<float, 4> editor_climate_color(ClimateType climate, int l) {
  static const float original[8][3] = {
      {194, 141, 64}, {113, 132, 31}, {74, 122, 24},   {41, 108, 21},
      {55, 25, 10},   {77, 83, 83},   {100, 106, 108}, {128, 134, 138}};
  std::array<float, 4> c = {{177 / 255.f, 163 / 255.f, 118 / 255.f, 1}};
  if (l > 0) {
    for (int k = 0; k < 3; ++k)
      c[k] = l <= 8
                 ? original[l - 1][k] / 255.f
                 : (original[7][k] + (240 - original[7][k]) * (l - 8) / 8.f) /
                       255.f;
  }
  static const float tint[7][3] = {{1, 1, 1},           {1.10f, 1.03f, .82f},
                                   {.80f, 1.18f, .79f}, {.72f, .90f, .85f},
                                   {.86f, .88f, .78f},  {1, 1.08f, 1.20f},
                                   {1.30f, 1.03f, .60f}};
  int t = std::max(0, std::min(6, int(climate)));
  for (int k = 0; k < 3; ++k) {
    c[k] = std::min(1.f, c[k] * tint[t][k]);
    if (t == 5 && l > 0)
      c[k] = c[k] * .25f + .72f;
  }
  return c;
}
void TerrainEditorState::initialize(const terrain_data &d) {
  valid_config(d.config);
  require(d.layers.size() == size_t(d.config.resolution) * d.config.resolution,
          "Invalid initial terrain");
  level = TerrainEditorLevel();
  level.terrain = d;
  level.grass_density.assign(d.layers.size(), 0);
  level.grass_type.assign(d.layers.size(), 0);
  level.climate.assign(d.layers.size(), 0);
  level.terrain_remeshed.assign(chunk_policy_count(d.config), 0);
  level.grass_remeshed.assign(chunk_policy_count(d.config), 0);
  for (size_t i = 0; i < d.layers.size(); ++i) {
    int v = d.layers[i];
    require(v >= -9 && v < 8, "Invalid generated terrain level");
    if (v >= 0)
      level.terrain.layers[i] = int8_t(v + 1);
    if (v >= 1 && v <= 4)
      level.grass_density[i] = 255;
  }
  undo_.clear();
  redo_.clear();
  stroke_cells_.clear();
  stroke_ = TerrainEditCommand();
  stroking_ = false;
  current_id_ = saved_id_ = 0;
  next_id_ = 1;
  is_dirty = false;
  current_level_path.clear();
  error.clear();
  pending_ = TerrainEditorDirty();
  building_surface.reset();
  ++terrain_revision;
  invalidate_all();
}
int TerrainEditorState::cell_at(float x, float z) const {
  if (!std::isfinite(x) || !std::isfinite(z))
    return -1;
  const auto &c = level.terrain.config;
  float half = c.world_size * .5f;
  if (x < -half || z < -half || x >= half || z >= half)
    return -1;
  int ix = int(std::floor((x / c.world_size + .5f) * c.resolution)),
      iz = int(std::floor((z / c.world_size + .5f) * c.resolution));
  return ix < 0 || iz < 0 || ix >= c.resolution || iz >= c.resolution
             ? -1
             : iz * c.resolution + ix;
}
int TerrainEditorState::level_at(float x, float z) const {
  int i = cell_at(x, z);
  return i >= 0 && size_t(i) < level.terrain.layers.size()
             ? level.terrain.layers[i]
             : terrain_data::empty_layer;
}
void TerrainEditorState::select_tool(TerrainEditorTool t) {
  end_stroke();
  current_tool = t;
}
void TerrainEditorState::set_target_terrace(int target) {
  end_stroke();
  target_terrace = std::max(-5, std::min(16, target));
}
void TerrainEditorState::begin_stroke() {
  if (stroking_)
    return;
  stroking_ = true;
  stroke_ = TerrainEditCommand();
  stroke_cells_.clear();
}
bool TerrainEditorState::paint(float x, float z, bool erase) {
  if (current_tool == TerrainEditorTool::Trees)
    return place_tree(x, z, erase);
  if (current_tool == TerrainEditorTool::Buildings)
    return place_building(x, z, erase);
  if (!std::isfinite(x) || !std::isfinite(z) || !std::isfinite(brush_size) ||
      brush_size <= 0)
    return false;
  const float edge = level.terrain.config.world_size * .5f + 16.f;
  if (std::abs(x) > edge || std::abs(z) > edge)
    return false;
  bool automatic = !stroking_;
  begin_stroke();
  bool changed = false;
  int n = level.terrain.config.resolution;
  float step = level.terrain.config.world_size / n,
        r = std::min(32.f, brush_size) * .5f,
        half = level.terrain.config.world_size * .5f;
  int x0 = std::max(0, int(std::floor((x - r + half) / step))),
      x1 = std::min(n - 1, int(std::floor((x + r + half) / step))),
      z0 = std::max(0, int(std::floor((z - r + half) / step))),
      z1 = std::min(n - 1, int(std::floor((z + r + half) / step)));
  for (int iz = z0; iz <= z1; ++iz)
    for (int ix = x0; ix <= x1; ++ix) {
      float dx = (ix + .5f) * step - half - x,
            dz = (iz + .5f) * step - half - z;
      if (dx * dx + dz * dz > r * r)
        continue;
      uint32_t i = iz * n + ix;
      auto old = cell(level, i), v = old;
      if (v.level == -9)
        continue;
      bool sculpt = current_tool == TerrainEditorTool::Terrain;
      bool occupied = false;
      for (const auto &b : level.buildings)
        if (point_inside_building(
                b, (ix + .5f) * step - half, (iz + .5f) * step - half,
                building_clearance +
                    (sculpt ? 1.414214f * (terrain_editor_rebuild_halo + .5f) *
                                  step
                            : .707107f * step))) {
          occupied = true;
          break;
        }
      if (occupied)
        continue;
      if (sculpt) {
        if (v.level <= -6)
          continue;
        const auto previous = stroke_cells_.find(i);
        const int original =
            previous == stroke_cells_.end()
                ? int(old.level)
                : int(stroke_.cells[previous->second].before.level);
        const int target = std::max(-5, std::min(16, target_terrace));
        // A flat inner half and smooth falloff keep integer terrace elevations.
        // Use the stroke's original height so repeated stamps cannot pump the
        // feathered edge to the target; a closer stamp can still finish a cell.
        const float t = std::max(
            0.f, std::min(1.f, 2.f * (1.f - std::sqrt(dx * dx + dz * dz) / r)));
        const float influence = t * t * (3.f - 2.f * t);
        const int candidate =
            int(std::round(original + (target - original) * influence));
        v.level =
            int8_t(original < target ? std::max(int(old.level), candidate)
                                     : std::min(int(old.level), candidate));
      } else if (current_tool == TerrainEditorTool::Grass) {
        v.density = erase ? 0
                          : uint8_t(std::round(
                                std::max(0.f, std::min(1.f, strength)) * 255));
        v.grass = uint8_t(selected_grass);
      }
      if (same(old, v))
        continue;
      auto found = stroke_cells_.find(i);
      if (found == stroke_cells_.end()) {
        EditorCellChange ch;
        ch.index = i;
        ch.before = old;
        ch.after = v;
        stroke_cells_[i] = stroke_.cells.size();
        stroke_.cells.push_back(ch);
      } else
        stroke_.cells[found->second].after = v;
      put(level, i, v);
      TerrainEditorDirty d;
      d.x0 = d.x1 = ix;
      d.z0 = d.z1 = iz;
      d.terrain = sculpt;
      d.grass = sculpt || current_tool == TerrainEditorTool::Grass;
      d.trees = sculpt;
      unite(stroke_.dirty, d);
      unite(pending_, d);
      changed = true;
    }
  if (changed && current_tool == TerrainEditorTool::Terrain) {
    const auto& config=level.terrain.config;const float cell=config.world_size/config.resolution;
    for(auto it=level.stairs.begin();it!=level.stairs.end();){
      bool affected=false;
      for(const auto& change:stroke_.cells)if(change.before.level!=change.after.level){
        float x=(change.index%config.resolution+.5f)*cell-config.world_size*.5f;
        float z=(change.index/config.resolution+.5f)*cell-config.world_size*.5f;
        if(point_inside_stairs(*it,x,z,cell*(terrain_editor_rebuild_halo+1))){affected=true;break;}
      }
      if(affected){TerrainEditCommand::StairsChange change;change.had_before=true;change.before=*it;stroke_.stairs.push_back(change);
        const auto d=stairs_dirty(*it,config);unite(stroke_.dirty,d);unite(pending_,d);it=level.stairs.erase(it);
      }else ++it;
    }

    for (auto &tree : level.trees) {
      int i = cell_at(tree.position[0], tree.position[2]);
      if (i < 0 || !stroke_cells_.count(uint32_t(i)))
        continue;
      float h =
          editor_level_height(level.terrain.config, level.terrain.layers[i]);
      if (h == tree.position[1])
        continue;
      auto found = std::find_if(stroke_.trees.begin(), stroke_.trees.end(),
                                [&](const EditorTreeChange &c) {
                                  return c.has_after && c.after.id == tree.id;
                                });
      if (found == stroke_.trees.end()) {
        EditorTreeChange c;
        c.had_before = c.has_after = true;
        c.before = c.after = tree;
        c.after.position[1] = h;
        stroke_.trees.push_back(c);
      } else
        found->after.position[1] = h;
      tree.position[1] = h;
    }
  }
  if (changed) {
    if (current_tool == TerrainEditorTool::Terrain)
      ++terrain_revision;
    ++revision;
    is_dirty = true;
  }
  if (automatic)
    end_stroke();
  return changed;
}
bool TerrainEditorState::place_tree(float x, float z, bool erase) {
  int i = cell_at(x, z);
  if (i < 0 || (!erase && level_at(x, z) <= 0))
    return false;
  if (!erase)
    for (const auto &b : level.buildings)
      if (point_inside_building(b, x, z))
        return false;
  if (!erase)
    for (const auto &stairs : level.stairs)
      if (point_inside_stairs(stairs, x, z, .4f)) return false;
  bool automatic = !stroking_;
  begin_stroke();
  EditorTreeChange c;
  if (erase) {
    float best = std::max(1.5f, brush_size * .5f);
    best *= best;
    auto found = level.trees.end();
    for (auto t = level.trees.begin(); t != level.trees.end(); ++t) {
      float dx = t->position[0] - x, dz = t->position[2] - z,
            d = dx * dx + dz * dz;
      if (d <= best) {
        best = d;
        found = t;
      }
    }
    if (found == level.trees.end()) {
      if (automatic)
        end_stroke();
      return false;
    }
    c.had_before = true;
    c.before = *found;
    level.trees.erase(found);
  } else {
    if (level.next_tree_id == std::numeric_limits<uint32_t>::max()) {
      error = "Tree identity limit reached";
      if (automatic)
        end_stroke();
      return false;
    }
    c.has_after = true;
    c.after.id = level.next_tree_id++;
    c.after.type = selected_tree;
    c.after.position = {
        {x, editor_level_height(level.terrain.config, level_at(x, z)), z}};
    c.after.yaw = float((c.after.id * 2654435761u) % 6283) * .001f;
    level.trees.push_back(c.after);
  }
  stroke_.trees.push_back(c);
  const auto &t = erase ? c.before : c.after;
  int ti = cell_at(t.position[0], t.position[2]);
  TerrainEditorDirty d;
  d.x0 = d.x1 = ti % level.terrain.config.resolution;
  d.z0 = d.z1 = ti / level.terrain.config.resolution;
  d.trees = true;
  unite(stroke_.dirty, d);
  unite(pending_, d);
  ++revision;
  is_dirty = true;
  if (automatic)
    end_stroke();
  return true;
}
bool TerrainEditorState::stairs_preview(float x,float z,EditorStairs& candidate) const {
 candidate=EditorStairs();candidate.position={{x,editor_level_height(level.terrain.config,std::max(1,level_at(x,z))),z}};
 if(!building_surface||building_surface_revision!=terrain_revision)return false;
 if(!find_stairs_site(*building_surface,level.terrain.config,x,z,candidate))return false;
 for(const auto& stairs:level.stairs)if(stairs_overlap(candidate,stairs))return false;
 for(const auto& building:level.buildings)if(stairs_building_overlap(candidate,building))return false;
 for(const auto& tree:level.trees)if(point_inside_stairs(candidate,tree.position[0],tree.position[2],.35f))return false;
 return true;
}
bool TerrainEditorState::place_stairs(float x,float z,bool erase){
 TerrainEditCommand::StairsChange change;
 if(erase){
  auto found=std::find_if(level.stairs.begin(),level.stairs.end(),[&](const EditorStairs& stairs){return point_inside_stairs(stairs,x,z,.2f);});
  if(found==level.stairs.end())return false;change.had_before=true;change.before=*found;
 }else{
  if(level.next_stairs_id==std::numeric_limits<uint32_t>::max()||!stairs_preview(x,z,change.after))return false;
  change.has_after=true;change.after.id=level.next_stairs_id;
 }
 end_stroke();begin_stroke();
 const auto stairs=erase?change.before:change.after;
 if(erase)level.stairs.erase(std::remove_if(level.stairs.begin(),level.stairs.end(),[&](const EditorStairs& value){return value.id==stairs.id;}),level.stairs.end());
 else{level.stairs.push_back(stairs);++level.next_stairs_id;}
 stroke_.stairs.push_back(change);const auto d=stairs_dirty(stairs,level.terrain.config);unite(stroke_.dirty,d);unite(pending_,d);
 ++revision;is_dirty=true;error.clear();end_stroke();
 if(std::getenv("GB_EDITOR_PROFILE"))std::cout<<"Editor stairs "<<(erase?"removed":"placed")<<" id="<<stairs.id<<" lower="<<stairs.lower_terrace<<" x="<<stairs.position[0]<<" z="<<stairs.position[2]<<std::endl;
 return true;
}
bool TerrainEditorState::can_place_building(float x, float z) const {
  if (!building_surface || building_surface_revision != terrain_revision ||
      int(selected_building) < 0 || int(selected_building) >= 3 ||
      !std::isfinite(building_yaw))
    return false;
  int terrace = level_at(x, z);
  if (terrace <= 0)
    return false;
  EditorBuilding b;
  b.type = selected_building;
  b.position = {{x, editor_level_height(level.terrain.config, terrace), z}};
  b.yaw = building_yaw;
  b.terrace = terrace;
  for(const auto& stairs:level.stairs)if(stairs_building_overlap(stairs,b))return false;
  for (const auto &other : level.buildings)
    if (buildings_overlap(b, other))
      return false;
  const auto &d = building_definition(b.type);
  return building_surface->covers(
      x, z, b.yaw, d.half_width + building_clearance,
      d.half_depth + building_clearance, b.position[1]);
}
void TerrainEditorState::rotate_building(float delta) {
  if (!std::isfinite(delta) || !std::isfinite(building_yaw))
    return;
  constexpr double radians_per_degree = 3.14159265358979323846 / 180.;
  const double angle = std::fmod(double(building_yaw) + double(delta),
                                360. * radians_per_degree);
  set_building_rotation_degrees(float(angle / radians_per_degree));
}
void TerrainEditorState::set_building_rotation_degrees(float degrees) {
  if (!std::isfinite(degrees))
    return;
  double angle = std::fmod(double(degrees), 360.);
  if (angle < 0)
    angle += 360.;
  if (building_rotation_snap == 15 || building_rotation_snap == 45 ||
      building_rotation_snap == 90)
    angle = std::round(angle / building_rotation_snap) * building_rotation_snap;
  constexpr double radians_per_degree = 3.14159265358979323846 / 180.;
  building_yaw = normalized_building_yaw(float(angle * radians_per_degree));
}
void TerrainEditorState::set_building_rotation_snap(int degrees) {
  if (degrees != 0 && degrees != 15 && degrees != 45 && degrees != 90)
    return;
  building_rotation_snap = degrees;
  if (degrees != 0) {
    constexpr double degrees_per_radian = 180. / 3.14159265358979323846;
    set_building_rotation_degrees(float(building_yaw * degrees_per_radian));
  }
}
bool TerrainEditorState::place_building(float x, float z, bool erase) {
  // Validation precedes history changes so rejected clicks are entirely inert.
  EditorBuildingChange change;
  if (erase) {
    auto it = std::find_if(level.buildings.begin(), level.buildings.end(),
                           [&](const EditorBuilding &b) {
                             return point_inside_building(b, x, z);
                           });
    if (it == level.buildings.end())
      return false;
    change.had_before = true;
    change.before = *it;
  } else {
    if (!can_place_building(x, z) ||
        level.next_building_id == std::numeric_limits<uint32_t>::max())
      return false;
    change.has_after = true;
    auto &b = change.after;
    b.id = level.next_building_id;
    b.type = selected_building;
    b.terrace = level_at(x, z);
    b.position = {{x, editor_level_height(level.terrain.config, b.terrace), z}};
    b.yaw = normalized_building_yaw(building_yaw);
  }
  end_stroke();
  begin_stroke();
  const auto &b = erase ? change.before : change.after;
  int n = level.terrain.config.resolution;
  float step = level.terrain.config.world_size / n,
        half = level.terrain.config.world_size * .5f;
  TerrainEditorDirty dirty;
  dirty.buildings = true;
  int center = cell_at(b.position[0], b.position[2]);
  dirty.x0 = dirty.x1 = center % n;
  dirty.z0 = dirty.z1 = center / n;
  if (erase)
    level.buildings.erase(std::remove_if(level.buildings.begin(),
                                         level.buildings.end(),
                                         [&](const EditorBuilding &other) {
                                           return other.id == b.id;
                                         }),
                          level.buildings.end());
  else {
    level.buildings.push_back(b);
    ++level.next_building_id;
    for (int iz = 0; iz < n; ++iz)
      for (int ix = 0; ix < n; ++ix)
        if (point_inside_building(b, (ix + .5f) * step - half,
                                  (iz + .5f) * step - half,
                                  building_clearance + step * .707107f)) {
          uint32_t i = iz * n + ix;
          auto old = cell(level, i);
          if (old.density) {
            EditorCellChange c;
            c.index = i;
            c.before = c.after = old;
            c.after.density = 0;
            stroke_.cells.push_back(c);
            put(level, i, c.after);
            dirty.grass = true;
            dirty.x0 = std::min(dirty.x0, ix);
            dirty.x1 = std::max(dirty.x1, ix);
            dirty.z0 = std::min(dirty.z0, iz);
            dirty.z1 = std::max(dirty.z1, iz);
          }
        }
    for (auto it = level.trees.begin(); it != level.trees.end();)
      if (point_inside_building(b, it->position[0], it->position[2])) {
        EditorTreeChange c;
        c.had_before = true;
        c.before = *it;
        stroke_.trees.push_back(c);
        // Tree batches belong to their own cells, even when the footprint has
        // no grass and its center lies in a different reconstruction chunk.
        const int tree_cell = cell_at(it->position[0], it->position[2]);
        if (tree_cell >= 0) {
          dirty.x0 = std::min(dirty.x0, tree_cell % n);
          dirty.x1 = std::max(dirty.x1, tree_cell % n);
          dirty.z0 = std::min(dirty.z0, tree_cell / n);
          dirty.z1 = std::max(dirty.z1, tree_cell / n);
        }
        it = level.trees.erase(it);
        dirty.trees = true;
      } else
        ++it;
  }
  stroke_.buildings.push_back(change);
  unite(stroke_.dirty, dirty);
  unite(pending_, dirty);
  ++revision;
  is_dirty = true;
  end_stroke();
  return true;
}
void TerrainEditorState::end_stroke() {
  if (!stroking_)
    return;
  stroking_ = false;
  // Save callbacks run before the next mesh update. Persist the intended local
  // reconstruction policy now, including the contour filter's rebuild halo.
  if (stroke_.dirty.terrain || stroke_.dirty.grass)
    mark_remeshing(level, stroke_.dirty);
  stroke_.cells.erase(std::remove_if(stroke_.cells.begin(), stroke_.cells.end(),
                                     [](const EditorCellChange &c) {
                                       return same(c.before, c.after);
                                     }),
                      stroke_.cells.end());
  if (!stroke_.stairs.empty() || !stroke_.cells.empty() || !stroke_.trees.empty() ||
      !stroke_.buildings.empty()) {
    // Unique history identities distinguish a new branch from an old saved
    // command even when both happen to occupy the same undo-stack depth.
    stroke_.before_id = current_id_;
    stroke_.after_id = next_id_++;
    current_id_ = stroke_.after_id;
    undo_.push_back(std::move(stroke_));
    redo_.clear();
  }
  stroke_ = TerrainEditCommand();
  stroke_cells_.clear();
  is_dirty = current_id_ != saved_id_;
}
void TerrainEditorState::apply_command(const TerrainEditCommand &c,
                                       bool forward) {
  for (const auto &v : c.cells)
    put(level, v.index, forward ? v.after : v.before);
  auto tree = [&](const EditorTreeChange &v) {
    const EditorTree *remove = forward ? (v.had_before ? &v.before : nullptr)
                                       : (v.has_after ? &v.after : nullptr);
    const EditorTree *add = forward ? (v.has_after ? &v.after : nullptr)
                                    : (v.had_before ? &v.before : nullptr);
    if (remove)
      level.trees.erase(std::remove_if(level.trees.begin(), level.trees.end(),
                                       [&](const EditorTree &t) {
                                         return t.id == remove->id;
                                       }),
                        level.trees.end());
    if (add)
      level.trees.push_back(*add);
  };
  if (forward)
    for (const auto &t : c.trees)
      tree(t);
  else
    for (auto t = c.trees.rbegin(); t != c.trees.rend(); ++t)
      tree(*t);
  auto building = [&](const EditorBuildingChange &v) {
    const auto *remove = forward ? (v.had_before ? &v.before : nullptr)
                                 : (v.has_after ? &v.after : nullptr);
    const auto *add = forward ? (v.has_after ? &v.after : nullptr)
                              : (v.had_before ? &v.before : nullptr);
    if (remove)
      level.buildings.erase(std::remove_if(level.buildings.begin(),
                                           level.buildings.end(),
                                           [&](const EditorBuilding &b) {
                                             return b.id == remove->id;
                                           }),
                            level.buildings.end());
    if (add)
      level.buildings.push_back(*add);
  };
  if (forward)
    for (const auto &b : c.buildings)
      building(b);
  else
    for (auto b = c.buildings.rbegin(); b != c.buildings.rend(); ++b)
      building(*b);
  auto stair=[&](const TerrainEditCommand::StairsChange& v){
    const auto* remove=forward?(v.had_before?&v.before:nullptr):(v.has_after?&v.after:nullptr);
    const auto* add=forward?(v.has_after?&v.after:nullptr):(v.had_before?&v.before:nullptr);
    if(remove)level.stairs.erase(std::remove_if(level.stairs.begin(),level.stairs.end(),[&](const EditorStairs& s){return s.id==remove->id;}),level.stairs.end());
    if(add)level.stairs.push_back(*add);
  };
  if(forward)for(const auto& v:c.stairs)stair(v);else for(auto i=c.stairs.rbegin();i!=c.stairs.rend();++i)stair(*i);
  unite(pending_, c.dirty);
  if (c.dirty.terrain)
    ++terrain_revision;
  ++revision;
}
bool TerrainEditorState::undo() {
  end_stroke();
  if (undo_.empty())
    return false;
  auto c = std::move(undo_.back());
  undo_.pop_back();
  apply_command(c, false);
  current_id_ = c.before_id;
  redo_.push_back(std::move(c));
  is_dirty = current_id_ != saved_id_;
  return true;
}
bool TerrainEditorState::redo() {
  end_stroke();
  if (redo_.empty())
    return false;
  auto c = std::move(redo_.back());
  redo_.pop_back();
  apply_command(c, true);
  current_id_ = c.after_id;
  undo_.push_back(std::move(c));
  is_dirty = current_id_ != saved_id_;
  return true;
}
TerrainEditorDirty TerrainEditorState::consume_dirty() {
  auto d = pending_;
  // Initialization/load invalidation rebuilds existing policy; it is not an
  // edit.
  if (stroking_ && (stroke_.dirty.terrain || stroke_.dirty.grass))
    mark_remeshing(level, stroke_.dirty);
  pending_ = TerrainEditorDirty();
  return d;
}
void TerrainEditorState::invalidate_all() {
  TerrainEditorDirty d;
  d.x1 = d.z1 = level.terrain.config.resolution - 1;
  d.terrain = d.grass = d.trees = d.climate = d.buildings = d.stairs = true;
  unite(pending_, d);
  ++revision;
}
bool TerrainEditorState::save(const std::string &input) {
  end_stroke();
  FILE *file = nullptr;
  std::string temporary;
  try {
    require(!input.empty(), "Empty level path");
    valid_level(level);
    require((int(current_tool) >= 0 && int(current_tool) <= 4) &&
                int(selected_building) >= 0 && int(selected_building) < 3 &&
                std::isfinite(building_yaw) && building_yaw >= 0 &&
                building_yaw < 6.283185307179586f && target_terrace >= -5 &&
                target_terrace <= 16 && int(selected_grass) >= 0 &&
                int(selected_grass) < 3 && int(selected_tree) >= 0 &&
                int(selected_tree) < tree_species_count && int(selected_climate) >= 0 &&
                int(selected_climate) < 7 && current_terrace >= -9 &&
                current_terrace <= 16 && std::isfinite(brush_size) &&
                brush_size >= 1 && brush_size <= 32 &&
                std::isfinite(strength) && strength >= 0 && strength <= 1,
            "Invalid editor settings");
    std::string path = input;
    std::string ext = path.size() >= 6 ? path.substr(path.size() - 6) : "";
    std::transform(ext.begin(), ext.end(), ext.begin(), [](char c) {
      return char(std::tolower(static_cast<unsigned char>(c)));
    });
    if (ext != ".level")
      path += ".level";
    Writer w;
    w.u(0x564c4247);
    w.u(4);
    const auto &c = level.terrain.config;
    w.f(c.world_size);
    w.f(c.island_size);
    w.u(c.resolution);
    w.u(c.layers_count);
    w.u(c.underwater_layers_count);
    w.f(c.underwater_terrace_width);
    w.f(c.layer_height);
    w.f(c.shore_height);
    w.u(c.seed);
    w.f(c.water_y);
    w.f(c.water_size);
    w.u(c.debug_layer_colors ? 1 : 0);
    w.u(uint32_t(level.terrain.layers.size()));
    for (auto v : level.terrain.layers)
      w.b.push_back(uint8_t(v));
    w.bytes(level.grass_density);
    w.bytes(level.grass_type);
    w.bytes(level.climate);
    w.u(level.next_tree_id);
    w.u(uint32_t(level.trees.size()));
    for (const auto &t : level.trees) {
      w.u(t.id);
      w.u(uint32_t(t.type));
      for (float v : t.position)
        w.f(v);
      w.f(t.yaw);
      w.f(t.scale);
    }
    w.u(static_cast<uint32_t>(level.terrain_remeshed.size()));
    w.bytes(level.terrain_remeshed);
    w.bytes(level.grass_remeshed);
    w.u(uint32_t(current_tool));
    w.f(brush_size);
    w.f(strength);
    w.u(uint32_t(current_terrace));
    w.u(uint32_t(selected_grass));
    w.u(uint32_t(selected_tree));
    w.u(uint32_t(selected_climate));
    w.u(uint32_t(target_terrace));
    w.u(uint32_t(selected_building));
    w.f(building_yaw);
    w.u(level.next_building_id);
    w.u(uint32_t(level.buildings.size()));
    for (const auto &b : level.buildings) {
      w.u(b.id);
      w.u(uint32_t(b.type));
      for (auto v : b.position)
        w.f(v);
      w.f(b.yaw);
      w.u(uint32_t(b.terrace));
    }
    w.u(level.next_stairs_id);w.u(static_cast<uint32_t>(level.stairs.size()));
    for(const auto& stairs:level.stairs){w.u(stairs.id);for(float v:stairs.position)w.f(v);w.f(stairs.yaw);w.u(uint32_t(stairs.lower_terrace));}
    w.u(crc(w.b, w.b.size()));
    temporary =
        path + ".tmp-" +
        std::to_string(
            std::chrono::steady_clock::now().time_since_epoch().count());
    file = open_file(temporary, true);
    require(file != nullptr, "Cannot open level for writing");
    require(std::fwrite(w.b.data(), 1, w.b.size(), file) == w.b.size(),
            "Cannot write level");
    int result = std::fclose(file);
    file = nullptr;
    require(result == 0, "Cannot finish level write");
    require(replace_file(temporary, path), "Cannot replace saved level");
    temporary.clear();
    current_level_path = path;
    saved_id_ = current_id_;
    is_dirty = false;
    error.clear();
    return true;
  } catch (const std::exception &e) {
    if (file)
      std::fclose(file);
    if (!temporary.empty())
      remove_temporary(temporary);
    error = e.what();
    return false;
  }
}
bool TerrainEditorState::load(const std::string &path) {
  FILE *file = nullptr;
  try {
    file = open_file(path, false);
    require(file != nullptr, "Cannot open level");
    require(std::fseek(file, 0, SEEK_END) == 0, "Cannot seek level");
    long length = std::ftell(file);
    require(length >= 12 && length <= 128 * 1024 * 1024,
            "Invalid level file size");
    std::rewind(file);
    std::vector<uint8_t> bytes(static_cast<size_t>(length));
    require(std::fread(bytes.data(), 1, bytes.size(), file) == bytes.size(),
            "Cannot read level");
    std::fclose(file);
    file = nullptr;
    Reader checksum{bytes};
    checksum.p = bytes.size() - 4;
    require(checksum.u() == crc(bytes, bytes.size() - 4),
            "Level checksum mismatch");
    Reader r{bytes};
    require(r.u() == 0x564c4247, "Invalid level magic");
    const auto version = r.u();
    require(version >= 1 && version <= 4, "Unsupported level version");
    TerrainEditorLevel temp;
    auto &c = temp.terrain.config;
    c.world_size = r.f();
    c.island_size = r.f();
    c.resolution = int(r.u());
    c.layers_count = int(r.u());
    c.underwater_layers_count = int(r.u());
    c.underwater_terrace_width = r.f();
    c.layer_height = r.f();
    c.shore_height = r.f();
    c.seed = r.u();
    c.water_y = r.f();
    c.water_size = r.f();
    auto debug = r.u();
    require(debug <= 1, "Invalid debug flag");
    c.debug_layer_colors = debug != 0;
    valid_config(c);
    size_t count = r.u();
    require(count == size_t(c.resolution) * c.resolution, "Invalid map count");
    auto layers = r.bytes(count);
    temp.terrain.layers.assign(layers.begin(), layers.end());
    temp.grass_density = r.bytes(count);
    temp.grass_type = r.bytes(count);
    temp.climate = r.bytes(count);
    temp.next_tree_id = r.u();
    size_t trees = r.u();
    require(trees <= 1000000 && trees <= (bytes.size() - r.p) / 28,
            "Invalid tree count");
    temp.trees.reserve(trees);
    for (size_t i = 0; i < trees; ++i) {
      EditorTree t;
      t.id = r.u();
      t.type = TreeType(r.u());
      for (float &v : t.position)
        v = r.f();
      t.yaw = r.f();
      t.scale = r.f();
      temp.trees.push_back(t);
    }
    size_t policies = r.u();
    require(policies == chunk_policy_count(c),
            "Invalid reconstruction policy count");
    temp.terrain_remeshed = r.bytes(policies);
    temp.grass_remeshed = r.bytes(policies);
    auto tool = r.u();
    float brush = r.f(), power = r.f();
    int terrace = int(r.u());
    auto grass = r.u(), tree = r.u(), climate = r.u();
    const int target =
        version >= 2
            ? int(r.u())
            : (terrace >= -5 && terrace <= 16
                   ? std::max(-5, std::min(16, terrace + (tool == 0 ? -1 : 1)))
                   : 8);
    require(tool < (version == 1   ? 5u
                    : version == 2 ? 4u
                                   : 5u) &&
                (version != 3 || tool != 3) && target >= -5 && target <= 16 &&
                grass < 3 && tree < tree_species_count && climate < 7 && terrace >= -9 &&
                terrace <= 16 && std::isfinite(brush) && brush >= 1 &&
                brush <= 32 && std::isfinite(power) && power >= 0 && power <= 1,
            "Invalid editor settings");
    uint32_t building_type = 0;
    float yaw = 0;
    if (version >= 3) {
      building_type = r.u();
      yaw = r.f();
      require(building_type < 3 && std::isfinite(yaw) && yaw >= 0 &&
                  yaw < 6.283185307179586f,
              "Invalid building selection");
      temp.next_building_id = r.u();
      auto count = r.u();
      require(count <= 1000000 && count <= (bytes.size() - r.p) / 28,
              "Invalid building count");
      temp.buildings.reserve(count);
      for (uint32_t i = 0; i < count; ++i) {
        EditorBuilding b;
        b.id = r.u();
        b.type = BuildingType(r.u());
        for (auto &v : b.position)
          v = r.f();
        b.yaw = r.f();
        b.terrace = int(r.u());
        temp.buildings.push_back(b);
      }
    }
    if(version>=4){
      temp.next_stairs_id=r.u();const auto count=r.u();
      require(count<=100000&&count<=(bytes.size()-r.p)/24,"Invalid stair count");
      temp.stairs.reserve(count);
      for(uint32_t i=0;i<count;++i){EditorStairs stairs;stairs.id=r.u();for(float& v:stairs.position)v=r.f();stairs.yaw=r.f();stairs.lower_terrace=int(r.u());temp.stairs.push_back(stairs);}
    }
    require(r.p == bytes.size() - 4, "Unexpected level payload");
    valid_level(temp);
    level = std::move(temp);
    // Retired Climate selections in existing files fall back to Terrain.
    const auto migrated_tool = version == 1 ? (tool <= 1 ? 0 : tool - 1) : tool;
    current_tool = (migrated_tool < 3 || version >= 3 && migrated_tool == 4 || version >= 4 && migrated_tool == 3)
                       ? TerrainEditorTool(migrated_tool)
                       : TerrainEditorTool::Terrain;
    selected_building = BuildingType(building_type);
    building_yaw = yaw;
    building_surface.reset();
    ++terrain_revision;
    target_terrace = target;
    brush_size = brush;
    strength = power;
    current_terrace = terrace;
    selected_grass = GrassType(grass);
    selected_tree = TreeType(tree);
    selected_climate = ClimateType(climate);
    ++load_generation;
    undo_.clear();
    redo_.clear();
    stroke_ = TerrainEditCommand();
    stroke_cells_.clear();
    stroking_ = false;
    current_id_ = saved_id_ = 0;
    next_id_ = 1;
    is_dirty = false;
    current_level_path = path;
    error.clear();
    pending_ = TerrainEditorDirty();
    invalidate_all();
    return true;
  } catch (const std::exception &e) {
    if (file)
      std::fclose(file);
    error = e.what();
    return false;
  }
}
} // namespace game
