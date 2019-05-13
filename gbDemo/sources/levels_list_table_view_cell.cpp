//
//  levels_list_table_view_cell.cpp
//  gbDemo
//
//  Created by serhii.s on 5/8/19.
//  Copyright © 2019 sergey.sergeev. All rights reserved.
//

#include "levels_list_table_view_cell.h"
#include "button.h"
#include "textfield.h"
#include "sprite.h"
#include "image_button.h"
#include "ces_bound_touch_2d_component.h"
#include "ces_transformation_2d_component.h"
#include "ces_material_component.h"
#include "material.h"
#include "scene_fabricator.h"

namespace game
{
    namespace ui
    {
        levels_list_table_view_cell_data::levels_list_table_view_cell_data()
        {
            id.getter([=]() {
                return m_id;
            });
            
            id.setter([=](i32 value) {
                m_id = value;
            });
            
            name.getter([=]() {
                return m_name;
            });
            
            name.setter([=](const std::string& value) {
                m_name = value;
            });
            
            stars_received.getter([=]() {
                return m_stars_received;
            });
            
            stars_received.setter([=](i32 value) {
                m_stars_received = value;
            });
            
            scores_required_to_win.getter([=]() {
                return m_scores_required_to_win;
            });
            
            scores_required_to_win.setter([=](i32 value) {
                m_scores_required_to_win = value;
            });
            
            is_locked.getter([=]() {
                return m_is_locked;
            });
            
            is_locked.setter([=](bool value) {
                m_is_locked = is_locked;
            });
        }
        
        const std::string levels_list_table_view_cell::k_start_level_button_id = "start_level_button";
        const std::string levels_list_table_view_cell::k_show_replay_level_button_id = "show_replay_level_button";
        const std::string levels_list_table_view_cell::k_level_index_label_id = "level_index_label";
        const std::string levels_list_table_view_cell::k_locked_unlocked_image_id = "locked_unlocked_image"; 
        const std::string levels_list_table_view_cell::k_star1_image_id = "star1_image";
        const std::string levels_list_table_view_cell::k_star2_image_id = "star2_image";
        const std::string levels_list_table_view_cell::k_star3_image_id = "star3_image";
        const std::string levels_list_table_view_cell::k_score_label_id = "score_label";
        const std::string levels_list_table_view_cell::k_score_value_label_id = "score_value_label";
        
        levels_list_table_view_cell::levels_list_table_view_cell(const gb::scene_fabricator_shared_ptr& fabricator, i32 index, const std::string& identifier) :
        gb::ui::table_view_cell(fabricator, index, identifier),
        m_start_level_button_callback(nullptr)
        {
            size.setter([=](const glm::vec2& size) {
                
                m_size = size;
                auto bound_touch_component = ces_entity::get_component<gb::ces_bound_touch_component>();
                bound_touch_component->as_2d()->set_bounds(glm::vec4(0.f, 0.f, m_size.x, m_size.y));
                m_elements[control::k_background_element_name]->size = size;
            });
        }
        
        void levels_list_table_view_cell::setup_components()
        {
            gb::ui::table_view_cell::setup_components();
        }
        
