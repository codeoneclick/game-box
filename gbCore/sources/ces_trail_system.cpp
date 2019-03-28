//
//  ces_trail_system.cpp
//  gbCore
//
//  Created by serhii.s on 3/28/19.
//  Copyright © 2019 sergey.sergeev. All rights reserved.
//

#include "ces_trail_system.h"
#include "ces_trail_component.h"

namespace gb
{
    ces_trail_system::ces_trail_system()
    {
        ces_base_system::add_required_component_guid(m_components_mask, ces_trail_component::class_guid());
        ces_base_system::add_required_components_mask(m_components_mask);
    }
    
    void ces_trail_system::on_feed_start(f32 dt)
    {
        
    }
    
    void ces_trail_system::on_feed(const ces_entity_shared_ptr& root, f32 dt)
    {
        ces_base_system::enumerate_entities_with_components(m_components_mask, [=](const ces_entity_shared_ptr& entity) {
            
            
        });
    }
    
    void ces_trail_system::on_feed_end(f32 dt)
    {
        
    }
}
