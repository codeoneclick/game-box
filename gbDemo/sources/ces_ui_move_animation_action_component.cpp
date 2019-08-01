//
//  ces_ui_animation_action_component.cpp
//  gbDemo
//
//  Created by serhii.s on 7/31/19.
//  Copyright © 2019 sergey.sergeev. All rights reserved.
//

#include "ces_ui_move_animation_action_component.h"

namespace game
{
    
    f32 ces_ui_move_animation_action_component::get_progress() const
    {
        return m_progress;
    }

    void ces_ui_move_animation_action_component::set_duration_in_second(f32 duration)
    {
        m_duration_in_seconds = duration;
    }
    
    void ces_ui_move_animation_action_component::set_move_direction(e_move_direction direction)
    {
        m_move_direction = direction;
    }
    
    void ces_ui_move_animation_action_component::set_move_mode(e_move_mode mode)
    {
        m_move_mode = mode;
    }
    
    void ces_ui_move_animation_action_component::play(const std::function<void(const gb::ces_entity_shared_ptr&, f32 dt, e_move_direction, e_move_mode, f32)>& callback)
    {
        m_current_duration_in_seconds = m_progress * m_duration_in_seconds;
        if (m_update_callback == nullptr)
        {
            m_update_callback = [=](const gb::ces_entity_shared_ptr& entity, f32 dt) {
                m_current_duration_in_seconds += dt;
                if (m_current_duration_in_seconds > m_duration_in_seconds)
                {
                    m_is_finished = true;
                }
                m_progress = m_current_duration_in_seconds / m_duration_in_seconds;
                m_progress = glm::clamp(m_progress, 0.f, 1.f);
                callback(entity, dt, m_move_direction, m_move_mode, m_progress);
            };
        }
    }
    
    bool ces_ui_move_animation_action_component::get_is_finished() const
    {
        return m_is_finished;
    }
}
