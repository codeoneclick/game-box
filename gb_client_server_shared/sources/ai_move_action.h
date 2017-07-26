//
//  ai_move_action.h
//  gbDemo
//
//  Created by sergey.sergeev on 10/22/15.
//  Copyright © 2015 sergey.sergeev. All rights reserved.
//

#pragma once

#include "ai_action.h"

namespace game
{
    class ai_move_action : public ai_action
    {
    private:
        
    protected:
        
        static f32 m_trashhold_distance;
        std::queue<glm::vec2> m_path;
        
    public:
        
        CTTI_CLASS_GUID(ai_move_action, ai_action::g_guids_container)
        ai_move_action(const gb::ces_entity_shared_ptr& owner);
        ~ai_move_action();
        
        void set_parameters(const std::queue<glm::vec2>& path);
        
        void update(f32 dt);
    };
};
