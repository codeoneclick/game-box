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
        ces_base_system::add_required_component_guid(m_action_components_mask, ces_action_component::class_guid());
        ces_base_system::add_required_components_mask(m_action_components_mask);
    }
    
    ces_actions_system::~ces_actions_system()
    {
        
    }
    
    void ces_actions_system::on_feed_start(f32 dt)
    {
        
    }
    
    void ces_actions_system::on_feed(const ces_entity_shared_ptr& root, f32 dt)
    {
        ces_base_system::enumerate_entities_with_components(m_action_components_mask, [dt](const ces_entity_shared_ptr& entity) {
            auto action_component = entity->get_component<ces_action_component>();
            if (action_component)
            {
                action_component->on_update(entity, dt);
            }
        });
    }
    
    void ces_actions_system::on_feed_end(f32 dt)
    {
        
    }
}
