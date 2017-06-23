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
            const auto& touch_event = m_events.front();
            if(std::get<4>(touch_event) <= 1)
            {
                ces_entity_shared_ptr intersected_entity = ces_touch_system::intersected_entity(entity, touch_event);
                
                for(size_t i = 0; i < m_captured_entities.size(); ++i)
                {
                    if(i != std::get<4>(touch_event))
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
                
                if(std::get<1>(touch_event) == e_input_state_released)
                {
                    const auto& captured_entities = m_captured_entities[std::get<4>(touch_event)];
                    for(const auto& entity_weak : captured_entities)
                    {
                        if(!entity_weak.expired())
                        {
                            auto entity = entity_weak.lock();
                            auto bound_touch_component = entity->get_component<ces_bound_touch_component>();
                            std::list<ces_bound_touch_component::t_callback> callbacks = bound_touch_component->get_callbacks(std::get<1>(touch_event));
                            for(const auto& callback : callbacks)
                            {
                                glm::vec2 touch_point_in_viewport_space = ces_touch_system::convert_touch_point_to_viewport_space(touch_event);
                                callback(entity, touch_point_in_viewport_space, std::get<0>(touch_event), std::get<1>(touch_event));
                            }
                        }
                    }
                    m_captured_entities[std::get<4>(touch_event)].clear();
                }
                if(intersected_entity)
                {
                    if(std::get<1>(touch_event) == e_input_state_pressed)
                    {
                        m_captured_entities[std::get<4>(touch_event)].insert(intersected_entity);
                    }
                    
                    auto bound_touch_component = intersected_entity->get_component<ces_bound_touch_component>();
                    std::list<ces_bound_touch_component::t_callback> callbacks = bound_touch_component->get_callbacks(std::get<1>(touch_event));
                    for(const auto& callback : callbacks)
                    {
                        const auto& captured_entities = m_captured_entities[std::get<4>(touch_event)];
                        auto iterator = std::find_if(captured_entities.begin(), captured_entities.end(), [intersected_entity] (const ces_entity_weak_ptr& entity_weak) {
                            return !entity_weak.expired() && entity_weak.lock() == intersected_entity;
                        });
                        if(iterator != captured_entities.end())
                        {
                            glm::vec2 touch_point_in_viewport_space = ces_touch_system::convert_touch_point_to_viewport_space(touch_event);
                            callback(intersected_entity, touch_point_in_viewport_space, std::get<0>(touch_event), std::get<1>(touch_event));
                        }
                    }
                }
                if(std::get<1>(touch_event) == e_input_state_dragged &&
                   m_captured_entities[std::get<4>(touch_event)].size() != 0)
                {
                    const auto& captured_entities = m_captured_entities[std::get<4>(touch_event)];
                    for(const auto& entity_weak : captured_entities)
                    {
                        if(!entity_weak.expired())
                        {
                            auto entity = entity_weak.lock();
                            auto bound_touch_component = entity->get_component<ces_bound_touch_component>();
                            
                            std::list<ces_bound_touch_component::t_callback> callbacks = bound_touch_component->get_callbacks(std::get<1>(touch_event));
                            for(const auto& callback : callbacks)
                            {
                                glm::vec2 touch_point_in_viewport_space = ces_touch_system::convert_touch_point_to_viewport_space(touch_event);
                                callback(entity, touch_point_in_viewport_space, std::get<0>(touch_event), std::get<1>(touch_event));
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
    
    ces_entity_shared_ptr ces_touch_system::intersected_entity(const ces_entity_shared_ptr& entity, const touch_event_t& touch_event)
    {
        const std::list<ces_entity_shared_ptr>& children = entity->children;
        ces_entity_shared_ptr intersected_entity = nullptr;
        
        for(const auto& child : children)
        {
            ces_entity_shared_ptr intersected_child_entity = ces_touch_system::intersected_entity(child, touch_event);
            if(intersected_child_entity)
            {
                intersected_entity = intersected_child_entity;
            }
        }
        
        auto bound_touch_component = entity->get_component<ces_bound_touch_component>();
        if(bound_touch_component &&
           !intersected_entity &&
           bound_touch_component->is_respond_to(std::get<1>(touch_event), std::get<0>(touch_event)) &&
           entity->visible)
        {
            auto transformation_component = entity->get_component<ces_transformation_2d_component>();
        
			glm::vec2 point_in_viewport_space = ces_touch_system::convert_touch_point_to_viewport_space(touch_event);
            
            glm::mat4 mat_m = transformation_component->get_absolute_transformation();
            
            glm::vec2 camera_offset = glm::vec2(0.f);
            if(transformation_component->is_in_camera_space())
            {
                auto camera = ces_base_system::get_current_camera_2d();
                glm::vec2 camera_pivot = camera->pivot;
                glm::vec2 camera_position = camera->get_position();
                glm::ivec2 viewport_size = camera->viewport_size;
                camera_offset = camera_position + glm::vec2(viewport_size.x * camera_pivot.x,
					viewport_size.y * camera_pivot.y);
            }
            point_in_viewport_space = point_in_viewport_space - camera_offset;
            
            glm::vec4 bounds = bound_touch_component->get_bounds();
            glm::vec2 vertices[4];
            vertices[0] = glm::transform(glm::vec2(bounds.x, bounds.y), mat_m);
            vertices[1] = glm::transform(glm::vec2(bounds.z, bounds.y), mat_m);
            vertices[2] = glm::transform(glm::vec2(bounds.z, bounds.w), mat_m);
            vertices[3] = glm::transform(glm::vec2(bounds.x, bounds.w), mat_m);
            
            glm::vec2 min_bound = glm::vec2(INT16_MAX);
            glm::vec2 max_bound = glm::vec2(INT16_MIN);
            
            for(i32 i = 0; i < 4; ++i)
            {
                min_bound = glm::min(vertices[i], min_bound);
                max_bound = glm::max(vertices[i], max_bound);
            }
            
            bounds = glm::vec4(min_bound.x, min_bound.y, max_bound.x, max_bound.y);
            
            if(glm::intersect(bounds, point_in_viewport_space))
            {
                intersected_entity = entity;
            }
        }
        return intersected_entity;
    }
    
    void ces_touch_system::on_gr_pressed(const glm::ivec2& touch_point,
                                         const glm::ivec2& touch_area_size,
                                         e_input_source input_source,
                                         ui32 index)
    {
        m_events.push(std::make_tuple(input_source, e_input_state_pressed, touch_point, touch_area_size, index));
    }
    
    void ces_touch_system::on_gr_released(const glm::ivec2& touch_point,
                                          const glm::ivec2& touch_area_size,
                                          e_input_source input_source,
                                          ui32 index)
    {
        m_events.push(std::make_tuple(input_source, e_input_state_released, touch_point, touch_area_size, index));
    }
    
    void ces_touch_system::on_gr_moved(const glm::ivec2& touch_point,
                                       const glm::ivec2& touch_area_size,
                                       const glm::ivec2& delta,
                                       ui32 index)
    {
        m_events.push(std::make_tuple(e_input_source_none, e_input_state_moved, touch_point, touch_area_size, index));
    }
    
    void ces_touch_system::on_gr_dragged(const glm::ivec2& touch_point,
                                         const glm::ivec2& touch_area_size,
                                         const glm::ivec2& delta,
                                         e_input_source input_source,
                                         ui32 index)
    {
        m_events.push(std::make_tuple(input_source, e_input_state_dragged, touch_point, touch_area_size, index));
    }
    
    glm::vec2 ces_touch_system::convert_touch_point_to_viewport_space(const touch_event_t& touch_event)
    {
        glm::ivec2 point_in_touch_area_space = std::get<2>(touch_event);
        glm::vec2 point_in_viewport_space = glm::vec2(point_in_touch_area_space.x,
                                                      point_in_touch_area_space.y);
        
        auto camera = ces_base_system::get_current_camera_2d();
        glm::ivec2 viewport_size = camera->viewport_size;
        glm::ivec2 touch_area_size = std::get<3>(touch_event);
        
        glm::vec2 point_scale_factor = glm::vec2(static_cast<f32>(viewport_size.x) / static_cast<f32>(touch_area_size.x),
                                                 static_cast<f32>(viewport_size.y) / static_cast<f32>(touch_area_size.y));
        
        point_in_viewport_space *= point_scale_factor;
        return point_in_viewport_space;
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
