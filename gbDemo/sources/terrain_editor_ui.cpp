#include "terrain_editor_layout.h"
#include <chrono>
#include "terrain_lod_settings.h"
#include "terrain_editor_ui.h"
#include "terrain_editor_state.h"
#include "render_debug_state.h"
#include "game_object_2d.h"
#include "ces_shader_uniforms_component.h"
#include "building_catalog.h"
#include "stylized_post_process.h"
#include "stylized_lighting_settings.h"
#include "scene_fabricator.h"
#include "ui_fabricator.h"
#include "button.h"
#include "panel.h"
#include "slider.h"
#include "textfield.h"
#include "ces_textedit_component.h"
#include "label.h"
#include "sprite.h"
#include "ces_bound_touch_component.h"
#include "ces_bound_touch_2d_component.h"
#include "ces_transformation_2d_component.h"
#include "ces_transformation_extension.h"
#include "glm_extensions.h"
#include "ces_material_component.h"
#include <array>
#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <iomanip>
#include <limits>
#include <sstream>
#include <stdexcept>

namespace game {
namespace {
class render_preview_uniforms final : public gb::ces_shader_uniforms_component::shader_uniforms {
    glm::vec4 values = glm::vec4(0);
public:
    render_preview_uniforms(gb::ces_shader_uniforms_component::e_shader_uniform_mode mode, const std::string& name)
        : shader_uniforms(mode, name) {
        m_uniforms["preview_params"] = std::make_shared<gb::shader_uniform>(gb::e_uniform_type_vec4);
        set(glm::vec4(0), "preview_params");
    }
    void* get_values() override { values = m_uniforms["preview_params"]->get_vec4(); return &values; }
    ui32 get_values_size() override { static_assert(sizeof(glm::vec4) == 16, "Preview uniform layout"); return 16; }
};
using control_state = gb::ui::e_control_state;
const glm::u8vec4 panel_color(5, 36, 53, 250);
const glm::u8vec4 header_color(22, 57, 66, 235);
const glm::u8vec4 border_color(7, 42, 59, 250);
const glm::u8vec4 control_color(7, 42, 59, 250);
const glm::u8vec4 selected_color(16, 128, 162, 255);
const glm::u8vec4 cyan(100, 237, 240, 255);
const glm::u8vec4 text_color(247, 244, 229, 255);
const glm::u8vec4 muted(98, 121, 127, 220);
const glm::u8vec4 disabled_color(12, 30, 45, 245);
const std::array<const char*, 5> tool_names = {"Terrain", "Stairs", "Grass", "Trees", "Buildings"};
const std::array<TerrainEditorTool, 5> tool_types = {
    TerrainEditorTool::Terrain, TerrainEditorTool::Stairs, TerrainEditorTool::Grass,
    TerrainEditorTool::Trees, TerrainEditorTool::Buildings};
// Keep the tree palette's atlas slots stable; Stairs reuses retired Climate tile 7.
const std::array<int, 5> tool_icon_tiles = {4, 7, 5, 6, -1};
const std::array<BuildingType, 3> building_types = {
    BuildingType::Forester, BuildingType::Stonecutter, BuildingType::Woodcutter};
const std::array<const char*, 3> building_preview_configs = {
    "building_preview_forester.xml", "building_preview_stonecutter.xml", "building_preview_woodcutter.xml"};
const std::array<int, 4> rotation_snaps = {{0, 15, 45, 90}};
const std::array<const char*, 4> rotation_snap_names = {{"Off", "15", "45", "90"}};
float building_degrees(float yaw) {
    float degrees = std::fmod(yaw * 180.f / 3.14159265358979323846f, 360.f);
    return degrees < 0.f ? degrees + 360.f : degrees;
}
bool parse_rotation(const std::string& text, float& degrees) {
    if(text.empty()) return false;
    char* end = nullptr;
    degrees = std::strtof(text.c_str(), &end);
    return end == text.c_str() + text.size() && std::isfinite(degrees) && degrees >= 0.f && degrees <= 360.f;
}
const std::array<const char*, 3> grass_names = {"Green", "Dry", "Snow"};
const std::array<const char*, 5> tree_names = {"Pine", "Oak", "Birch", "Palm", "Thuja"};
const std::array<int, 5> tree_icon_tiles = {8, 9, 10, 11, 15};
const std::array<const char*, 4> post_toggle_names = {{"SSAO", "ToneMap", "Grade", "Fog"}};
const std::array<bool StylizedPostProcessSettings::*, 4> post_toggle_fields = {{
    &StylizedPostProcessSettings::enableSSAO, &StylizedPostProcessSettings::enableToneMapping,
    &StylizedPostProcessSettings::enableColorGrading, &StylizedPostProcessSettings::enableFog}};
const std::array<const char*, 5> post_slider_names = {{"Exposure (EV)", "Saturation", "Contrast", "Fog strength", "SSAO strength"}};
const std::array<float StylizedPostProcessSettings::*, 5> post_slider_fields = {{
    &StylizedPostProcessSettings::exposure, &StylizedPostProcessSettings::saturation,
    &StylizedPostProcessSettings::contrast, &StylizedPostProcessSettings::fogStrength,
    &StylizedPostProcessSettings::ssaoIntensity}};
const std::array<float, 5> post_slider_min = {{-2.f, .5f, .5f, 0.f, 0.f}};
const std::array<float, 5> post_slider_max = {{2.f, 1.5f, 1.5f, .3f, 3.f}};
const std::array<const char*, 4> post_view_names = {{"Final", "SceneHDR", "ToneMapped", "Graded"}};
const std::array<int, 4> post_view_values = {{0, 1, 4, 5}};
const std::array<const char*, 12> lighting_slider_names = {{
    "Sun intensity", "Ambient strength", "Wrap amount", "Diffuse power", "Global specular",
    "Vegetation backlight", "Sun R (linear)", "Sun G (linear)", "Sun B (linear)",
    "Ambient R (linear)", "Ambient G (linear)", "Ambient B (linear)"}};
const std::array<float, 12> lighting_slider_min = {{0, 0, .2f, .4f, 0, 0, 0, 0, 0, 0, 0, 0}};
const std::array<float, 12> lighting_slider_max = {{3, 1, .5f, 2, 2, .2f, 1.5f, 1.5f, 1.5f, 1.5f, 1.5f, 1.5f}};
float& lighting_slider_value(StylizedLightingSettings& model, size_t index) {
    switch(index) {
        case 0: return model.sunIntensity;
        case 1: return model.ambientStrength;
        case 2: return model.wrapAmount;
        case 3: return model.diffusePower;
        case 4: return model.globalSpecularMultiplier;
        case 5: return model.vegetationBacklightStrength;
        default: return index < 9 ? model.sunColor[index - 6] : model.ambientColor[index - 9];
    }
}
std::string post_decimal(float value) {
    std::ostringstream out;
    out << std::fixed << std::setprecision(2) << value;
    return out.str();
}
std::string decimal(float value) {
    std::ostringstream out;
    out << std::fixed << std::setprecision(1) << value;
    return out.str();
}
std::string terrace(int value) {
    return (value > 0 ? "+" : "") + std::to_string(value);
}
std::string ascii_message(const std::string& input, size_t limit = 120) {
    std::string text;
    text.reserve(std::min(input.size(), limit));
    for(unsigned char character : input) {
        if(text.size() >= limit) break;
        text.push_back(character >= 32 && character < 127 ? static_cast<char>(character) : ' ');
    }
    if(input.size() > limit && text.size() > 3) text.replace(text.size() - 3, 3, "...");
    return text;
}
void set_text(const gb::label_shared_ptr& label, const std::string& text) {
    if(static_cast<std::string>(label->text) != text) label->text = text;
}
void set_panel_title(const gb::ui::panel_shared_ptr& panel, const std::string& text) {
    const auto label = panel->get_element_as<gb::label>(gb::ui::control::k_label_element_name);
    label->font_name = "Vera.ttf";
    label->font_size = 22.f;
    label->font_color = text_color;
    set_text(label, text);
    label->position = glm::vec2(18.f, 14.f);
}
void style_button(const gb::ui::button_shared_ptr& button, bool enabled, bool selected) {
    button->disable(!enabled);
    if(enabled) button->set_is_selected(selected);
}
void configure_button(const gb::ui::button_shared_ptr& button) {
    button->set_background_color(border_color, control_state::e_none);
    button->set_background_color(disabled_color, control_state::e_disabled);
    button->set_background_color(selected_color, control_state::e_selected);
    button->set_foreground_color(control_color, control_state::e_none);
    button->set_foreground_color(disabled_color, control_state::e_disabled);
    button->set_foreground_color(selected_color, control_state::e_selected);
    button->set_text_color(text_color, control_state::e_none);
    button->set_text_color(muted, control_state::e_disabled);
    button->set_text_color(cyan, control_state::e_selected);
    button->set_font_size(15.f);
    // Use the existing readable UI face promoted into the canonical font bundle.
    const auto label = button->get_element_as<gb::label>(gb::ui::control::k_label_element_name);
    label->font_name = "Vera.ttf";
    // Native alignment is calculated from the current font's mesh dimensions.
    button->set_text(button->get_text());
    style_button(button, true, false);
}
}

struct TerrainEditorUI::implementation : std::enable_shared_from_this<implementation> {
    gb::scene_fabricator_shared_ptr fabricator;
    gb::ui::ui_fabricator controls;
    std::shared_ptr<TerrainEditorState> state;
    gb::ces_entity_shared_ptr entity;
    gb::sprite_shared_ptr theme_source;
    std::vector<gb::sprite_shared_ptr> hidden_decorations;
    glm::ivec2 resolution;
    gb::ui::panel_shared_ptr top, daytime, tools, settings, trees, buildings, status;
    gb::ui::slider_shared_ptr daytime_slider, skybox_slider;
    gb::label_shared_ptr daytime_value, skybox_value;
    std::shared_ptr<TerrainLodSettings> terrain_lod=std::make_shared<TerrainLodSettings>();
    std::array<gb::ui::slider_shared_ptr,2> lod_sliders;
    gb::ui::button_shared_ptr wireframe_toggle;
    gb::label_shared_ptr terrain_triangle_label;
    std::chrono::steady_clock::time_point terrain_triangle_label_update;
    int last_wireframe=-1,last_wireframe_available=-1,last_lod_disabled=-1;
    std::array<gb::label_shared_ptr,2> lod_values;
    float last_daytime = std::numeric_limits<float>::quiet_NaN();
    float last_skybox = std::numeric_limits<float>::quiet_NaN();
    std::shared_ptr<StylizedPostProcessSettings> post_settings;
    gb::ui::panel_shared_ptr post_panel;
    std::function<bool()> graphics_save_callback;
    gb::label_shared_ptr graphics_save_status;
    std::array<gb::ui::button_shared_ptr, 4> post_toggle_buttons;
    std::array<gb::ui::slider_shared_ptr, 5> post_sliders;
    std::array<gb::label_shared_ptr, 5> post_values;
    std::array<float, 5> last_post_values;
    std::array<gb::ui::slider_shared_ptr, 12> lighting_sliders;
    std::array<gb::label_shared_ptr, 12> lighting_values;
    std::array<float, 12> last_lighting_values;
    std::array<int, 6> last_post_toggles = {{-1, -1, -1, -1, -1, -1}};
    int last_lighting_mode = -1, last_post_view = -1;
    gb::ui::button_shared_ptr post_view_button, lighting_mode_button, specular_button, backlight_button;
    bool post_visible = false;
    std::shared_ptr<RenderDebugState> render_debug;
    size_t selected_pass = 0, selected_attachment = 0;
    int list_scroll = 0, detail_scroll = 0, detail_extent = 0;
    std::vector<gb::ui::button_shared_ptr> pass_buttons;
    gb::label_shared_ptr pass_title, pass_triangles, attachment_info;
    std::array<gb::label_shared_ptr,6> terrain_info;
    std::array<gb::ui::slider_shared_ptr,3> error_sliders;
    std::array<gb::label_shared_ptr,3> error_values;
    gb::ui::button_shared_ptr lod_debug_button;
    std::array<gb::ui::button_shared_ptr,3> water_buttons;
    std::array<gb::ui::slider_shared_ptr,3> water_sliders;
    std::array<gb::label_shared_ptr,3> water_values;
    gb::ui::button_shared_ptr pass_enabled, attachment_button;
    gb::sprite_shared_ptr preview, list_thumb, detail_thumb;
    gb::game_object_2d_shared_ptr preview_group;
    std::shared_ptr<render_preview_uniforms> preview_uniforms;
    struct debug_row { gb::game_object_2d_shared_ptr node; std::string pass; float y, height; };
    std::vector<debug_row> debug_rows;
    static constexpr int list_page = 14;
    static constexpr float detail_height = 470.f;

