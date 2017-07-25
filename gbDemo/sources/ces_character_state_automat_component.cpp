//
//  ces_character_state_automat_component.cpp
//  gbDemo
//
//  Created by serhii serhiiv on 7/21/17.
//  Copyright © 2017 sergey.sergeev. All rights reserved.
//

#include "ces_character_state_automat_component.h"
#include "ai_actions_processor.h"

namespace game
{
    ces_character_state_automat_component::ces_character_state_automat_component() :
    m_mode(e_mode_unknown),
    m_state(e_state_idle),
    m_actions_processor(std::make_shared<ai_actions_processor>())
    {
        
    }
    
    ces_character_state_automat_component::e_mode ces_character_state_automat_component::get_mode() const
    {
        return m_mode;
    }
    
    void ces_character_state_automat_component::set_mode(e_mode mode)
    {
        m_mode = mode;
    }
    
    ces_character_state_automat_component::e_state ces_character_state_automat_component::get_state() const
    {
        return  m_state;
    }
    
    void ces_character_state_automat_component::ces_character_state_automat_component::set_state(e_state state)
    {
        if(m_state != state)
        {
            m_state = state;
            m_on_state_changed_callbacks.erase(std::remove_if(m_on_state_changed_callbacks.begin(), m_on_state_changed_callbacks.end(), [this](const std::tuple<gb::ces_entity_weak_ptr, on_state_changed_callback_t>& it) {
                if(!std::get<0>(it).expired())
                {
                    std::get<1>(it)(m_owner.lock(), m_state);
                    return false;
                }
                return true;
            }), m_on_state_changed_callbacks.end());
        }
    }
    
    ai_actions_processor_shared_ptr ces_character_state_automat_component::get_actions_processor() const
    {
        return m_actions_processor;
    }
    
    void ces_character_state_automat_component::add_on_state_changed_callback(const gb::ces_entity_shared_ptr& owner, const on_state_changed_callback_t& callback)
    {
        assert(owner);
        assert(callback);
        m_on_state_changed_callbacks.push_back(std::make_tuple(owner, callback));
    }
    
    void ces_character_state_automat_component::remove_on_state_changed_callback(const gb::ces_entity_shared_ptr& owner)
    {
        m_on_state_changed_callbacks.erase(std::remove_if(m_on_state_changed_callbacks.begin(), m_on_state_changed_callbacks.end(), [owner](const std::tuple<gb::ces_entity_weak_ptr, on_state_changed_callback_t>& it) {
            if(!std::get<0>(it).expired())
            {
                return owner == std::get<0>(it).lock();
            }
            return true;
        }), m_on_state_changed_callbacks.end());
    }
}
