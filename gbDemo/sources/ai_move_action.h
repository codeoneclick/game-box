//
//  ai_move_action.h
//  gbDemo
//
//  Created by sergey.sergeev on 10/22/15.
//  Copyright © 2015 sergey.sergeev. All rights reserved.
//

#pragma once

#include "main_headers.h"
#include "declarations.h"
#include "ns_declarations.h"
#include "ai_action.h"

namespace game
{
    class ai_move_action : public ai_action
    {
    private:
        
    protected:
        
        static f32 m_trashhold_distance;
        
        gb::game_object_weak_ptr m_executor;
        glm::vec2 m_goal_position;
        
    public:
        
        ai_move_action();
        ~ai_move_action();
        
        void set_parameters(const gb::game_object_shared_ptr& executor,
                            const glm::vec2& goal_position);
        
        void update(f32 deltatime);
    };
};