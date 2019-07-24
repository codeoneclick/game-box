//
//  sphere_deferred_light_source_3d.cpp
//  gbCore
//
//  Created by serhii.s on 3/22/19.
//  Copyright © 2019 sergey.sergeev. All rights reserved.
//

#include "deferred_point_light_3d.h"
#include "ces_geometry_3d_component.h"
#include "ces_transformation_3d_component.h"
#include "mesh_constructor.h"
#include "ces_deferred_light_source_3d_component.h"

namespace gb
{
    deferred_point_light_3d::shader_uniforms::shader_uniforms(ces_shader_uniforms_component::e_shader_uniform_mode mode, const std::string& name) : ces_shader_uniforms_component::shader_uniforms(mode, name)
    {
        m_uniforms[ces_deferred_light_source_3d_component::k_light_position_uniform] = std::make_shared<shader_uniform>(e_uniform_type_vec3);
        m_uniforms[ces_deferred_light_source_3d_component::k_light_ray_length_uniform] = std::make_shared<shader_uniform>(e_uniform_type_f32);
        m_uniforms[ces_deferred_light_source_3d_component::k_light_color_uniform] = std::make_shared<shader_uniform>(e_uniform_type_vec4);
        m_uniforms[ces_deferred_light_source_3d_component::k_camera_position_uniform] = std::make_shared<shader_uniform>(e_uniform_type_vec4);
    }
    
    void* deferred_point_light_3d::shader_uniforms::get_values()
    {
        m_values.m_light_position_and_ray_length.x = m_uniforms[ces_deferred_light_source_3d_component::k_light_position_uniform]->get_vec3().x;
        m_values.m_light_position_and_ray_length.y = m_uniforms[ces_deferred_light_source_3d_component::k_light_position_uniform]->get_vec3().y;
        m_values.m_light_position_and_ray_length.z = m_uniforms[ces_deferred_light_source_3d_component::k_light_position_uniform]->get_vec3().z;
        m_values.m_light_position_and_ray_length.w = m_uniforms[ces_deferred_light_source_3d_component::k_light_ray_length_uniform]->get_f32();
        m_values.m_light_color = m_uniforms[ces_deferred_light_source_3d_component::k_light_color_uniform]->get_vec4();
        m_values.m_camera_position = m_uniforms[ces_deferred_light_source_3d_component::k_camera_position_uniform]->get_vec4();
        return static_cast<void*>(&m_values);
    }
    
    ui32 deferred_point_light_3d::shader_uniforms::get_values_size()
    {
        return sizeof(deferred_point_light_3d::shader_uniforms::values);
    }
    
    deferred_point_light_3d::deferred_point_light_3d()
    {
        ray_length.setter([=](f32 ray_length) {
            const auto deferred_light_source_3d_component = ces_entity::get_component<ces_deferred_light_source_3d_component>();
            deferred_light_source_3d_component->set_ray_length(ray_length);
            const auto transformation_component = ces_entity::get_component<ces_transformation_3d_component>();
            transformation_component->set_scale(glm::vec3(ray_length * 2.f));
            const auto geometry_component = ces_entity::get_component<ces_geometry_3d_component>();
            geometry_component->set_bounding_radius(ray_length);
        });
        ray_length.getter([=]() {
            const auto deferred_light_source_3d_component = ces_entity::get_component<ces_deferred_light_source_3d_component>();
            return deferred_light_source_3d_component->get_ray_length();
        });
    }
    
    deferred_point_light_3d::~deferred_point_light_3d()
    {
        
    }
    
    void deferred_point_light_3d::setup_components()
    {
        const auto geometry_component = ces_entity::get_component<ces_geometry_3d_component>();
        geometry_component->set_mesh(mesh_constructor::create_shared_box());
        geometry_component->bounding_mode = ces_geometry_component::e_box;
        
        const auto shader_uniforms_component = ces_entity::get_component<ces_shader_uniforms_component>();
        shader_uniforms_component->construct_uniforms_buffer<deferred_point_light_3d::shader_uniforms>(ces_shader_uniforms_component::e_shader_uniform_mode::e_fragment, "deferred_point_light", 0);
    }
}
