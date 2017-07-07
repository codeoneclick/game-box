//
//  ces_base_component.cpp
//  gbCore
//
//  Created by sergey.sergeev on 8/11/15.
//  Copyright (c) 2015 sergey.sergeev. All rights reserved.
//

#include "ces_base_component.h"
#include "game_commands_container.h"
#include "ces_entity.h"

namespace gb
{
    std::set<uintptr_t> ces_base_component::g_guids_container;
    std::unordered_map<std::bitset<std::numeric_limits<uint8_t>::max()>, std::list<ces_entity_weak_ptr>> ces_base_component::m_references_to_entities;
    
    ces_base_component::ces_base_component()
    {
    }
    
    void ces_base_component::on_component_added(const ces_entity_shared_ptr& entity)
    {
        /*std::bitset<std::numeric_limits<uint8_t>::max()> mask;
        mask.set(instance_guid());
        m_references_to_entities[mask].push_back(entity);*/
    }
    
    void ces_base_component::on_component_removed(const ces_entity_weak_ptr& entity)
    {
        /*m_references_to_entities[instance_guid()].remove_if([=](const ces_entity_weak_ptr& weak_entity) {
            return weak_entity.owner_before(entity);
        });*/
    }
    
    const std::list<ces_entity_weak_ptr>& ces_base_component::get_references_to_entities(uintptr_t guid)
    {
        std::bitset<std::numeric_limits<uint8_t>::max()> mask;
        mask.set(guid);
        return ces_base_component::get_references_to_entities(mask);
    }
    
    const std::list<ces_entity_weak_ptr>& ces_base_component::get_references_to_entities(const std::bitset<std::numeric_limits<uint8_t>::max()>& mask)
    {
        return m_references_to_entities[mask];
    }
};
