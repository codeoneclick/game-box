//
//  custom_mesh_deferred_light_source_3d.cpp
//  gbCore
//
//  Created by serhii.s on 3/22/19.
//  Copyright © 2019 sergey.sergeev. All rights reserved.
//

#include "custom_mesh_deferred_light_source_3d.h"
#include "ces_deferred_light_source_3d_component.h"

namespace gb
{
    custom_mesh_deferred_light_source_3d::custom_mesh_deferred_light_source_3d()
    {
        //ray_length.setter([=](f32 ray_length) {
        //    const auto deferred_light_source_3d_component = ces_entity::get_component<ces_deferred_light_source_3d_component>();
        //    deferred_light_source_3d_component->set_ray_length(ray_length);
        //});
    }
    
    custom_mesh_deferred_light_source_3d::~custom_mesh_deferred_light_source_3d()
    {
        
    }
    
    void custom_mesh_deferred_light_source_3d::setup_components()
    {
        deferred_light_source_3d::setup_components();
    }
}
