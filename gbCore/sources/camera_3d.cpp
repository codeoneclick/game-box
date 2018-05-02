//
//  camera_3d.cpp
//  gbCore
//
//  Created by serhii serhiiv on 5/17/17.
//  Copyright © 2017 sergey.sergeev. All rights reserved.
//

#include "camera_3d.h"
#include "frustum_3d.h"

namespace gb
{
    camera_3d::camera_3d(f32 _fov, f32 _near, f32 _far, glm::ivec4 _viewport, bool _is_perspective) :
    m_fov(_fov),
    m_near(_near),
    m_far(_far),
    m_rotation(0.f),
    m_viewport(_viewport),
    m_is_matrix_m_computed(false)
    {
        m_aspect = static_cast<f32>(_viewport.z) / static_cast<f32>(_viewport.w);
        if(_is_perspective)
        {
            m_mat_p = glm::perspective(m_fov, m_aspect, m_near, m_far);
        }
        else
        {
            m_mat_p = glm::ortho(static_cast<f32>(_viewport.x),
                                 static_cast<f32>(_viewport.z),
                                 static_cast<f32>(_viewport.w),
                                 static_cast<f32>(_viewport.y), 1.f, 1024.f);
        }
        m_up = glm::vec3(0.f, 1.f, 0.f);
        m_frustum = std::make_shared<frustum_3d>();
    }
    
    camera_3d::~camera_3d()
    {
        
    }
    
    void camera_3d::set_position(const glm::vec3& position)
    {
        m_position = position;
        m_is_matrix_m_computed = false;
    }
    
    glm::vec3 camera_3d::get_position() const
    {
        return m_position;
    }
    
    void camera_3d::set_rotation(f32 rotation)
    {
        m_rotation = glm::radians(rotation);
        m_is_matrix_m_computed = false;
    }
    
    f32 camera_3d::get_rotation() const
    {
        return glm::degrees(m_rotation);
    }
    
    void camera_3d::set_look_at(const glm::vec3& look_at)
    {
        m_look_at = look_at;
        m_is_matrix_m_computed = false;
    }
    
    glm::vec3 camera_3d::get_look_at() const
    {
        return m_look_at;
    }
    
    void camera_3d::set_distance_to_look_at(const glm::vec3& distance)
    {
        m_distance = distance;
        m_is_matrix_m_computed = false;
    }
    
    glm::vec3 camera_3d::get_distance_to_look_at() const
    {
        return m_distance;
    }
    
    glm::mat4 camera_3d::get_mat_p() const
    {
        return m_mat_p;
    }
    
    glm::mat4 camera_3d::get_mat_v()
    {
        if(!m_is_matrix_m_computed)
        {
            m_position.y = m_distance.y;
            m_position.x = m_look_at.x + cosf(-m_rotation) * -m_distance.x;
            m_position.z = m_look_at.z + sinf(-m_rotation) * -m_distance.z;
            m_mat_v = glm::lookAt(m_position, m_look_at, m_up);
        }
        return m_mat_v;
    }
    
    f32 camera_3d::get_fov() const
    {
        return m_fov;
    }
    
    f32 camera_3d::get_aspect() const
    {
        return m_aspect;
    }
    
    f32 camera_3d::get_near() const
    {
        return m_near;
    }
    
    f32 camera_3d::get_far() const
    {
        return m_far;
    }
    
    frustum_3d_shared_ptr camera_3d::get_frustum()
    {
        m_frustum->update(shared_from_this());
        return m_frustum;
    }
}
