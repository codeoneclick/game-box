//
//  ai_action.cpp
//  gbDemo
//
//  Created by sergey.sergeev on 10/22/15.
//  Copyright © 2015 sergey.sergeev. All rights reserved.
//

#include "ai_action.h"

namespace koth
{
    ai_action::ai_action() :
    m_start_callback(nullptr),
    m_in_progress_callback(nullptr),
    m_end_callback(nullptr),
    m_state(e_ai_action_state_none)
    {
        
    }
    
    ai_action::~ai_action()
    {
        m_start_callback = nullptr;
        m_end_callback = nullptr;
    }
    
    void ai_action::set_start_callback(const std::function<void(const ai_action_shared_ptr&)>& start_callback)
    {
        m_start_callback = start_callback;
    }
    
    void ai_action::set_in_progress_callback(const std::function<void(const ai_action_shared_ptr&)>& in_progress_callback)
    {
        m_in_progress_callback = in_progress_callback;
    }
    
    void ai_action::set_end_callback(const std::function<void(const ai_action_shared_ptr&)>& end_callback)
    {
        m_end_callback = end_callback;
    }
    
    e_ai_action_state ai_action::get_state() const
    {
        return m_state;
    }
    
    void ai_action::interapt()
    {
        m_state = e_ai_action_state_interapted;
    }
}