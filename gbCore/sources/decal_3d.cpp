//
//  decal_3d.cpp
//  gbCore
//
//  Created by serhii.s on 7/26/19.
//  Copyright © 2019 sergey.sergeev. All rights reserved.
//

#include "decal_3d.h"
#include "ces_material_component.h"
#include "ces_geometry_3d_component.h"
#include "ces_transformation_component.h"
#include "mesh_3d.h"

namespace gb
{
    decal_3d::decal_3d()
    {
        
#if USED_GRAPHICS_API != NO_GRAPHICS_API
        
        ces_entity::add_deferred_component_constructor<ces_material_component>();
        
#endif
        
        ces_entity::add_deferred_component_constructor<ces_geometry_3d_component>();
    }
}
