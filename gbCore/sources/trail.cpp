//
//  trail.cpp
//  gbCore
//
//  Created by serhii.s on 3/28/19.
//  Copyright © 2019 sergey.sergeev. All rights reserved.
//

#include "trail.h"
#include "ces_material_component.h"
#include "ces_geometry_3d_component.h"
#include "ces_transformation_component.h"
#include "ces_trail_component.h"
#include "mesh_3d.h"

namespace gb
{
    trail::trail()
    {
        
#if USED_GRAPHICS_API != NO_GRAPHICS_API
        
        ces_entity::add_deferred_component_constructor<ces_material_component>();
        
#endif
        
        ces_entity::add_deferred_component_constructor<ces_geometry_3d_component>();
        ces_entity::add_deferred_component_constructor<ces_trail_component>();
    }
    
    trail::~trail()
    {
        
    }
    
    void trail::setup_components()
    {
        game_object_3d::setup_components();
    }
}
