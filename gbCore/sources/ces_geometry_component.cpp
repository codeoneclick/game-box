//
//  ces_geometry_component.cpp
//  gbCore
//
//  Created by sergey.sergeev on 1/6/16.
//  Copyright © 2016 sergey.sergeev. All rights reserved.
//

#include "ces_geometry_component.h"

namespace gb
{
    CTTI_GUID_CPP(ces_geometry_component)
    ces_geometry_component::ces_geometry_component() :
    m_mesh(nullptr)
    {

    }
    
    ces_geometry_component::~ces_geometry_component()
    {
        
    }
    
    mesh_shared_ptr ces_geometry_component::get_mesh() const
    {
        return m_mesh;
    }
}