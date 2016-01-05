//
//  ces_camera_component.hpp
//  gbCore
//
//  Created by sergey.sergeev on 12/18/15.
//  Copyright © 2015 sergey.sergeev. All rights reserved.
//


namespace gb
{
    inline glm::mat4 ces_camera_component::get_matrix_v() const
    {
        return m_matrix_v;
    }
    
    inline glm::mat4 ces_camera_component::get_matrix_i_v() const
    {
        return m_matrix_i_v;
    }
    
    inline glm::mat4 ces_camera_component::get_matrix_i_vp() const
    {
        return m_matrix_i_vp;
    }
    
    inline glm::mat4 ces_camera_component::get_matrix_p() const
    {
        return m_matrix_p;
    }
    
    inline glm::mat4 ces_camera_component::get_matrix_n() const
    {
        return m_matrix_n;
    }
    
    inline void ces_camera_component::set_position(const glm::vec3& position)
    {
        m_position = position;
    }
    
    inline glm::vec3 ces_camera_component::get_position() const
    {
        return m_position;
    }
    
    inline void ces_camera_component::set_look_at(const glm::vec3& look_at)
    {
        m_look_at = look_at;
    }
    
    inline glm::vec3 ces_camera_component::get_look_at() const
    {
        return m_look_at;
    }
    
    inline void ces_camera_component::set_up(const glm::vec3& up)
    {
        m_up = up;
    }
    
    inline glm::vec3 ces_camera_component::get_up() const
    {
        return m_up;
    }
    
    inline void ces_camera_component::set_fov(f32 fov)
    {
        m_fov = fov;
        m_matrix_p = glm::perspective(m_fov, m_aspect, m_near, m_far);
    }
    
    inline f32 ces_camera_component::get_fov() const
    {
        return m_fov;
    }
    
    inline f32 ces_camera_component::get_aspect() const
    {
        return m_aspect;
    }

    inline f32 ces_camera_component::get_near() const
    {
        return m_near;
    }
    
    inline void ces_camera_component::set_near(f32 near)
    {
        m_near = near;
        m_matrix_p = glm::perspective(m_fov, m_aspect, m_near, m_far);
    }
    
    inline f32 ces_camera_component::get_far() const
    {
        return m_far;
    }
    
    inline void ces_camera_component::set_far(f32 far)
    {
        m_far = far;
        m_matrix_p = glm::perspective(m_fov, m_aspect, m_near, m_far);
    }
    
    inline glm::ivec4 ces_camera_component::get_viewport() const
    {
        return m_viewport;
    }
    
    inline void ces_camera_component::set_viewport(const glm::ivec4 &viewport)
    {
        m_viewport = viewport;
        m_aspect = static_cast<f32>(m_viewport.z) / static_cast<f32>(m_viewport.w);
        m_matrix_p = glm::perspective(m_fov, m_aspect, m_near, m_far);
    }
    
    inline frustum_shared_ptr ces_camera_component::get_frustum() const
    {
        return m_frustum;
    }
    
    inline f32 ces_camera_component::get_yaw() const
    {
        if (m_matrix_v[0][0] == 1.f ||
            m_matrix_v[0][0] == -1.f)
        {
            return atan2f(m_matrix_v[0][2], m_matrix_v[2][3]);
        }
        return atan2(-m_matrix_v[2][0], m_matrix_v[0][0]);
    }
    
    inline f32 ces_camera_component::get_pitch() const
    {
        if (m_matrix_v[0][0] == 1.f ||
            m_matrix_v[0][0] == -1.f)
        {
            return 0.f;
        }
        return asin(-m_matrix_v[1][0]);
    }
    
    inline f32 ces_camera_component::get_roll() const
    {
        if (m_matrix_v[0][0] == 1.f ||
            m_matrix_v[0][0] == -1.f)
        {
            return 0.f;
        }
        return atan2(-m_matrix_v[1][2], m_matrix_v[1][1]);
    }
    
    inline void ces_camera_component::set_horizontal_angle(f32 angle)
    {
        m_horizontal_angle = angle;
    }
    
    inline f32 ces_camera_component::get_horizontal_angle() const
    {
        return m_horizontal_angle;
    }
    
    inline void ces_camera_component::set_vertical_angle(f32 angle)
    {
        m_vertical_angle = angle;
    }
    
    inline f32 ces_camera_component::get_vertical_angle() const
    {
        return m_vertical_angle;
    }
    
    inline glm::vec3 ces_camera_component::get_direction() const
    {
        return m_position - m_look_at;
    }
}
