#pragma once
#include "stylized_post_process.h"
#include "stylized_lighting_settings.h"
#include "terrain_lod_settings.h"
#include <string>

namespace game {
// Application preferences, independent of terrain/level serialization and UI.
std::string graphics_settings_path();
bool load_graphics_settings(const std::string& path, StylizedPostProcessSettings& post,
                            StylizedLightingSettings& lighting, std::string& error);
bool save_graphics_settings(const std::string& path, const StylizedPostProcessSettings& post,
                            const StylizedLightingSettings& lighting, std::string& error);
bool load_graphics_settings(const std::string& path, StylizedPostProcessSettings& post,
                            StylizedLightingSettings& lighting,TerrainLodSettings& lod,std::string& error);
bool save_graphics_settings(const std::string& path,const StylizedPostProcessSettings& post,
                            const StylizedLightingSettings& lighting,const TerrainLodSettings& lod,std::string& error);
}
