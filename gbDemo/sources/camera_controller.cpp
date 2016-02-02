//
//  camera_controller.cpp
//  gbDemo
//
//  Created by sergey.sergeev on 1/15/16.
//  Copyright © 2016 sergey.sergeev. All rights reserved.
//

#include "camera_controller.h"
#include "ed_declarations.h"
#include "grid.h"
#include "ces_bound_touch_component.h"
#include "camera.h"

namespace cs
{
    camera_controller::camera_controller(const gb::camera_shared_ptr& camera) :
    m_camera(camera)
    {
        
    }
    
    camera_controller::~camera_controller()
    {
        
    }
    
    void camera_controller::set_map(const gb::game_object_shared_ptr& map)
    {
        m_map = map;
        if(m_map)
        {
            gb::ces_bound_touch_component_shared_ptr bound_touch_compoent = std::make_shared<gb::ces_bound_touch_component>();
            bound_touch_compoent->enable(gb::e_input_state_dragged, gb::e_input_source_mouse_right, false);
            bound_touch_compoent->add_callback(gb::e_input_state_dragged, std::bind(&camera_controller::on_dragged, this, std::placeholders::_1,
                                                                                std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));
            bound_touch_compoent->enable(gb::e_input_state_pressed, gb::e_input_source_mouse_right, true);
            bound_touch_compoent->add_callback(gb::e_input_state_pressed, std::bind(&camera_controller::on_touched, this, std::placeholders::_1,
                                                                                    std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));
            bound_touch_compoent->enable(gb::e_input_state_released, gb::e_input_source_mouse_right, true);
            bound_touch_compoent->add_callback(gb::e_input_state_released, std::bind(&camera_controller::on_touched, this, std::placeholders::_1,
                                                                                    std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));
            m_map->add_component(bound_touch_compoent);
            bound_touch_compoent->set_frame(m_map->get_bound());
        }
    }
    
    void camera_controller::on_touched(const gb::ces_entity_shared_ptr&, const glm::vec2& point, gb::e_input_source input_source, gb::e_input_state input_state)
    {
        m_previous_dragged_point = point;
        if(input_state == gb::e_input_state_pressed)
        {
            gb::ces_bound_touch_component_shared_ptr bound_touch_compoent = std::static_pointer_cast<gb::ces_bound_touch_component>(m_map->get_component(gb::e_ces_component_type_bound_touch));
            bound_touch_compoent->enable(gb::e_input_state_dragged, gb::e_input_source_mouse_right, true);
        }
        else if(input_state == gb::e_input_state_released)
        {
            gb::ces_bound_touch_component_shared_ptr bound_touch_compoent = std::static_pointer_cast<gb::ces_bound_touch_component>(m_map->get_component(gb::e_ces_component_type_bound_touch));
            bound_touch_compoent->enable(gb::e_input_state_dragged, gb::e_input_source_mouse_right, false);
        }
    }
    
    void camera_controller::on_dragged(const gb::ces_entity_shared_ptr&, const glm::vec2& point, gb::e_input_source input_source, gb::e_input_state input_state)
    {
        glm::vec2 delta = point - m_previous_dragged_point;
        glm::vec2 new_camera_position = m_camera->get_position() + delta;
        new_camera_position = glm::clamp(new_camera_position, -glm::vec2(m_map->get_bound().z - m_camera->get_width(), m_map->get_bound().w - m_camera->get_height()),
                                         glm::vec2(m_map->get_bound().x, m_map->get_bound().y));
        m_camera->set_position(new_camera_position);
        
        m_previous_dragged_point = point;
    }
}