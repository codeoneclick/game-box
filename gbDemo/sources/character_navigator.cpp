//
//  character_navigator.cpp
//  gbDemo
//
//  Created by serhii serhiiv on 11/1/16.
//  Copyright © 2016 sergey.sergeev. All rights reserved.
//

#include "character_navigator.h"
#include "ces_box2d_body_component.h"
#include "ces_transformation_component.h"
#include "ces_entity.h"

character_navigator::character_navigator(f32 move_forward_speed, f32 move_backward_speed, f32 strafe_speed, f32 steer_speed) :
m_move_forward_speed(move_forward_speed),
m_move_backward_speed(move_backward_speed),
m_strafe_speed(strafe_speed),
m_steer_speed(steer_speed),
m_move_state(e_move_state_none),
m_steer_state(e_steer_state_none)
{
    
}

void character_navigator::update(const gb::ces_entity_shared_ptr& entity, f32 deltatime)
{
    gb::ces_box2d_body_component_shared_ptr box2d_body_component = entity->get_component<gb::ces_box2d_body_component>();
    gb::ces_transformation_component_shared_ptr transformation_component = entity->get_component<gb::ces_transformation_component>();
    b2Body* box2d_body = box2d_body_component->box2d_body;

    if(box2d_body)
    {
        box2d_body->SetAwake(true);
        
        b2Vec2 velocity = b2Vec2(0.f, 0.f);
        f32 current_rotation = transformation_component->get_rotation();
        if(m_move_state != e_move_state_none)
        {
            velocity.x += sinf(glm::radians(current_rotation)) * m_move_forward_speed;
            velocity.y -= cosf(glm::radians(current_rotation)) * m_move_forward_speed;
        }
        /*switch (m_move_state)
        {
            case e_move_state_forward:
            {
                velocity.x += sinf(glm::radians(current_rotation)) * m_move_forward_speed;
                velocity.y += cosf(glm::radians(current_rotation)) * m_move_forward_speed;
            }
                break;
                
            case e_move_state_backward:
            {
                velocity.x -= sinf(glm::radians(current_rotation)) * m_move_backward_speed;
                velocity.y -= cosf(glm::radians(current_rotation)) * m_move_backward_speed;
            }
                break;
                
            case e_move_state_left:
            {
                velocity.x -= sinf(glm::radians(-current_rotation)) * m_strafe_speed;
                velocity.y -= cosf(glm::radians(-current_rotation)) * m_strafe_speed;
            }
                break;
                
            case e_move_state_right:
            {
                velocity.x += sinf(glm::radians(-current_rotation)) * m_strafe_speed;
                velocity.y += cosf(glm::radians(-current_rotation)) * m_strafe_speed;
            }
                break;
                
            default:
            {
 
            }
                break;
        }*/
        
       
        m_move_state = e_move_state_none;
        
        /*switch (m_steer_state) {
            case e_steer_state_left:
            {
                current_rotation += m_steer_speed;
            }
                break;
                
            case e_steer_state_right:
            {
                current_rotation -= m_steer_speed;
            }
                break;
                
            default:
                break;
        }
        
        box2d_body->SetTransform(box2d_body->GetPosition(), current_rotation);
        m_steer_state = e_steer_state_none;*/
    }
}

void character_navigator::set_position(const glm::vec2& position)
{
    
}

void character_navigator::set_rotation(f32 rotation)
{
    
}

void character_navigator::move_forward()
{
    m_move_state = e_move_state_forward;
}

void character_navigator::move_backward()
{
    m_move_state = e_move_state_backward;
}

void character_navigator::move_left()
{
    m_move_state = e_move_state_left;
}

void character_navigator::move_right()
{
    m_move_state = e_move_state_right;
}

void character_navigator::steer_left()
{
    m_steer_state = e_steer_state_left;
}

void character_navigator::steer_right()
{
    m_steer_state = e_steer_state_right;
}
