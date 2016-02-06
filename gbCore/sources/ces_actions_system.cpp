//
//  ces_actions_system.cpp
//  gbCore
//
//  Created by sergey.sergeev on 1/22/16.
//  Copyright © 2016 sergey.sergeev. All rights reserved.
//

#include "ces_actions_system.h"
#include "ces_action_component.h"

namespace gb
{
    ces_actions_system::ces_actions_system()
    {
        m_type = e_ces_system_type_actions;
    }
    
    ces_actions_system::~ces_actions_system()
    {
        
    }
    
    void ces_actions_system::on_feed_start(f32 deltatime)
    {
        
    }
    
    void ces_actions_system::on_feed(const ces_entity_shared_ptr& entity, f32 deltatime)
    {
        ces_actions_system::update_recursively(entity, deltatime);
    }
    
    void ces_actions_system::on_feed_end(f32 deltatime)
    {
        
    }
    
    void ces_actions_system::update_recursively(const ces_entity_shared_ptr& entity, f32 deltatime)
    {
        ces_action_component *action_component = unsafe_get_action_component(entity);
        
        if(action_component)
        {
            action_component->on_update(entity, deltatime);
        }
        
        std::list<ces_entity_shared_ptr> children = entity->get_children();
        for(const auto& child : children)
        {
            ces_actions_system::update_recursively(child, deltatime);
        }
    }
}