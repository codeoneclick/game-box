#include "terrain_editor_ui.h"
#include "terrain_editor_state.h"
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
const std::array<const char*, 4> tool_names = {"Terrain", "Grass", "Trees", "Buildings"};
// Tool value 3 belongs to the retired Climate tool in legacy level files.
const std::array<TerrainEditorTool, 4> tool_types = {
    TerrainEditorTool::Terrain, TerrainEditorTool::Grass, TerrainEditorTool::Trees, TerrainEditorTool::Buildings};
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
    gb::ui::slider_shared_ptr daytime_slider;
    gb::label_shared_ptr daytime_value;
    float last_daytime = std::numeric_limits<float>::quiet_NaN();
    std::shared_ptr<StylizedPostProcessSettings> post_settings;
    gb::ui::panel_shared_ptr post_panel;
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
    std::array<gb::ui::button_shared_ptr, 4> actions;
    std::array<gb::ui::button_shared_ptr, 4> tool_buttons;
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
        fill->size = glm::vec2(std::max(4.f, (size.x - 4.f) * offset), size.y - 4.f);
        fill->position = glm::vec2(2.f);
        fill->visible = offset > .001f;
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

    void build_post_panel() {
        if(post_panel || !post_settings) return;
        const float scale = std::min(1.f, std::min(
            std::max(1.f, static_cast<float>(resolution.x) - 40.f) / 740.f,
            std::max(1.f, static_cast<float>(resolution.y) - 40.f) / 620.f));
        const glm::vec2 size(740, 620);
        post_panel = make_panel((glm::vec2(resolution) - size * scale) * .5f, size, "Lighting & Post Processing");
        post_panel->scale = glm::vec2(scale);
        post_panel->set_background_color(glm::u8vec4(5, 36, 53, 238));
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
                });
        post_view_button = make_button(post_panel, glm::vec2(16, 202), glm::vec2(348, 36), "View: Final",
            [](implementation& view) {
                if(!view.post_settings || !view.post_visible) return;
                view.post_settings->sanitize();
                const auto current = std::find(post_view_values.begin(), post_view_values.end(), view.post_settings->debugView);
                const size_t next = current == post_view_values.end() ? 0 :
                    (size_t(current - post_view_values.begin()) + 1) % post_view_values.size();
                view.post_settings->debugView = post_view_values[next];
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
                if(!view || !view->post_settings || !view->post_visible) return;
                view->post_settings.get()->*post_slider_fields[i] = post_slider_min[i] +
                    glm::clamp(offset, 0.f, 1.f) * (post_slider_max[i] - post_slider_min[i]);
                view->post_settings->sanitize();
                view->refresh_post_panel();
            });
        }
        lighting_mode_button = make_button(post_panel, glm::vec2(378, 54), glm::vec2(346, 32), "Lighting: Soft",
            [](implementation& view) {
                if(view.post_visible) {
                    auto& lighting = stylized_lighting_settings();
                    lighting.mode = lighting.mode == 0 ? 1 : 0;
                }
            });
        make_button(post_panel, glm::vec2(378, 96), glm::vec2(168, 32), "Preset: Soft", [](implementation& view) {
            if(view.post_visible) stylized_lighting_settings() = StylizedLightingSettings::soft();
        });
        make_button(post_panel, glm::vec2(556, 96), glm::vec2(168, 32), "Preset: Sunset", [](implementation& view) {
            if(view.post_visible) stylized_lighting_settings() = StylizedLightingSettings::sunset();
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
                if(!view || !view->post_visible) return;
                auto& lighting = stylized_lighting_settings();
                lighting_slider_value(lighting, i) = lighting_slider_min[i] +
                    glm::clamp(offset, 0.f, 1.f) * (lighting_slider_max[i] - lighting_slider_min[i]);
                lighting.sanitize();
                view->refresh_post_panel();
            });
        }
        specular_button = make_button(post_panel, glm::vec2(16, 492), glm::vec2(348, 32), "Stylized specular: On",
            [](implementation& view) {
                if(view.post_visible) {
                    auto& lighting = stylized_lighting_settings();
                    lighting.enableStylizedSpecular = !lighting.enableStylizedSpecular;
                }
            });
        backlight_button = make_button(post_panel, glm::vec2(16, 534), glm::vec2(348, 32), "Vegetation backlight: On",
            [](implementation& view) {
                if(view.post_visible) {
                    auto& lighting = stylized_lighting_settings();
                    lighting.enableVegetationBacklight = !lighting.enableVegetationBacklight;
                }
            });
        make_label(post_panel, glm::vec2(16, 592), "F8: Show / Hide    |    RGB colors use linear values", 12.f)->font_color = muted;
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
    }

    void build() {
        top = make_panel(glm::vec2(18, 18), glm::vec2(380, 76), "");
        // Keep the top row clear of the statistics HUD on narrower windows.
        const float toolbar_scale = std::min(1.f, std::max(1.f,
            static_cast<float>(resolution.x) - 318.f) / 740.f);
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
            glm::vec2(250, 76), "");
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
        dirty_label = make_label(top, glm::vec2(666, 55), "Saved", 13.f);

        tools = make_panel(glm::vec2(18, 114), glm::vec2(108, 420), "");
        for(size_t i = 0; i < tool_buttons.size(); ++i) {
            tool_buttons[i] = make_button(tools, glm::vec2(10, 10.f + i * 102.f), glm::vec2(88, 94), tool_names[i],
                [i](implementation& view) { view.state->select_tool(tool_types[i]); });
            if(tool_types[i] == TerrainEditorTool::Buildings) {
                tool_buttons[i]->set_font_size(13.f);
                tool_buttons[i]->set_text(tool_names[i]);
                building_icon_button(tool_buttons[i], 0, 64.f);
            } else icon_button(tool_buttons[i], 4 + int(i), 64.f);
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
        lighting.sanitize();refresh_daytime();
    }

    void refresh_daytime() {
        auto lighting = stylized_lighting_settings();
        lighting.sanitize();
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
        if(!entity->get_is_visible()) {
            if(rotation_focused) rotation_field->get_component<gb::ui::ces_textedit_component>()->focus = false;
            rotation_slider_active = false;
            last_tool = -1;
            return;
        }
        const auto& config = terrain_tool_ui_config(state->current_tool);
        const int tool = static_cast<int>(state->current_tool);
        const bool is_building_tool = state->current_tool == TerrainEditorTool::Buildings;
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
        } else if(config.trees) line = "LMB Place  |  Mode: Trees  |  Tree: " + std::string(tree_names.at(static_cast<size_t>(state->selected_tree)));
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
                               const std::shared_ptr<TerrainEditorState>& state, glm::ivec2 resolution)
    : m_impl(std::make_shared<implementation>(fabricator, state, resolution)) { m_impl->build(); }
