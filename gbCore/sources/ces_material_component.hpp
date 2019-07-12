//
//  ces_material_component.hpp
//  gbCore
//
//  Created by sergey.sergeev on 1/5/16.
//  Copyright © 2016 sergey.sergeev. All rights reserved.
//

#if USED_GRAPHICS_API != NO_GRAPHICS_API

namespace gb
{
    template<typename T_VALUE>
    void ces_material_component::set_custom_shader_uniform(T_VALUE value, const std::string& uniform,
                                                           const std::string& technique_name)
    {
        if(technique_name.length() != 0)
        {
            material_shared_ptr material = ces_material_component::get_material(technique_name);
            assert(material);
            material->set_custom_shader_uniform(value, uniform);
        }
        else
        {
            for(const auto& material_it : m_materials)
            {
                material_it.second->set_custom_shader_uniform(value, uniform);
            }
        }
    };
    
    template<typename T_VALUE>
    void ces_material_component::set_custom_shader_uniform_array(T_VALUE array, i32 size, const std::string& uniform,
                                                                 const std::string& technique_name)
    {
        if(technique_name.length() != 0)
        {
            material_shared_ptr material = ces_material_component::get_material(technique_name);
            assert(material);
            material->set_custom_shader_uniform(array, size, uniform);
        }
        else
        {
            for(const auto& material_it : m_materials)
            {
                material_it.second->set_custom_shader_uniform(array, size, uniform);
            }
        }
    };
};

#endif