    std::array<gb::ui::button_shared_ptr, 4> actions;
    std::array<gb::ui::button_shared_ptr, 5> tool_buttons;
    std::array<gb::ui::button_shared_ptr, 3> grass_buttons;
    std::array<gb::ui::button_shared_ptr, 5> tree_buttons;
    std::array<gb::ui::button_shared_ptr, 3> building_buttons;
    std::array<gb::ui::button_shared_ptr, 4> rotation_snap_buttons;
    gb::ui::slider_shared_ptr rotation_slider;
    gb::ui::textfield_shared_ptr rotation_field;
    gb::label_shared_ptr selected_building_label;
    bool rotation_focused = false, rotation_replace_text = false;
    bool rotation_slider_active = false, rotation_slider_callback = false;
    int last_rotation_snap = -1;
    gb::ui::slider_shared_ptr brush_slider, strength_slider;
    std::array<gb::ui::button_shared_ptr, 2> target_buttons;
    gb::label_shared_ptr dirty_label, brush_label, brush_value, strength_label, strength_value;
    gb::label_shared_ptr level_label, level_value, target_value, palette_label, status_label, error_label, tree_hint;
    gb::label_shared_ptr palette_value;
    std::function<void()> save_callback, load_callback;
    int last_tool = -1, last_grass = -1, last_tree = -1, last_building = -1;
    int last_target = -100;
    bool last_undo = false, last_redo = false, first_update = true;

    bool inspector_only = false;
    implementation(const gb::scene_fabricator_shared_ptr& source,
                   const std::shared_ptr<TerrainEditorState>& model, glm::ivec2 size)
        : fabricator(source), controls(source), state(model),
          entity(gb::ces_entity::construct<gb::ces_entity>()), resolution(size) {
        if(!source || !model) throw std::invalid_argument("Terrain editor UI requires fabricator and state");
        entity->tag = "island.terrain_editor.ui";
    }

    // Share a configured editor material. Each sprite keeps native geometry/color;
    // this does not alter the shared UI fabricator or other application controls.
    void theme_sprite(const gb::sprite_shared_ptr& sprite) {
        if(!theme_source) theme_source = fabricator->create_sprite("editor_ui.xml");
        sprite->get_component<gb::ces_material_component>()->add_material("ws.ui",
            theme_source->get_component<gb::ces_material_component>()->get_material("ws.ui"));
    }

    gb::sprite_shared_ptr make_icon(const gb::ces_entity_shared_ptr& parent,
                                   glm::vec2 position, glm::vec2 size, int tile) {
        const auto icon = fabricator->create_sprite("editor_icon.xml");
        icon->position = position;
        icon->size = size;
        icon->color = glm::u8vec4(255);
        const int column = tile % 8, row = tile / 8;
        // The PNG loader flips source rows before uploading.
        icon->set_custom_texcoord(glm::vec4(column / 8.f, (3 - row) / 3.f,
                                            (column + 1) / 8.f, (2 - row) / 3.f));
        parent->add_child(icon);
        return icon;
    }

    void icon_button(const gb::ui::button_shared_ptr& button, int tile, float icon_size) {
        const glm::vec2 size = button->size;
        const auto label = button->get_element_as<gb::label>(gb::ui::control::k_label_element_name);
        const glm::vec2 label_size = label->size;
        make_icon(button, glm::vec2((size.x - icon_size) * .5f, 5), glm::vec2(icon_size), tile);
        label->position = glm::vec2((size.x - label_size.x) * .5f, size.y - 23);
    }

    void building_icon_button(const gb::ui::button_shared_ptr& button, size_t index, float icon_size) {
        const glm::vec2 size = button->size;
        // These images depict the authored model. Sample the entire image;
        // flip V to account for the PNG loader, without an atlas tile crop.
        const auto icon = fabricator->create_sprite(building_preview_configs.at(index));
        icon->position = glm::vec2((size.x - icon_size) * .5f, 5.f);
        icon->size = glm::vec2(icon_size);
        icon->color = glm::u8vec4(255);
        icon->set_custom_texcoord(glm::vec4(0.f, 1.f, 1.f, 0.f));
        button->add_child(icon);
        const auto label = button->get_element_as<gb::label>(gb::ui::control::k_label_element_name);
        const glm::vec2 label_size = label->size;
        label->position = glm::vec2((size.x - label_size.x) * .5f, size.y - 23.f);
    }

    void rotation_focus(bool focused) {
        const bool was_focused = rotation_focused;
        rotation_focused = focused && state->current_tool == TerrainEditorTool::Buildings && entity->get_is_visible();
        if(rotation_focused && !was_focused) rotation_replace_text = true;
        rotation_field->set_background_color(rotation_focused ? selected_color : control_color);
        if(!rotation_focused) rotation_field->set_text(decimal(building_degrees(state->building_yaw)));
    }

    void edit_rotation(const std::string& text) {
        rotation_field->set_text(text);
        float degrees = 0.f;
        const bool valid = parse_rotation(text, degrees);
        rotation_field->set_text_color(valid || text.empty() ? text_color : glm::u8vec4(255, 159, 132, 255));
        if(valid) state->set_building_rotation_degrees(degrees);
    }

    void theme_slider(const gb::ui::slider_shared_ptr& slider) {
        theme_sprite(slider->get_element_as<gb::sprite>(gb::ui::control::k_background_element_name));
        theme_sprite(slider->get_element_as<gb::sprite>(slider->k_foreground_element_name));
        theme_sprite(slider->get_element_as<gb::sprite>(slider->k_slider_element_name));
        slider->set_background_color(glm::u8vec4(54, 102, 123, 255));
        slider->set_foreground_color(glm::u8vec4(54, 102, 123, 255));
        slider->set_slider_color(glm::u8vec4(250, 246, 227, 255));
    }

    void slider_fill(const gb::ui::slider_shared_ptr& slider, float offset) {
        const auto fill = slider->get_element_as<gb::sprite>(slider->k_foreground_element_name);
        const glm::vec2 size = slider->size;
        const glm::vec2 target(std::max(4.f, (size.x - 4.f) * offset), size.y - 4.f);
        const glm::vec2 current_size=fill->size,current_position=fill->position;
        if(current_size!=target)fill->size=target;
        if(current_position!=glm::vec2(2.f))fill->position=glm::vec2(2.f);
        if(fill->get_is_visible()!=(offset>.001f))fill->visible=offset>.001f;
    }

    gb::ui::panel_shared_ptr make_panel(glm::vec2 position, glm::vec2 size, const std::string& title) {
        const auto panel = controls.create_panel(size, title);
        panel->position = position;
        panel->set_background_color(panel_color);
        theme_sprite(panel->get_element_as<gb::sprite>(gb::ui::control::k_background_element_name));
        panel->set_header_color(header_color);
        const auto header = panel->get_element_as<gb::sprite>(panel->k_header_element_name);
        header->size = glm::vec2(size.x, 24.f);
        header->position = glm::vec2(0.f);
        header->visible = false;
        hidden_decorations.push_back(header);
        set_panel_title(panel, title);
        // Native panels support dragging; editor panel layout is intentionally fixed.
        panel->get_component<gb::ces_bound_touch_component>()->set_disabled(true);
        entity->add_child(panel);
        return panel;
    }

    gb::label_shared_ptr make_label(const gb::ces_entity_shared_ptr& parent, glm::vec2 position,
                                   const std::string& text, float size = 18.f) {
        const auto label = fabricator->create_label_2d("button_label.xml");
        label->font_name = "Vera.ttf";
        label->font_size = size;
        label->font_color = text_color;
        label->text = text;
        label->position = position;
        parent->add_child(label);
        return label;
    }

    gb::ui::button_shared_ptr make_button(const gb::ces_entity_shared_ptr& parent, glm::vec2 position,
                                          glm::vec2 size, const std::string& text,
                                          std::function<void(implementation&)> callback) {
        const std::weak_ptr<implementation> weak = shared_from_this();
        const auto button = controls.create_button(size, [weak, callback](const gb::ces_entity_shared_ptr&) {
            const auto view = weak.lock();
            if(!view) return;
            callback(*view);
            view->last_tool = -1; // Restore persistent selection after native release feedback.
            view->refresh();
        });
        button->position = position;
        button->set_text(text);
        configure_button(button);
        theme_sprite(button->get_element_as<gb::sprite>(gb::ui::control::k_background_element_name));
        const auto foreground = button->get_element_as<gb::sprite>(gb::ui::button::k_foreground_element_name);
        foreground->visible = false;
        hidden_decorations.push_back(foreground);
        parent->add_child(button);
        return button;
    }

    void graphics_changed() {
        if(graphics_save_status) graphics_save_status->text = "";
    }

    void remap_debug_tree() {
        std::function<void(const gb::ces_entity_shared_ptr&)> visit = [&](const gb::ces_entity_shared_ptr& node) {
            if(const auto materials = node->get_component<gb::ces_material_component>()) {
                const int index = gb::ces_material_component::get_technique_name_index("ws.ui");
                if(index >= 0 && size_t(index) < materials->get_materials().size()) {
                    if(const auto material = materials->get_material(index)) {
                        materials->add_material("ws.debug", material);
                        materials->remove_material("ws.ui");
                    }
                }
            }
            node->enumerate_children(visit);
        };
        visit(post_panel);
    }

    gb::sprite_shared_ptr debug_bar(glm::vec2 position, glm::vec2 size, glm::u8vec4 color) {
        auto bar = fabricator->create_sprite("editor_ui.xml");
        bar->position = position; bar->size = size; bar->color = color;
        post_panel->add_child(bar); return bar;
    }

    void bind_debug_scrollbar(const gb::sprite_shared_ptr& track, bool detail) {
        const glm::vec2 size=track->size;
        auto touch=track->add_component<gb::ces_bound_touch_2d_component>();
        touch->set_bounds(glm::vec4(-5,0,size.x+5,size.y));
        const std::weak_ptr<implementation> weak=shared_from_this();
        const auto scroll=[weak,detail](const gb::ces_entity_shared_ptr&,const glm::vec2& point,gb::e_input_source,gb::e_input_state) {
            const auto view=weak.lock();if(!view || !view->post_visible)return;
            const glm::vec2 origin=view->post_panel->position,scale=view->post_panel->scale;
            const float y=(point.y-origin.y)/scale.y;
            const float fraction=glm::clamp((y-(detail?151.f:116.f))/(detail?402.f:420.f),0.f,1.f);
            if(detail)view->detail_scroll=int(std::round(fraction*std::max(0,view->detail_extent-int(detail_height))));
            else if(view->render_debug)view->list_scroll=int(std::round(fraction*std::max(0,int(view->render_debug->passes.size())-list_page)));
            view->refresh_post_panel();
        };
        for(const auto state:{gb::e_input_state_pressed,gb::e_input_state_dragged,gb::e_input_state_released}) {
            touch->enable(state,gb::e_input_source_mouse_left,true);touch->add_callback(state,scroll);
        }
    }

