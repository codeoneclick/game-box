#pragma once
#include "input_context.h"
#include <functional>
#include <memory>

namespace game {
class TerrainEditorState;
class TerrainEditorUI;
enum class TerrainEditorTool;

class TerrainEditorInput final : public gb::input_context_listener {
    std::shared_ptr<TerrainEditorState> state_;
    std::shared_ptr<TerrainEditorUI> ui_;
    gb::camera_3d_shared_ptr camera_;
    std::function<void()> save_, load_;
    glm::ivec2 pointer_ = glm::ivec2(-1);
    glm::vec3 brush_ = glm::vec3(0), previous_stamp_ = glm::vec3(0);
    bool shift_ = false, ctrl_ = false, alt_ = false, left_ = false, right_ = false, middle_ = false;
    bool has_pointer_ = false, has_hit_ = false, has_previous_stamp_ = false;
    float since_stamp_ = 0.f;
    TerrainEditorTool stroke_tool_;
    bool pick(glm::vec3& hit) const;
    void move(glm::ivec2 point);
    void stamp(bool initial);
public:
    TerrainEditorInput(const std::shared_ptr<TerrainEditorState>& state,
                       const std::shared_ptr<TerrainEditorUI>& ui,
                       const gb::camera_3d_shared_ptr& camera);
    void update(float dt);
    bool camera_rotation_allowed() const { return alt_; }
    bool pointer_over_ui(glm::ivec2 point) const;
    bool pick_surface(glm::ivec2 point, glm::vec3& hit) const;
    bool brush_visible() const;
    bool building_preview_visible() const;
    glm::vec3 brush_position() const { return brush_; }
    void set_save_callback(std::function<void()> callback) { save_ = std::move(callback); }
    void set_load_callback(std::function<void()> callback) { load_ = std::move(callback); }
    void finish_stroke();
    void on_gr_pressed(const glm::ivec2&, const glm::ivec2&, gb::e_input_source, ui32) override;
    void on_gr_released(const glm::ivec2&, const glm::ivec2&, gb::e_input_source, ui32) override;
    void on_gr_moved(const glm::ivec2&, const glm::ivec2&, const glm::ivec2&, ui32) override;
    void on_gr_dragged(const glm::ivec2&, const glm::ivec2&, const glm::ivec2&, gb::e_input_source, ui32) override;
    void on_key_down(i32 key) override;
    void on_key_up(i32 key) override;
    void on_virtual_keyboard_input(const std::string&) override {}
    void on_virtual_keyboard_backspace() override {}
    void on_virtual_keyboard_hidden() override {}
};
}
