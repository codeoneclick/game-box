//
//  ces_ai_component.h
//  gbDemo
//
//  Created by serhii serhiiv on 3/9/17.
//  Copyright © 2017 sergey.sergeev. All rights reserved.
//

#pragma once

#include "ces_base_component.h"
#include "ns_declarations.h"

namespace game
{
    class ces_ai_component : public gb::ces_base_component
    {
    public:
        
        enum e_ai_action
        {
            e_ai_action_idle = 0,
            e_ai_action_move
        };
        
    private:
        
    protected:
        
        e_ai_action m_current_action;
        glm::vec2 m_goal_position;
        glm::ivec2 m_goal_position_index;
        ai_actions_processor_shared_ptr m_actions_processor;
        
    public:
        
        CTTI_CLASS_GUID(ces_ai_component, gb::ces_base_component::g_guids_container)
        ces_ai_component();
        ~ces_ai_component();
        
        std::property_rw<e_ai_action> current_action;
        std::property_rw<glm::vec2> goal_position;
        std::property_rw<glm::ivec2> goal_position_index;
        std::property_ro<ai_actions_processor_shared_ptr> actions_processor;
    };
};

