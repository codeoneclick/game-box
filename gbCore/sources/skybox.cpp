//
//  skybox.cpp
//  gbCore
//
//  Created by sergey.sergeev on 10/13/15.
//  Copyright © 2015 sergey.sergeev. All rights reserved.
//

#include "skybox.h"
#include "ces_geometry_component.h"
#include "ces_skybox_component.h"

namespace gb
{
    skybox::skybox()
    {
        ces_geometry_component_shared_ptr geometry_component = std::make_shared<ces_geometry_component>();
        ces_entity::add_component(geometry_component);
        
        ces_skybox_component_shared_ptr skybox_component = std::make_shared<ces_skybox_component>();
        ces_entity::add_component(skybox_component);
    }
    
    skybox::~skybox()
    {
        
    }
    
    void skybox::set_mesh(const mesh_shared_ptr& mesh)
    {
        unsafe_get_geometry_component_from_this->set_mesh(mesh);
    }
}