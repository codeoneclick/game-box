//
//  ces_material_component.cpp
//  gbCore
//
//  Created by sergey.sergeev on 8/17/15.
//  Copyright (c) 2015 sergey.sergeev. All rights reserved.
//

#include "ces_material_component.h"

#if USED_GRAPHICS_API != NO_GRAPHICS_API

#include "texture.h"
#include "mesh_2d.h"
#include "render_pipeline.h"

namespace gb
{
    std::map<std::string, i32> ces_material_component::m_cached_render_technique_names;

    i32 ces_material_component::get_technique_name_index(const std::string& technique_name)
    {
        i32 result = -1;
        const auto it = m_cached_render_technique_names.find(technique_name);
        if (it != m_cached_render_technique_names.end())
        {
            result = it->second;
        }
        
        return result;
    }

    ces_material_component::ces_material_component() :
    m_bind_material_imposer_callback(nullptr)
    {
        assert(render_pipeline::get_render_techniques_count() > 0);
        m_materials.resize(render_pipeline::get_render_techniques_count(), nullptr);
    }
    
    ces_material_component::~ces_material_component()
    {
       
    }
    
    void ces_material_component::add_material(const std::string& technique_name, const material_shared_ptr& material)
    {
        i32 technique_index = get_technique_name_index(technique_name);
        if (technique_index == -1)
        {
            technique_index = static_cast<i32>(m_cached_render_technique_names.size() + 1);
            m_cached_render_technique_names.insert({technique_name, technique_index});
        }
        m_materials[technique_index] = material;
    }
    
    void ces_material_component::remove_material(const std::string& technique_name)
    {
        i32 technique_index = get_technique_name_index(technique_name);
        assert(technique_index != -1);
        m_materials[technique_index] = nullptr;
    }
    
    material_shared_ptr ces_material_component::get_material(const std::string& technique_name) const
    {
        i32 technique_index = get_technique_name_index(technique_name);
        assert(technique_index != -1);
        material_shared_ptr material = m_materials[technique_index];
        return material;
    }

    material_shared_ptr ces_material_component::get_material(i32 technique_index) const
    {
        assert(technique_index < m_materials.size());
        material_shared_ptr material = m_materials[technique_index];
        return material;
    }
    
    void ces_material_component::set_texture(const texture_shared_ptr& texture, e_shader_sampler sampler, const std::string& technique_name)
    {
        if(technique_name.length() != 0)
        {
            material_shared_ptr material = ces_material_component::get_material(technique_name);
            assert(material);
            material->set_texture(texture, sampler);
        }
        else
        {
            for (const auto& material_it : m_materials)
            {
                if (material_it)
                {
                    material_it->set_texture(texture, sampler);
                }
            }
        }
    }
    
    void ces_material_component::bind_custom_shader_uniforms(const material_shared_ptr& material)
    {
        assert(material);
        
        if(m_bind_material_imposer_callback)
        {
            m_bind_material_imposer_callback(material);
        }
    }
    
#if USED_GRAPHICS_API == VULKAN_API

	void ces_material_component::on_bind(const std::string& technique_name,
                                         const VkPipelineVertexInputStateCreateInfo& vertex_input_state,
                                         const material_shared_ptr& material)

#elif USED_GRAPHICS_API == METAL_API
    
    void ces_material_component::on_bind(const std::string& technique_name,
                                         const mtl_vertex_descriptor_shared_ptr& vertex_descriptor,
                                         const material_shared_ptr& material)
    
#else

	void ces_material_component::on_bind(const std::string& technique_name,
                                         const material_shared_ptr& material)

#endif
    {
        material_shared_ptr using_material = material;
        if(!using_material)
        {
            using_material = ces_material_component::get_material(technique_name);
        }
        assert(using_material);
        assert(using_material->get_shader()->is_commited());
        
#if USED_GRAPHICS_API == VULKAN_API

		using_material->bind(vertex_input_state);
        
#elif USED_GRAPHICS_API == METAL_API
        
        using_material->bind(vertex_descriptor);

#else

		using_material->bind();

#endif
        
        ces_material_component::bind_custom_shader_uniforms(using_material);
    }
    
