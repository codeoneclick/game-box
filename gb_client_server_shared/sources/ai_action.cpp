//
//  ai_action.cpp
//  gbDemo
//
//  Created by sergey.sergeev on 10/22/15.
//  Copyright © 2015 sergey.sergeev. All rights reserved.
//

#include "ai_action.h"

namespace game
{
    std::set<uintptr_t> ai_action::g_guids_container;
    
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
    
    void ai_action::add_sub_action(const ai_action_shared_ptr& action)
    {
        m_sub_actions.push(action);
    }
    
    std::queue<ai_action_shared_ptr>& ai_action::get_sub_actions()
    {
        return m_sub_actions;
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
    
    ai_action::e_ai_action_state ai_action::get_state() const
    {
        return m_state;
    }
    
    void ai_action::interrupt()
    {
        m_state = e_ai_action_state_interrupted;
    }
    
    bool ai_action::is_start_callback_exist() const
    {
        return m_start_callback != nullptr;
    }
    
    bool ai_action::is_progress_callback_exist() const
    {
        return m_in_progress_callback != nullptr;
    }
    
    bool ai_action::is_end_callback_exist() const
    {
        return m_end_callback != nullptr;
    }
}
