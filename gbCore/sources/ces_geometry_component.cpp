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
    ces_geometry_component::ces_geometry_component() :
    m_mesh(nullptr),
    m_bounds(0.f),
    m_bounds_version(-1)
    {

    }
    
    ces_geometry_component::~ces_geometry_component()
    {
        
    }
    
    mesh_2d_shared_ptr ces_geometry_component::get_mesh() const
    {
        return m_mesh;
    }
    
    void ces_geometry_component::set_bounds(const glm::vec4& bounds, ui32 bounds_version)
    {
        m_bounds = bounds;
        m_bounds_version = bounds_version;
    }
    
    glm::vec4 ces_geometry_component::get_bounds() const
    {
        return m_bounds;
    }
    
    ui32 ces_geometry_component::get_bounds_version() const
    {
        return m_bounds_version;
    }
}
