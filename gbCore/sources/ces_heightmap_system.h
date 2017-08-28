//
//  ces_heightmap_system.h
//  gbCore
//
//  Created by serhii serhiiv on 8/24/17.
//  Copyright © 2017 sergey.sergeev. All rights reserved.
//

#pragma once

#include "ces_base_system.h"

namespace gb
{
    class ces_heightmap_system : public ces_base_system
    {
    private:
        
        std::bitset<std::numeric_limits<uint8_t>::max()> m_heightmap_components_mask;
        
    protected:
        
        void on_feed_start(f32 dt);
        void on_feed(const ces_entity_shared_ptr& root, f32 dt);
        void on_feed_end(f32 dt);
        
    public:
        
        CTTI_CLASS_GUID(ces_heightmap_system, ces_base_system::g_guids_container)
        ces_heightmap_system();
        ~ces_heightmap_system();
    };
};

