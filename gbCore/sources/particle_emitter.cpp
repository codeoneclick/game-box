//
//  particle_emitter.cpp
//  gbCore
//
//  Created by sergey.sergeev on 9/2/15.
//  Copyright (c) 2015 sergey.sergeev. All rights reserved.
//

#include "particle_emitter.h"
#include "ces_particle_emitter_component.h"
#include "ces_render_component.h"

namespace gb
{
    particle_emitter::particle_emitter()
    {
        ces_particle_emitter_component_shared_ptr particle_emitter_component = std::make_shared<ces_particle_emitter_component>();
        ces_entity::add_component(particle_emitter_component);
    }
    
    particle_emitter::~particle_emitter()
    {
        
    }
    
    void particle_emitter::set_settings(const std::shared_ptr<particle_emitter_configuration>& settings)
    {
        unsafe_get_particle_emitter_component_from_this->set_settings(settings);
    }
}