    void build_debug_contents() {
        // Rehouse existing native controls, preserving their callbacks and settings.
        // Snapshot before detach: add_child is not an implicit reparent operation.
        const std::vector<gb::ces_entity_shared_ptr> children = post_panel->children;
        for(const auto& child : children) {
            const auto item = std::dynamic_pointer_cast<gb::game_object_2d>(child);
            if(!item) continue;
            const glm::vec2 position = item->position;
            std::string pass; float y=0, height=42, local_x=0;
            if(position.y>=590) {
                if(child==graphics_save_status) item->position=glm::vec2(568,594);
                else if(position.x==16) { post_panel->remove_child(child); }
                continue;
            }
            if(position.y<45)continue; // panel decorations, title, Save, Close
            if(position.x>=378) {
                pass="ws.base";local_x=position.x-378;
                if(position.y<90)y=226;
                else if(position.y<130)y=268;
                else { const int row=int(std::round((position.y-150.f)/36.f));y=312.f+row*42.f; }
            } else if(child==specular_button) { pass="ws.base";y=822; }
            else if(child==backlight_button) { pass="ws.base";y=864; }
            else if(child==post_view_button) { pass="ss.stylized_final";y=542; }
            else if(position.y<150) {
                int toggle=-1;for(int i=0;i<4;++i)if(child==post_toggle_buttons[i])toggle=i;
                if(toggle<0)continue;
                pass=toggle==0?"ss.ao":"ss.stylized_final";
                y=toggle==0?226.f:226.f+(toggle-1)*40.f;
            } else {
                const int row=int(std::round((position.y-264.f)/42.f));
                if(row<0 || row>=5)continue;
                pass=row==4?"ss.ao":"ss.stylized_final";
                y=row==4?270.f:354.f+row*46.f;local_x=position.x-16;
            }
            auto group=gb::ces_entity::construct<gb::game_object_2d>();
            post_panel->remove_child(child);group->add_child(child);post_panel->add_child(group);
            item->position=glm::vec2(local_x,4);
            debug_rows.push_back({group,pass,y,height});
        }
        make_label(post_panel,glm::vec2(16,56),"Render passes",15);
        pass_title=make_label(post_panel,glm::vec2(266,54),"Select a pass",14);
        pass_triangles=make_label(post_panel,glm::vec2(458,58),"Triangles: 0",12);
        pass_enabled=make_button(post_panel,glm::vec2(624,50),glm::vec2(72,32),"On",[](implementation& view) {
            if(!view.post_visible || !view.render_debug || view.selected_pass>=view.render_debug->passes.size())return;
            auto& pass=view.render_debug->passes[view.selected_pass];
            const bool enabled=!pass.enabled;const std::string id=pass.id;
            pass.enabled=enabled;
            if(view.render_debug->set_enabled)view.render_debug->set_enabled(id,enabled);
            view.graphics_changed();
        });
        attachment_button=make_button(post_panel,glm::vec2(266,87),glm::vec2(430,30),"Attachment",[](implementation& view) {
            if(!view.render_debug || view.selected_pass>=view.render_debug->passes.size())return;
            const auto& attachments=view.render_debug->passes[view.selected_pass].attachments;
            if(!attachments.empty())view.selected_attachment=(view.selected_attachment+1)%attachments.size();
        });
        for(int i=0;i<list_page;++i)pass_buttons.push_back(make_button(post_panel,glm::vec2(16,88.f+i*34.f),glm::vec2(216,30),"",[i](implementation& view) {
            if(!view.post_visible || !view.render_debug)return;
            const size_t index=size_t(view.list_scroll+i);
            if(index<view.render_debug->passes.size()){view.selected_pass=index;view.selected_attachment=0;view.detail_scroll=0;}
        }));
        for(const auto& button:pass_buttons)button->set_font_size(12.f);
        bind_debug_scrollbar(debug_bar(glm::vec2(240,116),glm::vec2(8,420),control_color),false);list_thumb=debug_bar(glm::vec2(240,116),glm::vec2(8,60),selected_color);
        bind_debug_scrollbar(debug_bar(glm::vec2(710,151),glm::vec2(8,402),control_color),true);detail_thumb=debug_bar(glm::vec2(710,151),glm::vec2(8,60),selected_color);
        for(int side=0;side<2;++side) {
            const float x=side?704.f:234.f;
            make_button(post_panel,glm::vec2(x,side?122.f:88.f),glm::vec2(20,24),"^",[side](implementation& view){if(side)view.detail_scroll-=42;else --view.list_scroll;});
            make_button(post_panel,glm::vec2(x,side?558.f:540.f),glm::vec2(20,24),"v",[side](implementation& view){if(side)view.detail_scroll+=42;else ++view.list_scroll;});
        }
        auto group=gb::ces_entity::construct<gb::game_object_2d>();post_panel->add_child(group);
        preview_group=group;
        debug_rows.push_back({group,"",0,214});
        preview=fabricator->create_sprite("render_debug_preview.xml");preview->size=glm::vec2(430,188);preview->color=glm::u8vec4(255);group->add_child(preview);
        preview_uniforms=preview->add_component<gb::ces_shader_uniforms_component>()->construct_uniforms_buffer<render_preview_uniforms>(
            gb::ces_shader_uniforms_component::e_shader_uniform_mode::e_fragment,"render.preview",0);
        attachment_info=make_label(group,glm::vec2(0,194),"Live GPU attachment",12);
        const char* water_names[]={"Reflection","Refraction","Caustics"};
        for(int i=0;i<3;++i){
            auto row=gb::ces_entity::construct<gb::game_object_2d>();post_panel->add_child(row);debug_rows.push_back({row,"ws.ocean",230.f+i*52.f,48});
            make_label(row,glm::vec2(0,4),water_names[i],13);
            water_values[i]=make_label(row,glm::vec2(288,4),"",12);
            water_sliders[i]=controls.create_slider(glm::vec2(160,18));water_sliders[i]->position=glm::vec2(120,4);theme_slider(water_sliders[i]);row->add_child(water_sliders[i]);
            water_sliders[i]->set_callback([weak=std::weak_ptr<implementation>(shared_from_this()),i](float offset){if(auto v=weak.lock()){
                if(!v->post_visible||!v->post_settings)return;
                float* values[]={&v->post_settings->waterReflectionIntensity,&v->post_settings->waterRefractionIntensity,&v->post_settings->waterCausticsIntensity};
                *values[i]=glm::clamp(offset,0.f,1.f)*3.f;v->graphics_changed();
            }});
            water_buttons[i]=make_button(row,glm::vec2(342,0),glm::vec2(80,32),"On",[i](implementation& v){
                if(!v.post_visible||!v.post_settings)return;
                bool* enabled[]={&v.post_settings->enableWaterReflection,&v.post_settings->enableWaterRefraction,&v.post_settings->enableWaterCaustics};
                *enabled[i]=!*enabled[i];v.graphics_changed();
            });
        }
        auto note=gb::ces_entity::construct<gb::game_object_2d>();post_panel->add_child(note);debug_rows.push_back({note,"ws.ocean",398,46});
        make_label(note,glm::vec2(0,0),"Strength: 0 - 3x | Save stores all water settings",11);
        make_label(note,glm::vec2(0,22),"Caustics light the bottom in ws.base",11);
        float terrain_y=230;for(const auto& row:debug_rows)if(row.pass=="ws.base")terrain_y=std::max(terrain_y,row.y+row.height+12);
        auto terrain_group=gb::ces_entity::construct<gb::game_object_2d>();post_panel->add_child(terrain_group);debug_rows.push_back({terrain_group,"ws.base",terrain_y,134});
        for(size_t i=0;i<terrain_info.size();++i)terrain_info[i]=make_label(terrain_group,glm::vec2(0,22.f*i),"",11);
        const char* error_names[]={"Near error px","Normal error px","Far error px"};
        for(int i=0;i<3;++i){
            auto row=gb::ces_entity::construct<gb::game_object_2d>();post_panel->add_child(row);debug_rows.push_back({row,"ws.base",terrain_y+144+i*44.f,42});
            make_label(row,glm::vec2(0,4),error_names[i],12);
            error_values[i]=make_label(row,glm::vec2(366,4),"",12);
            error_sliders[i]=controls.create_slider(glm::vec2(180,18));error_sliders[i]->position=glm::vec2(166,4);theme_slider(error_sliders[i]);row->add_child(error_sliders[i]);
            error_sliders[i]->set_callback([weak=std::weak_ptr<implementation>(shared_from_this()),i](float offset){if(auto v=weak.lock()){
                if(!v->terrain_lod)return;auto& s=*v->terrain_lod;float value=.05f+glm::clamp(offset,0.f,1.f)*5.95f;
                if(i==0)s.nearErrorPx=std::min(value,s.normalErrorPx);else if(i==1)s.normalErrorPx=std::max(s.nearErrorPx,std::min(value,s.farErrorPx));else s.farErrorPx=std::max(value,s.normalErrorPx);
                v->graphics_changed();
            }});
        }
        auto debug_group=gb::ces_entity::construct<gb::game_object_2d>();post_panel->add_child(debug_group);debug_rows.push_back({debug_group,"ws.base",terrain_y+280,38});
        lod_debug_button=make_button(debug_group,glm::vec2(0,0),glm::vec2(420,32),"Terrain debug: Off",[](implementation& v){if(v.terrain_lod){v.terrain_lod->debugMode=(v.terrain_lod->debugMode+1)%6;v.graphics_changed();}});
        make_label(post_panel,glm::vec2(16,594),"F8: Show / Hide | Wheel: scroll hovered column",11)->font_color=muted;
    }

