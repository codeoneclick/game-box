//
//  ces_character_selector_component.cpp
//  gbDemo
//
//  Created by serhii serhiiv on 6/19/17.
//  Copyright © 2017 sergey.sergeev. All rights reserved.
//

#include "ces_character_selector_component.h"

namespace game
{
    ces_character_selector_component::ces_character_selector_component()
    {
        
    }
    void ces_character_selector_component::add_selection(const gb::ces_entity_shared_ptr& entity)
    {
        m_selections.push_back(entity);
    }
    
    void ces_character_selector_component::remove_selection(const gb::ces_entity_shared_ptr& entity)
    {
        m_selections.erase(std::remove_if(m_selections.begin(), m_selections.end(), [entity](const gb::ces_entity_weak_ptr& entity_weak) {
            if(!entity_weak.expired())
            {
                return entity == entity_weak.lock();
            }
            return true;
        }), m_selections.end());
    }
    
    bool ces_character_selector_component::is_selections_exist() const
    {
        return m_selections.size() != 0;
    }
    
    std::vector<gb::ces_entity_weak_ptr> ces_character_selector_component::get_selections() const
    {
        return m_selections;
    }
    
    void ces_character_selector_component::enumerate_selections(const std::function<void(const gb::ces_entity_shared_ptr& entity)> enumerator) const
    {
        for(const auto& entity_weak : m_selections)
        {
            enumerator(entity_weak.lock());
        }
    }
    
    void ces_character_selector_component::remove_all_selections()
    {
        m_selections.clear();
    }
}
