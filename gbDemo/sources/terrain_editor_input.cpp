#include "terrain_editor_input.h"
#include "terrain_editor_state.h"
#include "terrain_editor_ui.h"
#include "camera_3d.h"
#include "ces_entity.h"
#include <algorithm>
#include <cmath>
#include <limits>
#include <stdexcept>

namespace game {
TerrainEditorInput::TerrainEditorInput(const std::shared_ptr<TerrainEditorState>& state,
    const std::shared_ptr<TerrainEditorUI>& ui, const gb::camera_3d_shared_ptr& camera)
    : state_(state), ui_(ui), camera_(camera), stroke_tool_(state ? state->current_tool : TerrainEditorTool::Terrain) {
    if(!state || !ui || !camera) throw std::invalid_argument("Terrain editor input requires state, UI and camera");
}
bool TerrainEditorInput::pointer_over_ui(glm::ivec2 point) const { return ui_->contains(point); }
bool TerrainEditorInput::brush_visible() const {
    return has_hit_ && !alt_ && !right_ && !middle_ && ui_->root()->get_is_visible() &&
        !pointer_over_ui(pointer_) && terrain_tool_ui_config(state_->current_tool).brush;
}
bool TerrainEditorInput::pick(glm::vec3& hit) const {
    return has_pointer_ && pick_surface(pointer_, hit);
}
bool TerrainEditorInput::building_preview_visible() const {
    return state_->current_tool==TerrainEditorTool::Buildings && has_hit_ &&
        !alt_ && !right_ && !middle_ && ui_->root()->get_is_visible() && !pointer_over_ui(pointer_);
}
bool TerrainEditorInput::stairs_preview_visible() const {
    return state_->current_tool==TerrainEditorTool::Stairs && has_hit_ && !shift_ &&
        !alt_ && !right_ && !middle_ && ui_->root()->get_is_visible() && !pointer_over_ui(pointer_);
}
bool TerrainEditorInput::pick_surface(glm::ivec2 point, glm::vec3& hit) const {
    const auto viewport = camera_->get_viewport();
    if(viewport.z <= 0 || viewport.w <= 0) return false;
    if(point.x < viewport.x || point.y < viewport.y || point.x >= viewport.x + viewport.z || point.y >= viewport.y + viewport.w) return false;
    const glm::vec2 ndc(2.f * (point.x - viewport.x) / viewport.z - 1.f,
                        1.f - 2.f * (point.y - viewport.y) / viewport.w);
    const glm::vec4 far_point = glm::inverse(camera_->get_mat_p() * camera_->get_mat_v()) * glm::vec4(ndc, 1.f, 1.f);
    if(!std::isfinite(far_point.w) || std::abs(far_point.w) < .000001f) return false;
    const auto origin = camera_->get_position();
    const auto delta = glm::vec3(far_point) / far_point.w - origin;
    const float length = glm::length(delta);
    if(!std::isfinite(length) || length < .000001f) return false;
    const auto ray = delta / length;
    const auto& config = state_->level.terrain.config;
    if(config.resolution < 1 || !std::isfinite(config.world_size) || config.world_size <= 0) return false;
    const float half = config.world_size * .5f;
    float begin = 0.f, end = std::min(6000.f, camera_->get_far());
    // Clip to logical grid bounds, avoiding whole-scene triangle scans.
    for(int axis : {0, 2}) {
        if(std::abs(ray[axis]) < .000001f) { if(std::abs(origin[axis]) >= half) return false; continue; }
        float a = (-half - origin[axis]) / ray[axis], b = (half - origin[axis]) / ray[axis];
        if(a > b) std::swap(a, b);
        begin = std::max(begin, a); end = std::min(end, b);
    }
    if(begin > end || end <= 0) return false;
    // Raised stone treads must remain pickable for Shift+LMB removal. Limit
    // terrain marching to the nearest flight so terrain in front still wins.
    float stairs_distance = std::numeric_limits<float>::infinity();
    if(state_->current_tool == TerrainEditorTool::Stairs) {
        for(const auto& stairs : state_->level.stairs) {
            float distance = 0.f;
            if(stairs_ray_hit(stairs, config, {{origin.x, origin.y, origin.z}},
                              {{ray.x, ray.y, ray.z}}, distance) &&
               distance >= begin && distance <= end)
                stairs_distance = std::min(stairs_distance, distance);
        }
        end = std::min(end, stairs_distance);
    }
    const float step = std::max(.025f, config.world_size / config.resolution * .25f);
    for(float distance = std::max(0.f, begin) + .0001f; distance <= end; distance += step) {
        const glm::vec3 point = origin + ray * distance;
        const int level = state_->level_at(point.x, point.z);
        if(level == terrain_data::empty_layer) continue;
        const float height = editor_level_height(config, level);
        if(point.y <= height) { hit = glm::vec3(point.x, height, point.z); return true; }
    }
    if(std::isfinite(stairs_distance)) {
        hit = origin + ray * stairs_distance;
        return true;
    }
    // Empty logical cells still have a stable water-plane hover position.
    if(std::abs(ray.y) < .000001f) return false;
    const float water = (config.water_y - origin.y) / ray.y;
    if(!std::isfinite(water) || water < begin || water > end) return false;
    hit = origin + ray * water;
    return true;
}
void TerrainEditorInput::move(glm::ivec2 point) {
    pointer_ = point; has_pointer_ = true;
    has_hit_ = !pointer_over_ui(point) && pick(brush_);
    if(has_hit_) state_->current_terrace = state_->level_at(brush_.x, brush_.z);
    else state_->current_terrace = terrain_data::empty_layer;
    if(left_ && pointer_over_ui(point)) finish_stroke();
}
void TerrainEditorInput::finish_stroke() {
    state_->end_stroke(); left_ = false; has_previous_stamp_ = false; since_stamp_ = 0.f;
}
void TerrainEditorInput::stamp(bool initial) {
    if(!left_ || alt_ || right_ || middle_ || !has_hit_ || pointer_over_ui(pointer_) || !ui_->root()->get_is_visible()) return;
    const auto tool = state_->current_tool;
    if(tool == TerrainEditorTool::Trees) {
        if(initial) state_->place_tree(brush_.x, brush_.z, shift_);
        return;
    }
    if(tool == TerrainEditorTool::Buildings) {
        if(initial) state_->place_building(brush_.x,brush_.z,shift_);
        return;
    }
    if(tool == TerrainEditorTool::Stairs) {
        if(initial) state_->place_stairs(brush_.x, brush_.z, shift_);
        return;
    }
    // Grass density has no effect on the target-height Terrain brush.
    const float interval = .08f;
    if(!initial && since_stamp_ < interval) return;
    state_->begin_stroke();
    const glm::vec3 previous = has_previous_stamp_ ? previous_stamp_ : brush_;
    const float spacing = std::max(.15f, std::min(32.f, state_->brush_size) * .15f);
    const float distance = glm::length(glm::vec2(brush_.x - previous.x, brush_.z - previous.z));
    const int steps = std::max(1, std::min(8192, static_cast<int>(std::ceil(distance / spacing))));
    for(int i = 1; i <= steps; ++i) {
        const auto point = previous + (brush_ - previous) * (float(i) / steps);
        state_->paint(point.x, point.z, shift_);
    }
    previous_stamp_ = brush_; has_previous_stamp_ = true; since_stamp_ = 0.f;
}
void TerrainEditorInput::update(float dt) {
    if(stroke_tool_ != state_->current_tool) { finish_stroke(); stroke_tool_ = state_->current_tool; }
    if(!ui_->root()->get_is_visible() || alt_) finish_stroke();
    if(std::isfinite(dt) && dt > 0) since_stamp_ += std::min(dt, 1.f);
    if(has_pointer_) move(pointer_); // Re-pick after camera/terrain changes, even at a stationary cursor.
    stamp(false);
}
void TerrainEditorInput::on_gr_pressed(const glm::ivec2& point, const glm::ivec2&, gb::e_input_source source, ui32 index) {
    if(index != 0) return;
    move(point);
    if(source == gb::e_input_source_mouse_right) { right_ = true; finish_stroke(); return; }
    if(source == gb::e_input_source_mouse_middle) { middle_ = true; finish_stroke(); return; }
    if(source != gb::e_input_source_mouse_left || alt_ || right_ || middle_ || pointer_over_ui(point) || !ui_->root()->get_is_visible()) return;
    finish_stroke(); left_ = true; stroke_tool_ = state_->current_tool;
    state_->begin_stroke(); stamp(true);
}
void TerrainEditorInput::on_gr_released(const glm::ivec2& point, const glm::ivec2&, gb::e_input_source source, ui32 index) {
    if(index != 0) return;
    move(point);
    if(source == gb::e_input_source_mouse_left) {
        // Preserve the tail of a fast drag even when its timed stamp was not due.
        if(state_->current_tool != TerrainEditorTool::Trees && state_->current_tool != TerrainEditorTool::Buildings &&
           state_->current_tool != TerrainEditorTool::Stairs) stamp(true);
        finish_stroke();
    }
    if(source == gb::e_input_source_mouse_right) right_ = false;
    if(source == gb::e_input_source_mouse_middle) middle_ = false;
}
void TerrainEditorInput::on_gr_moved(const glm::ivec2& point, const glm::ivec2&, const glm::ivec2&, ui32 index) { if(index == 0) move(point); }
void TerrainEditorInput::on_gr_dragged(const glm::ivec2& point, const glm::ivec2&, const glm::ivec2&, gb::e_input_source source, ui32 index) {
    if(index != 0) return;
    move(point); if(source == gb::e_input_source_mouse_left) stamp(false);
}
void TerrainEditorInput::on_gr_scrolled(const glm::ivec2& point,const glm::ivec2&,f32 delta){ui_->scroll_render_debug(point,delta);}
void TerrainEditorInput::on_key_down(i32 key) {
    if(key == 0x10) shift_ = true;
    if(key == 0x11) ctrl_ = true;
    if(key == 0x12) { alt_ = true; finish_stroke(); }
    if(key==0x77||key==0xF70B){finish_stroke();ui_->toggle_post_panel();return;}
    if(ui_->keyboard_captured())return;
    if(key >= 'a' && key <= 'z') key -= 'a' - 'A';
    if(!ctrl_&&!alt_&&ui_->root()->get_is_visible()&&state_->current_tool==TerrainEditorTool::Buildings){
        if(key=='Q'||key=='E'||key=='R'){
            finish_stroke();
            const float step=key=='R'?90.f:static_cast<float>(state_->building_rotation_snap?state_->building_rotation_snap:1);
            const float direction=key=='Q'?-1.f:1.f;
            state_->rotate_building(direction*step*.01745329252f);return;
        }
    }
    if(!ctrl_ || !ui_->root()->get_is_visible()) return;
    if(key == 'Z' || key == 'Y' || key == 'S' || key == 'O') finish_stroke();
    if(key == 'Z') state_->undo();
    else if(key == 'Y') state_->redo();
    else if(key == 'S' && save_) save_();
    else if(key == 'O' && load_) load_();
}
void TerrainEditorInput::on_key_up(i32 key) {
    if(key == 0x10) shift_ = false;
    if(key == 0x11) ctrl_ = false;
    if(key == 0x12) alt_ = false;
}
}
