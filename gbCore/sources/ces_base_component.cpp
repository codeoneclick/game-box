//
//  ces_base_component.cpp
//  gbCore
//
//  Created by sergey.sergeev on 8/11/15.
//  Copyright (c) 2015 sergey.sergeev. All rights reserved.
//

#include "ces_base_component.h"
#include "game_commands_container.h"

namespace gb
{
    std::set<uintptr_t> ces_base_component::g_guids_container;
    std::map<uintptr_t, std::list<ces_entity_weak_ptr>> ces_base_component::m_references_to_entities;
    
    ces_base_component::ces_base_component()
    {
      
    }
    
    void ces_base_component::on_component_added(const ces_entity_shared_ptr& entity)
    {
        m_references_to_entities[instance_guid()].push_back(entity);
    }
    
    void ces_base_component::on_component_removed(const ces_entity_shared_ptr& entity)
    {
        m_references_to_entities[instance_guid()].remove_if([=](const ces_entity_weak_ptr& weak_entity) {
            return weak_entity.lock() == entity;
        });
    }
    
    const std::list<ces_entity_weak_ptr>& ces_base_component::get_references_to_entities(uintptr_t guid)
    {
        return m_references_to_entities[guid];
    }
};
