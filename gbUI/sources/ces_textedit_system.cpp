//
//  ces_textedit_system.cpp
//  gbUI
//
//  Created by serhii serhiiv on 4/12/17.
//  Copyright © 2017 sergey.sergeev. All rights reserved.
//

#if !defined(__NO_RENDER__)

#include "ces_textedit_system.h"
#include "ces_bound_touch_component.h"
#include "ces_transformation_2d_component.h"
#include "ces_textedit_component.h"
#include "glm_extensions.h"
#include "camera_2d.h"

namespace gb
{
    namespace ui
    {
        ces_textedit_system::ces_textedit_system() :
        m_show_virtual_keyboard_callback(nullptr),
        m_hide_virtual_keyboard_callback(nullptr)
        {
            
        }
        
        ces_textedit_system::~ces_textedit_system()
        {
            
        }
        
        void ces_textedit_system::on_feed_start(f32 deltatime)
        {
            
        }
        
        void ces_textedit_system::on_feed(const ces_entity_shared_ptr& entity, f32 deltatime)
        {
            while (!m_events.empty())
            {
                const auto& event = m_events.front();
                if(std::get<3>(event) <= 1)
                {
                    ces_entity_shared_ptr intersected_entity = ces_textedit_system::intersected_entity(entity, event);
                    if(intersected_entity && intersected_entity != m_focused_entity.lock() && m_show_virtual_keyboard_callback)
                    {
                        m_show_virtual_keyboard_callback();
                        auto textedit_component = intersected_entity->get_component<ces_textedit_component>();
                        textedit_component->focus = true;
                    }
                    else if(!intersected_entity && m_hide_virtual_keyboard_callback)
                    {
                        m_hide_virtual_keyboard_callback();
                    }
                    if((!m_focused_entity.expired() && !intersected_entity) ||
                       (!m_focused_entity.expired() && intersected_entity && intersected_entity != m_focused_entity.lock()))
                    {
                        auto textedit_component = m_focused_entity.lock()->get_component<ces_textedit_component>();
                        textedit_component->focus = false;
                    }
                    m_focused_entity = intersected_entity;
                }
                m_events.pop();
            }
            
            while (!m_virtual_keyboard_events.empty() && !m_focused_entity.expired())
            {
                auto textedit_component = m_focused_entity.lock()->get_component<ces_textedit_component>();
                const auto& event = m_virtual_keyboard_events.front();
                if(std::get<0>(event).length() > 0)
                {
                    textedit_component->on_text_changed(std::get<0>(event));
                }
                else if(std::get<1>(event))
                {
                    textedit_component->on_backspace();
                }
                m_virtual_keyboard_events.pop();
            }
        }
        
        void ces_textedit_system::on_feed_end(f32 deltatime)
        {
            
        }
        
        ces_entity_shared_ptr ces_textedit_system::intersected_entity(const ces_entity_shared_ptr& entity, const touch_event_t& event)
        {
            std::list<ces_entity_shared_ptr> children = entity->children;
            ces_entity_shared_ptr intersected_entity = nullptr;
            
            for(const auto& child : children)
            {
                ces_entity_shared_ptr intersected_child_entity = ces_textedit_system::intersected_entity(child, event);
                if(intersected_child_entity)
                {
                    intersected_entity = intersected_child_entity;
                }
            }
            
            auto bound_touch_component = entity->get_component<ces_bound_touch_component>();
            auto textedit_component = entity->get_component<ces_textedit_component>();
            
            if(textedit_component && bound_touch_component && !intersected_entity && bound_touch_component->is_respond_to(std::get<1>(event), std::get<0>(event)) && entity->visible)
            {
                auto transformation_component = entity->get_component<ces_transformation_2d_component>();
                
                glm::mat4 mat_m = transformation_component->get_absolute_transformation();
                
                if(transformation_component->is_in_camera_space())
                {
                    mat_m *= ces_base_system::get_current_camera_2d()->get_mat_v();
                }
                
                glm::vec4 frame = bound_touch_component->get_frame();
                glm::vec2 min_bound = glm::transform(glm::vec2(frame.x, frame.y),
                                                     mat_m);
                glm::vec2 max_bound = glm::transform(glm::vec2(frame.z, frame.w),
                                                     mat_m);
                frame = glm::vec4(min_bound.x, min_bound.y, max_bound.x, max_bound.y);
                glm::ivec2 point = std::get<2>(event);
                
                if(glm::intersect(frame, glm::vec2(point)))
                {
                    intersected_entity = entity;
                }
            }
            return intersected_entity;
        }
        
        void ces_textedit_system::on_gr_pressed(const glm::ivec2& point,
			const glm::ivec2& screen_size,
			e_input_source input_source, 
			ui32 index)
        {
            m_events.push(std::make_tuple(input_source, e_input_state_pressed, point, index));
        }
        
        void ces_textedit_system::on_gr_released(const glm::ivec2& point,
			const glm::ivec2& screen_size,
			e_input_source input_source, 
			ui32 index)
        {
            m_events.push(std::make_tuple(input_source, e_input_state_released, point, index));
        }
        
        void ces_textedit_system::on_gr_moved(const glm::ivec2& point, 
			const glm::ivec2& screen_size,
			const glm::ivec2& delta, 
			ui32 index)
        {
            m_events.push(std::make_tuple(e_input_source_none, e_input_state_moved, point, index));
        }
        
        void ces_textedit_system::on_gr_dragged(const glm::ivec2& point,
			const glm::ivec2& screen_size,
			const glm::ivec2& delta,
                                             e_input_source input_source, 
			ui32 index)
        {
            m_events.push(std::make_tuple(input_source, e_input_state_dragged, point, index));
        }
        
        void ces_textedit_system::on_key_up(i32 key)
        {
            
        }
        
        void ces_textedit_system::on_key_down(i32 key)
        {
            
        }
        
        void ces_textedit_system::on_virtual_keyboard_input(const std::string& symbol)
        {
            m_virtual_keyboard_events.push(std::make_tuple(symbol, false));
        }
        
        void ces_textedit_system::on_virtual_keyboard_backspace()
        {
            m_virtual_keyboard_events.push(std::make_tuple("", true));
        }
        
        void ces_textedit_system::on_virtual_keyboard_hidden()
        {
            while(!m_virtual_keyboard_events.empty())
            {
                m_virtual_keyboard_events.pop();
            }
            if(!m_focused_entity.expired())
            {
                auto textedit_component = m_focused_entity.lock()->get_component<ces_textedit_component>();
                textedit_component->focus = false;
            }
            m_focused_entity.reset();
        }
        
        void ces_textedit_system::set_show_virtual_keyboard_callback(const input_context::show_virtual_keyboard_callback_t& callback)
        {
            m_show_virtual_keyboard_callback = callback;
        }
        
        void ces_textedit_system::set_hide_virtual_keyboard_callback(const input_context::hide_virtual_keyboard_callback_t& callback)
        {
            m_hide_virtual_keyboard_callback = callback;
        }
    }
}

#endif
