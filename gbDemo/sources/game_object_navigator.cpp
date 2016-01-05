//
//  game_object_navigator.cpp
//  gbDemo
//
//  Created by sergey.sergeev on 10/7/15.
//  Copyright (c) 2015 sergey.sergeev. All rights reserved.
//

#include "game_object_navigator.h"
#include "game_object.h"
#include "ces_box2d_component.h"

namespace koth
{
    game_object_navigator::game_object_navigator(f32 move_forward_speed,
                                                 f32 move_backward_speed,
                                                 f32 rotate_speed,
                                                 const gb::game_object_shared_ptr& game_object) :
    m_move_forward_speed(move_forward_speed),
    m_move_backward_speed(move_backward_speed),
    m_rotate_speed(rotate_speed),
    m_game_object(game_object),
    m_position(glm::vec3(0.f)),
    m_rotation(glm::vec3(0.f)),
    m_navigation_state_move(e_navigation_state_move_none),
    m_navigation_state_rotate(e_navigation_state_rotate_none)
    {
        
    }
    
    game_object_navigator::~game_object_navigator()
    {
        
    }
    
    void game_object_navigator::update(f32 deltatime)
    {
        gb::ces_box2d_component_shared_ptr box2d_component = std::static_pointer_cast<gb::ces_box2d_component>(m_game_object->get_component(gb::e_ces_component_type_box2d));
        if(box2d_component && box2d_component->get_box2d_body())
        {
            switch (m_navigation_state_rotate)
            {
                case e_navigation_state_rotate_left:
                {
                    m_rotation.y += m_rotate_speed;
                }
                    break;
                    
                case e_navigation_state_rotate_right:
                {
                    m_rotation.y -= m_rotate_speed;
                }
                    break;
                    
                case e_navigation_state_rotate_none:
                {
                    
                }
                    break;
                    
                default:
                {
                    assert(false);
                }
                    break;
            }
            m_game_object->set_rotation(m_rotation);
            m_navigation_state_rotate = e_navigation_state_rotate_none;
            
            b2Vec2 velocity = b2Vec2(0.f, 0.f);
            switch (m_navigation_state_move)
            {
                case e_navigation_state_move_forward:
                {
                    velocity.x += sinf(glm::radians(m_rotation.y)) * m_move_forward_speed;
                    velocity.y += cosf(glm::radians(m_rotation.y)) * m_move_forward_speed;
                }
                    break;
                    
                case e_navigation_state_move_backward:
                {
                    velocity.x -= sinf(glm::radians(m_rotation.y)) * m_move_backward_speed;
                    velocity.y -= cosf(glm::radians(m_rotation.y)) * m_move_backward_speed;

                }
                    break;
                    
                case e_navigation_state_move_none:
                {
                    
                }
                    break;
                    
                default:
                {
                    assert(false);
                }
                    break;
            }
            box2d_component->get_box2d_body()->SetAwake(true);
            box2d_component->get_box2d_body()->SetLinearVelocity(velocity);
            m_navigation_state_move = e_navigation_state_move_none;
            
            m_position = box2d_component->get_position();
        }
        m_game_object->set_position(m_position);
        m_game_object->set_rotation(m_rotation);
    }
    
    void game_object_navigator::set_position(const glm::vec3& position)
    {
        m_position = position;
        gb::ces_box2d_component_shared_ptr box2d_component = std::static_pointer_cast<gb::ces_box2d_component>(m_game_object->get_component(gb::e_ces_component_type_box2d));
        if(box2d_component && box2d_component->get_box2d_body())
        {
            box2d_component->get_box2d_body()->SetTransform(b2Vec2(m_position.x, m_position.z), .0f);
        }
    }
    
    void game_object_navigator::set_rotation(const glm::vec3& rotation)
    {
        m_rotation = rotation;
    }
    
    void game_object_navigator::move_forward()
    {
        m_navigation_state_move = e_navigation_state_move_forward;
    }
    
    void game_object_navigator::move_backward()
    {
        m_navigation_state_move = e_navigation_state_move_backward;
    }
    
    void game_object_navigator::rotate_left()
    {
        m_navigation_state_rotate = e_navigation_state_rotate_left;
    }
    
    void game_object_navigator::rotate_right()
    {
        m_navigation_state_rotate = e_navigation_state_rotate_right;
    }
}