//
//  ces_transformation_3d_component.cpp
//  gbCore
//
//  Created by serhii serhiiv on 5/15/17.
//  Copyright © 2017 sergey.sergeev. All rights reserved.
//

#include "ces_transformation_3d_component.h"

namespace gb
{
    ces_transformation_3d_component::ces_transformation_3d_component()
    {
        ces_transformation_3d_component::set_position(glm::vec3(0.f));
        ces_transformation_3d_component::set_rotation(glm::vec3(0.f));
        ces_transformation_3d_component::set_scale(glm::vec3(1.f));
    }
    
    ces_transformation_3d_component::~ces_transformation_3d_component()
    {
        
    }
    
    void ces_transformation_3d_component::set_position(const glm::vec3& position)
    {
        m_position = position;
        m_matrix_t = glm::translate(glm::mat4(1.f), glm::vec3(m_position.x, m_position.y, position.z));
        m_is_matrix_m_computed = false;
    }
    
    void ces_transformation_3d_component::set_rotation(const glm::vec3& rotation)
    {
        m_rotation = rotation;
        m_matrix_r = glm::rotate(glm::mat4(1.0f), m_rotation.x, glm::vec3(1.f, 0.f, 0.f));
        m_matrix_r = glm::rotate(m_matrix_r, m_rotation.z, glm::vec3(0.f, 0.f, 1.f));
        m_matrix_r = glm::rotate(m_matrix_r, m_rotation.y, glm::vec3(0.f, 1.f, 0.f));
        m_is_matrix_m_computed = false;
    }
    
    void ces_transformation_3d_component::set_scale(const glm::vec3& scale)
    {
        m_scale = scale;
        m_matrix_s = glm::scale(glm::mat4(1.f), glm::vec3(m_scale.x, m_scale.y, m_scale.z));
        m_is_matrix_m_computed = false;
    }
    
    glm::vec3 ces_transformation_3d_component::get_position() const
    {
        return m_position;
    }
    
    glm::vec3 ces_transformation_3d_component::get_rotation() const
    {
        return m_rotation;
    }
    
    glm::vec3 ces_transformation_3d_component::get_scale() const
    {
        return m_scale;
    }
}
