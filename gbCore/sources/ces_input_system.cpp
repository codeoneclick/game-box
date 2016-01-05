//
//  ces_input_system.cpp
//  gbCore
//
//  Created by sergey.sergeev on 8/31/15.
//  Copyright (c) 2015 sergey.sergeev. All rights reserved.
//

#include "ces_input_system.h"
#include "collision_manager.h"
#include "mesh.h"
#include "camera.h"
#include "scene_graph_parameters.h"
#include "ces_entity.h"
#include "ces_transformation_component.h"
#include "ces_geometry_component.h"
#include "ces_touch_component.h"

namespace gb
{
    ces_input_system::ces_input_system()
    {
        m_type = ces_system_type.ces_system_type_input;
    }
    
    void ces_input_system::add_touch_listener(const touch_listener_shared_ptr& listener)
    {
        m_listeners.insert(listener);
    }
    
    void ces_input_system::remove_touch_listener(const touch_listener_shared_ptr& listener)
    {
        m_listeners.erase(listener);
    }
    
    void ces_input_system::on_feed_start(f32 deltatime)
    {
        m_entities.clear();
    }
    
    void ces_input_system::on_feed(const ces_entity_shared_ptr& entity, f32 deltatime)
    {
        if(entity->is_component_exist(e_ces_component_type_touch))
        {
            m_entities.push_back(entity);
        }
    }
    
    void ces_input_system::on_feed_end(f32 deltatime)
    {
        while (!m_events.empty())
        {
            const auto& event = m_events.front();
            glm::ivec2 point = std::get<2>(event);
            
            for(const auto& entity : m_entities)
            {
                ces_touch_component_shared_ptr touch_component = std::static_pointer_cast<ces_touch_component>(entity->get_component(e_ces_component_type_touch));
                if(touch_component->is_enabled(std::get<1>(event)) && ces_input_system::is_entity_intersected(entity, point))
                {
                    for(const auto& iterator : m_listeners)
                    {
                        iterator->on_touch(glm::vec3(0.f), glm::vec3(0.f), entity, std::get<0>(event), std::get<1>(event));
                    }
                }
            }
            m_events.pop();
        }
    }
    
    bool ces_input_system::is_entity_intersected(const ces_entity_shared_ptr& entity, const glm::ivec2& point)
    {
        std::shared_ptr<ces_geometry_component> geometry_component = std::static_pointer_cast<ces_geometry_component>(entity->get_component(e_ces_component_type_geometry));
        std::shared_ptr<ces_transformation_component> transformation_component =
        std::static_pointer_cast<ces_transformation_component>(entity->get_component(e_ces_component_type_transformation));
        
        scene_graph_parameters_shared_ptr scene_graph_parameters = geometry_component->get_scene_graph_parameters();
        
        glm::ray ray;
        collision_manager::unproject(point, scene_graph_parameters->get_eye_matrix_v(), scene_graph_parameters->get_eye_matrix_p(),
                                     scene_graph_parameters->get_viewport(), &ray);
        
        std::tuple<glm::vec3, glm::vec3> bounds = geometry_component->get_bounds(transformation_component->get_matrix_m());
        return collision_manager::is_bounding_box_intersected(ray, std::get<0>(bounds), std::get<1>(bounds), point, nullptr);
    }
    
    void ces_input_system::on_gr_pressed(const glm::ivec2& point, e_input_element input_element)
    {
        m_events.push(std::make_tuple(input_element, e_input_state_pressed, point));
    }
    
    void ces_input_system::on_gr_released(const glm::ivec2& point, e_input_element input_element)
    {
        m_events.push(std::make_tuple(input_element, e_input_state_released, point));
    }
    
    void ces_input_system::on_gr_moved(const glm::ivec2& point, const glm::ivec2& delta)
    {
        m_events.push(std::make_tuple(e_input_element_none, e_input_state_moved, point));
    }
    
    void ces_input_system::on_gr_dragged(const glm::ivec2& point, const glm::ivec2& delta, e_input_element input_element)
    {
        m_events.push(std::make_tuple(input_element, e_input_state_dragged, point));
    }
    
    void ces_input_system::on_key_up(i32 key)
    {
        
    }
    
    void ces_input_system::on_key_down(i32 key)
    {
        
    }
}