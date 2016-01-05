//
//  ces_render_component.cpp
//  gbCore
//
//  Created by sergey.sergeev on 8/17/15.
//  Copyright (c) 2015 sergey.sergeev. All rights reserved.
//

#include "ces_render_component.h"
#include "texture.h"
#include "mesh.h"
#include "camera.h"
#include "shadow_cast_light.h"
#include "scene_graph_parameters.h"

namespace gb
{
    ces_render_component::ces_render_component() :
    m_bind_material_imposer_callback(nullptr),
    m_visible(true)
    {
        m_type = e_ces_component_type_render;
    }
    
    ces_render_component::~ces_render_component()
    {
        m_materials.clear();
    }
    
    void ces_render_component::add_material(const std::string& technique_name, i32 technique_pass, const std::shared_ptr<material>& material)
    {
        m_materials[technique_name][technique_pass] = material;
    }
    
    void ces_render_component::remove_material(const std::string& technique_name, i32 technique_pass)
    {
        const auto& iterator_01 = m_materials.find(technique_name);
        if(iterator_01 != m_materials.end())
        {
            const auto& iterator_02 = iterator_01->second.find(technique_pass);
            if(iterator_02 != iterator_01->second.end())
            {
                m_materials[technique_name].erase(iterator_02);
            }
        }
    }
    
    material_shared_ptr ces_render_component::get_material(const std::string& technique_name, i32 technique_pass) const
    {
        material_shared_ptr material = nullptr;
        const auto& iterator_01 = m_materials.find(technique_name);
        if(iterator_01 != m_materials.end())
        {
            const auto& iterator_02 = iterator_01->second.find(technique_pass);
            if(iterator_02 != iterator_01->second.end())
            {
                material = iterator_02->second;
            }
        }
        return material;
    }
    
    const std::unordered_map<std::string, std::unordered_map<i32, material_shared_ptr>>& ces_render_component::get_materials() const
    {
        return m_materials;
    }
    
    void ces_render_component::set_visible(bool value)
    {
        m_visible = value;
    }
    
    bool ces_render_component::get_visible() const
    {
        return m_visible;
    }
    
    void ces_render_component::set_texture(const std::shared_ptr<texture> &texture, e_shader_sampler sampler, const std::string& technique_name, i32 technique_pass)
    {
        if(technique_name.length() != 0)
        {
            if(technique_pass != -1)
            {
                material_shared_ptr material = ces_render_component::get_material(technique_name, technique_pass);
                assert(material);
                material->set_texture(texture, sampler);
            }
            else
            {
                const auto& iterator = m_materials.find(technique_name);
                for(const auto& material : iterator->second)
                {
                    material.second->set_texture(texture, sampler);
                }
            }
        }
        else
        {
            for(const auto& iterator : m_materials)
            {
                for(const auto& material : iterator.second)
                {
                    material.second->set_texture(texture, sampler);
                }
            }
        }
    }
    
    void ces_render_component::bind_custom_shader_uniforms(const material_shared_ptr& material)
    {
        assert(material != nullptr);
        std::map<std::string, std::shared_ptr<shader_uniform>> custom_shader_uniforms = material->get_custom_uniforms();
        std::shared_ptr<shader_uniform> current_uniform = nullptr;
        for(const auto& iterator : custom_shader_uniforms)
        {
            current_uniform = iterator.second;
            assert(current_uniform != nullptr);
            switch (current_uniform->get_type())
            {
                case e_uniform_type_mat4:
                {
                    material->get_shader()->set_custom_mat4(current_uniform->get_mat4(), iterator.first);
                }
                    break;
                    
                case e_uniform_type_mat4_array:
                {
                    material->get_shader()->set_custom_mat4_array(current_uniform->get_mat4_array(),
                                                                  current_uniform->get_array_size(),
                                                                  iterator.first);
                }
                    break;
                    
                case e_uniform_type_mat3:
                {
                    material->get_shader()->set_custom_mat3(current_uniform->get_mat3(), iterator.first);
                }
                    break;
                    
                case e_uniform_type_vec4:
                {
                    material->get_shader()->set_custom_vec4(current_uniform->get_vec4(), iterator.first);
                }
                    break;
                    
                case e_uniform_type_vec4_array:
                {
                    material->get_shader()->set_custom_vec4_array(current_uniform->get_vec4_array(),
                                                                  current_uniform->get_array_size(),
                                                                  iterator.first);
                }
                    break;
                    
                case e_uniform_type_vec3:
                {
                    material->get_shader()->set_custom_vec3(current_uniform->get_vec3(), iterator.first);
                }
                    break;
                    
                case e_uniform_type_vec2:
                {
                    material->get_shader()->set_custom_vec2(current_uniform->get_vec2(), iterator.first);
                }
                    break;
                    
                case e_uniform_type_f32:
                {
                    material->get_shader()->set_custom_f32(current_uniform->get_f32(), iterator.first);
                }
                    break;
                    
                case e_uniform_type_i32:
                {
                    material->get_shader()->set_custom_i32(current_uniform->get_i32(), iterator.first);
                }
                    break;
                    
                default:
                    break;
            }
        }
    }
    
