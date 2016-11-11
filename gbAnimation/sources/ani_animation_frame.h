#pragma once

#include "main_headers.h"
#include "declarations.h"

namespace gb
{
    namespace anim
    {
        class ani_texture_atlas;
        class ani_subobject_state;
        class ani_timeline_action;
        
        class ani_animation_frame
        {
        public:
            
            typedef std::vector<std::shared_ptr<ani_subobject_state>> ani_subobject_states_t;
            typedef std::vector<std::shared_ptr<ani_timeline_action>> ani_timeline_actions_t;
            
        private:
            
            ani_subobject_states_t m_subobject_states;
            ani_timeline_actions_t m_timeline_actions;
            
        public:
            
            ani_animation_frame();
            ~ani_animation_frame();
            
            const ani_subobject_states_t& get_object_states() const;
            const ani_timeline_actions_t& get_timeline_actions() const;
            
            void push_object_state(const std::shared_ptr<ani_subobject_state>& state);
            void push_timeline_action(const std::shared_ptr<ani_timeline_action>& action);
            void sort_states_by_z_index();
        };
    };
};
