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
    const std::string ces_deferred_light_source_3d_component::k_mat_i_vp_uniform = "u_mat_i_vp";
    const std::string ces_deferred_light_source_3d_component::k_frame_size_uniform = "u_frame_size";
    const std::string ces_deferred_light_source_3d_component::k_ray_length_uniform = "u_ray_length";
    const std::string ces_deferred_light_source_3d_component::k_center_uniform = "u_center";
    const std::string ces_deferred_light_source_3d_component::k_color_uniform = "u_color";
    const std::string ces_deferred_light_source_3d_component::k_camera_position_uniform = "u_camera_position";
    
    void ces_deferred_light_source_3d_component::set_ray_length(f32 ray_length)
    {
        m_ray_length = ray_length;
    }
    
    void ces_deferred_light_source_3d_component::set_color(const glm::vec4& color)
    {
        m_color = color;
    }
    
    f32 ces_deferred_light_source_3d_component::get_ray_length() const
    {
        return m_ray_length;
    }
    
    glm::vec4 ces_deferred_light_source_3d_component::get_color() const
    {
        return m_color;
    }
};
