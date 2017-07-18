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
        
        std::bitset<std::numeric_limits<uint8_t>::max()> m_light_components_mask;
        std::bitset<std::numeric_limits<uint8_t>::max()> m_shadow_components_mask;
        
        glm::vec4 m_camera_2d_bounds;
        
        bool m_inside_outside_process_thread_executed;
        std::thread m_inside_outside_process_thread;
        
        void inside_outside_process();
        
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
