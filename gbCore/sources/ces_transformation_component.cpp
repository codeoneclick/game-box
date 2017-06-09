//
//  ces_transformation_component.cpp
//  gbCore
//
//  Created by serhii serhiiv on 5/15/17.
//  Copyright © 2017 sergey.sergeev. All rights reserved.
//

#include "ces_transformation_component.h"
#include "ces_transformation_2d_component.h"
#include "ces_transformation_3d_component.h"

namespace gb
{
    ces_transformation_component::ces_transformation_component() :
    m_is_matrix_m_computed(false),
    m_matrix_m_version(0),
    m_absolute_matrix_version(0),
    m_absolute_matrix_m(1.f),
    m_is_in_camera_space(true)
    {

    }
    
    ces_transformation_component::~ces_transformation_component()
    {
        
    }
    
    ces_transformation_2d_component_shared_ptr ces_transformation_component::as_2d()
    {
        return std::static_pointer_cast<ces_transformation_2d_component>(shared_from_this());
    }
    
    ces_transformation_3d_component_shared_ptr ces_transformation_component::as_3d()
    {
        return std::static_pointer_cast<ces_transformation_3d_component>(shared_from_this());
    }
    
    glm::mat4 ces_transformation_component::get_matrix_m()
    {
        if(!m_is_matrix_m_computed)
        {
            m_matrix_m = m_matrix_t * m_matrix_r * m_matrix_s;
            m_is_matrix_m_computed = true;
            m_matrix_m_version++;
            m_absolute_matrix_version++;
        }
        return m_matrix_m;
    }
    
    ui32 ces_transformation_component::get_matrix_m_version() const
    {
        return m_matrix_m_version;
    }
    
    ui32 ces_transformation_component::get_absolute_matrix_version() const
    {
        return m_absolute_matrix_version;
    }
    
    void ces_transformation_component::update_absolute_transformation(const glm::mat4& parent_mat_m)
    {
        m_absolute_matrix_m = parent_mat_m * ces_transformation_component::get_matrix_m();
        m_absolute_matrix_version++;
    }
    
    glm::mat4 ces_transformation_component::get_absolute_transformation()
    {
        return m_absolute_matrix_m;
    }
    
    void ces_transformation_component::set_is_in_camera_space(bool value)
    {
        m_is_in_camera_space = value;
    }
    
    bool ces_transformation_component::is_in_camera_space() const
    {
        return m_is_in_camera_space;
    }
};
