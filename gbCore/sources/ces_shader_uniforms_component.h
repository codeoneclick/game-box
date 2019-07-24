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
        
        enum class e_shader_uniform_mode
        {
            e_undefined = -1,
            e_vertex,
            e_fragment
        };
        
        class shader_uniforms : public std::enable_shared_from_this<shader_uniforms>
        {
        private:
            
        protected:
            
            e_shader_uniform_mode m_mode = e_shader_uniform_mode::e_undefined;
            std::string m_name;
            std::unordered_map<std::string, std::shared_ptr<shader_uniform>> m_uniforms;
            
        public:
            
            shader_uniforms(e_shader_uniform_mode mode, const std::string& name);
            virtual ~shader_uniforms() = default;
            
            virtual void* get_values() = 0;
            virtual ui32 get_values_size() = 0;
            
            std::unordered_map<std::string, std::shared_ptr<shader_uniform>> get_uniforms() const;
            e_shader_uniform_mode get_mode() const;
            
            std::string get_name() const;
            
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
        
        std::unordered_map<i32, std::shared_ptr<shader_uniforms>> m_vertex_buffers;
        std::unordered_map<i32, std::shared_ptr<shader_uniforms>> m_fragment_buffers;
        
    public:
        
        STTI_CLASS_GUID(ces_shader_uniforms_component, ces_base_component::g_guids_container)
        ces_shader_uniforms_component() = default;
        ~ces_shader_uniforms_component() = default;
        
        template <typename T>
        std::shared_ptr<T> construct_uniforms_buffer(ces_shader_uniforms_component::e_shader_uniform_mode mode, const std::string& name, i32 index)
        {
            const auto uniforms = std::make_shared<T>(mode, name);
            if (mode == ces_shader_uniforms_component::e_shader_uniform_mode::e_vertex)
            {
                 m_vertex_buffers[index] = uniforms;
            }
            else if (mode == ces_shader_uniforms_component::e_shader_uniform_mode::e_fragment)
            {
                 m_fragment_buffers[index] = uniforms;
            }
           
            return uniforms;
        };
        
        const std::unordered_map<i32, std::shared_ptr<shader_uniforms>>& get_vertex_buffers() const;
        const std::unordered_map<i32, std::shared_ptr<shader_uniforms>>& get_fragment_buffers() const;
        std::shared_ptr<shader_uniforms> get_uniforms(ces_shader_uniforms_component::e_shader_uniform_mode mode, const std::string& name, i32 index) const;
        
        template <typename T>
        std::shared_ptr<T> get_uniforms_as(ces_shader_uniforms_component::e_shader_uniform_mode mode, const std::string& name, i32 index) const
        {
            std::shared_ptr<T> uniforms = nullptr;
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
        };
        
    };
};
