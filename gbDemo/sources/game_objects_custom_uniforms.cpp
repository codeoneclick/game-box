//
//  game_objects_custom_uniforms.cpp
//  gbDemo
//
//  Created by serhii.s on 5/14/19.
//  Copyright © 2019 sergey.sergeev. All rights reserved.
//

#include "game_objects_custom_uniforms.h"
#include "shader.h"

namespace game
{
    const std::string car_shader_uniforms::k_camera_position_uniform = "camera_position";
    const std::string car_shader_uniforms::k_i_view_mat_uniform = "i_view_mat";
    
    car_shader_uniforms::car_shader_uniforms(gb::ces_shader_uniforms_component::e_shader_uniform_type type) : gb::ces_shader_uniforms_component::shader_uniforms(type)
    {
        m_uniforms[k_camera_position_uniform] = std::make_shared<gb::shader_uniform>(gb::e_uniform_type_vec4);
        m_uniforms[k_i_view_mat_uniform] = std::make_shared<gb::shader_uniform>(gb::e_uniform_type_mat4);
    }
    
    void* car_shader_uniforms::get_values()
    {
        m_values.m_camera_position = m_uniforms[k_camera_position_uniform]->get_vec4();
        m_values.m_i_view_mat = m_uniforms[k_i_view_mat_uniform]->get_mat4();
        return static_cast<void*>(&m_values);
    }
    
    ui32 car_shader_uniforms::get_values_size()
    {
        return sizeof(car_shader_uniforms::values);
    }
};
