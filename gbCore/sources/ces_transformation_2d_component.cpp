//
//  ces_transformation_2d_component.cpp
//  gbCore
//
//  Created by sergey.sergeev on 8/17/15.
//  Copyright (c) 2015 sergey.sergeev. All rights reserved.
//

#include "ces_transformation_2d_component.h"

namespace gb
{
    const f32 ces_transformation_2d_component::k_z_order_step = .001f;
    
    ces_transformation_2d_component::ces_transformation_2d_component() :
    m_is_matrix_m_computed(false),
    m_is_in_camera_space(true),
    m_matrix_m_version(0),
    m_absolute_matrix_version(0),
    m_absolute_matrix_m(1.f)
    {
        ces_transformation_2d_component::set_position(glm::vec2(0.f));
        ces_transformation_2d_component::set_rotation(0.f);
        ces_transformation_2d_component::set_scale(glm::vec2(1.f));
        ces_transformation_2d_component::set_z_order(0.f);
    }
    
    ces_transformation_2d_component::~ces_transformation_2d_component()
    {
        
    }
    
    void ces_transformation_2d_component::set_position(const glm::vec2& position)
    {
        m_position = position;
        m_matrix_t = glm::translate(glm::mat4(1.f), glm::vec3(m_position.x, m_position.y, m_z_order));
        m_is_matrix_m_computed = false;
    }
    
    void ces_transformation_2d_component::set_rotation(f32 rotation)
    {
        m_rotation = rotation;
        m_matrix_r = glm::rotate(glm::mat4(1.f), m_rotation, glm::vec3(0.f, 0.f, 1.f));
        m_is_matrix_m_computed = false;
    }
    
    void ces_transformation_2d_component::set_scale(const glm::vec2& scale)
    {
        m_scale = scale;
        m_matrix_s = glm::scale(glm::mat4(1.f), glm::vec3(m_scale.x, m_scale.y, 1.f));
        m_is_matrix_m_computed = false;
    }
    
    void ces_transformation_2d_component::set_z_order(f32 z_order)
    {
        m_z_order = z_order;
        m_matrix_t = glm::translate(glm::mat4(1.f), glm::vec3(m_position.x, m_position.y, m_z_order));
        m_is_matrix_m_computed = false;
    }
    
    glm::vec2 ces_transformation_2d_component::get_position() const
    {
        return m_position;
    }
    
    f32 ces_transformation_2d_component::get_rotation() const
    {
        return m_rotation;
    }
    
    glm::vec2 ces_transformation_2d_component::get_scale() const
    {
        return m_scale;
    }
    
    f32 ces_transformation_2d_component::get_z_order() const
    {
        return m_z_order;
    }
    
    glm::mat4 ces_transformation_2d_component::get_matrix_m()
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
    
    void ces_transformation_2d_component::set_custom_matrix_m(const glm::mat4& matrix_m)
    {
        m_matrix_m = matrix_m;
        m_is_matrix_m_computed = true;
        m_matrix_m_version++;
        m_absolute_matrix_version++;
    }
    
    void ces_transformation_2d_component::set_is_in_camera_space(bool value)
    {
        m_is_in_camera_space = value;
    }
    
    bool ces_transformation_2d_component::is_in_camera_space() const
    {
        return m_is_in_camera_space;
    }
    
    ui32 ces_transformation_2d_component::get_matrix_m_version() const
    {
        return m_matrix_m_version;
    }
    
    ui32 ces_transformation_2d_component::get_absolute_matrix_version() const
    {
        return m_absolute_matrix_version;
    }
    
    void ces_transformation_2d_component::update_absolute_transformation(const glm::mat4& parent_mat_m)
    {
        m_absolute_matrix_m = parent_mat_m * ces_transformation_2d_component::get_matrix_m();
        m_absolute_matrix_version++;
    }
    
    glm::mat4 ces_transformation_2d_component::get_absolute_transformation()
    {
        return m_absolute_matrix_m;
    }
}
