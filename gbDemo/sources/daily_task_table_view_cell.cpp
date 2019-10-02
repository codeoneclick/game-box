//
//  daily_task_table_view_cell.cpp
//  gbDemo
//
//  Created by serhii.s on 9/30/19.
//  Copyright © 2019 sergey.sergeev. All rights reserved.
//

#include "daily_task_table_view_cell.h"
#include "button.h"
#include "textfield.h"
#include "sprite.h"
#include "progress_bar.h"
#include "image_button.h"
#include "ces_bound_touch_2d_component.h"
#include "ces_transformation_2d_component.h"
#include "ces_material_component.h"
#include "material.h"
#include "scene_fabricator.h"
#include "gameplay_ui_fabricator.h"

namespace game
{
    namespace ui
    {
        daily_task_table_view_cell_data::daily_task_table_view_cell_data()
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
            
            description.getter([=]() {
                return m_description;
            });
            
            description.setter([=](const std::string& value) {
                m_description = value;
            });
          
            is_done.getter([=]() {
                return m_is_done;
            });
            
            is_done.setter([=](bool value) {
                m_is_done = value;
            });
            
            progress.getter([=]() {
                return m_progress;
            });
            
            progress.setter([=](f32 value) {
                m_progress = value;
            });
            
            is_claimed.getter([=]() {
                return m_is_claimed;
            });
            
            is_claimed.setter([=](bool value) {
                m_is_claimed = value;
            });
            
            cash_reward.getter([=]() {
                return m_cash_reward;
            });
            
            cash_reward.setter([=](i32 value) {
                m_cash_reward = value;
            });
        }
        
        const std::string daily_task_table_view_cell::k_claim_reward_button_id = "claim_reward_button";
        const std::string daily_task_table_view_cell::k_name_label_id = "name_label";
        const std::string daily_task_table_view_cell::k_description_label_id = "description_label";
        const std::string daily_task_table_view_cell::k_progress_bar_id = "progress_bar";
        const std::string daily_task_table_view_cell::k_reward_label_id = "reward_label";
       
        
        daily_task_table_view_cell::daily_task_table_view_cell(const gb::scene_fabricator_shared_ptr& fabricator, i32 index, const std::string& identifier) :
        gb::ui::table_view_cell(fabricator, index, identifier),
        m_claim_reward_button_callback(nullptr)
        {
            size.setter([=](const glm::vec2& size) {
                
                m_size = size;
                auto bound_touch_component = ces_entity::get_component<gb::ces_bound_touch_component>();
                bound_touch_component->as_2d()->set_bounds(glm::vec4(0.f, 0.f, m_size.x, m_size.y));
                m_elements[control::k_background_element_name]->size = size;
            });
        }
        
        void daily_task_table_view_cell::setup_components()
        {
            gb::ui::table_view_cell::setup_components();
        }
        
