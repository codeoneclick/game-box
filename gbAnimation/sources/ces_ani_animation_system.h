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
#include "ani_declarations.h"

namespace gb
{
    namespace anim
    {
        class ani_display_list_container
        {
        public:
            
            std::unordered_map<i32, ces_entity_shared_ptr> m_entities;
            ani_animation_frame::ani_subobject_states_t m_states;
            std::list<ani_display_list_container_shared_ptr> m_container;
        };
        
        class ces_ani_animation_system : public ces_base_system
        {
        private:
            
            static ces_ani_animation_system_weak_ptr g_shared_instance;
            
            ui64 m_global_frame_iteration;
            void update_global_frame();
            
            static void update_animation_manualy(ces_entity_const_shared_ptr entity, i32 frame_index, bool is_playing = false, bool is_looped = false);
            
        protected:
            
            void on_feed_start(f32 deltatime);
            void on_feed(const ces_entity_shared_ptr& entity, f32 deltatime);
            void on_feed_end(f32 deltatime);
            
            void update_recursively(const ces_entity_shared_ptr& entity, f32 deltatime, bool force = false);
            
            void fill_display_list_recursively(const ces_entity_shared_ptr& entity,
                                               ani_display_list_container_const_shared_ptr display_list);
            void update_display_list_recursively(ani_display_list_container_const_shared_ptr display_list, f32 dt);
            
        public:
            
            CTTI_CLASS_GUID(ces_ani_animation_system, ces_base_system::g_guids_container)
            ces_ani_animation_system();
            ~ces_ani_animation_system();
            
            void init();
            
            static void goto_and_stop(ces_entity_const_shared_ptr entity, i32 frame_index);
            static void goto_and_play(ces_entity_const_shared_ptr entity, i32 frame_index, bool is_looped = false);
            static void goto_and_stop(ces_entity_const_shared_ptr entity, const std::string& animation_name);
            static void goto_and_play(ces_entity_const_shared_ptr entity, const std::string& animation_name, bool is_looped = false);

        };
    };
};

#endif
