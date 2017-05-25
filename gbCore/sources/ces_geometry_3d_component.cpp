//
//  ces_geometry_3d_component.cpp
//  gbCore
//
//  Created by serhii serhiiv on 5/16/17.
//  Copyright © 2017 sergey.sergeev. All rights reserved.
//

#include "ces_geometry_3d_component.h"
#include "mesh_3d.h"

namespace gb
{
    ces_geometry_3d_component::ces_geometry_3d_component()
    {
        
    }
    
    ces_geometry_3d_component::~ces_geometry_3d_component()
    {
        
    }
    
    void ces_geometry_3d_component::set_mesh(const mesh_3d_shared_ptr& mesh)
    {
        m_mesh = mesh;
    }
}
        