        void daily_task_table_view_cell::create()
        {
            gb::ui::table_view_cell::create();
            set_background_color(gameplay_ui_fabricator::k_control_background_color);
            
            const auto claim_button = gb::ces_entity::construct<gb::ui::button>(control::get_fabricator());
            claim_button->create();
            claim_button->size = glm::vec2(128.f, 48.f);
            claim_button->position = glm::vec2(300.f - 128.f - 8.f, 144.f - 48.f - 2.f);
            claim_button->set_text_color(gameplay_ui_fabricator::k_control_text_disabled_color, gb::ui::e_control_state::e_disabled);
            claim_button->set_background_color(gameplay_ui_fabricator::k_control_background_disabled_color, gb::ui::e_control_state::e_disabled);
            claim_button->set_foreground_color(gameplay_ui_fabricator::k_control_foreground_disabled_color, gb::ui::e_control_state::e_disabled);
            claim_button->set_text_color(gameplay_ui_fabricator::k_control_text_focused_color, gb::ui::e_control_state::e_focused);
            claim_button->set_background_color(gameplay_ui_fabricator::k_control_background_focused_color, gb::ui::e_control_state::e_focused);
            claim_button->set_foreground_color(gameplay_ui_fabricator::k_control_foreground_focused_color, gb::ui::e_control_state::e_focused);
            claim_button->set_text_color(gameplay_ui_fabricator::k_control_text_focused_color, gb::ui::e_control_state::e_selected);
            claim_button->set_background_color(gameplay_ui_fabricator::k_control_background_focused_color, gb::ui::e_control_state::e_selected);
            claim_button->set_foreground_color(gameplay_ui_fabricator::k_control_foreground_focused_color, gb::ui::e_control_state::e_selected);
            claim_button->set_text_color(gameplay_ui_fabricator::k_control_text_color);
            claim_button->set_background_color(gameplay_ui_fabricator::k_control_background_color);
            claim_button->set_foreground_color(gameplay_ui_fabricator::k_control_foreground_color);
            claim_button->set_text("CLAIM");
            claim_button->attach_sound("click.mp3", gb::ui::button::k_pressed_state);
            m_elements[k_claim_reward_button_id] = claim_button;
            add_child(claim_button);
            
            const auto name_label = gb::ces_entity::construct<gb::ui::textfield>(control::get_fabricator());
            name_label->create();
            name_label->size = glm::vec2(128.f, 48.f);
            name_label->set_text("Name");
            name_label->set_font_name("spincycle.otf");
            name_label->position = glm::vec2(8.f, 0.f);
            name_label->set_text_color(gameplay_ui_fabricator::k_control_text_color);
            name_label->set_text_horizontal_aligment(gb::ui::e_element_horizontal_aligment::e_element_horizontal_aligment_left);
            name_label->set_visible_edges(false);
            name_label->remove_component(gb::ces_bound_touch_component::class_guid());
            m_elements[k_name_label_id] = name_label;
            add_child(name_label);
            
            const auto reward_label = gb::ces_entity::construct<gb::ui::textfield>(control::get_fabricator());
            reward_label->create();
            reward_label->size = glm::vec2(128.f, 48.f);
            reward_label->set_text("+ 1000$");
            reward_label->set_font_name("spincycle.otf");
            reward_label->position = glm::vec2(300.f - 8.f - 128.f, 0.f);
            reward_label->set_text_color(gameplay_ui_fabricator::k_control_text_color);
            reward_label->set_text_horizontal_aligment(gb::ui::e_element_horizontal_aligment::e_element_horizontal_aligment_left);
            reward_label->set_visible_edges(false);
            reward_label->remove_component(gb::ces_bound_touch_component::class_guid());
            m_elements[k_reward_label_id] = reward_label;
            add_child(reward_label);
            
            const auto description_label = gb::ces_entity::construct<gb::ui::textfield>(control::get_fabricator());
            description_label->create();
            description_label->size = glm::vec2(284.f, 24.f);
            description_label->set_text("Description");
            description_label->set_multiline(true);
            description_label->set_font_name("spincycle.otf");
            description_label->position = glm::vec2(8.f, 48.f);
            description_label->set_text_color(gameplay_ui_fabricator::k_control_text_color);
            description_label->set_text_horizontal_aligment(gb::ui::e_element_horizontal_aligment::e_element_horizontal_aligment_left);
            description_label->set_visible_edges(false);
            description_label->remove_component(gb::ces_bound_touch_component::class_guid());
            m_elements[k_description_label_id] = description_label;
            add_child(description_label);
            
            const auto progress_bar = gb::ces_entity::construct<gb::ui::progress_bar>(control::get_fabricator());
            progress_bar->create();
            progress_bar->size = glm::vec2(144.f, 24.f);
            progress_bar->position = glm::vec2(8.f, 144.f - 24.f - 2.f);
            progress_bar->set_progress_line_color(gameplay_ui_fabricator::k_control_text_color);
            progress_bar->set_background_color(gameplay_ui_fabricator::k_control_background_color);
            progress_bar->set_progress(.01f);
            m_elements[k_progress_bar_id] = progress_bar;
            add_child(progress_bar);
            
            enumerate_children([=](const gb::ces_entity_shared_ptr &child) {
                const auto material_component = child->get_component<gb::ces_material_component>();
                if (material_component)
                {
                    const auto material = material_component->get_material("ws.ui");
                    if (material)
                    {
                        material->set_stencil_test(true);
                        material->set_stencil_function(gb::gl::constant::equal);
                        material->set_stencil_ref_value(128);
                        material->set_stencil_mask_read(255);
                        material->set_front_stencil_op_1(gb::gl::constant::keep);
                        material->set_front_stencil_op_2(gb::gl::constant::keep);
                        material->set_front_stencil_op_3(gb::gl::constant::keep);
                    }
                }
                
                child->enumerate_children([=](const gb::ces_entity_shared_ptr &child) {
                    const auto material_component = child->get_component<gb::ces_material_component>();
                    const auto material = material_component->get_material("ws.ui");
                    if (material)
                    {
                        material->set_stencil_test(true);
                        material->set_stencil_function(gb::gl::constant::equal);
                        material->set_stencil_ref_value(128);
                        material->set_stencil_mask_read(255);
                        material->set_front_stencil_op_1(gb::gl::constant::keep);
                        material->set_front_stencil_op_2(gb::gl::constant::keep);
                        material->set_front_stencil_op_3(gb::gl::constant::keep);
                    }
                });
            });
        }
        