        void levels_list_table_view_cell::create()
        {
            gb::ui::table_view_cell::create();
            set_background_color(glm::u8vec4(255, 255, 255, 255));
            
            const auto start_level_button = gb::ces_entity::construct<gb::ui::image_button>(control::get_fabricator());
            start_level_button->create("ui_next.png");
            start_level_button->size = glm::vec2(24.f, 24.f);
            start_level_button->position = glm::vec2(160.f, 60.f);
            start_level_button->set_image_color(glm::u8vec4(32, 32, 32, 255));
            start_level_button->set_background_color(glm::u8vec4(192, 192, 192, 255));
            m_elements[k_start_level_button_id] = start_level_button;
            add_child(start_level_button);
            
            const auto show_replay_level_button = gb::ces_entity::construct<gb::ui::image_button>(control::get_fabricator());
            show_replay_level_button->create("ui_video.png");
            show_replay_level_button->size = glm::vec2(24.f, 24.f);
            show_replay_level_button->position = glm::vec2(128.f, 60.f);
            show_replay_level_button->set_image_color(glm::u8vec4(32, 32, 32, 255));
            show_replay_level_button->set_background_color(glm::u8vec4(192, 192, 192, 255));
            m_elements[k_show_replay_level_button_id] = show_replay_level_button;
            add_child(show_replay_level_button);
            
            const auto locked_unlocked_image = control::get_fabricator()->create_sprite("ui_image.xml", "ui_unlocked.png");
            locked_unlocked_image->get_component<gb::ces_transformation_component>()->set_is_in_camera_space(false);
            locked_unlocked_image->size = glm::vec2(48.f, 48.f);
            locked_unlocked_image->color = glm::u8vec4(0, 0, 64, 255);
            locked_unlocked_image->position = glm::vec2(8.f, 8.f);
            m_elements[k_locked_unlocked_image_id] = locked_unlocked_image;
            add_child(locked_unlocked_image);
            
            const auto star1_image = control::get_fabricator()->create_sprite("ui_image.xml", "ui_star.png");
            star1_image->get_component<gb::ces_transformation_component>()->set_is_in_camera_space(false);
            star1_image->size = glm::vec2(24.f, 24.f);
            star1_image->color = glm::u8vec4(192, 0, 192, 255);
            star1_image->position = glm::vec2(64.f, 8.f);
            m_elements[k_star1_image_id] = star1_image;
            add_child(star1_image);
            
            const auto star2_image = control::get_fabricator()->create_sprite("ui_image.xml", "ui_star.png");
            star2_image->get_component<gb::ces_transformation_component>()->set_is_in_camera_space(false);
            star2_image->size = glm::vec2(24.f, 24.f);
            star2_image->color = glm::u8vec4(192, 0, 192, 255);
            star2_image->position = glm::vec2(96.f, 8.f);
            m_elements[k_star2_image_id] = star2_image;
            add_child(star2_image);
            
            const auto star3_image = control::get_fabricator()->create_sprite("ui_image.xml", "ui_star.png");
            star3_image->get_component<gb::ces_transformation_component>()->set_is_in_camera_space(false);
            star3_image->size = glm::vec2(24.f, 24.f);
            star3_image->color = glm::u8vec4(32, 32, 32, 255);
            star3_image->position = glm::vec2(128.f, 8.f);
            m_elements[k_star3_image_id] = star3_image;
            add_child(star3_image);
            
            const auto level_index_label = gb::ces_entity::construct<gb::ui::textfield>(control::get_fabricator());
            level_index_label->create();
            level_index_label->size = glm::vec2(24.f, 24.f);
            level_index_label->set_text("3");
            level_index_label->position = glm::vec2(-2.f, -2.f);
            level_index_label->set_font_color(glm::u8vec4(32, 32, 32, 255));
            level_index_label->set_visible_edges(false);
            m_elements[k_level_index_label_id] = level_index_label;
            add_child(level_index_label);
            
            const auto score_label = gb::ces_entity::construct<gb::ui::textfield>(control::get_fabricator());
            score_label->create();
            score_label->size = glm::vec2(24.f, 24.f);
            score_label->set_text("Score:");
            score_label->position = glm::vec2(64.f + score_label->get_content_size().x * .5f, 32.f);
            score_label->set_font_color(glm::u8vec4(32, 32, 32, 255));
            score_label->set_visible_edges(false);
            m_elements[k_score_label_id] = score_label;
            add_child(score_label);
            
            const auto score_value_label = gb::ces_entity::construct<gb::ui::textfield>(control::get_fabricator());
            score_value_label->create();
            score_value_label->size = glm::vec2(24.f, 24.f);
            score_value_label->set_text("286");
            score_value_label->position = glm::vec2(64.f + score_label->get_content_size().x + 8.f + score_value_label->get_content_size().x * .5f, 32.f);
            score_value_label->set_font_color(glm::u8vec4(0, 64, 0, 255));
            score_value_label->set_visible_edges(false);
            m_elements[k_score_value_label_id] = score_value_label;
            add_child(score_value_label);
            
            enumerate_children([=](const gb::ces_entity_shared_ptr &child) {
                const auto material_component = child->get_component<gb::ces_material_component>();
                if (material_component)
                {
                    const auto material = material_component->get_material("ws.ui", 0);
                    if (material)
                    {
                        material->set_stencil_test(true);
                        material->set_stencil_function(gb::gl::constant::equal);
                        material->set_stencil_mask_parameter(1);
                    }
                }
                
                child->enumerate_children([=](const gb::ces_entity_shared_ptr &child) {
                    const auto material_component = child->get_component<gb::ces_material_component>();
                    const auto material = material_component->get_material("ws.ui", 0);
                    if (material)
                    {
                        material->set_stencil_test(true);
                        material->set_stencil_function(gb::gl::constant::equal);
                        material->set_stencil_mask_parameter(1);
                    }
                });
            });
        }
        
        void levels_list_table_view_cell::set_start_level_button_callback_t(const start_level_button_callback_t &callback)
        {
            m_start_level_button_callback = callback;
            if(!std::static_pointer_cast<gb::ui::button>(m_elements[k_start_level_button_id])->is_pressed_callback_exist())
            {
                std::static_pointer_cast<gb::ui::button>(m_elements[k_start_level_button_id])->set_on_pressed_callback([this](const gb::ces_entity_shared_ptr&) {
                    if(m_start_level_button_callback)
                    {
                        m_start_level_button_callback(m_index);
                    }
                });
            }
        }
    }
}
