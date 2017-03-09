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
        
        glm::vec2 m_current_position;
        glm::vec2 m_goal_position;
        
        f32 m_rotation;
        
    public:
        
        ai_move_action();
        ~ai_move_action();
        
        void set_parameters(const glm::vec2& start_position,
                            const glm::vec2& goal_position);
        
        void update(f32 deltatime);
        
        glm::vec2 get_position() const;
        f32 get_rotation() const;
    };
};