    void layout_debug() {
        remap_debug_tree(); // Also covers children created by native controls after binding.
        const size_t count=render_debug?render_debug->passes.size():0;
        list_scroll=std::max(0,std::min(list_scroll,std::max(0,int(count)-list_page)));
        if(count)selected_pass=std::min(selected_pass,count-1);
        const RenderDebugPass* pass=count?&render_debug->passes[selected_pass]:nullptr;
        for(int i=0;i<list_page;++i) {
            auto button=pass_buttons[i];const size_t index=size_t(list_scroll+i);button->visible=index<count;
            if(index<count) {
                const auto& item=render_debug->passes[index];const std::string text=(item.enabled?"+ ":"- ")+ascii_message(item.id,24);
                if(button->get_text()!=text)button->set_text(text);style_button(button,true,index==selected_pass);
            }
        }
        set_text(pass_title,pass?ascii_message(pass->label.empty()?pass->id:pass->label,22):"No render passes");
        set_text(pass_triangles,"Triangles: "+std::to_string(pass?pass->triangles:0));
        std::istringstream terrain_lines(render_debug?render_debug->terrain_diagnostics:std::string());for(auto label:terrain_info){std::string line;std::getline(terrain_lines,line);set_text(label,line);}
        if(terrain_lod){
            const float values[]={terrain_lod->nearErrorPx,terrain_lod->normalErrorPx,terrain_lod->farErrorPx};
            for(int i=0;i<3;++i){const float offset=(values[i]-.05f)/5.95f;if(std::abs(error_sliders[i]->get_offset()-offset)>.0001f)error_sliders[i]->set_offset(offset);set_text(error_values[i],decimal(values[i]));}
            const char* modes[]={"Terrain debug: Off","Terrain debug: actual reduction","Terrain debug: contour wireframe","Terrain debug: screen error / budget","Terrain debug: collapse targets","Terrain debug: active contour vertices"};
            if(lod_debug_button->get_text()!=modes[terrain_lod->debugMode])lod_debug_button->set_text(modes[terrain_lod->debugMode]);
        }
        const std::string enabled_text=pass&&pass->enabled?"On":"Off";
        if(post_settings){
            const bool enabled[]={post_settings->enableWaterReflection,post_settings->enableWaterRefraction,post_settings->enableWaterCaustics};
            const float intensity[]={post_settings->waterReflectionIntensity,post_settings->waterRefractionIntensity,post_settings->waterCausticsIntensity};
            for(int i=0;i<3;++i){
                const std::string text=enabled[i]?"On":"Off";if(water_buttons[i]->get_text()!=text)water_buttons[i]->set_text(text);style_button(water_buttons[i],true,enabled[i]);
                const float offset=intensity[i]/3.f;if(std::abs(water_sliders[i]->get_offset()-offset)>.0001f)water_sliders[i]->set_offset(offset);
                set_text(water_values[i],decimal(intensity[i])+"x");
            }
        }
        if(pass_enabled->get_text()!=enabled_text)pass_enabled->set_text(enabled_text);
        style_button(pass_enabled,pass!=nullptr,pass&&pass->enabled);
        const RenderDebugAttachment* attachment=nullptr;
        if(pass && !pass->attachments.empty()) {selected_attachment=std::min(selected_attachment,pass->attachments.size()-1);attachment=&pass->attachments[selected_attachment];}
        const std::string text=attachment?"Attachment "+std::to_string(selected_attachment+1)+"/"+std::to_string(pass->attachments.size())+": "+ascii_message(attachment->name,30):"No color attachments";
        if(attachment_button->get_text()!=text)attachment_button->set_text(text);
        attachment_button->disable(!attachment || pass->attachments.size()<2);
        detail_extent=0;
        for(const auto& row:debug_rows)if(row.pass.empty() || (pass && row.pass==pass->id))detail_extent=std::max(detail_extent,int(row.y+row.height));
        detail_scroll=std::max(0,std::min(detail_scroll,std::max(0,detail_extent-int(detail_height))));
        for(const auto& row:debug_rows) {
            const float y=row.y-float(detail_scroll);row.node->position=glm::vec2(266,122+y);
            // Clip complete rows, including all native input descendants.
            row.node->visible=(row.pass.empty() || (pass && row.pass==pass->id)) && y>=0 && y+row.height<=detail_height;
        }
        preview->visible=preview_group->get_is_visible() && attachment && attachment->texture;
        if(attachment && attachment->texture) {
            preview->get_component<gb::ces_material_component>()->set_texture(attachment->texture,gb::e_shader_sampler_01,"ws.debug");
            preview_uniforms->set(glm::vec4(float(attachment->mode),0,0,0),"preview_params");
            const float aspect=attachment->height?float(attachment->width)/float(attachment->height):1.f;
            const float width=std::min(430.f,188.f*aspect),height=width/std::max(.001f,aspect);
            preview->size=glm::vec2(width,height);preview->position=glm::vec2((430-width)*.5f,(188-height)*.5f);
        }
        set_text(attachment_info,attachment?std::to_string(attachment->width)+" x "+std::to_string(attachment->height)+" | Live GPU texture":"This pass exposes no color texture");
        const float list_fraction=count?std::min(1.f,float(list_page)/float(count)):1.f;
        list_thumb->size=glm::vec2(8,420*list_fraction);list_thumb->position=glm::vec2(240,116+(420-420*list_fraction)*(count>size_t(list_page)?float(list_scroll)/float(count-list_page):0.f));
        const float fraction=std::min(1.f,detail_height/std::max(float(detail_height),float(detail_extent)));
        detail_thumb->size=glm::vec2(8,402*fraction);detail_thumb->position=glm::vec2(710,151+(402-402*fraction)*(detail_extent>int(detail_height)?float(detail_scroll)/float(detail_extent-int(detail_height)):0.f));
        for(const auto& decoration:hidden_decorations)if(decoration->get_is_visible())decoration->visible=false;
        for(const auto& slider:post_sliders){const bool visible=slider->get_is_visible();slider_fill(slider,slider->get_offset());if(!visible)slider->get_element_as<gb::sprite>(slider->k_foreground_element_name)->visible=false;}
        for(const auto& slider:lighting_sliders){const bool visible=slider->get_is_visible();slider_fill(slider,slider->get_offset());if(!visible)slider->get_element_as<gb::sprite>(slider->k_foreground_element_name)->visible=false;}
        for(const auto& slider:error_sliders){const bool visible=slider->get_is_visible();slider_fill(slider,slider->get_offset());if(!visible)slider->get_element_as<gb::sprite>(slider->k_foreground_element_name)->visible=false;}
        for(const auto& slider:water_sliders){const bool visible=slider->get_is_visible();slider_fill(slider,slider->get_offset());if(!visible)slider->get_element_as<gb::sprite>(slider->k_foreground_element_name)->visible=false;}
    }

    bool scroll_debug(glm::ivec2 point,float delta) {
        if(!post_panel || !post_visible || !post_panel->get_is_visible())return false;
        const glm::vec2 origin=post_panel->position,scale=post_panel->scale;
        const glm::vec2 local=(glm::vec2(point)-origin)/scale;
        if(local.x<0 || local.y<0 || local.x>=740 || local.y>=620)return false;
        if(local.y>=82 && local.y<592) {
            if(local.x<256)list_scroll+=delta>0?-3:delta<0?3:0;
            else detail_scroll+=delta>0?-84:delta<0?84:0;
            refresh_post_panel();
        }
        return true;
    }

    void build_post_panel() {
        if(post_panel || !post_settings) return;
        const float scale = std::min(1.f, std::min(
            std::max(1.f, static_cast<float>(resolution.x) - 40.f) / 740.f,
            std::max(1.f, static_cast<float>(resolution.y) - 40.f) / 620.f));
        const glm::vec2 size(740, 620);
        post_panel = make_panel((glm::vec2(resolution) - size * scale) * .5f, size, "Render Inspector");
        post_panel->scale = glm::vec2(scale);
        post_panel->set_background_color(glm::u8vec4(5, 36, 53, 238));
        make_button(post_panel, glm::vec2(568, 12), glm::vec2(110, 28), "Save", [](implementation& view) {
            if(!view.post_visible) return;
            view.refresh_post_panel();
            const bool saved=view.graphics_save_callback && view.graphics_save_callback();
            view.graphics_save_status->text=saved?"Saved":"Save failed";
            view.graphics_save_status->font_color=saved?glm::u8vec4(145,235,160,255):glm::u8vec4(255,120,100,255);
        });
        make_button(post_panel, glm::vec2(688, 12), glm::vec2(36, 28), "X", [](implementation& view) {
            view.post_visible = false;
        });
        for(size_t i = 0; i < post_toggle_buttons.size(); ++i)
            post_toggle_buttons[i] = make_button(post_panel,
                glm::vec2(16.f + (i % 2) * 180.f, 60.f + (i / 2) * 44.f), glm::vec2(168, 36), post_toggle_names[i],
                [i](implementation& view) {
                    if(!view.post_settings || !view.post_visible) return;
                    auto& enabled = view.post_settings.get()->*post_toggle_fields[i];
                    enabled = !enabled;
                    view.graphics_changed();
                });
        post_view_button = make_button(post_panel, glm::vec2(16, 202), glm::vec2(348, 36), "View: Final",
            [](implementation& view) {
                if(!view.post_settings || !view.post_visible) return;
                view.post_settings->sanitize();
                const auto current = std::find(post_view_values.begin(), post_view_values.end(), view.post_settings->debugView);
                const size_t next = current == post_view_values.end() ? 0 :
                    (size_t(current - post_view_values.begin()) + 1) % post_view_values.size();
                view.post_settings->debugView = post_view_values[next];
                view.graphics_changed();
            });
        const std::weak_ptr<implementation> weak = shared_from_this();
        last_post_values.fill(std::numeric_limits<float>::quiet_NaN());
        last_lighting_values.fill(std::numeric_limits<float>::quiet_NaN());
        for(size_t i = 0; i < post_sliders.size(); ++i) {
            const float y = 264.f + i * 42.f;
            make_label(post_panel, glm::vec2(16, y - 1), post_slider_names[i], 13.f);
            post_values[i] = make_label(post_panel, glm::vec2(318, y - 1), "0.00", 13.f);
            post_values[i]->font_color = cyan;
            post_sliders[i] = controls.create_slider(glm::vec2(160, 18));
            post_sliders[i]->position = glm::vec2(140, y);
            theme_slider(post_sliders[i]);
            post_sliders[i]->set_foreground_color(glm::u8vec4(0, 171, 217, 255));
            post_panel->add_child(post_sliders[i]);
            post_sliders[i]->set_callback([weak, i](float offset) {
                const auto view = weak.lock();
                if(!view || !view->post_settings || !view->post_visible || !view->post_sliders[i]->get_is_visible()) return;
                view->post_settings.get()->*post_slider_fields[i] = post_slider_min[i] +
                    glm::clamp(offset, 0.f, 1.f) * (post_slider_max[i] - post_slider_min[i]);
                view->post_settings->sanitize();
                view->graphics_changed();
                view->refresh_post_panel();
            });
        }
        lighting_mode_button = make_button(post_panel, glm::vec2(378, 54), glm::vec2(346, 32), "Lighting: Soft",
            [](implementation& view) {
                if(view.post_visible) {
                    auto& lighting = stylized_lighting_settings();
                    lighting.mode = lighting.mode == 0 ? 1 : 0;
                    view.graphics_changed();
                }
            });
        make_button(post_panel, glm::vec2(378, 96), glm::vec2(168, 32), "Preset: Soft", [](implementation& view) {
            if(view.post_visible) {
                const float rotation=stylized_lighting_settings().skyboxRotation;
                stylized_lighting_settings()=StylizedLightingSettings::soft();
                stylized_lighting_settings().skyboxRotation=rotation;
            }
            view.graphics_changed();
        });
        make_button(post_panel, glm::vec2(556, 96), glm::vec2(168, 32), "Preset: Sunset", [](implementation& view) {
            if(view.post_visible) {
                const float rotation=stylized_lighting_settings().skyboxRotation;
                stylized_lighting_settings()=StylizedLightingSettings::sunset();
                stylized_lighting_settings().skyboxRotation=rotation;
            }
            view.graphics_changed();
        });
        for(size_t i = 0; i < lighting_sliders.size(); ++i) {
            const float y = 150.f + i * 36.f;
            make_label(post_panel, glm::vec2(378, y - 1), lighting_slider_names[i], 12.f);
            lighting_values[i] = make_label(post_panel, glm::vec2(674, y - 1), "0.00", 12.f);
            lighting_values[i]->font_color = cyan;
            lighting_sliders[i] = controls.create_slider(glm::vec2(130, 18));
            lighting_sliders[i]->position = glm::vec2(526, y);
            theme_slider(lighting_sliders[i]);
            lighting_sliders[i]->set_foreground_color(glm::u8vec4(0, 171, 217, 255));
            post_panel->add_child(lighting_sliders[i]);
            lighting_sliders[i]->set_callback([weak, i](float offset) {
                const auto view = weak.lock();
                if(!view || !view->post_visible || !view->lighting_sliders[i]->get_is_visible()) return;
                auto& lighting = stylized_lighting_settings();
                lighting_slider_value(lighting, i) = lighting_slider_min[i] +
                    glm::clamp(offset, 0.f, 1.f) * (lighting_slider_max[i] - lighting_slider_min[i]);
                lighting.sanitize();
                view->graphics_changed();
                view->refresh_post_panel();
            });
        }
        specular_button = make_button(post_panel, glm::vec2(16, 492), glm::vec2(348, 32), "Stylized specular: On",
            [](implementation& view) {
                if(view.post_visible) {
                    auto& lighting = stylized_lighting_settings();
                    lighting.enableStylizedSpecular = !lighting.enableStylizedSpecular;
                    view.graphics_changed();
                }
            });
        backlight_button = make_button(post_panel, glm::vec2(16, 534), glm::vec2(348, 32), "Vegetation backlight: On",
            [](implementation& view) {
                if(view.post_visible) {
                    auto& lighting = stylized_lighting_settings();
                    lighting.enableVegetationBacklight = !lighting.enableVegetationBacklight;
                    view.graphics_changed();
                }
            });
        make_label(post_panel, glm::vec2(16, 592), "F8: Show / Hide | Linear RGB", 12.f)->font_color = muted;
        graphics_save_status=make_label(post_panel, glm::vec2(378, 592), "", 12.f);
        build_debug_contents();
        // The overlay follows the native UI's ascending depth convention.
        float depth = -.5f;
        std::function<void(const gb::ces_entity_shared_ptr&)> prepare = [&](const gb::ces_entity_shared_ptr& node) {
            const auto transform = node->get_component<gb::ces_transformation_2d_component>();
            if(transform) {
                transform->set_is_in_camera_space(false);
                transform->set_z_order(depth);
                depth += .0005f;
            }
            node->enumerate_children(prepare);
        };
        prepare(post_panel);
        remap_debug_tree();
        post_panel->visible = false;
    }

