//
//  ai_move_action.h
//  gbDemo
//
//  Created by sergey.sergeev on 10/22/15.
//  Copyright © 2015 sergey.sergeev. All rights reserved.
//

#ifndef ai_move_action_h
#define ai_move_action_h

#include "main_headers.h"
#include "declarations.h"
#include "koth_declarations.h"
#include "ai_action.h"

namespace koth
{
    class ai_move_action : public ai_action
    {
    private:
        
    protected:
        
        static f32 m_trashhold_distance;
        
        glm::vec3 m_current_position;
        glm::vec3 m_goal_position;
        
        f32 m_rotation;
        
    public:
        
        ai_move_action();
        ~ai_move_action();
        
        void set_parameters(const glm::vec3& start_position,
                            const glm::vec3& goal_position);
        
        void update(f32 deltatime);
        
        glm::vec3 get_position() const;
        f32 get_rotation() const;
    };
};

#endif
