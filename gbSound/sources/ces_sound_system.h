//
//  ces_sound_system.h
//  gb_sound
//
//  Created by serhii serhiiv on 6/2/17.
//  Copyright © 2017 sergey.sergeev. All rights reserved.
//

#pragma once

#include "ces_base_system.h"

namespace gb
{
    namespace al
    {
        class ces_sound_system : public ces_base_system
        {
        private:
            
            std::bitset<std::numeric_limits<uint8_t>::max()> m_sound_components_mask;
            
        protected:
            
            void on_feed_start(f32 deltatime);
            void on_feed(const ces_entity_shared_ptr& entity, f32 deltatime);
            void on_feed_end(f32 deltatime);
            
        public:
            
            STTI_CLASS_GUID(ces_sound_system, ces_base_system::g_guids_container)
            ces_sound_system();
            ~ces_sound_system();
        };
    };
};
