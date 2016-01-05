//
//  ces_camera_navigation_component.cpp
//  gbDemo
//
//  Created by sergey.sergeev on 12/18/15.
//  Copyright © 2015 sergey.sergeev. All rights reserved.
//

#include "ces_camera_navigation_component.h"
#include "ces_koth_component_types.h"

namespace koth
{
    
    ces_camera_navigation_component::ces_camera_navigation_component() :
    m_move_navigation_state(e_move_navigation_state_none),
    m_touch_point_delta(0),
    m_position(0.f),
    m_look_at(0.f),
    m_up(0.f, 1.f, 0.f)
    {
        m_type = e_ces_koth_component_type_camera_navigation;
    }
    
    ces_camera_navigation_component::~ces_camera_navigation_component()
    {
        
    }
    
    void ces_camera_navigation_component::set_move_navigation_state(e_move_navigation_state state)
    {
        m_move_navigation_state = state;
    }
    
    void ces_camera_navigation_component::set_touch_point_delta(const glm::ivec2& delta)
    {
        m_touch_point_delta = delta;
    }
    
    void ces_camera_navigation_component::set_position(const glm::vec3& position)
    {
        m_position = position;
    }
    
    glm::vec3 ces_camera_navigation_component::get_position() const
    {
        return m_position;
    }
    
    glm::vec3 ces_camera_navigation_component::get_look_at() const
    {
        return m_look_at;
    }
    
    glm::vec3 ces_camera_navigation_component::get_up() const
    {
        return m_up;
    }
    
    void ces_camera_navigation_component::update(f32 deltatime)
    {
        assert(false);
    }
}
