//
//  camera_3d.cpp
//  gbCore
//
//  Created by serhii serhiiv on 5/17/17.
//  Copyright © 2017 sergey.sergeev. All rights reserved.
//

#include "camera_3d.h"

namespace gb
{
    camera_3d::camera_3d(f32 fov, f32 near, f32 far, glm::ivec4 viewport, bool is_perspective) :
    m_fov(fov),
    m_near(near),
    m_far(far),
    m_rotation(0.f),
    m_viewport(viewport),
    m_is_matrix_m_computed(false)
    {
        m_aspect = static_cast<f32>(viewport.z) / static_cast<f32>(viewport.w);
        if(is_perspective)
        {
            m_mat_p = glm::perspective(m_fov, m_aspect, m_near, m_far);
        }
        else
        {
            m_mat_p = glm::ortho(static_cast<f32>(viewport.x),
                                 static_cast<f32>(viewport.z),
                                 static_cast<f32>(viewport.w),
                                 static_cast<f32>(viewport.y), 1.f, 1024.f);
        }
        m_up = glm::vec3(0.f, 1.f, 0.f);
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
        m_rotation = rotation;
        m_is_matrix_m_computed = false;
    }
    
    f32 camera_3d::get_rotation() const
    {
        return m_rotation;
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
}
