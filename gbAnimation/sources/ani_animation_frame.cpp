#include "ani_animation_frame.h"
#include "ani_subobject_state.h"
#include "ani_timeline_action.h"

namespace gb
{
    namespace anim
    {
        ani_animation_frame::ani_animation_frame()
        {
            
        }
        
        ani_animation_frame::~ani_animation_frame()
        {
            
        }
        
        const ani_animation_frame::ani_subobject_states_t& ani_animation_frame::get_object_states() const
        {
            return m_subobject_states;
        }
        
        const ani_animation_frame::ani_timeline_actions_t & ani_animation_frame::get_timeline_actions() const
        {
            return m_timeline_actions;
        }
        
        void ani_animation_frame::push_object_state(const std::shared_ptr<ani_subobject_state>& state)
        {
            m_subobject_states.push_back(state);
        }
        
        void ani_animation_frame::push_timeline_action(const std::shared_ptr<ani_timeline_action>& action)
        {
            m_timeline_actions.push_back(action);
        }
        
        void ani_animation_frame::sort_states_by_z_index()
        {
            std::sort(m_subobject_states.begin(), m_subobject_states.end(), [](std::shared_ptr<ani_subobject_state> state_01, std::shared_ptr<ani_subobject_state> state_02) {
                return state_01->get_z_index() < state_02->get_z_index();
            });
        }
    }
}
