//
//  ces_geometry_freeform_component.cpp
//  gbCore
//
//  Created by sergey.sergeev on 1/6/16.
//  Copyright © 2016 sergey.sergeev. All rights reserved.
//

#include "ces_geometry_freeform_component.h"

namespace gb
{
    ces_geometry_freeform_component::ces_geometry_freeform_component()
    {

    }
    
    ces_geometry_freeform_component::~ces_geometry_freeform_component()
    {
        
    }
    
    void ces_geometry_freeform_component::set_mesh(const std::shared_ptr<mesh_2d<vertex_attribute>>& mesh)
    {
        m_mesh = mesh;
    }
}
