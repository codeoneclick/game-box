//
//  ces_character_selector_component.h
//  gbDemo
//
//  Created by serhii serhiiv on 6/19/17.
//  Copyright © 2017 sergey.sergeev. All rights reserved.
//

#pragma once

#include "ces_base_component.h"
#include "ns_declarations.h"

namespace game
{
    class ces_character_selector_component : public gb::ces_base_component
    {
    private:
        
    protected:
        
        std::vector<gb::ces_entity_weak_ptr> m_selections;
        
    public:
        
        CTTI_CLASS_GUID(ces_character_selector_component, gb::ces_base_component::g_guids_container)
        ces_character_selector_component();
        ~ces_character_selector_component() = default;
        
        void add_selection(const gb::ces_entity_shared_ptr& entity);
        void remove_selection(const gb::ces_entity_shared_ptr& entity);
        
        std::vector<gb::ces_entity_weak_ptr> get_selections() const;
        void enumerate_selections(const std::function<void(const gb::ces_entity_shared_ptr& entity)> enumerator) const;
    };
};
