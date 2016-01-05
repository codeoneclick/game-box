//
//  ces_material_component.hpp
//  gbCore
//
//  Created by sergey.sergeev on 1/5/16.
//  Copyright © 2016 sergey.sergeev. All rights reserved.
//

namespace gb
{
    template<typename T_VALUE>
    void ces_material_component::set_custom_shader_uniform(T_VALUE value, const std::string& uniform,
                                                           const std::string& technique_name, i32 technique_pass)
    {
        if(technique_name.length() != 0)
        {
            if(technique_pass != -1)
            {
                material_shared_ptr material = ces_material_component::get_material(technique_name, technique_pass);
                assert(material);
                material->set_custom_shader_uniform(value, uniform);
            }
            else
            {
                const auto& iterator = m_materials.find(technique_name);
                if(iterator != m_materials.end())
                {
                    for(const auto& material : iterator->second)
                    {
                        material.second->set_custom_shader_uniform(value, uniform);
                    }
                }
            }
        }
        else
        {
            for(const auto& iterator : m_materials)
            {
                for(const auto& material : iterator.second)
                {
                    material.second->set_custom_shader_uniform(value, uniform);
                }
            }
        }
    };
    
    template<typename T_VALUE>
    void ces_material_component::set_custom_shader_uniform_array(T_VALUE array, i32 size, const std::string& uniform,
                                                                 const std::string& technique_name, i32 technique_pass)
    {
        if(technique_name.length() != 0)
        {
            if(technique_pass != -1)
            {
                material_shared_ptr material = ces_material_component::get_material(technique_name, technique_pass);
                assert(material);
                material->set_custom_shader_uniform(array, size, uniform);
            }
            else
            {
                const auto& iterator = m_materials.find(technique_name);
                if(iterator != m_materials.end())
                {
                    for(const auto& material : iterator->second)
                    {
                        material.second->set_custom_shader_uniform(array, size, uniform);
                    }
                }
            }
        }
        else
        {
            for(const auto& iterator : m_materials)
            {
                for(const auto& material : iterator.second)
                {
                    material.second->set_custom_shader_uniform(array, size, uniform);
                }
            }
        }
    };
};
