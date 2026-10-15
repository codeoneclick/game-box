#pragma once

#include "main_headers.h"
#include "declarations.h"
#include <functional>
#include <memory>

namespace game {
class TerrainEditorState;
struct StylizedPostProcessSettings;

// Native gbUI controls. Terrain edits and persistence belong to the shared state
// and scene callbacks; this view owns no generator or mesh-builder behavior.
class TerrainEditorUI {
    struct implementation;
    std::shared_ptr<implementation> m_impl;
public:
    TerrainEditorUI(const gb::scene_fabricator_shared_ptr& fabricator,
                    const std::shared_ptr<TerrainEditorState>& state,
                    glm::ivec2 resolution);
    ~TerrainEditorUI();
    gb::ces_entity_shared_ptr root() const;
    void update();
    bool contains(glm::ivec2 point) const;
    bool keyboard_captured() const;
    void set_save_callback(std::function<void()> callback);
    void set_load_callback(std::function<void()> callback);
    void bind_post_settings(std::shared_ptr<StylizedPostProcessSettings> settings);
    void toggle_post_panel();
};
}
