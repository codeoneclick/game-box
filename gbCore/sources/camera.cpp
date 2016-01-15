//
//  camera.cpp
//  gbCore
//
//  Created by sergey.sergeev on 1/11/16.
//  Copyright © 2016 sergey.sergeev. All rights reserved.
//

#include "camera.h"

namespace gb
{
    camera::camera(i32 width, i32 height) :
    m_is_matrix_m_computed(false),
    m_width(width),
    m_height(height)
    {
        m_mat_p = glm::ortho(0.f,
                             static_cast<f32>(width),
                             static_cast<f32>(height),
                             0.f, -1.f, 1.f);
        
        camera::set_position(glm::vec2(0.f));
        camera::set_zoom(1.f);
        camera::set_rotation(0.f);
    }
    
    camera::~camera()
    {
        
    }
    
    void camera::set_position(const glm::vec2& position)
    {
        m_position = position;
        m_mat_t = glm::translate(glm::mat4(1.f), glm::vec3(m_position.x, m_position.y, 0.f));
        m_is_matrix_m_computed = false;
    }
    
    glm::vec2 camera::get_position() const
    {
        return m_position;
    }
    
    void camera::set_zoom(f32 zoom)
    {
        m_zoom = zoom;
        m_mat_s = glm::scale(glm::mat4(1.f), glm::vec3(m_zoom, m_zoom, 1.f));
        m_is_matrix_m_computed = false;
    }
    
    void camera::set_rotation(f32 rotation)
    {
        m_rotation = rotation;
        m_mat_r = glm::rotate(glm::mat4(1.f), m_rotation, glm::vec3(0.f, 0.f, 1.f));
        m_is_matrix_m_computed = false;
    }
    
    f32 camera::get_rotation() const
    {
        return m_rotation;
    }
    
    f32 camera::get_zoom() const
    {
        return m_zoom;
    }
    
    glm::mat4 camera::get_mat_p() const
    {
        return m_mat_p;
    }
    
    glm::mat4 camera::get_mat_v()
    {
        if(!m_is_matrix_m_computed)
        {
            m_mat_v = m_mat_t * m_mat_r * m_mat_s;
            m_is_matrix_m_computed = true;
        }
        return m_mat_v;
    }
    
    f32 camera::get_width() const
    {
        return m_width;
    }
    
    f32 camera::get_height() const
    {
        return m_height;
    }
}
