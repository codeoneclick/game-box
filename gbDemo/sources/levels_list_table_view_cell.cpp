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
#include "ces_bound_touch_2d_component.h"
#include "ces_material_component.h"
#include "material.h"

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
        
        levels_list_table_view_cell::levels_list_table_view_cell(const gb::scene_fabricator_shared_ptr& fabricator, i32 index, const std::string& identifier) :
        gb::ui::table_view_cell(fabricator, index, identifier),
        m_start_level_button_callback(nullptr)
        {
            size.setter([=](const glm::vec2& size) {
                
                m_size = size;
                auto bound_touch_component = ces_entity::get_component<gb::ces_bound_touch_component>();
                bound_touch_component->as_2d()->set_bounds(glm::vec4(0.f, 0.f, m_size.x, m_size.y));
                m_elements[control::k_background_element_name]->size = size;
                
                m_elements[k_start_level_button_id]->position = glm::vec2(m_size.x - 128.f - 8.f, 8.f);
            });
        }
        
        void levels_list_table_view_cell::setup_components()
        {
            gb::ui::table_view_cell::setup_components();
        }
        
        void levels_list_table_view_cell::create()
        {
            gb::ui::table_view_cell::create();
            
            glm::vec2 cell_size = levels_list_table_view_cell::size;
            
            auto start_level_button = gb::ces_entity::construct<gb::ui::button>(control::get_fabricator());
            start_level_button->create();
            start_level_button->enumerate_children([=](const gb::ces_entity_shared_ptr &child) {
                const auto material_component = child->get_component<gb::ces_material_component>();
                const auto material = material_component->get_material("ws.ui", 0);
                if (material)
                {
                    material->set_stencil_test(true);
                    material->set_stencil_function(gb::gl::constant::equal);
                    material->set_stencil_mask_parameter(1);
                }
            });
            start_level_button->size = glm::vec2(128.f, 24.f);
            start_level_button->position = glm::vec2(cell_size.x - 128.f - 8.f, 8.f);
            start_level_button->set_text("Start");
            m_elements[k_start_level_button_id] = start_level_button;
            add_child(start_level_button);
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
