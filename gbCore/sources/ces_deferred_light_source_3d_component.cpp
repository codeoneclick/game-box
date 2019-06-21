//
//  ces_omni_deferred_light_source_3d_component.cpp
//  gbCore
//
//  Created by serhii.s on 2/27/19.
//  Copyright © 2019 sergey.sergeev. All rights reserved.
//

#include "ces_deferred_light_source_3d_component.h"

namespace gb
{
    const std::string ces_deferred_light_source_3d_component::k_light_ray_length_uniform = "u_light_ray_length";
    const std::string ces_deferred_light_source_3d_component::k_light_position_uniform = "u_light_position";
    const std::string ces_deferred_light_source_3d_component::k_light_direction_uniform = "u_light_direction";
    const std::string ces_deferred_light_source_3d_component::k_light_inner_cutoff_angle_uniform = "u_light_inner_cutoff_angle";
    const std::string ces_deferred_light_source_3d_component::k_light_outer_cutoff_angle_uniform = "u_light_outer_cutoff_angle";
    const std::string ces_deferred_light_source_3d_component::k_light_color_uniform = "u_light_color";
    const std::string ces_deferred_light_source_3d_component::k_camera_position_uniform = "u_camera_position";
    
    void ces_deferred_light_source_3d_component::set_ray_length(f32 ray_length)
    {
        m_ray_length = ray_length;
    }
    
    void ces_deferred_light_source_3d_component::set_direction(const glm::vec3& direction)
    {
        m_direction = direction;
    }
    
    void ces_deferred_light_source_3d_component::set_outer_cutoff_angle(f32 angle)
    {
        m_outer_cutoff_angle = angle;
    }
    
    void ces_deferred_light_source_3d_component::set_inner_cutoff_angle(f32 angle)
    {
        m_inner_cutoff_angle = angle;
    }
    
    void ces_deferred_light_source_3d_component::set_color(const glm::vec4& color)
    {
        m_color = color;
    }
    
    f32 ces_deferred_light_source_3d_component::get_ray_length() const
    {
        return m_ray_length;
    }
    
    glm::vec3 ces_deferred_light_source_3d_component::get_direction() const
    {
        return m_direction;
    }
    
    f32 ces_deferred_light_source_3d_component::get_outer_cutoff_angle() const
    {
        return m_outer_cutoff_angle;
    }
    
    f32 ces_deferred_light_source_3d_component::get_inner_cutoff_angle() const
    {
        return m_inner_cutoff_angle;
    }
    
    glm::vec4 ces_deferred_light_source_3d_component::get_color() const
    {
        return m_color;
    }
};
