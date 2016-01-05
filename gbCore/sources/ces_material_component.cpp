//
//  ces_material_component.cpp
//  gbCore
//
//  Created by sergey.sergeev on 8/17/15.
//  Copyright (c) 2015 sergey.sergeev. All rights reserved.
//

#include "ces_material_component.h"
#include "texture.h"
#include "mesh.h"

namespace gb
{
    ces_material_component::ces_material_component() :
    m_bind_material_imposer_callback(nullptr),
    m_visible(true)
    {
        m_type = e_ces_component_type_material;
    }
    
    ces_material_component::~ces_material_component()
    {
        m_materials.clear();
    }
    
    void ces_material_component::add_material(const std::string& technique_name, i32 technique_pass, const material_shared_ptr& material)
    {
        m_materials[technique_name][technique_pass] = material;
    }
    
    void ces_material_component::remove_material(const std::string& technique_name, i32 technique_pass)
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
    
    material_shared_ptr ces_material_component::get_material(const std::string& technique_name, i32 technique_pass) const
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
    
    void ces_material_component::set_visible(bool value)
    {
        m_visible = value;
    }
    
    bool ces_material_component::get_visible() const
    {
        return m_visible;
    }
    
    void ces_material_component::set_texture(const texture_shared_ptr& texture, e_shader_sampler sampler, const std::string& technique_name, i32 technique_pass)
    {
        if(technique_name.length() != 0)
        {
            if(technique_pass != -1)
            {
                material_shared_ptr material = ces_material_component::get_material(technique_name, technique_pass);
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
    
    void ces_material_component::bind_custom_shader_uniforms(const material_shared_ptr& material)
    {
        assert(material);
        
        std::map<std::string, std::shared_ptr<shader_uniform>> custom_shader_uniforms = material->get_custom_uniforms();
        std::shared_ptr<shader_uniform> current_uniform = nullptr;
        for(const auto& iterator : custom_shader_uniforms)
        {
            current_uniform = iterator.second;
            assert(current_uniform);
            
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
                {
                    assert(false);
                }
                    break;
            }
        }
        
        if(m_bind_material_imposer_callback)
        {
            m_bind_material_imposer_callback(material);
        }
    }
    
    void ces_material_component::on_bind(const std::string& technique_name, i32 technique_pass,
                                         const material_shared_ptr& material)
    {
        material_shared_ptr using_material = material;
        if(!using_material)
        {
            using_material = ces_material_component::get_material(technique_name, technique_pass);
        }
        assert(using_material);
        assert(using_material->get_shader()->is_commited());
        
        using_material->bind();
        
        ces_material_component::bind_custom_shader_uniforms(using_material);
    }
    
    void ces_material_component::on_unbind(const std::string& technique_name, i32 technique_pass,
                                           const material_shared_ptr& material)
    {
        material_shared_ptr using_material = material;
        if(!using_material)
        {
            using_material = ces_material_component::get_material(technique_name, technique_pass);
        }
        assert(using_material);
        using_material->unbind();
    }
    
    void ces_material_component::set_bind_material_imposer_callback(const std::function<void(const material_shared_ptr&)>& callback)
    {
        m_bind_material_imposer_callback = callback;
    }
}