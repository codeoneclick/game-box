//
//  light.cpp
//  gbCore
//
//  Created by sergey.sergeev on 1/8/16.
//  Copyright © 2016 sergey.sergeev. All rights reserved.
//

#include "light.h"
#include "ces_geometry_freeform_component.h"
#include "mesh_constructor.h"

namespace gb
{
    light::light()
    {
        ces_geometry_component_shared_ptr geometry_component = std::make_shared<ces_geometry_freeform_component>();
        ces_entity::add_component(geometry_component);
        unsafe_get_geometry_freeform_component_from_this->set_mesh(mesh_constructor::create_circle());
    }
    
    light::~light()
    {
        
    }
}