    void ces_render_component::bind_main_shader_uniforms(const material_shared_ptr& material)
    {
        scene_graph_parameters_shared_ptr scene_graph_parameters = ces_base_component::get_scene_graph_parameters();
        
        glm::mat4 matrix_p = material->is_shadowing() ? scene_graph_parameters->get_shadow_cast_light_matrix_p() :
        scene_graph_parameters->get_eye_matrix_p();
        
        glm::mat4 matrix_v;
        
        if(material->is_reflecting())
        {
            matrix_v = scene_graph_parameters->get_eye_matrix_i_v();
        }
        else if(material->is_shadowing())
        {
            matrix_v = scene_graph_parameters->get_shadow_cast_light_matrix_v();
        }
        else
        {
            matrix_v = scene_graph_parameters->get_eye_matrix_v();
        }
        
        material->get_shader()->set_mat4(matrix_p, e_shader_uniform_mat_p);
        material->get_shader()->set_mat4(matrix_v, e_shader_uniform_mat_v);
        material->get_shader()->set_mat4(scene_graph_parameters->get_eye_matrix_n(), e_shader_uniform_mat_n);
        material->get_shader()->set_mat4(scene_graph_parameters->get_eye_matrix_i_vp(), e_shader_uniform_mat_i_vp);

        material->get_shader()->set_vec3(scene_graph_parameters->get_eye_position(), e_shader_uniform_vec_camera_position);
        material->get_shader()->set_f32(scene_graph_parameters->get_eye_near(), e_shader_uniform_f32_camera_near);
        material->get_shader()->set_f32(scene_graph_parameters->get_eye_far(), e_shader_uniform_f32_camera_far);
        material->get_shader()->set_vec4(material->get_clipping_plane(), e_shader_uniform_vec_clip);
        
        material->get_shader()->set_vec3(scene_graph_parameters->get_shadow_cast_light_position(), e_shader_uniform_vec_global_light_position);
        material->get_shader()->set_mat4(scene_graph_parameters->get_shadow_cast_light_matrix_p(), e_shader_uniform_mat_global_light_p);
        material->get_shader()->set_mat4(scene_graph_parameters->get_shadow_cast_light_matrix_v(), e_shader_uniform_mat_global_light_v);
    }
    
    void ces_render_component::on_bind(const std::string& technique_name, i32 technique_pass,
                                       const material_shared_ptr& material)
    {
        material_shared_ptr using_material = material;
        if(!using_material)
        {
            using_material = ces_render_component::get_material(technique_name, technique_pass);
        }
        assert(using_material);
        assert(using_material->get_shader()->is_commited());
        
        if(m_bind_material_imposer_callback)
        {
            m_bind_material_imposer_callback(using_material);
        }
        
        using_material->bind();
        
        ces_render_component::bind_main_shader_uniforms(using_material);
        ces_render_component::bind_custom_shader_uniforms(using_material);
    }
    
    void ces_render_component::on_unbind(const std::string& technique_name, i32 technique_pass,
                                         const material_shared_ptr& material)
    {
        material_shared_ptr using_material = material;
        if(!using_material)
        {
            using_material = ces_render_component::get_material(technique_name, technique_pass);
        }
        assert(using_material);
        using_material->unbind();
    }
    
    void ces_render_component::set_bind_material_imposer_callback(const std::function<void(const material_shared_ptr&)>& callback)
    {
        m_bind_material_imposer_callback = callback;
    }

    void ces_render_component::on_draw(const std::string& technique_name, i32 technique_pass,
                                       const mesh_shared_ptr& mesh, const material_shared_ptr& material)
    {
        material_shared_ptr using_material = material;
        if(!using_material)
        {
            using_material = ces_render_component::get_material(technique_name, technique_pass);
        }
        assert(using_material);
        assert(using_material->get_shader()->is_commited());
        
        assert(mesh->is_commited());
        
        mesh->bind(using_material->get_shader()->get_guid(), using_material->get_shader()->get_attributes());
        mesh->draw();
        mesh->unbind(using_material->get_shader()->get_guid(), using_material->get_shader()->get_attributes());
    }
}