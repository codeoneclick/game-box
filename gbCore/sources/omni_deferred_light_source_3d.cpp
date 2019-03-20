//
//  omni_deffered_light_source.cpp
//  gbCore
//
//  Created by serhii.s on 2/26/19.
//  Copyright © 2019 sergey.sergeev. All rights reserved.
//

#include "omni_deferred_light_source_3d.h"
#include "ces_geometry_3d_component.h"
#include "mesh_constructor.h"
#include "ces_transformation_3d_component.h"
#include "ces_material_component.h"
#include "ces_box2d_body_component.h"
#include "ces_transformation_extension.h"
#include "ces_omni_deferred_light_source_3d_component.h"
#include "glm_extensions.h"

namespace gb
{
    static const std::string k_mat_i_vp_uniform = "u_mat_i_vp";
    static const std::string k_frame_size_uniform = "u_frame_size";
    static const std::string k_radius_uniform = "u_radius";
    static const std::string k_center_uniform = "u_center";
    static const std::string k_color_uniform = "u_color";
    
    omni_deferred_light_source_3d::shader_uniforms::shader_uniforms(ces_shader_uniforms_component::e_shader_uniform_type type) : ces_shader_uniforms_component::shader_uniforms(type)
    {
        m_uniforms[k_mat_i_vp_uniform] = std::make_shared<shader_uniform>(e_uniform_type_mat4);
        m_uniforms[k_frame_size_uniform] = std::make_shared<shader_uniform>(e_uniform_type_vec2);
        m_uniforms[k_center_uniform] = std::make_shared<shader_uniform>(e_uniform_type_vec3);
        m_uniforms[k_radius_uniform] = std::make_shared<shader_uniform>(e_uniform_type_f32);
        m_uniforms[k_color_uniform] = std::make_shared<shader_uniform>(e_uniform_type_vec4);
    }
    
    void* omni_deferred_light_source_3d::shader_uniforms::get_values()
    {
        return static_cast<void*>(&m_values);
    }
    
    ui32 omni_deferred_light_source_3d::shader_uniforms::get_values_size()
    {
        return sizeof(omni_deferred_light_source_3d::shader_uniforms::values);
    }
    
    void omni_deferred_light_source_3d::shader_uniforms::set_mat_i_vp(const glm::mat4& mat_i_vp)
    {
        m_values.m_mat_i_vp = mat_i_vp;
        m_uniforms[k_mat_i_vp_uniform]->set_mat4(mat_i_vp);
    }
    
    void omni_deferred_light_source_3d::shader_uniforms::set_frame_size(const glm::vec2& size)
    {
        m_values.m_frame_size = glm::vec4(size.x, size.y, 0.f, 0.f);
        m_uniforms[k_frame_size_uniform]->set_vec2(size);
    }
    
    void omni_deferred_light_source_3d::shader_uniforms::set_center(const glm::vec3& center)
    {
        m_values.m_center_radius.x = center.x;
        m_values.m_center_radius.y = center.y;
        m_values.m_center_radius.z = center.z;
        m_uniforms[k_center_uniform]->set_vec3(center);
    }
    
    void omni_deferred_light_source_3d::shader_uniforms::set_radius(f32 radius)
    {
        m_values.m_center_radius.w = radius;
        m_uniforms[k_radius_uniform]->set_f32(radius);
    }
    
    void omni_deferred_light_source_3d::shader_uniforms::set_color(const glm::vec4& color)
    {
        m_values.m_color = color;
        m_uniforms[k_color_uniform]->set_vec4(color);
    }
    
    omni_deferred_light_source_3d::omni_deferred_light_source_3d()
    {
        
#if USED_GRAPHICS_API != NO_GRAPHICS_API
        
        ces_entity::add_deferred_component_constructor<ces_material_component>();
        
#endif
        
        ces_entity::add_deferred_component_constructor<ces_geometry_3d_component>();
        ces_entity::add_deferred_component_constructor<ces_omni_deferred_light_source_3d_component>();
        ces_entity::add_deferred_component_constructor<ces_shader_uniforms_component>();
        
        radius.setter([=](f32 radius) {
            const auto omni_deferred_light_source_3d_component = ces_entity::get_component<ces_omni_deferred_light_source_3d_component>();
            omni_deferred_light_source_3d_component->set_radius(radius);
            const auto transformation_component = ces_entity::get_component<ces_transformation_3d_component>();
            transformation_component->set_scale(glm::vec3(radius));
        });
        radius.getter([=]() {
            const auto omni_deferred_light_source_3d_component = ces_entity::get_component<ces_omni_deferred_light_source_3d_component>();
            return omni_deferred_light_source_3d_component->get_radius();
        });
        
        color.setter([=](const glm::vec4& color) {
            const auto omni_deferred_light_source_3d_component = ces_entity::get_component<ces_omni_deferred_light_source_3d_component>();
            omni_deferred_light_source_3d_component->set_color(color);
        });
        color.getter([=]() {
            const auto omni_deferred_light_source_3d_component = ces_entity::get_component<ces_omni_deferred_light_source_3d_component>();
            return omni_deferred_light_source_3d_component->get_color();
        });
        
        position.setter([=](const glm::vec3& position) {
            const auto box2d_body_component = ces_entity::get_component<ces_box2d_body_component>();
            if(box2d_body_component && box2d_body_component->is_applied)
            {
                box2d_body_component->position = glm::vec2(position.x, position.z);
            }
            
            const auto transformation_component = ces_entity::get_component<ces_transformation_3d_component>();
            transformation_component->set_position(position);
            ces_transformation_extension::update_absolute_transformation_recursively(shared_from_this());
        });
    }
    
    omni_deferred_light_source_3d::~omni_deferred_light_source_3d()
    {
        
    }
    
    void omni_deferred_light_source_3d::setup_components()
    {
        const auto geometry_component = ces_entity::get_component<ces_geometry_3d_component>();
        geometry_component->set_mesh(mesh_constructor::create_sphere());
        
        const auto shader_uniforms_component = ces_entity::get_component<ces_shader_uniforms_component>();
        shader_uniforms_component->construct_uniforms<omni_deferred_light_source_3d::shader_uniforms>(ces_shader_uniforms_component::e_shader_uniform_type_fragment);
    }
}
