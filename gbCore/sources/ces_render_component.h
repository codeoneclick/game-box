//
//  ces_render_component.h
//  gbCore
//
//  Created by sergey.sergeev on 8/17/15.
//  Copyright (c) 2015 sergey.sergeev. All rights reserved.
//

#ifndef ces_render_component_h
#define ces_render_component_h

#include "ces_base_component.h"
#include "material.h"

namespace gb
{
    class ces_render_component : public ces_base_component
    {
    private:
        
    protected:
        
        std::unordered_map<std::string, std::unordered_map<i32, material_shared_ptr>> m_materials;
        
        bool m_visible;
        
        void bind_custom_shader_uniforms(const material_shared_ptr& material);
        void bind_main_shader_uniforms(const material_shared_ptr& material);
        
        std::function<void(const material_shared_ptr&)> m_bind_material_imposer_callback;
        
    public:
        
        ces_render_component();
        ~ces_render_component();
        
        void add_material(const std::string& technique_name, i32 technique_pass, const material_shared_ptr& material);
        void remove_material(const std::string& technique_name, i32 technique_pass);
        material_shared_ptr get_material(const std::string& technique_name, i32 technique_pass) const;
        const std::unordered_map<std::string, std::unordered_map<i32, material_shared_ptr>>& get_materials() const;
        
        void set_visible(bool value);
        bool get_visible() const;
        
        virtual void on_bind(const std::string& technique_name, i32 technique_pass,
                                            const material_shared_ptr& material = nullptr);
        
        virtual void on_draw(const std::string& technique_name, i32 technique_pass, const std::shared_ptr<mesh>& mesh,
                             const material_shared_ptr& material = nullptr);
        
        virtual void on_unbind(const std::string& technique_name, i32 technique_pass,
                               const material_shared_ptr& material = nullptr);
        
        void set_bind_material_imposer_callback(const std::function<void(const material_shared_ptr&)>& callback);
        
        void set_texture(const std::shared_ptr<texture>& texture, e_shader_sampler sampler, const std::string& technique_name = "", i32 technique_pass = -1);
        
        template<typename T_VALUE>
        void set_custom_shader_uniform(T_VALUE value, const std::string& uniform,
                                       const std::string& technique_name = "", i32 technique_pass = -1)
        {
            if(technique_name.length() != 0)
            {
                if(technique_pass != -1)
                {
                    material_shared_ptr material = ces_render_component::get_material(technique_name, technique_pass);
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
        void set_custom_shader_uniform_array(T_VALUE array, i32 size, const std::string& uniform,
                                             const std::string& technique_name = "", i32 technique_pass = -1)
        {
            if(technique_name.length() != 0)
            {
                if(technique_pass != -1)
                {
                    material_shared_ptr material = ces_render_component::get_material(technique_name, technique_pass);
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
};

#endif
