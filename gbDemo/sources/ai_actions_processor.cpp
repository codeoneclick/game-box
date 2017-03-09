//
//  ai_actions_processor.cpp
//  gbDemo
//
//  Created by sergey.sergeev on 10/22/15.
//  Copyright © 2015 sergey.sergeev. All rights reserved.
//

#include "ai_actions_processor.h"
#include "ai_action.h"

namespace game
{
    ai_actions_processor::ai_actions_processor()
    {
        
    }
    
    ai_actions_processor::~ai_actions_processor()
    {
        while (!m_actions.empty())
        {
            m_actions.pop();
        }
    }
    
    void ai_actions_processor::update(f32 deltatime)
    {
        if(!m_actions.empty())
        {
            ai_action_shared_ptr action = m_actions.front();
            if(action->get_state() != ai_action::e_ai_action_state_ended && action->get_state() != ai_action::e_ai_action_state_interapted)
            {
                action->update(deltatime);
            }
            else
            {
                m_actions.pop();
            }
        }
    }
    
    void ai_actions_processor::add_action(const ai_action_shared_ptr& action)
    {
        m_actions.push(action);
    }
    
    bool ai_actions_processor::is_actions_exist() const
    {
        return m_actions.size() != 0;
    }
    
    ai_action_shared_ptr ai_actions_processor::top_action() const
    {
        ai_action_shared_ptr action = nullptr;
        if(!m_actions.empty())
        {
            action = m_actions.front();
        }
        return action;
    }
}
