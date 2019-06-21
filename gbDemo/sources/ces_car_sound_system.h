//
//  ces_car_sound_system.h
//  gbDemo
//
//  Created by serhii.s on 4/19/19.
//  Copyright © 2019 sergey.sergeev. All rights reserved.
//

#pragma once

#include "ces_base_system.h"

namespace game
{
    class ces_car_sound_system : public gb::ces_base_system
    {
    private:
        
        std::bitset<std::numeric_limits<uint8_t>::max()> m_car_sound_components_mask;
        
    protected:
        
        void on_feed_start(f32 dt);
        void on_feed(const gb::ces_entity_shared_ptr& root, f32 dt);
        void on_feed_end(f32 dt);
        
    public:
        
        STTI_CLASS_GUID(ces_car_sound_system, gb::ces_base_system::g_guids_container)
        ces_car_sound_system();
        ~ces_car_sound_system();
    };
};

