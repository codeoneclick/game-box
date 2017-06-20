//
//  ces_battle_system.h
//  gbDemo
//
//  Created by serhii serhiiv on 2/3/17.
//  Copyright © 2017 sergey.sergeev. All rights reserved.
//

#pragma once

#include "ces_base_system.h"

namespace game
{
    class ces_battle_system : public gb::ces_base_system
    {
    private:
        
    protected:
        
        void on_feed_start(f32 deltatime);
        void on_feed(const gb::ces_entity_shared_ptr& entity, f32 deltatime);
        void on_feed_end(f32 deltatime);
        
        void update_recursively(const gb::ces_entity_shared_ptr& entity, f32 deltatime);
        
    public:
        
        CTTI_CLASS_GUID(ces_battle_system, gb::ces_base_system::g_guids_container)
        ces_battle_system();
        ~ces_battle_system();
    };
};