        void daily_task_table_view_cell::set_claim_reward_button_callback_t(const claim_reward_button_callback_t &callback)
        {
            m_claim_reward_button_callback = callback;
            if(!std::static_pointer_cast<gb::ui::image_button>(m_elements[k_claim_reward_button_id])->is_pressed_callback_exist())
            {
                std::static_pointer_cast<gb::ui::button>(m_elements[k_claim_reward_button_id])->set_on_pressed_callback([this](const gb::ces_entity_shared_ptr&) {
                    if(m_claim_reward_button_callback)
                    {
                        m_claim_reward_button_callback(m_index);
                    }
                });
            }
        }
    
        void daily_task_table_view_cell::set_name(const std::string& name)
        {
            std::static_pointer_cast<gb::ui::textfield>(m_elements[k_name_label_id])->set_text(name);
        }
    
        void daily_task_table_view_cell::set_description(const std::string& description)
        {
            std::static_pointer_cast<gb::ui::textfield>(m_elements[k_description_label_id])->set_text(description);
        }
    
        void daily_task_table_view_cell::set_progress(f32 value)
        {
            std::static_pointer_cast<gb::ui::progress_bar>(m_elements[k_progress_bar_id])->set_progress(value);
        }
    
        void daily_task_table_view_cell::set_is_done(bool value)
        {
            if (!value)
            {
                std::static_pointer_cast<gb::ui::button>(m_elements[k_claim_reward_button_id])->focus(false);
                std::static_pointer_cast<gb::ui::button>(m_elements[k_claim_reward_button_id])->disable(true);
            }
            else
            {
                std::static_pointer_cast<gb::ui::button>(m_elements[k_claim_reward_button_id])->disable(false);
                std::static_pointer_cast<gb::ui::button>(m_elements[k_claim_reward_button_id])->focus(true, .33f);
            }
        }
        
        void daily_task_table_view_cell::set_is_claimed(bool value)
        {
            if (value)
            {
                std::static_pointer_cast<gb::ui::button>(m_elements[k_claim_reward_button_id])->visible = false;
                std::static_pointer_cast<gb::ui::progress_bar>(m_elements[k_progress_bar_id])->visible = false;
                std::static_pointer_cast<gb::ui::textfield>(m_elements[k_reward_label_id])->set_text("DONE !");
            }
            else
            {
                std::static_pointer_cast<gb::ui::button>(m_elements[k_claim_reward_button_id])->visible = true;
                std::static_pointer_cast<gb::ui::progress_bar>(m_elements[k_progress_bar_id])->visible = true;
            }
           
        }
    
        void daily_task_table_view_cell::set_cash_reward(i32 value)
        {
            std::stringstream reward_value_string_stream;
            reward_value_string_stream<<"+ "<<value<<"$";
            std::static_pointer_cast<gb::ui::textfield>(m_elements[k_reward_label_id])->set_text(reward_value_string_stream.str());
        }
    }
}
