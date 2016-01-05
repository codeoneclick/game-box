//
//  ocean.cpp
//  gbCore
//
//  Created by sergey.sergeev on 10/13/15.
//  Copyright © 2015 sergey.sergeev. All rights reserved.
//

#include "ocean.h"
#include "ces_geometry_component.h"
#include "ces_ocean_component.h"

namespace gb
{
    ocean::ocean()
    {
        ces_geometry_component_shared_ptr geometry_component = std::make_shared<ces_geometry_component>();
        ces_entity::add_component(geometry_component);
        
        ces_ocean_component_shared_ptr ocean_component = std::make_shared<ces_ocean_component>();
        ces_entity::add_component(ocean_component);
    }
    
    ocean::~ocean()
    {
        
    }
    
    void ocean::set_wave_generator_interval(f32 wave_generator_interval)
    {
        unsafe_get_ocean_component_from_this->set_wave_generator_interval(wave_generator_interval);
    }
    
    void ocean::set_mesh(const mesh_shared_ptr& mesh)
    {
        unsafe_get_geometry_component_from_this->set_mesh(mesh);
    }
}