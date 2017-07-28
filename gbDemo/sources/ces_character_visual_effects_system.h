//
//  ces_character_visual_effects_system.h
//  gbDemo
//
//  Created by serhii serhiiv on 7/28/17.
//  Copyright © 2017 sergey.sergeev. All rights reserved.
//

#pragma once

#include "ces_base_system.h"

namespace game
{
    class ces_character_visual_effects_system : public gb::ces_base_system
    {
    private:
        
        std::bitset<std::numeric_limits<uint8_t>::max()> m_blinking_effect_components_mask;
        
    protected:
        
        void on_feed_start(f32 dt);
        void on_feed(const gb::ces_entity_shared_ptr& root, f32 dt);
        void on_feed_end(f32 dt);
        
    public:
        
        CTTI_CLASS_GUID(ces_character_visual_effects_system, gb::ces_base_system::g_guids_container)
        ces_character_visual_effects_system();
        ~ces_character_visual_effects_system();
    };
};