    void refresh_post_panel() {
        if(!post_panel) return;
        const bool show = post_visible && post_settings && entity->get_is_visible();
        if(post_panel->get_is_visible() != show) post_panel->visible = show;
        if(!show) return;
        auto model = *post_settings;
        model.sanitize();
        auto lighting = stylized_lighting_settings();
        lighting.sanitize();
        const auto refresh_toggle = [&](const gb::ui::button_shared_ptr& button, const char* name, bool enabled, size_t index) {
            if(last_post_toggles[index] == int(enabled) && last_tool != -1) return;
            const std::string text = std::string(name) + (enabled ? ": On" : ": Off");
            if(button->get_text() != text) button->set_text(text);
            style_button(button, true, enabled);
            last_post_toggles[index] = int(enabled);
        };
        for(size_t i = 0; i < post_toggle_buttons.size(); ++i)
            refresh_toggle(post_toggle_buttons[i], post_toggle_names[i], model.*post_toggle_fields[i], i);
        refresh_toggle(specular_button, "Stylized specular", lighting.enableStylizedSpecular, 4);
        refresh_toggle(backlight_button, "Vegetation backlight", lighting.enableVegetationBacklight, 5);
        if(last_lighting_mode != lighting.mode) {
            lighting_mode_button->set_text(lighting.mode == 0 ? "Lighting: Cel" : "Lighting: Soft");
            last_lighting_mode = lighting.mode;
        }
        const auto current = std::find(post_view_values.begin(), post_view_values.end(), model.debugView);
        const size_t view_index = current == post_view_values.end() ? 0 : size_t(current - post_view_values.begin());
        if(last_post_view != int(view_index)) {
            post_view_button->set_text(std::string("View: ") + post_view_names[view_index]);
            last_post_view = int(view_index);
        }
        for(size_t i = 0; i < post_sliders.size(); ++i) {
            const float value = model.*post_slider_fields[i];
            const float offset = glm::clamp((value - post_slider_min[i]) /
                (post_slider_max[i] - post_slider_min[i]), 0.f, 1.f);
            if(std::abs(post_sliders[i]->get_offset() - offset) > .0001f) post_sliders[i]->set_offset(offset);
            const auto fill = post_sliders[i]->get_element_as<gb::sprite>(post_sliders[i]->k_foreground_element_name);
            const glm::vec2 fill_size = fill->size;
            if(std::abs(fill_size.x - std::max(4.f, 156.f * offset)) > .0001f) slider_fill(post_sliders[i], offset);
            if(fill->get_is_visible() != (offset > .001f)) fill->visible = offset > .001f;
            if(last_post_values[i] != value) {
                set_text(post_values[i], post_decimal(value));
                last_post_values[i] = value;
            }
        }
        for(size_t i = 0; i < lighting_sliders.size(); ++i) {
            const float value = lighting_slider_value(lighting, i);
            const float offset = glm::clamp((value - lighting_slider_min[i]) /
                (lighting_slider_max[i] - lighting_slider_min[i]), 0.f, 1.f);
            if(std::abs(lighting_sliders[i]->get_offset() - offset) > .0001f) lighting_sliders[i]->set_offset(offset);
            const auto fill = lighting_sliders[i]->get_element_as<gb::sprite>(lighting_sliders[i]->k_foreground_element_name);
            const glm::vec2 fill_size = fill->size;
            if(std::abs(fill_size.x - std::max(4.f, 126.f * offset)) > .0001f) slider_fill(lighting_sliders[i], offset);
            if(fill->get_is_visible() != (offset > .001f)) fill->visible = offset > .001f;
            if(last_lighting_values[i] != value) {
                set_text(lighting_values[i], post_decimal(value));
                last_lighting_values[i] = value;
            }
        }
        layout_debug();
    }

