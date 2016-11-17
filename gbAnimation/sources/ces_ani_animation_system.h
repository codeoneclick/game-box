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
#include "ani_animation_frame.h"

namespace gb
{
    namespace anim
    {
        
        class ces_ani_animation_system : public ces_base_system
        {
        private:
            
            static ces_ani_animation_system* g_shared_instance;
            
        protected:
            
            void on_feed_start(f32 deltatime);
            void on_feed(const ces_entity_shared_ptr& entity, f32 deltatime);
            void on_feed_end(f32 deltatime);
            
            void update_recursively(const ces_entity_shared_ptr& entity, f32 deltatime);
            
            void fill_display_list_recursively(const ces_entity_shared_ptr& entity,
                                               std::unordered_map<i32 ,ces_entity_shared_ptr>& display_list,
                                               const ani_animation_frame::ani_subobject_states_t& states);
            
        public:
            
            CTTI_CLASS_GUID(ces_ani_animation_system, ces_base_system::g_guids_container)
            ces_ani_animation_system();
            ~ces_ani_animation_system();
            
            static void goto_and_stop(const ces_entity_shared_ptr& entity, i32 frame_index);
        };
    };
};

#endif
