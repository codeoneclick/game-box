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
    m_bounding_box(0.f),
    m_bounds_version(-1)
    {
        bounding_mode.setter([=](e_bounding_mode value) {
            m_bounding_mode = value;
        });
        
        bounding_mode.getter([=]() {
            return m_bounding_mode;
        });
    }
    
    ces_geometry_component::~ces_geometry_component()
    {
        
    }
    
    mesh_2d_shared_ptr ces_geometry_component::get_mesh() const
    {
        return m_mesh;
    }
    
    void ces_geometry_component::set_bounding_box(const glm::vec4& box, ui32 bounds_version)
    {
        m_bounding_box = box;
        m_bounds_version = bounds_version;
    }
    
    glm::vec4 ces_geometry_component::get_bounding_box() const
    {
        return m_bounding_box;
    }
    
    ui32 ces_geometry_component::get_bounds_version() const
    {
        return m_bounds_version;
    }
    
    void ces_geometry_component::set_bounding_radius(f32 radius)
    {
        m_bounding_radius = radius;
    }
    
    f32 ces_geometry_component::get_bounding_radius() const
    {
        return m_bounding_radius;
    }
}