TerrainEditorUI::~TerrainEditorUI() = default;
gb::ces_entity_shared_ptr TerrainEditorUI::root() const { return m_impl->entity; }
void TerrainEditorUI::update() { m_impl->refresh(); }
bool TerrainEditorUI::contains(glm::ivec2 point) const { return m_impl->hit(point); }
bool TerrainEditorUI::keyboard_captured() const {
    return m_impl->rotation_focused && m_impl->entity->get_is_visible() && m_impl->buildings->get_is_visible();
}
void TerrainEditorUI::set_save_callback(std::function<void()> callback) { m_impl->save_callback = std::move(callback); }
void TerrainEditorUI::set_load_callback(std::function<void()> callback) { m_impl->load_callback = std::move(callback); }
void TerrainEditorUI::bind_post_settings(std::shared_ptr<StylizedPostProcessSettings> settings) {
    m_impl->post_settings = std::move(settings);
    if(m_impl->post_settings) {
        m_impl->post_settings->sanitize();
        m_impl->build_post_panel();
    } else m_impl->post_visible = false;
    m_impl->refresh();
}
void TerrainEditorUI::toggle_post_panel() {
    if(!m_impl->post_settings) return;
    m_impl->post_visible = !m_impl->post_visible;
    if(m_impl->post_visible && m_impl->rotation_focused) m_impl->rotation_field->get_component<gb::ui::ces_textedit_component>()->focus = false;
    m_impl->refresh();
}
}
