//
//  content_list.cpp
//  gbUI
//
//  Created by sergey.sergeev on 1/15/16.
//  Copyright © 2016 sergey.sergeev. All rights reserved.
//

#include "content_list.h"
#include "scene_fabricator.h"
#include "sprite.h"
#include "ces_bound_touch_component.h"
#include "content_list_cell.h"
#include "ces_transformation_component.h"
#include "ces_action_component.h"
#include "ces_material_component.h"

namespace gb
{
    namespace ui
    {
        static const std::string k_color_state_uniform = "u_color";
        
        content_list::content_list(const scene_fabricator_shared_ptr& fabricator) :
        gb::ui::control(fabricator),
        m_on_create_cell_callback(nullptr),
        m_separator_offset(glm::vec2(10.f)),
        m_scroll_inertion(0.f),
        m_drag_events_sum(0.f),
        m_drag_events_count(0)
        {
            ces_bound_touch_component_shared_ptr bound_touch_compoent = std::make_shared<ces_bound_touch_component>();
            bound_touch_compoent->enable(e_input_state_dragged, true);
            bound_touch_compoent->enable(e_input_state_pressed, true);
            bound_touch_compoent->enable(e_input_state_released, true);
            bound_touch_compoent->set_callback(e_input_state_dragged, std::bind(&content_list::on_touched, this, std::placeholders::_1,
                                                                                std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));
            bound_touch_compoent->set_callback(e_input_state_pressed, std::bind(&content_list::on_touched, this, std::placeholders::_1,
                                                                                std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));
            bound_touch_compoent->set_callback(e_input_state_released, std::bind(&content_list::on_touched, this, std::placeholders::_1,
                                                                                 std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));
            ces_entity::add_component(bound_touch_compoent);
        }
        
        content_list::~content_list()
        {
            
        }
        
        void content_list::create()
        {
            gb::sprite_shared_ptr content_list_background =
            control::get_fabricator()->create_sprite("content_list_background.xml");
            m_elements["content_list_background"] = content_list_background;
            ces_entity::add_child(content_list_background);
            
            ces_material_component* material_component = unsafe_get_material_component(content_list_background);
            material_component->set_custom_shader_uniform(control::k_dark_gray_color, k_color_state_uniform);
            
            control::create();
        }
        
        void content_list::on_touched(const ces_entity_shared_ptr&, const glm::vec2& point, e_input_element input_element, e_input_state input_state)
        {
            if(input_state == e_input_state_dragged && m_cells.size() > 0)
            {
                glm::vec2 delta = point - m_previous_dragged_point;
                content_list::scroll_content(delta.y);
                
                if((m_drag_events_sum > 0.f && delta.y < 0.f) ||
                   (m_drag_events_sum < 0.f && delta.y > 0.f))
                {
                    m_drag_events_sum = 0.f;
                    m_drag_events_count = 0.f;
                }
                
                m_drag_events_sum += delta.y;
                m_drag_events_count++;
            }
            else if(input_state == e_input_state_pressed)
            {
                m_drag_events_sum = 0.f;
                m_drag_events_count = 0;
                content_list::remove_component(gb::e_ces_component_type_action);
            }
            else if(input_state == e_input_state_released)
            {
                gb::ces_action_component_shared_ptr action_component = std::make_shared<gb::ces_action_component>();
                action_component->set_update_callback(std::bind(&content_list::on_autoscroll, this, std::placeholders::_1, std::placeholders::_2));
                content_list::add_component(action_component);
            
                m_scroll_inertion = m_drag_events_sum / m_drag_events_count;
            }

            m_previous_dragged_point = point;
        }
        
        void content_list::set_separator_offset(const glm::vec2& separator_offset)
        {
            m_separator_offset = separator_offset;
        }
        
        void content_list::set_size(const glm::vec2& size)
        {
            control::set_size(size);
            
            unsafe_get_bound_touch_component_from_this->set_frame(glm::vec4(0.f, 0.f, m_size.x, m_size.y));
            
            std::static_pointer_cast<gb::sprite>(m_elements["content_list_background"])->set_size(glm::vec2(size.x + m_separator_offset.x * 2.f, size.y));
        }
        
        void content_list::set_on_create_cell_callback(const t_on_create_cell_callback& callback)
        {
            m_on_create_cell_callback = callback;
        }
        
        void content_list::set_data_source(const std::vector<content_list_data_shared_ptr>& data_source)
        {
            assert(m_on_create_cell_callback);
            
            f32 offset_y = m_separator_offset.y;
            for(i32 i = 0; i < data_source.size(); ++i)
            {
                content_list_cell_shared_ptr cell = m_on_create_cell_callback(i, data_source[i]);
                cell->set_position(glm::vec2(m_separator_offset.x, offset_y));
                ces_entity::add_child(cell);
                offset_y += cell->get_size().y + m_separator_offset.y;
                
                m_cells.push_back(cell);
            }
        }
        
        void content_list::scroll_content(f32 delta)
        {
            if(m_cells[0]->get_position().y + delta < m_separator_offset.y &&
               m_cells[m_cells.size() - 1]->get_position().y + delta > m_size.y - m_separator_offset.y - m_cells[0]->get_size().y)
            {
                for(const auto& cell : m_cells)
                {
                    gb::ces_transformation_component_shared_ptr transformation_component =
                    std::static_pointer_cast<gb::ces_transformation_component>(cell->get_component(gb::e_ces_component_type_transformation));
                    glm::vec2 new_position = transformation_component->get_position();
                    new_position.y += delta;
                    transformation_component->set_position(new_position);
                }
            }
        }
        
        void content_list::on_autoscroll(const gb::ces_entity_shared_ptr& entity, f32 deltatime)
        {
            content_list::scroll_content(m_scroll_inertion);
            m_scroll_inertion *= .9f;
        }
    }
}