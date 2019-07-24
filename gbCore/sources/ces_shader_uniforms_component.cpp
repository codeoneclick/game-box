//
//  ces_shader_uniforms_component.cpp
//  gbCore
//
//  Created by serhii.s on 3/19/19.
//  Copyright © 2019 sergey.sergeev. All rights reserved.
//

#include "ces_shader_uniforms_component.h"

namespace gb
{
    ces_shader_uniforms_component::shader_uniforms::shader_uniforms(e_shader_uniform_mode mode, const std::string& name) :
    m_mode(mode),
    m_name(name)
    {
        
    }
    
    std::unordered_map<std::string, std::shared_ptr<shader_uniform>> ces_shader_uniforms_component::shader_uniforms::get_uniforms() const
    {
        return m_uniforms;
    }
    
    ces_shader_uniforms_component::e_shader_uniform_mode ces_shader_uniforms_component::shader_uniforms::get_mode() const
    {
        return m_mode;
    }
    
    std::string ces_shader_uniforms_component::shader_uniforms::get_name() const
    {
        return m_name;
    }
    
    const std::unordered_map<i32, std::shared_ptr<ces_shader_uniforms_component::shader_uniforms>>& ces_shader_uniforms_component::get_vertex_buffers() const
    {
        return m_vertex_buffers;
    }
    
    const std::unordered_map<i32, std::shared_ptr<ces_shader_uniforms_component::shader_uniforms>>& ces_shader_uniforms_component::get_fragment_buffers() const
    {
        return m_fragment_buffers;
    }
    
    std::shared_ptr<ces_shader_uniforms_component::shader_uniforms> ces_shader_uniforms_component::get_uniforms(ces_shader_uniforms_component::e_shader_uniform_mode mode, const std::string& name, i32 index) const
    {
        std::shared_ptr<ces_shader_uniforms_component::shader_uniforms> uniforms = nullptr;
        if (mode == ces_shader_uniforms_component::e_shader_uniform_mode::e_vertex)
        {
            const auto uniforms_it = m_vertex_buffers.find(index);
            if (uniforms_it != m_vertex_buffers.end())
            {
                if (uniforms_it->second->get_name() == name)
                {
                    uniforms = uniforms_it->second;
                }
            }
        }
        else if (mode == ces_shader_uniforms_component::e_shader_uniform_mode::e_fragment)
        {
            const auto uniforms_it = m_fragment_buffers.find(index);
            if (uniforms_it != m_fragment_buffers.end())
            {
                if (uniforms_it->second->get_name() == name)
                {
                    uniforms = uniforms_it->second;
                }
            }
        }
        return uniforms;
    }
}
