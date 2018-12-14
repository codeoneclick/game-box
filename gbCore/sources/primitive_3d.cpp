//
//  primitive_3d.cpp
//  gbCore
//
//  Created by serhii.s on 12/12/18.
//  Copyright © 2018 sergey.sergeev. All rights reserved.
//

#include "primitive_3d.h"
#include "ces_material_component.h"
#include "ces_geometry_3d_component.h"
#include "ces_transformation_component.h"
#include "mesh_3d.h"

namespace gb
{
    primitive_3d::primitive_3d()
    {
        
#if USED_GRAPHICS_API != NO_GRAPHICS_API
        
        ces_entity::add_deferred_component_constructor<ces_material_component>();
        
#endif
        
        ces_entity::add_deferred_component_constructor<ces_geometry_3d_component>();
        
        min_bound.getter([=]() {
            auto geometry_component = ces_entity::get_component<ces_geometry_3d_component>();
            auto mesh = std::static_pointer_cast<mesh_3d>(geometry_component->get_mesh());
            return mesh->get_min_bound();
        });
        
        max_bound.getter([=]() {
            auto geometry_component = ces_entity::get_component<ces_geometry_3d_component>();
            auto mesh = std::static_pointer_cast<mesh_3d>(geometry_component->get_mesh());
            return mesh->get_max_bound();
        });
    }
    
    primitive_3d::~primitive_3d()
    {
        
    }
    
    void primitive_3d::setup_components()
    {
        auto transformation_component = ces_entity::get_component<ces_transformation_component>();
        transformation_component->set_is_in_camera_space(false);
    }
}
