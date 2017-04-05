//
//  ai_move_action.cpp
//  gbDemo
//
//  Created by sergey.sergeev on 10/22/15.
//  Copyright © 2015 sergey.sergeev. All rights reserved.
//

#include "ai_move_action.h"
#include "game_object.h"
#include "ces_box2d_body_component.h"
#include "glm_extensions.h"

namespace game
{
    f32 ai_move_action::m_trashhold_distance = 8.f;
    
    ai_move_action::ai_move_action() :
    m_goal_position(glm::vec2(.0f))
    {
        
    }
    
    ai_move_action::~ai_move_action()
    {
        
    }
    
    void ai_move_action::set_parameters(const gb::game_object_shared_ptr& executor,
                                        const glm::vec2& goal_position)
    {
        m_executor = executor;
        m_goal_position = goal_position;
    }
    
#define k_move_speed -1000.f
#define k_rotate_speed 100.f
    
    void ai_move_action::update(f32 deltatime)
    {
        if(!m_executor.expired())
        {
            gb::game_object_shared_ptr executor = m_executor.lock();
            glm::vec2 current_position = executor->position;
            f32 current_rotation = executor->rotation;
            if(m_state != e_ai_action_state_ended && m_state != e_ai_action_state_interrupted)
            {
                if(m_state == e_ai_action_state_none)
                {
                    m_state = e_ai_action_state_in_progress;
                    if(m_start_callback)
                    {
                        m_start_callback(shared_from_this());
                    }
                }
                f32 distance = glm::distance(current_position, m_goal_position);
                if(distance <= m_trashhold_distance)
                {
                    m_state = e_ai_action_state_ended;
                    
                    gb::ces_box2d_body_component_shared_ptr box2d_body_component =
                    executor->get_component<gb::ces_box2d_body_component>();
                    box2d_body_component->velocity = glm::vec2(0.f);
                    
                    if(m_end_callback)
                    {
                        m_end_callback(shared_from_this());
                    }
                }
                else
                {
                    gb::ces_box2d_body_component_shared_ptr box2d_body_component =
                    executor->get_component<gb::ces_box2d_body_component>();
                    
                    glm::vec2 direction = glm::normalize(m_goal_position - current_position);
                    f32 goal_rotation = atan2f(direction.x, -direction.y);
                    goal_rotation = glm::wrap_degrees(glm::degrees(goal_rotation));
                    current_rotation = glm::mix_angles_degrees(current_rotation, goal_rotation, 1.f);
                    
                    f32 current_move_speed = k_move_speed * deltatime;
                    
                    glm::vec2 velocity = glm::vec2(-sinf(glm::radians(current_rotation)) * current_move_speed,
                                                   cosf(glm::radians(current_rotation)) * current_move_speed);
                    
                    executor->position = current_position;
                    executor->rotation = current_rotation;
                    box2d_body_component->velocity = velocity;
                    
                    if(m_in_progress_callback)
                    {
                        m_in_progress_callback(shared_from_this());
                    }
                }
            }
        }
        else
        {
            assert(false);
        }
    }
}
