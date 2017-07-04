//
//  ces_deferred_lighting_system.hpp
//  gbCore
//
//  Created by sergey.sergeev on 1/11/16.
//  Copyright © 2016 sergey.sergeev. All rights reserved.
//

#pragma once

#if !defined(__NO_RENDER__)

#include "ces_base_system.h"

namespace gb
{
    class ces_deferred_lighting_system : public ces_base_system
    {
    private:
        
    protected:
        
        void on_feed_start(f32 deltatime);
        void on_feed(ces_entity_const_shared_ptr entity, f32 deltatime);
        void on_feed_end(f32 deltatime);
        
    public:
        
        CTTI_CLASS_GUID(ces_deferred_lighting_system, ces_base_system::g_guids_container)
        ces_deferred_lighting_system();
        ~ces_deferred_lighting_system();
    };
};

#endif
