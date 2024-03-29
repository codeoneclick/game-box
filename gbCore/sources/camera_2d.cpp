//
//  camera_2d.cpp
//  gbCore
//
//  Created by sergey.sergeev on 1/11/16.
//  Copyright © 2016 sergey.sergeev. All rights reserved.
//

#include "camera_2d.h"

namespace gb
{
    camera_2d::camera_2d(i32 width, i32 height) :
    m_is_matrix_m_computed(false),
    m_viewport_size(glm::ivec2(width, height)),
    m_pivot(glm::vec2(.5f))
    {
        m_mat_p = glm::ortho(0.f,
                             static_cast<f32>(width),
                             static_cast<f32>(height),
                             0.f, -1.f, 1.f);
        
        camera_2d::set_position(glm::vec2(0.f));
        camera_2d::set_zoom(1.f);
        camera_2d::set_rotation(0.f);
        
        viewport_size.getter([=]() {
            return m_viewport_size;
        });
        bound.getter([=]() {
            return glm::vec4(-m_position,
                             glm::vec2(-m_position.x + m_viewport_size.x,
                                       -m_position.y + m_viewport_size.y));
        });
        
        pivot.getter([=]() {
            return m_pivot;
        });
        
        pivot.setter([=](const glm::vec2& pivot) {
            m_pivot = pivot;
        });
        
        camera_2d::set_rotation(0.f);
    }
    
    camera_2d::~camera_2d()
    {
        
    }
    
    void camera_2d::set_position(const glm::vec2& position)
    {
        m_position_cache = position;
        m_position = position;
        m_position.x += m_viewport_size.x * m_pivot.x;
        m_position.y += m_viewport_size.y * m_pivot.y;

        m_mat_t = glm::translate(glm::mat4(1.f), glm::vec3(m_position, 0.f));
        m_is_matrix_m_computed = false;
    }
    
    glm::vec2 camera_2d::get_position() const
    {
        return m_position_cache;
    }
    
    void camera_2d::set_zoom(f32 zoom)
    {
        m_zoom = zoom;
        m_mat_s = glm::scale(glm::mat4(1.f), glm::vec3(m_zoom, m_zoom, 1.f));
        m_is_matrix_m_computed = false;
    }
    
    void camera_2d::set_rotation(f32 rotation)
    {
        m_rotation_cache = rotation;
        m_rotation = rotation;
        m_mat_r = glm::translate(glm::mat4(1.f), glm::vec3(m_viewport_size.x * m_pivot.x - m_position.x,
                                                           m_viewport_size.y * m_pivot.y - m_position.y, 0.f));
        m_mat_r = glm::rotate(m_mat_r, m_rotation, glm::vec3(0.f, 0.f, 1.f));
        m_mat_r = glm::translate(m_mat_r, glm::vec3(-m_viewport_size.x * m_pivot.x + m_position.x,
                                                    -m_viewport_size.y * m_pivot.y + m_position.y, 0.f));
        m_is_matrix_m_computed = false;
    }
    
    f32 camera_2d::get_rotation() const
    {
        return m_rotation_cache;
    }
    
    f32 camera_2d::get_zoom() const
    {
        return m_zoom;
    }
    
    glm::mat4 camera_2d::get_mat_p() const
    {
        return m_mat_p;
    }
    
    glm::mat4 camera_2d::get_mat_v()
    {
        if(!m_is_matrix_m_computed)
        {
            m_mat_v = m_mat_t * m_mat_r * m_mat_s;
            m_is_matrix_m_computed = true;
        }
        return m_mat_v;
    }
}
