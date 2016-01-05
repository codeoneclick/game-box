//
//  shadow_cast_light.cpp
//  gbCore
//
//  Created by sergey.sergeev on 8/17/15.
//  Copyright (c) 2015 sergey.sergeev. All rights reserved.
//

#include "shadow_cast_light.h"

namespace gb
{
    shadow_cast_light::shadow_cast_light(f32 fov, f32 near, f32 far) :
    m_position(glm::vec3(0.f)),
    m_look_at(glm::vec3(0.f))
    {
        m_matrix_p = glm::perspective(fov, 1.f, near, far);
    }
    
    shadow_cast_light::~shadow_cast_light()
    {
        
    }
    
    glm::vec3 shadow_cast_light::get_position() const
    {
        return m_position;
    }
    
    void shadow_cast_light::set_position(const glm::vec3& position)
    {
        m_position = position;
    }
    
    void shadow_cast_light::set_look_at(const glm::vec3& look_at)
    {
        m_look_at = look_at;
    }
    
    glm::vec3 shadow_cast_light::get_look_at() const
    {
        return m_look_at;
    }
    
    glm::mat4x4 shadow_cast_light::get_matrix_v() const
    {
        return m_matrix_v;
    }
    
    glm::mat4x4 shadow_cast_light::get_matrix_p() const
    {
        return m_matrix_p;
    }
    
    void shadow_cast_light::update(f32 deltatime)
    {
        m_matrix_v = glm::lookAt(m_position, m_look_at, glm::vec3(0.f, 1.f, 0.f));
    }
}