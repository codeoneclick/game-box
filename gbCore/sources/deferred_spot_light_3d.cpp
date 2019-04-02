//
//  custom_mesh_deferred_light_source_3d.cpp
//  gbCore
//
//  Created by serhii.s on 3/22/19.
//  Copyright © 2019 sergey.sergeev. All rights reserved.
//

#include "deferred_spot_light_3d.h"
#include "ces_deferred_light_source_3d_component.h"
#include "ces_deferred_light_source_3d_component.h"

namespace gb
{
    deferred_spot_light_3d::shader_uniforms::shader_uniforms(ces_shader_uniforms_component::e_shader_uniform_type type) : ces_shader_uniforms_component::shader_uniforms(type)
    {
        m_uniforms[ces_deferred_light_source_3d_component::k_light_position_uniform] = std::make_shared<shader_uniform>(e_uniform_type_vec3);
        m_uniforms[ces_deferred_light_source_3d_component::k_light_direction_uniform] = std::make_shared<shader_uniform>(e_uniform_type_vec3);
        m_uniforms[ces_deferred_light_source_3d_component::k_light_outer_cutoff_angle_uniform] = std::make_shared<shader_uniform>(e_uniform_type_f32);
        m_uniforms[ces_deferred_light_source_3d_component::k_light_inner_cutoff_angle_uniform] = std::make_shared<shader_uniform>(e_uniform_type_f32);
        m_uniforms[ces_deferred_light_source_3d_component::k_light_color_uniform] = std::make_shared<shader_uniform>(e_uniform_type_vec4);
        m_uniforms[ces_deferred_light_source_3d_component::k_camera_position_uniform] = std::make_shared<shader_uniform>(e_uniform_type_vec4);
    }
    
    void* deferred_spot_light_3d::shader_uniforms::get_values()
    {
        m_values.m_light_position.x = m_uniforms[ces_deferred_light_source_3d_component::k_light_position_uniform]->get_vec3().x;
        m_values.m_light_position.y = m_uniforms[ces_deferred_light_source_3d_component::k_light_position_uniform]->get_vec3().y;
        m_values.m_light_position.z = m_uniforms[ces_deferred_light_source_3d_component::k_light_position_uniform]->get_vec3().z;
        m_values.m_light_position.w = 0.f;
        m_values.m_light_direction.x = m_uniforms[ces_deferred_light_source_3d_component::k_light_direction_uniform]->get_vec3().x;
        m_values.m_light_direction.y = m_uniforms[ces_deferred_light_source_3d_component::k_light_direction_uniform]->get_vec3().y;
        m_values.m_light_direction.z = m_uniforms[ces_deferred_light_source_3d_component::k_light_direction_uniform]->get_vec3().z;
        m_values.m_light_direction.w = 0.f;
        m_values.m_light_cutoff_angles.x = m_uniforms[ces_deferred_light_source_3d_component::k_light_inner_cutoff_angle_uniform]->get_f32();
        m_values.m_light_cutoff_angles.y = m_uniforms[ces_deferred_light_source_3d_component::k_light_outer_cutoff_angle_uniform]->get_f32();
        m_values.m_light_cutoff_angles.z = 0.f;
        m_values.m_light_cutoff_angles.w = 0.f;
        m_values.m_light_color = m_uniforms[ces_deferred_light_source_3d_component::k_light_color_uniform]->get_vec4();
        m_values.m_camera_position = m_uniforms[ces_deferred_light_source_3d_component::k_camera_position_uniform]->get_vec4();
        return static_cast<void*>(&m_values);
    }
    
    ui32 deferred_spot_light_3d::shader_uniforms::get_values_size()
    {
        return sizeof(deferred_spot_light_3d::shader_uniforms::values);
    }
    
    deferred_spot_light_3d::deferred_spot_light_3d()
    {
        direction.setter([=](const glm::vec3& direction) {
            const auto deferred_light_source_3d_component = ces_entity::get_component<ces_deferred_light_source_3d_component>();
            deferred_light_source_3d_component->set_direction(direction);
        });
        direction.getter([=]() {
            const auto deferred_light_source_3d_component = ces_entity::get_component<ces_deferred_light_source_3d_component>();
            return deferred_light_source_3d_component->get_direction();
        });
        
        outer_cutoff_angle.setter([=](f32 angle) {
            const auto deferred_light_source_3d_component = ces_entity::get_component<ces_deferred_light_source_3d_component>();
            deferred_light_source_3d_component->set_outer_cutoff_angle(angle);
        });
        outer_cutoff_angle.getter([=]() {
            const auto deferred_light_source_3d_component = ces_entity::get_component<ces_deferred_light_source_3d_component>();
            return deferred_light_source_3d_component->get_outer_cutoff_angle();
        });
        
        inner_cutoff_angle.setter([=](f32 angle) {
            const auto deferred_light_source_3d_component = ces_entity::get_component<ces_deferred_light_source_3d_component>();
            deferred_light_source_3d_component->set_inner_cutoff_angle(angle);
        });
        inner_cutoff_angle.getter([=]() {
            const auto deferred_light_source_3d_component = ces_entity::get_component<ces_deferred_light_source_3d_component>();
            return deferred_light_source_3d_component->get_inner_cutoff_angle();
        });
    }
    
    deferred_spot_light_3d::~deferred_spot_light_3d()
    {
        
    }
    
    void deferred_spot_light_3d::setup_components()
    {
        deferred_light_source_3d::setup_components();
        
        const auto shader_uniforms_component = ces_entity::get_component<ces_shader_uniforms_component>();
        shader_uniforms_component->construct_uniforms<deferred_spot_light_3d::shader_uniforms>(ces_shader_uniforms_component::e_shader_uniform_type_fragment);
    }
}
