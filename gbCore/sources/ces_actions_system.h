//
//  ces_actions_system.h
//  gbCore
//
//  Created by sergey.sergeev on 1/22/16.
//  Copyright © 2016 sergey.sergeev. All rights reserved.
//

#pragma once

#include "ces_base_system.h"

namespace gb
{
    class ces_actions_system : public ces_base_system
    {
    private:
        
    protected:
        
        void on_feed_start(f32 deltatime);
        void on_feed(const ces_entity_shared_ptr& entity, f32 deltatime);
        void on_feed_end(f32 deltatime);
        
        void update_recursively(const ces_entity_shared_ptr& entity, f32 deltatime);
        
    public:
        
        CTTI_CLASS_GUID(ces_actions_system, ces_base_system::g_guids_container)
        ces_actions_system();
        ~ces_actions_system();
    };
};
