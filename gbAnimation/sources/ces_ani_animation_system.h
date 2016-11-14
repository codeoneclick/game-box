//
//  ces_ani_animation_system.hpp
//  gbAnimation
//
//  Created by serhii serhiiv on 11/14/16.
//  Copyright © 2016 sergey.sergeev. All rights reserved.
//

#ifndef ces_ani_animation_system_h
#define ces_ani_animation_system_h

#include "ces_base_system.h"

namespace gb
{
    namespace anim
    {
        class ces_ani_animation_system : public ces_base_system
        {
        private:
            
        protected:
            
            void on_feed_start(f32 deltatime);
            void on_feed(const ces_entity_shared_ptr& entity, f32 deltatime);
            void on_feed_end(f32 deltatime);
            
            void update_recursively(const ces_entity_shared_ptr& entity, f32 deltatime);
            
        public:
            
            CTTI_CLASS_GUID(ces_ani_animation_system, ces_base_system::g_guids_container)
            ces_ani_animation_system();
            ~ces_ani_animation_system();
        };
    };
};

#endif
