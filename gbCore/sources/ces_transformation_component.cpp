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
    static auto k_ONE_MATRIX = glm::mat4(1.f);
    
    ces_transformation_component::ces_transformation_component() :
    m_is_matrix_m_computed(false),
    m_matrix_m_version(0),
    m_absolute_matrix_version(0),
    m_absolute_matrix_m(1.f),
    m_is_in_camera_space(true),
    m_mode(e_mode_unknown)
    {
        m_parent_matrix_m = k_ONE_MATRIX;
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
    
    bool ces_transformation_component::is_2d() const
    {
        return m_mode == e_mode_2d;
    }
    
    bool ces_transformation_component::is_3d() const
    {
        return m_mode == e_mode_3d;
    }
    
    glm::mat4 ces_transformation_component::get_matrix_m()
    {
        if(!m_is_matrix_m_computed)
        {
            m_matrix_m = m_matrix_t * m_matrix_r * m_matrix_s;
            m_is_matrix_m_computed = true;
            m_matrix_m_version++;
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
        m_parent_matrix_m = parent_mat_m;
        m_is_absolute_matrix_m_computed = false;
    }
    
    glm::mat4 ces_transformation_component::get_absolute_transformation()
    {
        if (!m_is_absolute_matrix_m_computed || !m_is_matrix_m_computed)
        {
            m_absolute_matrix_m = m_parent_matrix_m * ces_transformation_component::get_matrix_m();
            m_is_absolute_matrix_m_computed = true;
            m_absolute_matrix_version++;
        }
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