    void build() {
        if(inspector_only) return;
        top = make_panel(glm::vec2(18, 18), glm::vec2(380, 76), "");
        // Keep the top row clear of the statistics HUD on narrower windows.
        const float toolbar_scale = terrain_editor_toolbar_scale(float(resolution.x),float(resolution.y));
        top->scale = glm::vec2(toolbar_scale);
        const auto toolbar_background = top->get_element_as<gb::sprite>(gb::ui::control::k_background_element_name);
        toolbar_background->visible = false;
        hidden_decorations.push_back(toolbar_background);
        const std::array<const char*, 4> action_names = {"Undo", "Redo", "Save", "Load"};
        for(size_t i = 0; i < actions.size(); ++i) {
            actions[i] = make_button(top, glm::vec2(i * 94.f, 0), glm::vec2(84, 76), action_names[i],
                [i](implementation& view) {
                    view.state->end_stroke();
                    if(i == 0) view.state->undo();
                    else if(i == 1) view.state->redo();
                    else if(i == 2 && view.save_callback) view.save_callback();
                    else if(i == 3 && view.load_callback) view.load_callback();
                    else view.state->error = "Native file dialog is unavailable";
                });
            icon_button(actions[i], int(i), 42.f);
        }
        daytime = make_panel(glm::vec2(18.f + 400.f * toolbar_scale, 18.f),
            glm::vec2(500, 152), "");
        daytime->scale = glm::vec2(toolbar_scale);
        make_label(daytime, glm::vec2(18, 10), "Time of day", 14.f);
        daytime_value = make_label(daytime, glm::vec2(188, 10), "10.0 h", 13.f);
        daytime_value->font_color = cyan;
        daytime_slider = controls.create_slider(glm::vec2(180, 18));
        daytime_slider->position = glm::vec2(18, 36);
        theme_slider(daytime_slider);
        daytime_slider->set_foreground_color(glm::u8vec4(0, 171, 217, 255));
        daytime->add_child(daytime_slider);
        make_label(daytime, glm::vec2(18, 58), "0 h", 10.f)->font_color = muted;
        make_label(daytime, glm::vec2(175, 58), "24 h", 10.f)->font_color = muted;
        const std::weak_ptr<implementation> daytime_weak = shared_from_this();
        daytime_slider->set_callback([daytime_weak](float offset) {
            const auto view = daytime_weak.lock();
            if(!view || !view->entity->get_is_visible()) return;
            view->set_daytime_offset(offset);
        });
        // Native sliders ignore points outside their track. Keep this captured
        // drag clamped at 0/24h even when the pointer crosses a track end.
        const auto daytime_touch=daytime_slider->get_component<gb::ces_bound_touch_component>();
        const auto drag_daytime=[daytime_weak](const gb::ces_entity_shared_ptr&,const glm::vec2& point,gb::e_input_source,gb::e_input_state){
            const auto view=daytime_weak.lock();if(!view||!view->entity->get_is_visible())return;
            const auto matrix=gb::ces_transformation_extension::get_absolute_transformation_in_ws(view->daytime_slider);
            const auto left=glm::transform(glm::vec2(0),matrix);
            const glm::vec2 size=view->daytime_slider->size;
            const auto right=glm::transform(size,matrix);
            if(right.x>left.x)view->set_daytime_offset((point.x-left.x)/(right.x-left.x));
        };
        daytime_touch->add_callback(gb::e_input_state_dragged,drag_daytime);
        daytime_touch->add_callback(gb::e_input_state_released,drag_daytime);
        make_label(daytime, glm::vec2(18, 86), "Skybox rotation", 14.f);
        skybox_value=make_label(daytime, glm::vec2(188, 86), "0 deg", 13.f);
        skybox_value->font_color=cyan;
        skybox_slider=controls.create_slider(glm::vec2(180, 18));
        skybox_slider->position=glm::vec2(18, 112);
        theme_slider(skybox_slider);
        skybox_slider->set_foreground_color(glm::u8vec4(0, 171, 217, 255));
        daytime->add_child(skybox_slider);
        make_label(daytime, glm::vec2(18, 134), "0", 10.f)->font_color=muted;
        make_label(daytime, glm::vec2(175, 134), "360", 10.f)->font_color=muted;
        skybox_slider->set_callback([daytime_weak](float offset) {
            const auto view=daytime_weak.lock();
            if(view && view->entity->get_is_visible()) view->set_skybox_offset(offset);
        });
        const auto skybox_touch=skybox_slider->get_component<gb::ces_bound_touch_component>();
        const auto drag_skybox=[daytime_weak](const gb::ces_entity_shared_ptr&,const glm::vec2& point,gb::e_input_source,gb::e_input_state){
            const auto view=daytime_weak.lock();if(!view||!view->entity->get_is_visible())return;
            const auto matrix=gb::ces_transformation_extension::get_absolute_transformation_in_ws(view->skybox_slider);
            const auto left=glm::transform(glm::vec2(0),matrix);
            const glm::vec2 size=view->skybox_slider->size;
            const auto right=glm::transform(size,matrix);
            if(right.x>left.x)view->set_skybox_offset((point.x-left.x)/(right.x-left.x));
        };
        skybox_touch->add_callback(gb::e_input_state_dragged,drag_skybox);
        skybox_touch->add_callback(gb::e_input_state_released,drag_skybox);
        const char* lod_names[2]={"Terrain LODs","Contour simplification"};
        for(size_t i=0;i<2;++i){
            const float y=10.f+76.f*i;
            make_label(daytime,glm::vec2(268,y),lod_names[i],14.f);
            lod_values[i]=make_label(daytime,glm::vec2(i==0?374:456,y),i==0?"5":"0%",13.f);lod_values[i]->font_color=cyan;
            lod_sliders[i]=controls.create_slider(glm::vec2(i==0?100:180,18));lod_sliders[i]->position=glm::vec2(268,y+26);theme_slider(lod_sliders[i]);daytime->add_child(lod_sliders[i]);
            make_label(daytime,glm::vec2(268,y+48),i==0?"1":"0%",10.f)->font_color=muted;
            make_label(daytime,glm::vec2(i==0?350:425,y+48),i==0?"8":"90%",10.f)->font_color=muted;
            lod_sliders[i]->set_callback([daytime_weak,i](float offset){if(auto view=daytime_weak.lock())view->set_lod_offset(i,offset);});
            const auto drag_lod=[daytime_weak,i](const gb::ces_entity_shared_ptr&,const glm::vec2& point,gb::e_input_source,gb::e_input_state){
                auto view=daytime_weak.lock();if(!view||!view->entity->get_is_visible())return;
                const auto matrix=gb::ces_transformation_extension::get_absolute_transformation_in_ws(view->lod_sliders[i]);
                const glm::vec2 size=view->lod_sliders[i]->size;
                const auto left=glm::transform(glm::vec2(0),matrix),right=glm::transform(size,matrix);
                if(right.x>left.x)view->set_lod_offset(i,(point.x-left.x)/(right.x-left.x));
            };
            const auto touch=lod_sliders[i]->get_component<gb::ces_bound_touch_component>();touch->add_callback(gb::e_input_state_dragged,drag_lod);touch->add_callback(gb::e_input_state_released,drag_lod);
        }
        make_label(daytime,glm::vec2(398,10),"Wireframe",12.f);
        wireframe_toggle=make_button(daytime,glm::vec2(398,32),glm::vec2(84,26),"Off",[](implementation& view){
            if(!view.terrain_lod||!view.terrain_lod->wireframe_available)return;
            view.terrain_lod->wireframe=!view.terrain_lod->wireframe;
            view.graphics_changed();view.refresh_lod();
        });
        terrain_triangle_label=make_label(daytime,glm::vec2(398,64),"Tris: --",10.f);
        dirty_label = make_label(top, glm::vec2(282, 82), "Saved", 13.f);

        tools = make_panel(glm::vec2(18, 114), glm::vec2(108, 522), "");
        for(size_t i = 0; i < tool_buttons.size(); ++i) {
            tool_buttons[i] = make_button(tools, glm::vec2(10, 10.f + i * 102.f), glm::vec2(88, 94), tool_names[i],
                [i](implementation& view) { view.state->select_tool(tool_types[i]); });
            if(tool_types[i] == TerrainEditorTool::Buildings) {
                tool_buttons[i]->set_font_size(13.f);
                tool_buttons[i]->set_text(tool_names[i]);
                building_icon_button(tool_buttons[i], 0, 64.f);
            } else icon_button(tool_buttons[i], tool_icon_tiles[i], 64.f);
        }

        // Fit the two right-hand panels without overlap at the demo's 720px
        // window height while keeping the reference proportions on taller views.
        const float right_scale = glm::clamp((static_cast<float>(resolution.y) - 390.f) / 398.f, .65f, 1.f);
        const float right = static_cast<float>(resolution.x) - 18.f - 386.f * right_scale;
        settings = make_panel(glm::vec2(right, 116), glm::vec2(386, 398), "Terrain Tool");
        settings->scale = glm::vec2(right_scale);
        make_icon(settings, glm::vec2(14, 8), glm::vec2(42), 4);
        settings->get_element_as<gb::label>(gb::ui::control::k_label_element_name)->position = glm::vec2(62, 18);
        brush_label = make_label(settings, glm::vec2(20, 64), "Brush Size");
        brush_value = make_label(settings, glm::vec2(328, 64), "8.0 m");
        brush_slider = controls.create_slider(glm::vec2(180, 18));
        brush_slider->position = glm::vec2(132, 61);
        brush_slider->set_background_color(border_color);
        brush_slider->set_foreground_color(control_color);
        brush_slider->set_slider_color(cyan);
        theme_slider(brush_slider);
        settings->add_child(brush_slider);
        make_label(settings, glm::vec2(20, 88), "Diameter: 1 - 32 m", 11.f);

        strength_label = make_label(settings, glm::vec2(20, 119), "Strength");
        strength_value = make_label(settings, glm::vec2(333, 119), "70%");
        strength_slider = controls.create_slider(glm::vec2(180, 18));
        strength_slider->position = glm::vec2(132, 116);
        strength_slider->set_background_color(border_color);
        strength_slider->set_foreground_color(control_color);
        strength_slider->set_slider_color(cyan);
        theme_slider(strength_slider);
        settings->add_child(strength_slider);
        level_label = make_label(settings, glm::vec2(20, 159), "Terrace Target", 14.f);
        target_value = make_label(settings, glm::vec2(235, 160), "+8", 18.f);
        for(size_t i = 0; i < target_buttons.size(); ++i) {
            target_buttons[i] = make_button(settings, glm::vec2(i == 0 ? 175.f : 303.f, 151), glm::vec2(40, 36),
                "", [i](implementation& view) {
                    if(view.state->current_tool == TerrainEditorTool::Terrain)
                        view.state->set_target_terrace(view.state->target_terrace + (i == 0 ? -1 : 1));
                });
            make_icon(target_buttons[i], glm::vec2(8, 6), glm::vec2(24), 13 + int(i));
        }
        level_value = make_label(settings, glm::vec2(20, 196), "Current: --   Range: -5 to +16", 12.f);
        palette_label = make_label(settings, glm::vec2(20, 235), "Grass Palette", 14.f);
        palette_value = make_label(settings, glm::vec2(20, 381), "Green", 11.f);

        for(size_t i = 0; i < grass_buttons.size(); ++i) {
            grass_buttons[i] = make_button(settings, glm::vec2(58.f + i * 102.f, 270), glm::vec2(84, 102), grass_names[i],
                [i](implementation& view) {
                    if(!terrain_tool_ui_config(view.state->current_tool).palette || view.state->current_tool != TerrainEditorTool::Grass) return;
                    view.state->end_stroke();
                    view.state->selected_grass = static_cast<GrassType>(i);
                });
            const std::array<glm::u8vec4, 3> grass_colors = {
                glm::u8vec4(39, 94, 57, 230), glm::u8vec4(126, 95, 42, 230), glm::u8vec4(126, 151, 158, 230)};
            grass_buttons[i]->set_foreground_color(grass_colors[i], control_state::e_none);
            grass_buttons[i]->set_font_size(13.f);
            grass_buttons[i]->set_text(grass_buttons[i]->get_text());
            icon_button(grass_buttons[i], i == 2 ? 21 : 16 + int(i), 58.f);
        }
        const std::weak_ptr<implementation> weak = shared_from_this();
        brush_slider->set_callback([weak](float offset) {
            const auto view = weak.lock();
            if(!view || !terrain_tool_ui_config(view->state->current_tool).brush) return;
            view->state->end_stroke();
            view->state->brush_size = glm::clamp(1.f + offset * 31.f, 1.f, 32.f);
            view->refresh();
        });
        strength_slider->set_callback([weak](float offset) {
            const auto view = weak.lock();
            if(!view || !terrain_tool_ui_config(view->state->current_tool).strength) return;
            view->state->end_stroke();
            view->state->strength = glm::clamp(offset, 0.f, 1.f);
            view->refresh();
        });

        trees = make_panel(glm::vec2(right, static_cast<float>(resolution.y) - 260.f), glm::vec2(386, 176), "Trees");
        trees->scale = glm::vec2(right_scale);
        for(size_t i = 0; i < tree_buttons.size(); ++i) {
            tree_buttons[i] = make_button(trees, glm::vec2(12.f + i * 72.f, 46.f),
                glm::vec2(70, 100), tree_names[i], [i](implementation& view) {
                    if(!terrain_tool_ui_config(view.state->current_tool).trees) return;
                    view.state->end_stroke();
                    view.state->selected_tree = static_cast<TreeType>(i);
                });
            tree_buttons[i]->set_font_size(15.f);
            tree_buttons[i]->set_text(tree_buttons[i]->get_text());
            icon_button(tree_buttons[i], tree_icon_tiles[i], 60.f);
        }
        tree_hint = make_label(trees, glm::vec2(16, 151), "LMB Place  /  Shift+LMB Remove", 11.f);

        buildings = make_panel(glm::vec2(right, static_cast<float>(resolution.y) - 84.f - 340.f * right_scale),
            glm::vec2(386, 340), "Buildings");
        buildings->scale = glm::vec2(right_scale);
        for(size_t i = 0; i < building_buttons.size(); ++i) {
            building_buttons[i] = make_button(buildings, glm::vec2(12.f + i * 122.f, 46.f),
                glm::vec2(116, 126), building_definition(building_types[i]).name, [i](implementation& view) {
                    if(view.state->current_tool != TerrainEditorTool::Buildings) return;
                    view.state->end_stroke();
                    view.state->selected_building = building_types[i];
                });
            building_buttons[i]->set_font_size(13.f);
            building_buttons[i]->set_text(building_buttons[i]->get_text());
            building_icon_button(building_buttons[i], i, 96.f);
        }
        selected_building_label = make_label(buildings, glm::vec2(16, 184), "Selected: Forester", 13.f);
        make_label(buildings, glm::vec2(16, 208), "Rotation", 14.f);
        rotation_slider = controls.create_slider(glm::vec2(210, 18));
        rotation_slider->position = glm::vec2(18, 236);
        theme_slider(rotation_slider);
        rotation_slider->set_foreground_color(glm::u8vec4(0, 171, 217, 255));
        buildings->add_child(rotation_slider);
        make_label(buildings, glm::vec2(18, 258), "0", 10.f);
        make_label(buildings, glm::vec2(202, 258), "360", 10.f);
        rotation_field = controls.create_textfield(glm::vec2(88, 34), "0.0");
        rotation_field->position = glm::vec2(244, 228);
        rotation_field->set_font_name("Vera.ttf");
        rotation_field->set_font_size(16.f);
        rotation_field->set_text_color(text_color);
        rotation_field->set_background_color(control_color);
        theme_sprite(rotation_field->get_element_as<gb::sprite>(gb::ui::control::k_background_element_name));
        const auto field_foreground = rotation_field->get_element_as<gb::sprite>(rotation_field->k_foreground_element_name);
        field_foreground->visible = false;
        hidden_decorations.push_back(field_foreground);
        buildings->add_child(rotation_field);
        make_label(buildings, glm::vec2(299, 208), "deg", 10.f);
        for(size_t i = 0; i < 2; ++i) {
            const auto step = make_button(buildings, glm::vec2(342, 228.f + i * 18.f), glm::vec2(28, 16),
                i == 0 ? "+" : "-", [i](implementation& view) {
                    if(view.state->current_tool != TerrainEditorTool::Buildings) return;
                    view.rotation_field->get_component<gb::ui::ces_textedit_component>()->focus = false;
                    const float step=view.state->building_rotation_snap?static_cast<float>(view.state->building_rotation_snap):1.f;
                    view.state->set_building_rotation_degrees(building_degrees(view.state->building_yaw) + (i == 0 ? step : -step));
                });
            step->set_font_size(11.f);
            step->set_text(step->get_text());
        }
        make_label(buildings, glm::vec2(16, 285), "Snap", 13.f);
        for(size_t i = 0; i < rotation_snap_buttons.size(); ++i) {
            rotation_snap_buttons[i] = make_button(buildings, glm::vec2(74.f + i * 74.f, 278), glm::vec2(68, 30),
                rotation_snap_names[i], [i](implementation& view) {
                    if(view.state->current_tool != TerrainEditorTool::Buildings) return;
                    view.state->set_building_rotation_snap(rotation_snaps[i]);
                });
            rotation_snap_buttons[i]->set_font_size(13.f);
            rotation_snap_buttons[i]->set_text(rotation_snap_buttons[i]->get_text());
        }
        make_label(buildings, glm::vec2(16, 319), "Q/E Rotate  /  LMB Place  /  Shift+LMB Remove", 10.f);

        rotation_slider->set_callback([weak](float offset) {
            const auto view = weak.lock();
            if(!view || view->state->current_tool != TerrainEditorTool::Buildings) return;
            view->rotation_field->get_component<gb::ui::ces_textedit_component>()->focus = false;
            view->state->set_building_rotation_degrees(offset * 360.f);
            view->rotation_slider_callback = true;
            view->refresh();
            view->rotation_slider_callback = false;
        });
        const auto rotation_touch = rotation_slider->get_component<gb::ces_bound_touch_component>();
        rotation_touch->add_callback(gb::e_input_state_pressed, [weak](const gb::ces_entity_shared_ptr&, const glm::vec2&, gb::e_input_source, gb::e_input_state) {
            if(const auto view = weak.lock()) view->rotation_slider_active = true;
        });
        rotation_touch->add_callback(gb::e_input_state_released, [weak](const gb::ces_entity_shared_ptr&, const glm::vec2&, gb::e_input_source, gb::e_input_state) {
            if(const auto view = weak.lock()) view->rotation_slider_active = false;
        });
        // The native focus getter asserts and native backspace assumes nonempty
        // text. Keep safe editing callbacks local to this editor field.
        const auto textedit = rotation_field->get_component<gb::ui::ces_textedit_component>();
        textedit->set_on_focus_changed_callback([weak](bool focused) {
            if(const auto view = weak.lock()) view->rotation_focus(focused);
        });
        textedit->set_on_text_changed_callback([weak](const std::string& input) {
            const auto view = weak.lock();
            if(!view || !view->rotation_focused) return;
            if(input == "\r" || input == "\n" || input == "\x1b") {
                view->rotation_field->get_component<gb::ui::ces_textedit_component>()->focus = false;
                return;
            }
            std::string accepted;
            for(char symbol : input) {
#if defined(__WINOS__)
                const auto code=static_cast<unsigned char>(symbol);
                if(code>=0x60&&code<=0x69)symbol=static_cast<char>('0'+code-0x60);
                else if(code==0xbe||code==0x6e)symbol='.';
#endif
                if((symbol >= '0' && symbol <= '9') || symbol == '.') accepted.push_back(symbol);
            }
            if(accepted.empty()) return;
            std::string text = view->rotation_replace_text ? std::string() : view->rotation_field->get_text();
            for(char symbol : accepted) {
                if(symbol != '.' || text.find('.') == std::string::npos) text.push_back(symbol);
            }
            if(text.size() <= 8) {
                view->rotation_replace_text = false;
                view->edit_rotation(text);
            }
        });
        textedit->set_on_backspace_callback([weak]() {
            const auto view = weak.lock();
            if(!view || !view->rotation_focused) return;
            auto text = view->rotation_field->get_text();
            if(view->rotation_replace_text) text.clear();
            else if(!text.empty()) text.pop_back();
            view->rotation_replace_text = false;
            view->edit_rotation(text);
        });
        rotation_field->get_component<gb::ces_bound_touch_component>()->add_callback(gb::e_input_state_released,
            [weak](const gb::ces_entity_shared_ptr&, const glm::vec2&, gb::e_input_source, gb::e_input_state) {
                if(const auto view = weak.lock()) view->rotation_field->get_component<gb::ui::ces_textedit_component>()->focus = true;
            });

        status = make_panel(glm::vec2(18, static_cast<float>(resolution.y) - 68.f),
            glm::vec2(static_cast<float>(resolution.x) - 36.f, 52), "");
        make_icon(status, glm::vec2(12, 7), glm::vec2(34), 4);
        status_label = make_label(status, glm::vec2(55, 10), "LMB Sculpt", 14.f);
        error_label = make_label(status, glm::vec2(55, 31), "MMB Orbit  |  Ctrl+Z Undo  |  Ctrl+Y Redo", 10.f);

        // Match the existing native island HUD: fixed 2D pixel coordinates and
        // unique depths within the camera range. Children keep local transforms.
        float depth = -.9f;
        std::function<void(const gb::ces_entity_shared_ptr&)> prepare = [&](const gb::ces_entity_shared_ptr& node) {
            const auto transform = node->get_component<gb::ces_transformation_2d_component>();
            if(transform) {
                transform->set_is_in_camera_space(false);
                transform->set_z_order(depth);
                depth += .0005f;
            }
            node->enumerate_children(prepare);
        };
        prepare(entity);
        refresh();
    }

