#pragma once

#include "main_headers.h"
#include "declarations.h"
#include "tags_defines.h"

namespace gb
{
    namespace anim
    {
        typedef std::vector<std::string> ani_action_parameters_t;
        
        class ani_timeline_action
        {
        public:
            
            enum ani_parameter_index
            {
                PI_FRAME = 0,
                PI_EVENT_TYPE = 0,
                PI_EVENT_BUBBLES,
                PI_EVENT_CANCELABLE,
                PI_EVENT_DATA
            };
            
        public:
            
            ani_timeline_action();

            void set_action(e_ani_action_type action, const ani_action_parameters_t& parameters, const std::string& scope);
            e_ani_action_type get_action();
            const std::string& get_parameters(ani_parameter_index index);
            
        private:
            
            e_ani_action_type m_action;
            ani_action_parameters_t m_parameters;
            std::string m_scope;
        };
    };
};
