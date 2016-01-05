//
//  ces_camera_component.cpp
//  gbCore
//
//  Created by sergey.sergeev on 12/18/15.
//  Copyright © 2015 sergey.sergeev. All rights reserved.
//

#include "ces_camera_component.h"
#include "frustum.h"

namespace gb
{
    ces_camera_component::ces_camera_component() :
    m_up(0.f, 1.f, 0.f),
    m_horizontal_angle(M_PI),
    m_vertical_angle(0.f),
    m_frustum(std::make_shared<frustum>())
    {
        m_type = e_ces_component_type_camera;
    }
    
    ces_camera_component::~ces_camera_component()
    {
        
    }
    
    void ces_camera_component::update(f32 deltatime)
    {
        m_matrix_v = glm::lookAt(m_position, m_look_at, m_up);
        
        glm::vec3 inv_position = m_position;
        inv_position.y = -inv_position.y + 1.f;
        glm::vec3 inv_look_at = m_look_at;
        inv_look_at.y = -inv_look_at.y + 1.f;
        m_matrix_i_v = glm::lookAt(inv_position, inv_look_at, m_up * -1.f);
        
        m_matrix_n = glm::inverse(m_matrix_v);
        m_matrix_n = glm::transpose(m_matrix_n);
        
        m_matrix_i_vp = glm::inverse(m_matrix_p * m_matrix_v);
        
        m_frustum->update(m_fov, m_aspect, m_near, m_far, m_position, m_up, m_look_at);
    }
    
    glm::mat4x4 ces_camera_component::get_matrix_c(const glm::vec3& position)
    {
        glm::vec3 direction = m_position - position;
        direction = glm::normalize(direction);
        
        glm::vec3 up = glm::vec3(0.f, 1.f, 0.f);
        glm::vec3 right = glm::cross(direction, up);
        right = glm::normalize(right);
        direction = glm::cross(right, direction);
        
        glm::mat4x4 matrix_c;
        matrix_c[0][0] = right.x;
        matrix_c[0][1] = right.y;
        matrix_c[0][2] = right.z;
        matrix_c[0][3] = 0.f;
        matrix_c[1][0] = up.x;
        matrix_c[1][1] = up.y;
        matrix_c[1][2] = up.z;
        matrix_c[1][3] = 0.f;
        matrix_c[2][0] = direction.x;
        matrix_c[2][1] = direction.y;
        matrix_c[2][2] = direction.z;
        matrix_c[2][3] = 0.f;
        
        matrix_c[3][0] = position.x;
        matrix_c[3][1] = position.y;
        matrix_c[3][2] = position.z;
        matrix_c[3][3] = 1.f;
        
        return matrix_c;
    }
    
    glm::mat4x4 ces_camera_component::get_matrix_s(const glm::vec3& position)
    {
        glm::vec3 direction = position - m_position;
        direction = glm::normalize(direction);
        
        glm::vec3 up = glm::vec3(m_matrix_v[1][0],
                                 m_matrix_v[1][1],
                                 m_matrix_v[1][2]);
        up = glm::normalize(up);
        
        glm::vec3 right = glm::cross(direction, up);
        right = glm::normalize(right);
        
        up = glm::cross(direction, right);
        up = glm::normalize(up);
        
        glm::mat4x4 matrix_s;
        matrix_s[0][0] = right.x;
        matrix_s[0][1] = right.y;
        matrix_s[0][2] = right.z;
        matrix_s[0][3] = 0.f;
        matrix_s[1][0] = up.x;
        matrix_s[1][1] = up.y;
        matrix_s[1][2] = up.z;
        matrix_s[1][3] = 0.f;
        matrix_s[2][0] = direction.x;
        matrix_s[2][1] = direction.y;
        matrix_s[2][2] = direction.z;
        matrix_s[2][3] = 0.f;
        
        matrix_s[3][0] = position.x;
        matrix_s[3][1] = position.y;
        matrix_s[3][2] = position.z;
        matrix_s[3][3] = 1.f;
        
        return matrix_s;
    }
}