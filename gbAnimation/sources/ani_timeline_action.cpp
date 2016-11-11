#include "ani_timeline_action.h"

namespace gb
{
    namespace anim
    {
        ani_timeline_action::ani_timeline_action()
        : m_action(e_ani_action_type::e_none)
        {
            
        }
        
        void ani_timeline_action::set_action(e_ani_action_type action, const ani_action_parameters_t& parameters, const std::string& scope)
        {
            m_action = action;
            m_scope = scope;
            
            switch (m_action)
            {
                case e_ani_action_type::e_none:
                case e_ani_action_type::e_stop:
                case e_ani_action_type::e_play:
                {
                    assert(parameters.size() == 0);
                }
                    break;
                case e_ani_action_type::e_goto_and_stop:
                case e_ani_action_type::e_goto_and_play:
                {
                    assert(parameters.size() == 1);
                    m_parameters = parameters;
                }
                    break;
                case e_ani_action_type::e_dispatch_event:
                {
                    assert(parameters.size() > 0 && parameters.size() < 5);
                    m_parameters = parameters;
                }
                    break;
                default:
                    break;
            }
        }
        
        e_ani_action_type ani_timeline_action::get_action()
        {
            return m_action;
        }
        
        const std::string& ani_timeline_action::get_parameters(ani_parameter_index index)
        {
            assert(index < m_parameters.size());
            return m_parameters[index];
        }
    };
};