    void ces_material_component::on_unbind(const std::string& technique_name,
                                           const material_shared_ptr& material)
    {
        material_shared_ptr using_material = material;
        if(!using_material)
        {
            using_material = ces_material_component::get_material(technique_name);
        }
        assert(using_material);
        using_material->unbind();
    }
    
    void ces_material_component::set_bind_material_imposer_callback(const std::function<void(const material_shared_ptr&)>& callback)
    {
        m_bind_material_imposer_callback = callback;
    }
    
    void ces_material_component::set_is_batching(bool value, const std::string& technique_name)
    {
        if (technique_name.length() != 0)
        {
            material_shared_ptr material = ces_material_component::get_material(technique_name);
            assert(material);
            material->set_is_batching(value);
        }
        else
        {
            for (const auto& material_it : m_materials)
            {
                if (material_it)
                {
                    material_it->set_is_batching(value);
                }
            }
        }
    }
    
    void ces_material_component::set_culling_mode(ui32 value)
    {
        for (const auto& material_it : m_materials)
        {
            material_it->set_culling_mode(value);
        }
    }
    
    bool ces_material_component::get_is_batching(const std::string& technique_name) const
    {
        material_shared_ptr material = ces_material_component::get_material(technique_name);
        assert(material);
        return material->get_is_batching();
    }
    
    bool ces_material_component::get_is_batching() const
    {
        for(const auto& material_it : m_materials)
        {
            if(!material_it->get_is_batching())
            {
                return false;
            }
        }
        return true;
    }
    
    void ces_material_component::set_custom_shader_uniforms(const std::unordered_map<std::string, std::shared_ptr<shader_uniform>>& uniforms, const std::string& technique_name)
    {
        for (auto uniform_it : uniforms)
        {
            switch(uniform_it.second->get_type())
            {
                case e_uniform_type_mat4:
                {
                    set_custom_shader_uniform(uniform_it.second->get_mat4(),
                                              uniform_it.first,
                                              technique_name);
                }
                    break;
                    
                case e_uniform_type_mat4_array:
                {
                    // set_custom_shader_uniform_array(uniform_it.second->get_mat4_array(),
                    //                                uniform_it.second->get_array_size(),
                    //                                uniform_it.first,
                    //                                technique_name, technique_pass);
                }
                    break;
                    
                case e_uniform_type_mat3:
                {
                    set_custom_shader_uniform(uniform_it.second->get_mat3(),
                                              uniform_it.first,
                                              technique_name);
                }
                    break;
                    
                case e_uniform_type_vec4:
                {
                    set_custom_shader_uniform(uniform_it.second->get_vec4(),
                                              uniform_it.first,
                                              technique_name);
                }
                    break;
                    
                case e_uniform_type_vec4_array:
                {
                    // set_custom_shader_uniform_array(uniform_it.second->get_vec4_array(),
                    //                                uniform_it.second->get_array_size(),
                    //                                uniform_it.first,
                    //                                technique_name, technique_pass);
                }
                    break;
                    
                case e_uniform_type_vec3:
                {
                    set_custom_shader_uniform(uniform_it.second->get_vec3(),
                                              uniform_it.first,
                                              technique_name);
                }
                    break;
                    
                case e_uniform_type_vec2:
                {
                    set_custom_shader_uniform(uniform_it.second->get_vec2(),
                                              uniform_it.first,
                                              technique_name);
                }
                    break;
                    
                case e_uniform_type_f32:
                {
                    set_custom_shader_uniform(uniform_it.second->get_f32(),
                                              uniform_it.first,
                                              technique_name);
                }
                    break;
                    
                case e_uniform_type_i32:
                {
                    set_custom_shader_uniform(uniform_it.second->get_i32(),
                                              uniform_it.first,
                                              technique_name);
                }
                    break;
                    
                default:
                {
                    assert(false);
                }
                    break;
            }
        }
    }
}

#endif
