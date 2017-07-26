//
//  ai_chase_action.h
//  gbDemo
//
//  Created by serhii serhiiv on 6/14/17.
//  Copyright © 2017 sergey.sergeev. All rights reserved.
//

#pragma once

#include "ai_action.h"

namespace game
{
    class ai_chase_action : public ai_action
    {
    private:
        
    protected:
        
        gb::game_object_2d_weak_ptr m_target;
        path_map_weak_ptr m_path_grid;
        
    public:
        
        CTTI_CLASS_GUID(ai_chase_action, ai_action::g_guids_container)
        ai_chase_action(const gb::ces_entity_shared_ptr& owner);
        ~ai_chase_action();
        
        void set_parameters(const gb::game_object_2d_shared_ptr& target,
                            const path_map_shared_ptr& path_grid);
        
        void update(f32 dt);
        
        std::property_ro<gb::game_object_2d_shared_ptr> target;
    };
};
