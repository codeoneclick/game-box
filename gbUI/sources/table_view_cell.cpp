//
//  table_view_cell.cpp
//  gbUI
//
//  Created by sergey.sergeev on 1/22/16.
//  Copyright © 2016 sergey.sergeev. All rights reserved.
//

#include "table_view_cell.h"
#include "scene_fabricator.h"
#include "sprite.h"
#include "ces_transformation_2d_component.h"
#include "ces_bound_touch_2d_component.h"
#include "ces_action_component.h"

#define k_cell_loading_time 300.f

namespace gb
{
    namespace ui
    {
        table_view_cell::table_view_cell(const scene_fabricator_shared_ptr& fabricator, i32 index, const std::string& identifier) :
        gb::ui::control(fabricator),
        m_index(index),
        m_identifier(identifier),
        m_max_loading_time(k_cell_loading_time),
        m_loading_time(k_cell_loading_time)
        {
            ces_entity::add_deferred_component_constructor<ces_bound_touch_2d_component>();
            ces_entity::add_deferred_component_constructor<gb::ces_action_component>();
            
            size.setter([=](const glm::vec2& size) {
                
                m_size = size;
                auto bound_touch_component = ces_entity::get_component<ces_bound_touch_component>();
                if (bound_touch_component)
                {
                    bound_touch_component->as_2d()->set_bounds(glm::vec4(0.f, 0.f, m_size.x, m_size.y));
                }
                m_elements[control::k_background_element_name]->size = size;
            });
        }
        
        table_view_cell::~table_view_cell()
        {
            
        }
        
        void table_view_cell::setup_components()
        {
            control::setup_components();
            auto action_component = ces_entity::get_component<gb::ces_action_component>();
            action_component->set_update_callback(std::bind(&table_view_cell::update, this,
                                                            std::placeholders::_1, std::placeholders::_2));
        }
        
        void table_view_cell::create()
        {
            gb::sprite_shared_ptr table_view_cell_background =
            control::get_fabricator()->create_sprite("table_view_cell_background.xml");
            m_elements[control::k_background_element_name] = table_view_cell_background;
            table_view_cell::add_child(table_view_cell_background);
            control::set_color(control::k_background_element_name, control::k_gray_color);
            control::create();
        }
        
        void table_view_cell::on_touched(const ces_entity_shared_ptr&, const glm::vec2& point,
                                         e_input_source input_source, e_input_state input_state)
        {
            
        }
        
        i32 table_view_cell::get_index() const
        {
            return m_index;
        }
        
        void table_view_cell::set_index(i32 index)
        {
            m_index = index;
        }
        
        std::string table_view_cell::get_identifier() const
        {
            return m_identifier;
        }
        
        void table_view_cell::update(const gb::ces_entity_shared_ptr& entity, f32 dt)
        {
            if(m_loading_time > 0.f)
            {
                //f32 delta_based_on_time = m_loading_time / m_max_loading_time;
                //f32 current_alpha = glm::mix(static_cast<i32>(m_alpha), 0, delta_based_on_time);
                //table_view_cell::set_alpha(current_alpha);
            }
            else
            {
                //table_view_cell::set_alpha(m_alpha);
            }
            m_loading_time -= dt * 1000.f;
        }
        
        void table_view_cell::set_loading(bool value)
        {
            m_loading_time = value ? m_max_loading_time : 0.f;
        }
        
        void table_view_cell::set_background_color(const glm::u8vec4& color)
        {
            control::set_background_color(color);
            m_alpha = color.w;
        }
    };
};