    void set_daytime_offset(float offset) {
        offset=glm::clamp(offset,0.f,1.f);
        if(offset<.01f)offset=0.f;else if(offset>.99f)offset=1.f;
        auto& lighting=stylized_lighting_settings();lighting.timeOfDay=offset*24.f;
        graphics_changed();
        lighting.sanitize();refresh_daytime();
    }

    void set_skybox_offset(float offset) {
        offset=glm::clamp(offset,0.f,1.f);
        if(offset<.01f)offset=0.f;else if(offset>.99f)offset=1.f;
        stylized_lighting_settings().skyboxRotation=offset*360.f;
        graphics_changed();refresh_daytime();
    }

    void set_lod_offset(size_t i,float offset){
        if(!entity->get_is_visible()||!terrain_lod||(i==1&&terrain_lod->lodCount==1))return;
        offset=glm::clamp(offset,0.f,1.f);
        bool changed=i==0?terrain_lod->set_lod_count(int(std::round(1+offset*7))):terrain_lod->set_contour_percent(std::round(offset*90));
        if(changed)graphics_changed();refresh_lod();
    }
    void refresh_lod(){
        if(!terrain_lod||!lod_sliders[0])return;
        // Text/color setters recreate the font mesh and its GPU resources.
        // Reapply native feedback only when the persistent state changes.
        if(wireframe_toggle&&(last_wireframe!=int(terrain_lod->wireframe)||last_wireframe_available!=int(terrain_lod->wireframe_available))){
            wireframe_toggle->set_text(terrain_lod->wireframe_available?(terrain_lod->wireframe?"On":"Off"):"N/A");
            style_button(wireframe_toggle,terrain_lod->wireframe_available,terrain_lod->wireframe);
            last_wireframe=int(terrain_lod->wireframe);last_wireframe_available=int(terrain_lod->wireframe_available);
        }
        set_text(lod_values[0],std::to_string(terrain_lod->lodCount));set_text(lod_values[1],std::to_string(int(terrain_lod->contourSimplificationPercent))+"%");
        const float values[2]={(terrain_lod->lodCount-1)/7.f,terrain_lod->contourSimplificationPercent/90.f};
        for(size_t i=0;i<2;++i){if(std::abs(lod_sliders[i]->get_offset()-values[i])>.0001f)lod_sliders[i]->set_offset(values[i]);slider_fill(lod_sliders[i],values[i]);}
        const int disabled=terrain_lod->lodCount==1;
        if(last_lod_disabled!=disabled){lod_sliders[1]->disable(disabled);last_lod_disabled=disabled;}
    }
    void refresh_daytime() {
        refresh_lod();
        auto lighting = stylized_lighting_settings();
        lighting.sanitize();
        const float rotation=lighting.skyboxRotation,sky_offset=rotation/360.f;
        if(std::abs(skybox_slider->get_offset()-sky_offset)>.0001f) skybox_slider->set_offset(sky_offset);
        const auto sky_fill=skybox_slider->get_element_as<gb::sprite>(skybox_slider->k_foreground_element_name);
        const glm::vec2 sky_fill_size=sky_fill->size;
        if(first_update||std::abs(sky_fill_size.x-std::max(4.f,176.f*sky_offset))>.0001f) slider_fill(skybox_slider,sky_offset);
        if(sky_fill->get_is_visible()!=(sky_offset>.001f))sky_fill->visible=sky_offset>.001f;
        if(last_skybox!=rotation){set_text(skybox_value,std::to_string(static_cast<int>(std::round(rotation)))+" deg");last_skybox=rotation;}
        const float value = lighting.timeOfDay;
        const float offset = value / 24.f;
        if(std::abs(daytime_slider->get_offset() - offset) > .0001f)
            daytime_slider->set_offset(offset);
        const auto fill = daytime_slider->get_element_as<gb::sprite>(daytime_slider->k_foreground_element_name);
        const glm::vec2 fill_size = fill->size;
        if(first_update || std::abs(fill_size.x - std::max(4.f, 176.f * offset)) > .0001f)
            slider_fill(daytime_slider, offset);
        if(fill->get_is_visible() != (offset > .001f)) fill->visible = offset > .001f;
        if(last_daytime != value) {
            set_text(daytime_value, decimal(value) + " h");
            last_daytime = value;
        }
    }

