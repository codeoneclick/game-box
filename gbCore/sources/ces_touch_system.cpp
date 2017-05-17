//
//  ces_touch_system.cpp
//  gbCore
//
//  Created by sergey.sergeev on 1/14/16.
//  Copyright © 2016 sergey.sergeev. All rights reserved.
//

#if !defined(__NO_RENDER__)

#include "ces_touch_system.h"
#include "ces_bound_touch_component.h"
#include "ces_transformation_2d_component.h"
#include "glm_extensions.h"
#include "camera_2d.h"

namespace gb
{
    ces_touch_system::ces_touch_system()
    {
        
    }
    
    ces_touch_system::~ces_touch_system()
    {
        
    }
    
    void ces_touch_system::on_feed_start(f32 deltatime)
    {

    }
    
    void ces_touch_system::on_feed(const ces_entity_shared_ptr& entity, f32 deltatime)
    {
        while (!m_events.empty())
        {
            const auto& event = m_events.front();
            if(std::get<3>(event) <= 1)
            {
                ces_entity_shared_ptr intersected_entity = ces_touch_system::intersected_entity(entity, event);
                
                for(size_t i = 0; i < m_captured_entities.size(); ++i)
                {
                    if(i != std::get<3>(event))
                    {
                        const auto& captured_entities = m_captured_entities[i];
                        auto iterator = std::find_if(captured_entities.begin(), captured_entities.end(), [intersected_entity] (const ces_entity_weak_ptr& entity_weak) {
                            return !entity_weak.expired() && entity_weak.lock() == intersected_entity;
                        });
                        if(iterator != captured_entities.end())
                        {
                            intersected_entity = nullptr;
                            break;
                        }
                    }
                }
                
                if(std::get<1>(event) == e_input_state_released)
                {
                    const auto& captured_entities = m_captured_entities[std::get<3>(event)];
                    for(const auto& entity_weak : captured_entities)
                    {
                        if(!entity_weak.expired())
                        {
                            auto entity = entity_weak.lock();
                            auto bound_touch_component = entity->get_component<ces_bound_touch_component>();
                            std::list<ces_bound_touch_component::t_callback> callbacks = bound_touch_component->get_callbacks(std::get<1>(event));
                            for(const auto& callback : callbacks)
                            {
                                callback(entity, glm::vec2(std::get<2>(event)), std::get<0>(event), std::get<1>(event));
                            }
                        }
                    }
                    m_captured_entities[std::get<3>(event)].clear();
                }
                if(intersected_entity)
                {
                    if(std::get<1>(event) == e_input_state_pressed)
                    {
                        m_captured_entities[std::get<3>(event)].insert(intersected_entity);
                    }
                    
                    auto bound_touch_component = intersected_entity->get_component<ces_bound_touch_component>();
                    std::list<ces_bound_touch_component::t_callback> callbacks = bound_touch_component->get_callbacks(std::get<1>(event));
                    for(const auto& callback : callbacks)
                    {
                        const auto& captured_entities = m_captured_entities[std::get<3>(event)];
                        auto iterator = std::find_if(captured_entities.begin(), captured_entities.end(), [intersected_entity] (const ces_entity_weak_ptr& entity_weak) {
                            return !entity_weak.expired() && entity_weak.lock() == intersected_entity;
                        });
                        if(iterator != captured_entities.end())
                        {
                            callback(intersected_entity, glm::vec2(std::get<2>(event)), std::get<0>(event), std::get<1>(event));
                        }
                    }
                }
                if(std::get<1>(event) == e_input_state_dragged && m_captured_entities[std::get<3>(event)].size() != 0)
                {
                    const auto& captured_entities = m_captured_entities[std::get<3>(event)];
                    for(const auto& entity_weak : captured_entities)
                    {
                        if(!entity_weak.expired())
                        {
                            auto entity = entity_weak.lock();
                            auto bound_touch_component = entity->get_component<ces_bound_touch_component>();
                            
                            std::list<ces_bound_touch_component::t_callback> callbacks = bound_touch_component->get_callbacks(std::get<1>(event));
                            for(const auto& callback : callbacks)
                            {
                                callback(entity, glm::vec2(std::get<2>(event)), std::get<0>(event), std::get<1>(event));
                            }
                        }
                    }
                }
            }
            m_events.pop();
        }
    }
    
    void ces_touch_system::on_feed_end(f32 deltatime)
    {

    }
    
    ces_entity_shared_ptr ces_touch_system::intersected_entity(const ces_entity_shared_ptr& entity, const touch_event_t& event)
    {
        std::list<ces_entity_shared_ptr> children = entity->children;
        ces_entity_shared_ptr intersected_entity = nullptr;
        
        for(const auto& child : children)
        {
            ces_entity_shared_ptr intersected_child_entity = ces_touch_system::intersected_entity(child, event);
            if(intersected_child_entity)
            {
                intersected_entity = intersected_child_entity;
            }
        }
        
        auto bound_touch_component = entity->get_component<ces_bound_touch_component>();
        if(bound_touch_component && !intersected_entity && bound_touch_component->is_respond_to(std::get<1>(event), std::get<0>(event)) && entity->visible)
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
    
    void ces_touch_system::on_gr_pressed(const glm::ivec2& point, e_input_source input_source, ui32 index)
    {
        m_events.push(std::make_tuple(input_source, e_input_state_pressed, point, index));
    }
    
    void ces_touch_system::on_gr_released(const glm::ivec2& point, e_input_source input_source, ui32 index)
    {
        m_events.push(std::make_tuple(input_source, e_input_state_released, point, index));
    }
    
    void ces_touch_system::on_gr_moved(const glm::ivec2& point, const glm::ivec2& delta, ui32 index)
    {
        m_events.push(std::make_tuple(e_input_source_none, e_input_state_moved, point, index));
    }
    
    void ces_touch_system::on_gr_dragged(const glm::ivec2& point, const glm::ivec2& delta,
                                         e_input_source input_source, ui32 index)
    {
        m_events.push(std::make_tuple(input_source, e_input_state_dragged, point, index));
    }
    
    void ces_touch_system::on_key_up(i32 key)
    {
        
    }
    
    void ces_touch_system::on_key_down(i32 key)
    {
        
    }
    
    void ces_touch_system::on_virtual_keyboard_input(const std::string& symbol)
    {
        
    }
    
    void ces_touch_system::on_virtual_keyboard_backspace()
    {
        
    }
    
    void ces_touch_system::on_virtual_keyboard_hidden()
    {
        
    }
}

#endif
