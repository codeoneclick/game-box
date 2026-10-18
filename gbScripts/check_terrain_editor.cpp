#include "terrain_editor_state.h"
#include "ocean_wave_model.h"
#include <cassert>
#include <cstdio>
#include <fstream>
#include <iostream>
#ifdef _WIN32
#include <direct.h>
#else
#include <sys/stat.h>
#include <unistd.h>
#endif
using namespace game;
int main() {
  terrain_data brush_data;brush_data.config.resolution=32;brush_data.config.world_size=32;brush_data.config.island_size=16;brush_data.config.water_size=48;brush_data.layers.assign(1024,2);
  TerrainEditorState target;target.initialize(brush_data);target.consume_dirty();target.brush_size=16;
  target.select_tool(TerrainEditorTool::Terrain);target.set_target_terrace(9);target.begin_stroke();
  assert(target.paint(.5f,.5f));assert(target.level_at(.5f,.5f)==9);
  const int feather=target.level_at(7.5f,.5f);assert(feather>3&&feather<9);assert(target.level_at(8.5f,.5f)==3);
  target.paint(.5f,.5f);assert(target.level_at(7.5f,.5f)==feather); // Holding does not erase falloff.
  target.paint(7.5f,.5f);assert(target.level_at(7.5f,.5f)==9);target.end_stroke();
  assert(target.undo());assert(target.level_at(.5f,.5f)==3&&target.level_at(7.5f,.5f)==3);assert(target.redo());
  target.set_target_terrace(-2);target.paint(.5f,.5f);assert(target.level_at(.5f,.5f)==-2);assert(target.level_at(7.5f,.5f)>-2);
  target.set_target_terrace(-999);assert(target.target_terrace==-5);target.set_target_terrace(999);assert(target.target_terrace==16);
  assert(target.save("editor-target.level"));TerrainEditorState target_loaded;assert(target_loaded.load("editor-target.level"));assert(target_loaded.target_terrace==16);
  std::remove("editor-target.level");
  game::ocean_settings waves;
  float maximum_wave_displacement=0;
  for(const auto& wave:waves.waves)maximum_wave_displacement+=wave.amplitude;
  maximum_wave_displacement=std::max(.001f,1.09f*maximum_wave_displacement);
  game::terrain_config sea;
  float submerged=game::editor_level_height(sea,0);
  assert(submerged<=sea.water_y-maximum_wave_displacement-.079f);
  assert(submerged>game::editor_level_height(sea,-1));
  terrain_data d;
  d.config.resolution = 8;
  d.config.world_size = 8;
  d.config.island_size = 4;
  d.config.water_size = 12;
  d.layers.assign(64, 2);
  d.layers[0] = -8;
  d.layers[1] = -7;
  d.layers[2] = -6;
  TerrainEditorState s;
  s.initialize(d);
  s.consume_dirty();
  assert(s.level.terrain.layers[3] == 3);
  assert(s.level.grass_density[3] == 255);
  assert(s.level.terrain_remeshed == std::vector<uint8_t>{0});
  assert(s.level.grass_remeshed == std::vector<uint8_t>{0});
  // These are logical generation policies; their bytes select seed-derived
  // versus locally edited reconstruction without saving mesh vertices.
  s.level.terrain_remeshed[0] = 1;
  s.level.grass_remeshed[0] = 1;
  TerrainEditorState before_cache;
  auto small = d;
  small.config.resolution=32;small.config.world_size=32;small.config.water_size=48;
  small.layers.assign(1024,2);
  before_cache.initialize(small);before_cache.consume_dirty();before_cache.brush_size=1;
  assert(before_cache.level.terrain_remeshed==std::vector<uint8_t>({0,0,0,0}));
  assert(before_cache.level.grass_remeshed==std::vector<uint8_t>({0,0,0,0}));
  before_cache.begin_stroke();before_cache.paint(-12.5f,-12.5f);before_cache.end_stroke();
  assert(before_cache.level.terrain_remeshed==std::vector<uint8_t>({1,0,0,0}));
  assert(before_cache.level.grass_remeshed==std::vector<uint8_t>({1,0,0,0}));
  assert(before_cache.save("editor-before-cache.level"));
  TerrainEditorState before_cache_load;assert(before_cache_load.load("editor-before-cache.level"));
  assert(before_cache_load.level.terrain_remeshed==before_cache.level.terrain_remeshed);
  before_cache_load.consume_dirty();
  assert(before_cache_load.level.terrain_remeshed==std::vector<uint8_t>({1,0,0,0}));
  assert(before_cache_load.level.grass_remeshed==std::vector<uint8_t>({1,0,0,0}));
  assert(before_cache.undo());
  assert(before_cache.level.terrain_remeshed==std::vector<uint8_t>({1,0,0,0}));
  before_cache.paint(-1.5f,-12.5f);
  assert(before_cache.level.terrain_remeshed==std::vector<uint8_t>({1,1,0,0}));
  s.brush_size = 32;
  s.select_tool(TerrainEditorTool::Terrain);s.set_target_terrace(2);
  s.begin_stroke();
  assert(s.paint(0, 0));
  s.paint(0, 0);
  s.end_stroke();
  assert(s.level.terrain.layers[3] == 2);
  assert(s.level.terrain.layers[0] == -8 && s.level.terrain.layers[1] == -7 &&
         s.level.terrain.layers[2] == -6);
  assert(s.is_dirty);
  auto dirty = s.consume_dirty();
  assert(dirty.terrain && dirty.grass && dirty.trees && !dirty.climate);
  assert(s.undo());
  assert(!s.is_dirty && s.level.terrain.layers[3] == 3);
  assert(s.redo());
  assert(s.save("editor-roundtrip"));
  assert(!s.is_dirty);
  TerrainEditorState loaded;
  assert(loaded.load("editor-roundtrip.level"));
  assert(loaded.level.terrain.layers == s.level.terrain.layers);
  assert(loaded.level.grass_density == s.level.grass_density);
  assert(loaded.level.terrain_remeshed == s.level.terrain_remeshed);
  assert(loaded.level.grass_remeshed == s.level.grass_remeshed);
  s.set_target_terrace(-5);
  for (int i = 0; i < 30; ++i) {
    s.begin_stroke();
    s.paint(0, 0);
    s.end_stroke();
  }
  assert(s.level.terrain.layers[3] == -5);
  s.select_tool(TerrainEditorTool::Terrain);s.set_target_terrace(16);
  for (int i = 0; i < 30; ++i) {
    s.begin_stroke();
    s.paint(0, 0);
    s.end_stroke();
  }
  assert(s.level.terrain.layers[3] == 16 && s.level.terrain.layers[0] == -8);
  s.select_tool(TerrainEditorTool::Grass);
  s.strength = .5f;
  s.selected_grass = GrassType::Snow;
  s.begin_stroke();
  s.paint(0, 0);
  s.end_stroke();
  assert(s.level.grass_density[3] == 128 && s.level.grass_type[3] == 2);
  // Preserve biome data from older levels even though its painting tool is retired.
  s.level.climate[3] = 5;
  s.select_tool(TerrainEditorTool::Trees);
  s.begin_stroke();
  assert(s.place_tree(0, 0));
  s.end_stroke();
  assert(s.level.trees.size() == 1);
  assert(s.level.trees[0].position[1] ==
         editor_level_height(s.level.terrain.config, 16));
  assert(s.save("editor-roundtrip.level"));
  assert(loaded.load("editor-roundtrip.level"));
  assert(loaded.level.trees.size() == 1);
  // Grass/climate must not silently move seeded tree heights to logical
  // terraces.
  auto seeded_height = s.level.trees[0].position[1] + .25f;
  s.level.trees[0].position[1] = seeded_height;
  s.select_tool(TerrainEditorTool::Grass);
  s.strength = .3f;
  s.begin_stroke();
  s.paint(0, 0);
  s.end_stroke();
  assert(s.level.trees[0].position[1] == seeded_height);
  assert(s.undo());
  s.level.trees[0].position[1] = loaded.level.trees[0].position[1];
  s.select_tool(TerrainEditorTool::Trees);
  s.begin_stroke();
  assert(s.place_tree(0, 0, true));
  s.end_stroke();
  assert(s.level.trees.empty());
  assert(s.undo());
  assert(s.level.trees.size() == 1 && !s.is_dirty);
  assert(s.undo());
  assert(s.is_dirty);
  s.begin_stroke();
  s.place_tree(1, 1);
  s.end_stroke();
  assert(!s.can_redo() && s.is_dirty);
  std::ifstream input("editor-roundtrip.level", std::ios::binary);
  std::string bytes((std::istreambuf_iterator<char>(input)), {});
  input.close();
  const auto valid_bytes = bytes;
  // Version 1 files retain logical data; both old sculpt tools map to Terrain.
  for(uint32_t old_tool=0;old_tool<5;++old_tool){
    auto legacy=valid_bytes;legacy.erase(legacy.size()-12,8);legacy.erase(legacy.size()-20,16);legacy.erase(legacy.size()-8,4);
    legacy[4]=1;legacy[5]=legacy[6]=legacy[7]=0;
    const size_t tool_offset=legacy.size()-32;
    for(int b=0;b<4;++b)legacy[tool_offset+b]=char(old_tool>>(b*8));
    uint32_t checksum=~0u;for(size_t i=0;i<legacy.size()-4;++i){checksum^=uint8_t(legacy[i]);for(int b=0;b<8;++b)checksum=(checksum>>1)^(0xedb88320u&uint32_t(-int(checksum&1)));}
    checksum=~checksum;for(int b=0;b<4;++b)legacy[legacy.size()-4+b]=char(checksum>>(b*8));
    std::ofstream oldfile("editor-v1.level",std::ios::binary|std::ios::trunc);oldfile.write(legacy.data(),legacy.size());oldfile.close();
    TerrainEditorState migrated;assert(migrated.load("editor-v1.level"));assert(int(migrated.current_tool)==int(old_tool<=1||old_tool==4?0:old_tool-1));
    assert(migrated.level.terrain.layers==s.level.terrain.layers);assert(migrated.target_terrace>=-5&&migrated.target_terrace<=16);
  }
  std::remove("editor-v1.level");
  bytes[bytes.size() / 2] ^= 1;
  std::ofstream bad("editor-bad.level", std::ios::binary);
  bad.write(bytes.data(), bytes.size());
  bad.close();
  auto before = loaded.level.terrain.layers;
  assert(!loaded.load("editor-bad.level"));
  assert(before == loaded.level.terrain.layers);
  // Recompute the checksum so these exercise semantic validation rather than
  // CRC.
  auto reject_field = [&](size_t offset, uint32_t value) {
    auto mutated = valid_bytes;
    for (int i = 0; i < 4; ++i)
      mutated[offset + i] = char(value >> (8 * i));
    uint32_t checksum = ~0u;
    for (size_t i = 0; i < mutated.size() - 4; ++i) {
      checksum ^= uint8_t(mutated[i]);
      for (int bit = 0; bit < 8; ++bit)
        checksum =
            (checksum >> 1) ^ (0xedb88320u & uint32_t(-int(checksum & 1)));
    }
    checksum = ~checksum;
    for (int i = 0; i < 4; ++i)
      mutated[mutated.size() - 4 + i] = char(checksum >> (8 * i));
    std::ofstream out("editor-bad.level", std::ios::binary | std::ios::trunc);
    out.write(mutated.data(), mutated.size());
    out.close();
    auto revision = loaded.revision;
    auto path = loaded.current_level_path;
    auto trees = loaded.level.trees.size();
    assert(!loaded.load("editor-bad.level"));
    assert(loaded.level.terrain.layers == before &&
           loaded.revision == revision && loaded.current_level_path == path &&
           loaded.level.trees.size() == trees);
  };
  reject_field(0, 0);           // magic
  reject_field(4, 5);           // unsupported version
  reject_field(valid_bytes.size()-32,17); // target terrace out of range
  reject_field(8, 0x7fc00000);  // non-finite world size
  reject_field(16, 4096);       // resolution exceeds bounded allocations
  reject_field(56, 0xffffffff); // impossible map size
  const size_t policy_count_offset=60+4*d.layers.size()+8+28*loaded.level.trees.size();
  reject_field(policy_count_offset,0xffffffff); // incompatible chunk policy allocation
  reject_field(policy_count_offset+4,0x00000102); // terrain policy must be boolean
  reject_field(
      44, 0x447a0000); // sea above the shore: breaks physical level ordering
  std::ofstream shortfile("editor-bad.level",
                          std::ios::binary | std::ios::trunc);
  shortfile.write(bytes.data(), 12);
  shortfile.close();
  assert(!loaded.load("editor-bad.level"));
  assert(before == loaded.level.terrain.layers);
  assert(editor_level_height(d.config, 1) == .5f &&
         editor_level_height(d.config, 16) == 30.f);
  assert(terrain_tool_ui_config(TerrainEditorTool::Grass).palette_count == 3);
  assert(!terrain_tool_ui_config(TerrainEditorTool::Terrain).strength);
  assert(terrain_tool_ui_config(TerrainEditorTool::Grass).strength);
  assert(!terrain_tool_ui_config(TerrainEditorTool::Trees).top_panel);
  auto token = loaded.load_generation;
  assert(loaded.load("editor-roundtrip.level"));
  assert(loaded.load_generation == token + 1);
  auto custom = loaded.level.terrain.config;
  custom.world_size = 24;
  custom.island_size = 12;
  custom.water_size = 32;
  custom.layer_height = 3;
  custom.shore_height = .75f;
  custom.water_y = -.9f;
  loaded.level.terrain.config = custom;
  assert(loaded.save("editor-custom.level"));
  TerrainEditorState custom_loaded;
  assert(custom_loaded.load("editor-custom.level"));
  assert(custom_loaded.level.terrain.config.world_size == 24 &&
         custom_loaded.level.terrain.config.water_size == 32 &&
         custom_loaded.level.terrain.config.water_y == -.9f &&
         custom_loaded.level.terrain.config.layer_height == 3);
  loaded.select_tool(TerrainEditorTool::Grass);
  loaded.selected_grass = GrassType::Dry;
  loaded.paint(0, 0);
  assert(loaded.is_dirty);
  assert(!loaded.save("missing-editor-directory/blocked.level"));
  assert(loaded.is_dirty);
  assert(custom_loaded.load("editor-custom.level"));
  // A complete temporary write followed by a failed replacement must also
  // preserve the dirty state and current path, and clean up its temporary file.
#ifdef _WIN32
  assert(_mkdir("editor-blocked.level") == 0);
#else
  assert(mkdir("editor-blocked.level", 0700) == 0);
#endif
  auto previous_path = loaded.current_level_path;
  assert(!loaded.save("editor-blocked.level"));
  assert(loaded.is_dirty && loaded.current_level_path == previous_path);
#ifdef _WIN32
  assert(_rmdir("editor-blocked.level") == 0);
#else
  assert(rmdir("editor-blocked.level") == 0);
#endif
  std::remove("editor-roundtrip.level");
  std::remove("editor-bad.level");
  std::remove("editor-custom.level");
  std::remove("editor-before-cache.level");
  TerrainEditorState foliage;foliage.initialize(brush_data);foliage.consume_dirty();
  foliage.select_tool(TerrainEditorTool::Trees);foliage.selected_tree=TreeType::Thuja;
  assert(foliage.paint(.5f,.5f));assert(foliage.level.trees.size()==1);
  assert(foliage.undo()&&foliage.level.trees.empty());assert(foliage.redo());
  assert(foliage.save("editor-thuja.level"));TerrainEditorState foliage_loaded;
  assert(foliage_loaded.load("editor-thuja.level"));
  assert(foliage_loaded.selected_tree==TreeType::Thuja&&foliage_loaded.level.trees.size()==1);
  assert(foliage_loaded.level.trees[0].type==TreeType::Thuja);
  std::remove("editor-thuja.level");
  std::cout << "Terrain editor CPU checks passed\n";
}
