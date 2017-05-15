//
//  ai_attack_action.h
//  gbDemo
//
//  Created by serhii serhiiv on 3/16/17.
//  Copyright © 2017 sergey.sergeev. All rights reserved.
//

#pragma once

#include "ai_action.h"

namespace game
{
    class ai_attack_action : public ai_action
    {
    private:
        
    protected:
        
        gb::game_object_2d_weak_ptr m_executor;
        gb::game_object_2d_weak_ptr m_target;
        f32 m_shoot_distance;
        glm::ivec4 m_move_bounds;
        
        i32 m_shoot_timeinterval;
        i32 m_last_shoot_deltatime;
        
    public:
        
        CTTI_CLASS_GUID(ai_attack_action, ai_action::g_guids_container)
        ai_attack_action();
        ~ai_attack_action();
        
        void set_parameters(const gb::game_object_2d_shared_ptr& executor,
                            const gb::game_object_2d_shared_ptr& target,
                            f32 shoot_distance,
                            const glm::ivec4& move_bounds);
        
        void update(f32 deltatime);
        
        std::property_ro<gb::game_object_2d_shared_ptr> target;
    };
};

