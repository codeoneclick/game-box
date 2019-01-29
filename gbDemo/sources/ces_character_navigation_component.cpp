//
//  ces_character_navigation_component.cpp
//  gbDemo
//
//  Created by serhii.s on 1/23/19.
//  Copyright © 2019 sergey.sergeev. All rights reserved.
//

#include "ces_character_navigation_component.h"

namespace game
{
    
    ces_character_navigation_component::ces_character_navigation_component()
    {
        
    }
    
    void ces_character_navigation_component::set_rotation(f32 rotation)
    {
        m_rotation = rotation;
    }
    
    f32 ces_character_navigation_component::get_rotation() const
    {
        return m_rotation;
    }
    
    void ces_character_navigation_component::move_forward()
    {
        m_character_navigation_move_state = e_character_navigation_move_state::e_character_navigation_move_state_forward;
    }
    
    void ces_character_navigation_component::move_backward()
    {
        m_character_navigation_move_state = e_character_navigation_move_state::e_character_navigation_move_state_backward;
    }
    
    void ces_character_navigation_component::move_left()
    {
        m_character_navigation_move_state = e_character_navigation_move_state::e_character_navigation_move_state_left;
    }
    
    void ces_character_navigation_component::move_right()
    {
        m_character_navigation_move_state = e_character_navigation_move_state::e_character_navigation_move_state_right;
    }

    void ces_character_navigation_component::steer_left()
    {
        m_character_navigation_steer_state = e_character_navigation_steer_state::e_character_navigation_steer_state_left;
    }
    
    void ces_character_navigation_component::steer_right()
    {
        m_character_navigation_steer_state = e_character_navigation_steer_state::e_character_navigation_steer_state_right;
    }
    
    void ces_character_navigation_component::update(f32 dt)
    {
        m_velocity = glm::vec2(0.f);
        
        switch (m_character_navigation_steer_state)
        {
            case e_character_navigation_steer_state_left:
            {
                m_rotation += m_steer_speed;
            }
                break;
            case e_character_navigation_steer_state_right:
            {
                m_rotation -= m_steer_speed;
            }
                break;
            default:
                break;
        }
        
        switch (m_character_navigation_move_state)
        {
            case e_character_navigation_move_state_forward:
            {
                m_velocity.x += std::sinf(glm::radians(m_rotation)) * m_move_forward_speed;
                m_velocity.y += std::cosf(glm::radians(m_rotation)) * m_move_forward_speed;
            }
                break;
                
            case e_character_navigation_move_state_backward:
            {
                m_velocity.x -= std::sinf(glm::radians(m_rotation)) * m_move_backward_speed;
                m_velocity.y -= std::cosf(glm::radians(m_rotation)) * m_move_backward_speed;
            }
                break;
                
            default:
            {

            }
                break;
        }
    }
    
    glm::vec2 ces_character_navigation_component::get_velocity() const
    {
        return m_velocity;
    }
    
    void ces_character_navigation_component::stop_move()
    {
        m_character_navigation_move_state = e_character_navigation_move_state_none;
    }
    
    void ces_character_navigation_component::stop_steer()
    {
        m_character_navigation_steer_state = e_character_navigation_steer_state_none;
    }
    
    bool ces_character_navigation_component::is_move() const
    {
        return m_character_navigation_move_state != e_character_navigation_move_state_none;
    }
}
