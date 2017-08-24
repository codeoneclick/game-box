//
//  ces_heightmap_system.cpp
//  gbCore
//
//  Created by serhii serhiiv on 8/24/17.
//  Copyright © 2017 sergey.sergeev. All rights reserved.
//

#include "ces_heightmap_system.h"
#include "ces_heightmap_container_component.h"

namespace gb
{
    ces_heightmap_system::ces_heightmap_system()
    {
        ces_base_system::add_required_component_guid(m_heightmap_components_mask, ces_heightmap_container_component::class_guid());
        ces_base_system::add_required_components_mask(m_heightmap_components_mask);
    }
    
    ces_heightmap_system::~ces_heightmap_system()
    {
        
    }
    
    void ces_heightmap_system::on_feed_start(f32 dt)
    {
        
    }
    
    void ces_heightmap_system::on_feed(const ces_entity_shared_ptr& root, f32 dt)
    {
        ces_base_system::enumerate_entities_with_components(m_heightmap_components_mask, [dt](const ces_entity_shared_ptr& entity) {

        });
    }
    
    void ces_heightmap_system::on_feed_end(f32 dt)
    {
        
    }
}
