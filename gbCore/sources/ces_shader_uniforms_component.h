//
//  ces_shader_uniforms_component.h
//  gbCore
//
//  Created by serhii.s on 3/19/19.
//  Copyright © 2019 sergey.sergeev. All rights reserved.
//
#pragma once

#include "ces_base_component.h"
#include "shader.h"

namespace gb
{
    class ces_shader_uniforms_component : public ces_base_component
    {
    public:
        
        enum e_shader_uniform_type
        {
            e_shader_uniform_type_unknown = 0,
            e_shader_uniform_type_vertex,
            e_shader_uniform_type_fragment
        };
        
        class shader_uniforms : public std::enable_shared_from_this<shader_uniforms>
        {
        private:
            
        protected:
            
            e_shader_uniform_type m_type = e_shader_uniform_type_unknown;
            std::unordered_map<std::string, std::shared_ptr<shader_uniform>> m_uniforms;
            
        public:
            
            shader_uniforms(e_shader_uniform_type type);
            virtual ~shader_uniforms() = default;
            
            virtual void* get_values() = 0;
            virtual ui32 get_values_size() = 0;
            
            std::unordered_map<std::string, std::shared_ptr<shader_uniform>> get_uniforms() const;
            e_shader_uniform_type get_type() const;
            
            template<typename T>
            void set(T value, const std::string& name)
            {
                auto uniform_it = m_uniforms.find(name);
                if (uniform_it != m_uniforms.end())
                {
                    uniform_it->second->set(value);
                }
            };
        };
        
    private:
        
    protected:
        
        std::shared_ptr<shader_uniforms> m_uniforms = nullptr;
        
    public:
        
        CTTI_CLASS_GUID(ces_shader_uniforms_component, ces_base_component::g_guids_container)
        ces_shader_uniforms_component() = default;
        ~ces_shader_uniforms_component() = default;
        
        template <typename T>
        std::shared_ptr<T> construct_uniforms(ces_shader_uniforms_component::e_shader_uniform_type type)
        {
            const auto uniforms = std::make_shared<T>(type);
            m_uniforms = uniforms;
            return uniforms;
        };
        
        std::shared_ptr<shader_uniforms> get_uniforms() const;
        
        template <typename T>
        std::shared_ptr<T> get_uniforms_as() const
        {
            return std::static_pointer_cast<T>(m_uniforms);
        };
        
    };
};

