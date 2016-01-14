//
//  ces_geometry_freeform_component.cpp
//  gbCore
//
//  Created by sergey.sergeev on 1/6/16.
//  Copyright © 2016 sergey.sergeev. All rights reserved.
//

#include "ces_geometry_freeform_component.h"
#include "game_commands_container.h"

namespace gb
{
    ces_geometry_freeform_component::ces_geometry_freeform_component()
    {
        m_type = e_ces_component_type_geometry;
    }
    
    ces_geometry_freeform_component::~ces_geometry_freeform_component()
    {
        
    }
    
    void ces_geometry_freeform_component::set_mesh(const mesh_shared_ptr &mesh)
    {
        m_mesh = mesh;
        m_event_listeners->execute<geometry_on_mesh_updated::t_command>(geometry_on_mesh_updated::guid);
    }
}