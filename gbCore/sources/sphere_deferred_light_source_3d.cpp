//
//  sphere_deferred_light_source_3d.cpp
//  gbCore
//
//  Created by serhii.s on 3/22/19.
//  Copyright © 2019 sergey.sergeev. All rights reserved.
//

#include "sphere_deferred_light_source_3d.h"
#include "ces_geometry_3d_component.h"
#include "mesh_constructor.h"

namespace gb
{
    sphere_deferred_light_source_3d::sphere_deferred_light_source_3d()
    {
        
    }
    
    sphere_deferred_light_source_3d::~sphere_deferred_light_source_3d()
    {
        
    }
    
    void sphere_deferred_light_source_3d::setup_components()
    {
        deferred_light_source_3d::setup_components();
        const auto geometry_component = ces_entity::get_component<ces_geometry_3d_component>();
        geometry_component->set_mesh(mesh_constructor::create_sphere());
    }
}
