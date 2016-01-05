//
//  common_navigator.cpp
//  gbDemo
//
//  Created by sergey.sergeev on 11/24/15.
//  Copyright © 2015 sergey.sergeev. All rights reserved.
//

#include "common_navigator.h"

namespace koth
{
    common_navigator::common_navigator(f32 move_forward_speed,
                                       f32 move_backward_speed,
                                       f32 move_side_speed,
                                       f32 rotate_speed) :
    m_move_forward_speed(move_forward_speed),
    m_move_backward_speed(move_backward_speed),
    m_move_side_speed(move_side_speed),
    m_rotate_speed(rotate_speed),
    m_position(glm::vec3(0.f)),
    m_rotation(glm::vec3(0.f)),
    m_navigation_state_move(e_navigation_state_move_none),
    m_navigation_state_rotate(e_navigation_state_rotate_none)
    {
        
    }
    
    common_navigator::~common_navigator()
    {
        
    }
    
    void common_navigator::update(f32 deltatime)
    {
        
    }
    
    void common_navigator::set_position(const glm::vec3& position)
    {
        m_position = position;
    }
    
    void common_navigator::set_rotation(const glm::vec3& rotation)
    {
        m_rotation = rotation;
    }
    
    void common_navigator::move_forward()
    {
        m_navigation_state_move = e_navigation_state_move_forward;
    }
    
    void common_navigator::move_backward()
    {
        m_navigation_state_move = e_navigation_state_move_backward;
    }
    
    void common_navigator::move_left()
    {
        m_navigation_state_move = e_navigation_state_move_left;
    }

    void common_navigator::move_right()
    {
        m_navigation_state_move = e_navigation_state_move_right;
    }
    
    void common_navigator::rotate_left()
    {
        m_navigation_state_rotate = e_navigation_state_rotate_left;
    }
    
    void common_navigator::rotate_right()
    {
        m_navigation_state_rotate = e_navigation_state_rotate_right;
    }
    
    void common_navigator::rotate_up()
    {
        m_navigation_state_rotate = e_navigation_state_rotate_up;
    }
    
    void common_navigator::rotate_down()
    {
        m_navigation_state_rotate = e_navigation_state_rotate_down;
    }
}