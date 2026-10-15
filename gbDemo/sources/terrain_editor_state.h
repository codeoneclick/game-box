#pragma once
#include "building_placement.h"
#include "terrain_data.h"
#include <functional>
#include <map>
#include <memory>
#include <string>
namespace game {
// Two filter samples plus the neighboring triangle that owns a corner.
constexpr int terrain_editor_rebuild_halo = 3;
enum class TerrainEditorTool {
  Terrain = 0,
  Grass = 1,
  Trees = 2,
  Buildings = 4
};
enum class GrassType { Green, Dry, Snow };
enum class TreeType { Pine=0, Oak=1, Birch=2, Palm=3, Thuja=4 };
constexpr int tree_species_count=5;
enum class ClimateType {
  Temperate,
  Mediterranean,
  Tropical,
  Boreal,
  Tundra,
  Polar,
  Desert
};
struct TerrainToolUIConfig {
  bool top_panel, brush, strength, level, palette, trees;
  int palette_count;
  const char *title;
};
const TerrainToolUIConfig &terrain_tool_ui_config(TerrainEditorTool);
struct EditorTree {
  uint32_t id = 0;
  TreeType type = TreeType::Pine;
  std::array<float, 3> position = {{0, 0, 0}};
  float yaw = 0, scale = 1;
};
struct TerrainEditorLevel {
  terrain_data terrain;
  std::vector<uint8_t> grass_density, grass_type, climate;
  // Logical reconstruction policies for 16-cell chunks; no mesh data is stored.
  // Once locally remeshed, a chunk keeps that policy across undo/redo.
  std::vector<uint8_t> terrain_remeshed, grass_remeshed;
  std::vector<EditorTree> trees;
  std::vector<EditorBuilding> buildings;
  uint32_t next_building_id = 1;
  uint32_t next_tree_id = 1;
};
struct TerrainEditorDirty {
  int x0 = 0, z0 = 0, x1 = -1, z1 = -1;
  bool terrain = false, grass = false, trees = false, climate = false,
       buildings = false;
  bool empty() const { return x1 < x0 || z1 < z0; }
};
struct EditorCellValue {
  int8_t level = 0;
  uint8_t density = 0, grass = 0, climate = 0;
};
struct EditorCellChange {
  uint32_t index = 0;
  EditorCellValue before, after;
};
struct EditorTreeChange {
  bool had_before = false, has_after = false;
  EditorTree before, after;
};
struct EditorBuildingChange {
  bool had_before = false, has_after = false;
  EditorBuilding before, after;
};
struct TerrainEditCommand {
  std::vector<EditorBuildingChange> buildings;
  std::vector<EditorCellChange> cells;
  std::vector<EditorTreeChange> trees;
  TerrainEditorDirty dirty;
  uint64_t before_id = 0, after_id = 0;
};
float editor_level_height(const terrain_config &, int level);
std::array<float, 4> editor_climate_color(ClimateType, int level);
class TerrainEditorState {
  std::vector<TerrainEditCommand> undo_, redo_;
  TerrainEditCommand stroke_;
  std::map<uint32_t, size_t> stroke_cells_;
  bool stroking_ = false;
  TerrainEditorDirty pending_;
  uint64_t current_id_ = 0, saved_id_ = 0, next_id_ = 1;
  void apply_command(const TerrainEditCommand &, bool forward);

public:
  TerrainEditorLevel level;
  TerrainEditorTool current_tool = TerrainEditorTool::Terrain;
  float brush_size = 8, strength = .7f;
  int current_terrace = 1;
  int target_terrace = 8;
  GrassType selected_grass = GrassType::Green;
  TreeType selected_tree = TreeType::Pine;
  ClimateType selected_climate = ClimateType::Temperate;
  bool is_dirty = false;
  uint64_t revision = 0, terrain_revision = 0;
  BuildingType selected_building = BuildingType::Forester;
  float building_yaw = 0;
  // Editor preference only; zero preserves continuous rotation.
  int building_rotation_snap = 0;
  std::shared_ptr<const BuildingSurfaceSupport> building_surface;
  uint64_t building_surface_revision = 0;
  // Successful logical level replacements invalidate preserved render caches.
  uint64_t load_generation = 0;
  std::string current_level_path, error;
  void initialize(const terrain_data &);
  int cell_at(float x, float z) const;
  int level_at(float x, float z) const;
  bool stroke_active() const { return stroking_; }
  void select_tool(TerrainEditorTool);
  void set_target_terrace(int);
  void begin_stroke();
  bool paint(float x, float z, bool erase = false);
  bool place_tree(float x, float z, bool erase = false);
  bool can_place_building(float x, float z) const;
  bool place_building(float x, float z, bool erase = false);
  void rotate_building(float delta_radians);
  void set_building_rotation_degrees(float degrees);
  void set_building_rotation_snap(int degrees);
  void end_stroke();
  bool undo();
  bool redo();
  bool can_undo() const { return !undo_.empty(); }
  bool can_redo() const { return !redo_.empty(); }
  TerrainEditorDirty consume_dirty();
  void invalidate_all();
  bool save(const std::string &);
  bool load(const std::string &);
};
} // namespace game
