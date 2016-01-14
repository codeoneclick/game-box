//
//  ces_touch_system.cpp
//  gbCore
//
//  Created by sergey.sergeev on 1/14/16.
//  Copyright © 2016 sergey.sergeev. All rights reserved.
//

#include "ces_touch_system.h"
#include "ces_bound_touch_component.h"
#include "ces_transformation_component.h"
#include "ces_scene_component.h"
#include "glm_extensions.h"
#include "camera.h"

namespace gb
{
    ces_touch_system::ces_touch_system()
    {
        m_type = e_ces_system_type_touch;
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
            ces_entity_shared_ptr intersected_entity = ces_touch_system::intersected_entity(entity, event);
            if(intersected_entity)
            {
                std::cout<<"intersected"<<std::endl;
            }
            m_events.pop();
        }
    }
    
    void ces_touch_system::on_feed_end(f32 deltatime)
    {

    }
    
    ces_entity_shared_ptr ces_touch_system::intersected_entity(const ces_entity_shared_ptr& entity, const std::tuple<e_input_element, e_input_state, glm::ivec2>& event)
    {
        std::list<ces_entity_shared_ptr> children = entity->get_children();
        ces_entity_shared_ptr intersected_entity = nullptr;
        
        for(const auto& child : children)
        {
            intersected_entity = ces_touch_system::intersected_entity(child, event);
        }
        
        ces_bound_touch_component* bound_touch_component = unsafe_get_bound_touch_component(entity);
        if(bound_touch_component && !intersected_entity)
        {
            ces_transformation_component* transformation_component = unsafe_get_transformation_component(entity);
            ces_scene_component* scene_component = unsafe_get_scene_component(entity);
            
            glm::mat4 matrix_m = glm::mat4(1.f);
            ces_entity_shared_ptr parent = entity->get_parent();
            
            while(parent)
            {
                ces_transformation_component* transformation_component = unsafe_get_transformation_component(parent);
                matrix_m = matrix_m * transformation_component->get_matrix_m();
                parent = parent->get_parent();
            }
            
            matrix_m = matrix_m * transformation_component->get_matrix_m() * scene_component->get_camera()->get_mat_v();
            
            glm::vec4 frame = bound_touch_component->get_frame();
            glm::vec2 min_bound = glm::transform(glm::vec2(frame.x, frame.y),
                                                 matrix_m);
            glm::vec2 max_bound = glm::transform(glm::vec2(frame.z, frame.w),
                                                 matrix_m);
            frame = glm::vec4(min_bound.x, min_bound.y, max_bound.x, max_bound.y);
            glm::ivec2 point = std::get<2>(event);
            if(glm::intersect(frame, glm::vec2(point)))
            {
                intersected_entity = entity;
            }
        }
        return intersected_entity;
    }
    
    void ces_touch_system::on_gr_pressed(const glm::ivec2& point, e_input_element input_element)
    {
        m_events.push(std::make_tuple(input_element, e_input_state_pressed, point));
    }
    
    void ces_touch_system::on_gr_released(const glm::ivec2& point, e_input_element input_element)
    {
        m_events.push(std::make_tuple(input_element, e_input_state_released, point));
    }
    
    void ces_touch_system::on_gr_moved(const glm::ivec2& point, const glm::ivec2& delta)
    {
        m_events.push(std::make_tuple(e_input_element_none, e_input_state_moved, point));
    }
    
    void ces_touch_system::on_gr_dragged(const glm::ivec2& point, const glm::ivec2& delta, e_input_element input_element)
    {
        m_events.push(std::make_tuple(input_element, e_input_state_dragged, point));
    }
    
    void ces_touch_system::on_key_up(i32 key)
    {
        
    }
    
    void ces_touch_system::on_key_down(i32 key)
    {
        
    }
}