//
//  ces_particle_emitter_system.cpp
//  gbCore
//
//  Created by sergey.sergeev on 9/2/15.
//  Copyright (c) 2015 sergey.sergeev. All rights reserved.
//

#include "ces_particle_emitter_system.h"
#include "ces_entity.h"
#include "ces_particle_emitter_component.h"

namespace gb
{
    ces_particle_emitter_system::ces_particle_emitter_system()
    {
        m_type = ces_system_type.ces_system_type_particle_emitter;
    }
    
    ces_particle_emitter_system::~ces_particle_emitter_system()
    {
        
    }
    
    void ces_particle_emitter_system::on_feed_start(f32 deltatime)
    {
        
    }
    
    void ces_particle_emitter_system::on_feed(const ces_entity_shared_ptr& entity, f32 deltatime)
    {
        ces_particle_emitter_component_shared_ptr particle_emitter_component =
        std::static_pointer_cast<ces_particle_emitter_component>(entity->get_component(e_ces_component_type_particle_emitter));
        
        if(particle_emitter_component)
        {
            particle_emitter_component->update(deltatime);
        }
    }
    
    void ces_particle_emitter_system::on_feed_end(f32 deltatime)
    {
        
    }
}