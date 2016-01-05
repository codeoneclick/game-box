//
//  ai_move_action.cpp
//  gbDemo
//
//  Created by sergey.sergeev on 10/22/15.
//  Copyright © 2015 sergey.sergeev. All rights reserved.
//

#include "ai_move_action.h"

namespace koth
{
    f32 ai_move_action::m_trashhold_distance = .05f;
    
    ai_move_action::ai_move_action() :
    m_current_position(glm::vec3(.0f)),
    m_goal_position(glm::vec3(.0f)),
    m_rotation(.0f)
    {
        
    }
    
    ai_move_action::~ai_move_action()
    {
        
    }
    
    void ai_move_action::set_parameters(const glm::vec3& start_position,
                                        const glm::vec3& goal_position)
    {
        m_current_position = start_position;
        m_goal_position = goal_position;
    }
    
    void ai_move_action::update(f32 deltatime)
    {
        if(m_state != e_ai_action_state_ended && m_state != e_ai_action_state_interapted)
        {
            if(m_state == e_ai_action_state_none)
            {
                m_state = e_ai_action_state_in_progress;
                if(m_start_callback)
                {
                    m_start_callback(shared_from_this());
                }
            }
            f32 distance = glm::distance(m_current_position, m_goal_position);
            if(distance <= m_trashhold_distance)
            {
                m_state = e_ai_action_state_ended;
                if(m_end_callback)
                {
                    m_end_callback(shared_from_this());
                }
            }
            else
            {
                glm::vec3 direction = glm::normalize(m_goal_position - m_current_position);
                m_current_position += direction * m_trashhold_distance;

                m_rotation = atan2f(direction.x, direction.z);
                
                if(m_in_progress_callback)
                {
                    m_in_progress_callback(shared_from_this());
                }
            }
        }
    }
    
    glm::vec3 ai_move_action::get_position() const
    {
        return m_current_position;
    }
    
    f32 ai_move_action::get_rotation() const
    {
        return m_rotation;
    }
}