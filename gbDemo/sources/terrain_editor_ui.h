#pragma once

#include "main_headers.h"
#include "declarations.h"
#include <functional>
#include <memory>

namespace game {
class TerrainEditorState;
struct StylizedPostProcessSettings;
struct RenderDebugState;
struct TerrainLodSettings;

// Native gbUI controls. Terrain edits and persistence belong to the shared state
// and scene callbacks; this view owns no generator or mesh-builder behavior.
class TerrainEditorUI {
    struct implementation;
    std::shared_ptr<implementation> m_impl;
public:
    TerrainEditorUI(const gb::scene_fabricator_shared_ptr& fabricator,
                    const std::shared_ptr<TerrainEditorState>& state,
                    glm::ivec2 resolution, bool inspector_only = false);
    ~TerrainEditorUI();
    gb::ces_entity_shared_ptr root() const;
    void update();
    void set_terrain_triangle_count(size_t triangles);
    bool contains(glm::ivec2 point) const;
    bool keyboard_captured() const;
    void set_save_callback(std::function<void()> callback);
    void set_load_callback(std::function<void()> callback);
    void set_graphics_save_callback(std::function<bool()> callback);
    void bind_post_settings(std::shared_ptr<StylizedPostProcessSettings> settings);
    void toggle_post_panel();
    bool post_panel_visible() const;
    void bind_render_debug(std::shared_ptr<RenderDebugState> state);
    void bind_terrain_lod_settings(std::shared_ptr<TerrainLodSettings> settings);
    bool scroll_render_debug(glm::ivec2 point, float delta);
};
}
