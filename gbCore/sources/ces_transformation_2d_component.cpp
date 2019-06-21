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
    const f32 ces_transformation_2d_component::k_z_order_step = .0001f;
    
    ces_transformation_2d_component::ces_transformation_2d_component()
    {
        m_mode = e_mode_2d;
        ces_transformation_2d_component::set_position(glm::vec2(0.f));
        ces_transformation_2d_component::set_rotation(0.f);
        ces_transformation_2d_component::set_scale(glm::vec2(1.f));
        ces_transformation_2d_component::set_z_order(-1.f);
    }
    
    ces_transformation_2d_component::~ces_transformation_2d_component()
    {
        
    }
    
    void ces_transformation_2d_component::set_position(const glm::vec2& position)
    {
        assert(m_mode == e_mode_2d);
        m_position = position;
        m_matrix_t = glm::translate(glm::mat4(1.f), glm::vec3(m_position.x, m_position.y, m_z_order));
        m_is_matrix_m_computed = false;
    }
    
    void ces_transformation_2d_component::set_rotation(f32 rotation)
    {
        assert(m_mode == e_mode_2d);
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
    
    glm::mat4 ces_transformation_2d_component::get_absolute_transformation()
    {
        ces_transformation_component::get_absolute_transformation();
        m_absolute_matrix_m[3].z = m_z_order;
        return m_absolute_matrix_m;
    }
}
