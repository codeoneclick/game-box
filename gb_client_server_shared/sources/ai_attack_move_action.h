//
//  ai_attack_move_action.h
//  gbDemo
//
//  Created by serhii serhiiv on 3/17/17.
//  Copyright © 2017 sergey.sergeev. All rights reserved.
//

#pragma once

#include "ai_action.h"

namespace game
{
    class ai_attack_move_action : public ai_action
    {
    private:
        
    protected:
        
        static f32 m_trashhold_distance;
        
        gb::game_object_2d_weak_ptr m_executor;
        glm::vec2 m_goal_position;
        
    public:
        
        CTTI_CLASS_GUID(ai_attack_move_action, ai_action::g_guids_container)
        ai_attack_move_action();
        ~ai_attack_move_action();
        
        void set_parameters(const gb::game_object_2d_shared_ptr& executor,
                            const glm::vec2& goal_position);
        
        void update(f32 deltatime);
    };
};

