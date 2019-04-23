//
//  ces_render_technique_uniforms_component.cpp
//  gbCore
//
//  Created by serhii.s on 4/23/19.
//  Copyright © 2019 sergey.sergeev. All rights reserved.
//

#include "ces_render_technique_uniforms_component.h"

namespace gb
{
    ces_render_technique_uniforms_component::shader_uniforms::shader_uniforms(e_shader_uniform_type type) :
    m_type(type)
    {
        
    }
    
    std::unordered_map<std::string, std::shared_ptr<shader_uniform>> ces_render_technique_uniforms_component::shader_uniforms::get_uniforms() const
    {
        return m_uniforms;
    }
    
    ces_render_technique_uniforms_component::e_shader_uniform_type ces_render_technique_uniforms_component::shader_uniforms::get_type() const
    {
        return m_type;
    }
    
    std::shared_ptr<ces_render_technique_uniforms_component::shader_uniforms> ces_render_technique_uniforms_component::get_uniforms(const std::string& technique_name) const
    {
         std::shared_ptr<ces_render_technique_uniforms_component::shader_uniforms> result = nullptr;
        auto uniforms_it = m_uniforms.find(technique_name);
        if (uniforms_it != m_uniforms.end())
        {
            result = uniforms_it->second;
        }
        return result;
    }
}
