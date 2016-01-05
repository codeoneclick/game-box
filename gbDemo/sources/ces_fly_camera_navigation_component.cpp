//
//  ces_fly_camera_navigation_component.cpp
//  gbDemo
//
//  Created by sergey.sergeev on 12/18/15.
//  Copyright © 2015 sergey.sergeev. All rights reserved.
//

#include "ces_fly_camera_navigation_component.h"

namespace koth
{
    ces_fly_camera_navigation_component::ces_fly_camera_navigation_component() :
    m_horizontal_angle(0.f),
    m_vertical_angle(0.f)
    {

    }
    
    ces_fly_camera_navigation_component::~ces_fly_camera_navigation_component()
    {
        
    }
    
    void ces_fly_camera_navigation_component::update(f32 deltatime)
    {
        m_horizontal_angle += m_touch_point_delta.x * deltatime * -.1f;
        m_vertical_angle += m_touch_point_delta.y * deltatime * -.1f;
        
        glm::vec3 direction(cosf(m_vertical_angle) * sinf(m_horizontal_angle),
                            sinf(m_vertical_angle),
                            cosf(m_vertical_angle) * cosf(m_horizontal_angle));
        
        glm::vec3 right(sinf(m_horizontal_angle - M_PI * .5f),
                        0.f,
                        cosf(m_horizontal_angle - M_PI * .5f));
        
        m_up = glm::cross(right, direction);
        
        if(m_move_navigation_state == e_move_navigation_state_forward)
        {
            m_position += direction * deltatime * 5.f;
        }
        if(m_move_navigation_state == e_move_navigation_state_backward)
        {
            m_position -= direction * deltatime * 5.f;
        }
        if(m_move_navigation_state == e_move_navigation_state_right)
        {
            m_position += right * deltatime * 5.f;
        }
        if(m_move_navigation_state == e_move_navigation_state_left)
        {
            m_position -= right * deltatime * 5.f;
        }
        m_look_at = m_position + direction;
    }
}