    void refresh() {
        if(inspector_only) { refresh_post_panel(); return; }
        if(!entity->get_is_visible()) {
            if(rotation_focused) rotation_field->get_component<gb::ui::ces_textedit_component>()->focus = false;
            rotation_slider_active = false;
            last_tool = -1;
            return;
        }
        const auto& config = terrain_tool_ui_config(state->current_tool);
        const int tool = static_cast<int>(state->current_tool);
        const bool is_building_tool = state->current_tool == TerrainEditorTool::Buildings;
        const bool is_stairs_tool = state->current_tool == TerrainEditorTool::Stairs;
        const bool tool_changed = last_tool != tool;
        if(!is_building_tool) {
            if(rotation_focused) rotation_field->get_component<gb::ui::ces_textedit_component>()->focus = false;
            rotation_slider_active = false;
        }
        if(tool_changed) {
            settings->visible = config.top_panel;
            trees->visible = config.trees;
            buildings->visible = is_building_tool;
            settings->size = glm::vec2(386, config.palette ? 398.f : 232.f);
            set_panel_title(settings, config.title);
            settings->get_element_as<gb::label>(gb::ui::control::k_label_element_name)->position = glm::vec2(62, 18);
            for(size_t i = 0; i < tool_buttons.size(); ++i)
                style_button(tool_buttons[i], true, tool_types[i] == state->current_tool);
            brush_slider->disable(!config.brush);
            strength_slider->disable(!config.strength);
            brush_slider->set_foreground_color(config.brush ? glm::u8vec4(0, 171, 217, 255) : disabled_color);
            brush_slider->set_slider_color(config.brush ? glm::u8vec4(250, 246, 227, 255) : muted);
            strength_slider->set_foreground_color(config.strength ? glm::u8vec4(0, 171, 217, 255) : disabled_color);
            strength_slider->set_slider_color(config.strength ? glm::u8vec4(250, 246, 227, 255) : muted);
            brush_label->font_color = config.brush ? text_color : muted;
            brush_value->font_color = config.brush ? cyan : muted;
            strength_label->font_color = config.strength ? text_color : muted;
            strength_value->font_color = config.strength ? cyan : muted;
            level_label->font_color = config.level ? text_color : muted;
            level_value->font_color = config.level ? cyan : muted;
            target_value->font_color = config.level ? cyan : muted;
            for(const auto& button : target_buttons) style_button(button, config.level, false);
            palette_label->font_color = config.palette ? text_color : muted;
            set_text(strength_label, state->current_tool == TerrainEditorTool::Grass ? "Density" : "Strength");
            palette_label->visible = config.palette;
            palette_value->visible = config.palette;
        }

        if(tool_changed || last_grass != int(state->selected_grass)) {
            for(size_t i = 0; i < grass_buttons.size(); ++i) {
                grass_buttons[i]->visible = config.top_panel && config.palette;
                style_button(grass_buttons[i], config.palette, int(i) == int(state->selected_grass));
            }
            set_text(palette_value, grass_names.at(size_t(state->selected_grass)));
        }
        if(tool_changed || last_tree != int(state->selected_tree))
            for(size_t i = 0; i < tree_buttons.size(); ++i) style_button(tree_buttons[i], config.trees, int(i) == int(state->selected_tree));
        if(tool_changed || last_building != int(state->selected_building))
            for(size_t i = 0; i < building_buttons.size(); ++i)
                style_button(building_buttons[i], is_building_tool, building_types[i] == state->selected_building);
        if(tool_changed || last_rotation_snap != state->building_rotation_snap)
            for(size_t i = 0; i < rotation_snap_buttons.size(); ++i)
                style_button(rotation_snap_buttons[i], is_building_tool, rotation_snaps[i] == state->building_rotation_snap);
        set_text(selected_building_label, "Selected: " + std::string(building_definition(state->selected_building).name));
        const float degrees = building_degrees(state->building_yaw);
        if(!rotation_slider_active && !rotation_slider_callback && std::abs(rotation_slider->get_offset() - degrees / 360.f) > .0001f)
            rotation_slider->set_offset(degrees / 360.f);
        const auto rotation_fill = rotation_slider->get_element_as<gb::sprite>(rotation_slider->k_foreground_element_name);
        const glm::vec2 rotation_fill_size = rotation_fill->size;
        if(is_building_tool && (first_update || std::abs(rotation_fill_size.x - std::max(4.f, 206.f * rotation_slider->get_offset())) > .0001f))
            slider_fill(rotation_slider, rotation_slider->get_offset());
        const bool show_rotation_fill = is_building_tool && rotation_slider->get_offset() > .001f;
        if(rotation_fill->get_is_visible() != show_rotation_fill) rotation_fill->visible = show_rotation_fill;
        if(!rotation_focused) {
            rotation_field->set_text(decimal((rotation_slider_active || rotation_slider_callback) && rotation_slider->get_offset() >= .9999f ? 360.f : degrees));
            rotation_field->set_text_color(text_color);
        }

        const bool undo = state->can_undo(), redo = state->can_redo();
        if(first_update || last_undo != undo) style_button(actions[0], undo, false);
        if(first_update || last_redo != redo) style_button(actions[1], redo, false);
        const float brush = glm::clamp(state->brush_size, 1.f, 32.f);
        const float strength = glm::clamp(state->strength, 0.f, 1.f);
        if(std::abs(brush_slider->get_offset() - (brush - 1.f) / 31.f) > .0001f) brush_slider->set_offset((brush - 1.f) / 31.f);
        if(std::abs(strength_slider->get_offset() - strength) > .0001f) strength_slider->set_offset(strength);
        const glm::vec2 brush_fill_size = brush_slider->get_element_as<gb::sprite>(brush_slider->k_foreground_element_name)->size;
        const glm::vec2 strength_fill_size = strength_slider->get_element_as<gb::sprite>(strength_slider->k_foreground_element_name)->size;
        if(first_update || std::abs(brush_fill_size.x - std::max(4.f, 176.f * (brush - 1.f) / 31.f)) > .0001f)
            slider_fill(brush_slider, (brush - 1.f) / 31.f);
        if(first_update || std::abs(strength_fill_size.x - std::max(4.f, 176.f * strength)) > .0001f)
            slider_fill(strength_slider, strength);
        set_text(brush_value, decimal(brush) + " m");
        set_text(strength_value, std::to_string(static_cast<int>(std::round(strength * 100.f))) + "%");

        const int current = state->current_terrace;
        set_text(target_value, terrace(state->target_terrace));
        set_text(level_value, "Current: " + (current == terrain_data::empty_layer ? std::string("--") : terrace(current)) +
                 (current <= -6 && current >= -8 ? " (Locked)" : "") + "   Range: -5 to +16");
        if(config.level && (tool_changed || last_target != state->target_terrace)) {
            style_button(target_buttons[0], state->target_terrace > -5, false);
            style_button(target_buttons[1], state->target_terrace < 16, false);
        }
        set_text(dirty_label, state->is_dirty ? "Modified" : "Saved");

        std::string line;
        if(is_building_tool) {
            line = "LMB Place  |  Mode: Buildings  |  Building: " + std::string(building_definition(state->selected_building).name);
            line += "  |  Rotation: " + decimal(degrees) + " deg";
            line += "  |  Snap: " + (state->building_rotation_snap == 0 ? std::string("Off") : std::to_string(state->building_rotation_snap) + " deg");
        } else if(is_stairs_tool) line = "LMB Place  |  Shift+LMB Remove  |  Mode: Stairs";
        else if(config.trees) line = "LMB Place  |  Mode: Trees  |  Tree: " + std::string(tree_names.at(static_cast<size_t>(state->selected_tree)));
        else {
            line = state->current_tool == TerrainEditorTool::Grass ? "LMB Paint" : "LMB Sculpt";
            const auto type = std::find(tool_types.begin(), tool_types.end(), state->current_tool);
            const char* name = type == tool_types.end() ? config.title : tool_names[size_t(type - tool_types.begin())];
            line += "  |  Brush: " + decimal(brush) + " m  |  Mode: " + name;
            line += "  |  Terrace: " + (current == terrain_data::empty_layer ? std::string("--") : terrace(current));
        }
        line += "  |  World: " + decimal(state->level.terrain.config.world_size) + " x " + decimal(state->level.terrain.config.world_size) + " m";
        set_text(status_label, line);
        const char* hint = is_building_tool ? "Q/E Rotate  |  R Quarter Turn  |  Shift+LMB Remove  |  MMB Orbit  |  Ctrl+Z Undo" :
            is_stairs_tool ? "Snaps between adjacent terraces  |  MMB Orbit  |  Ctrl+Z Undo  |  Ctrl+Y Redo" :
            "MMB Orbit  |  Ctrl+Z Undo  |  Ctrl+Y Redo";
        set_text(error_label, state->error.empty() ? hint : "Error: " + ascii_message(state->error));
        // Updating only on actual error-state changes avoids rebuilding the font
        // mesh every frame just to restore the same color.
        const auto color = state->error.empty() ? muted : glm::u8vec4(255, 159, 132, 255);
        const glm::u8vec4 current_error_color = error_label->font_color;
        if(current_error_color != color) error_label->font_color = color;
        refresh_daytime();
        refresh_post_panel();
        // Entity visibility propagates to every descendant. Restore only this
        // editor's suppressed native decorations after panels/cards are shown.
        for(const auto& decoration : hidden_decorations)
            if(decoration->get_is_visible()) decoration->visible = false;
        const auto brush_fill = brush_slider->get_element_as<gb::sprite>(brush_slider->k_foreground_element_name);
        const auto strength_fill = strength_slider->get_element_as<gb::sprite>(strength_slider->k_foreground_element_name);
        if(brush_fill->get_is_visible() != (config.top_panel && brush > 1.031f))
            brush_fill->visible = config.top_panel && brush > 1.031f;
        if(strength_fill->get_is_visible() != (config.top_panel && strength > .001f))
            strength_fill->visible = config.top_panel && strength > .001f;
        last_tool = tool;
        last_grass = int(state->selected_grass);
        last_tree = int(state->selected_tree);
        last_building = int(state->selected_building);
        last_rotation_snap = state->building_rotation_snap;
        last_target = state->target_terrace;
        last_undo = undo;
        last_redo = redo;
        first_update = false;
    }

    bool hit(glm::ivec2 point) const {
        if(!entity->get_is_visible()) return false;
        const std::array<gb::ui::panel_shared_ptr, 8> panels = {top, daytime, tools, settings, trees, buildings, status, post_panel};
        for(const auto& panel : panels) {
            if(!panel || !panel->get_is_visible()) continue;
            if(panel == post_panel && (!post_visible || !post_settings)) continue;
            const glm::vec2 origin = panel->position;
            const glm::vec2 panel_size = panel->size;
            const glm::vec2 panel_scale = panel->scale;
            const glm::vec2 size = panel_size * panel_scale;
            if(point.x >= origin.x && point.y >= origin.y && point.x < origin.x + size.x && point.y < origin.y + size.y) return true;
        }
        return false;
    }
};

TerrainEditorUI::TerrainEditorUI(const gb::scene_fabricator_shared_ptr& fabricator,
                               const std::shared_ptr<TerrainEditorState>& state, glm::ivec2 resolution, bool inspector_only)
    : m_impl(std::make_shared<implementation>(fabricator, state, resolution)) {
    m_impl->inspector_only=inspector_only; m_impl->build(); }
TerrainEditorUI::~TerrainEditorUI() = default;
gb::ces_entity_shared_ptr TerrainEditorUI::root() const { return m_impl->entity; }
void TerrainEditorUI::update() { m_impl->refresh(); }
void TerrainEditorUI::set_terrain_triangle_count(size_t triangles) {
    const auto now=std::chrono::steady_clock::now();
    if(m_impl->terrain_triangle_label&&now-m_impl->terrain_triangle_label_update>=std::chrono::milliseconds(250)){
        set_text(m_impl->terrain_triangle_label,"Tris: "+std::to_string(triangles));m_impl->terrain_triangle_label_update=now;
    }
}
bool TerrainEditorUI::contains(glm::ivec2 point) const { return m_impl->hit(point); }
bool TerrainEditorUI::keyboard_captured() const {
    return m_impl->rotation_focused && m_impl->entity->get_is_visible() && m_impl->buildings->get_is_visible();
}
void TerrainEditorUI::set_save_callback(std::function<void()> callback) { m_impl->save_callback = std::move(callback); }
void TerrainEditorUI::set_graphics_save_callback(std::function<bool()> callback) { m_impl->graphics_save_callback = std::move(callback); }
void TerrainEditorUI::set_load_callback(std::function<void()> callback) { m_impl->load_callback = std::move(callback); }
void TerrainEditorUI::bind_post_settings(std::shared_ptr<StylizedPostProcessSettings> settings) {
    m_impl->post_settings = std::move(settings);
    if(m_impl->post_settings) {
        m_impl->post_settings->sanitize();
        m_impl->build_post_panel();
    } else m_impl->post_visible = false;
    m_impl->refresh();
}
void TerrainEditorUI::bind_terrain_lod_settings(std::shared_ptr<TerrainLodSettings> settings){m_impl->terrain_lod=std::move(settings);m_impl->refresh_lod();}
void TerrainEditorUI::bind_render_debug(std::shared_ptr<RenderDebugState> state) {
    m_impl->render_debug=std::move(state);m_impl->selected_pass=0;m_impl->selected_attachment=0;
    m_impl->list_scroll=0;m_impl->detail_scroll=0;m_impl->refresh();
}
bool TerrainEditorUI::scroll_render_debug(glm::ivec2 point,float delta) { return m_impl->scroll_debug(point,delta); }
void TerrainEditorUI::toggle_post_panel() {
    if(!m_impl->post_settings) return;
    m_impl->post_visible = !m_impl->post_visible;
    if(m_impl->post_visible && m_impl->rotation_focused) m_impl->rotation_field->get_component<gb::ui::ces_textedit_component>()->focus = false;
    m_impl->refresh();
}
bool TerrainEditorUI::post_panel_visible() const {return m_impl->post_visible;}
}
