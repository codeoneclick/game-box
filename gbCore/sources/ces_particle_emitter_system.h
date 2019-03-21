//
//  ces_particle_emitter_system.h
//  gbCore
//
//  Created by serhii.s on 3/21/19.
//  Copyright © 2019 sergey.sergeev. All rights reserved.
//

#pragma once

#include "ces_base_system.h"

namespace gb
{
    class ces_particle_emitter_system : public ces_base_system
    {
    private:
        
        std::bitset<std::numeric_limits<uint8_t>::max()> m_components_mask;
        
    protected:
        
        void on_feed_start(f32 dt);
        void on_feed(const ces_entity_shared_ptr& root, f32 dt);
        void on_feed_end(f32 dt);
        
    public:
        
        CTTI_CLASS_GUID(ces_particle_emitter_system, ces_base_system::g_guids_container)
        ces_particle_emitter_system();
        ~ces_particle_emitter_system() = default;
    };
};

