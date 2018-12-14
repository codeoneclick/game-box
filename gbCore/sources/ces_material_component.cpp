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

namespace gb
{
    ces_material_component::ces_material_component() :
    m_bind_material_imposer_callback(nullptr)
    {
        
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
        
        if(m_bind_material_imposer_callback)
        {
            m_bind_material_imposer_callback(material);
        }
    }
    
#if USED_GRAPHICS_API == VULKAN_API

	void ces_material_component::on_bind(const std::string& technique_name, i32 technique_pass, const VkPipelineVertexInputStateCreateInfo& vertex_input_state,
		const material_shared_ptr& material)

#else

	void ces_material_component::on_bind(const std::string& technique_name, i32 technique_pass, const material_shared_ptr& material)

#endif
    {
        material_shared_ptr using_material = material;
        if(!using_material)
        {
            using_material = ces_material_component::get_material(technique_name, technique_pass);
        }
        assert(using_material);
        assert(using_material->get_shader()->is_commited());
        
#if USED_GRAPHICS_API == VULKAN_API

		using_material->bind(vertex_input_state);

#else

		using_material->bind();

#endif
        
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
    
    void ces_material_component::set_is_batching(bool value, const std::string& technique_name, i32 technique_pass)
    {
        if(technique_name.length() != 0)
        {
            if(technique_pass != -1)
            {
                material_shared_ptr material = ces_material_component::get_material(technique_name, technique_pass);
                assert(material);
                material->set_is_batching(value);
            }
            else
            {
                const auto& iterator = m_materials.find(technique_name);
                for(const auto& material : iterator->second)
                {
                    material.second->set_is_batching(value);
                }
            }
        }
        else
        {
            for(const auto& iterator : m_materials)
            {
                for(const auto& material : iterator.second)
                {
                    material.second->set_is_batching(value);
                }
            }
        }
    }
    
    bool ces_material_component::get_is_batching(const std::string& technique_name, i32 technique_pass) const
    {
        material_shared_ptr material = ces_material_component::get_material(technique_name, technique_pass);
        assert(material);
        return material->get_is_batching();
    }
    
    bool ces_material_component::get_is_batching() const
    {
        for(const auto& iterator : m_materials)
        {
            for(const auto& material : iterator.second)
            {
                if(!material.second->get_is_batching())
                {
                    return false;
                }
            }
        }
        return true;
    }
}

#endif
