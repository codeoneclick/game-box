//
//  vbo.h
//  gbCore
//
//  Created by sergey.sergeev on 8/13/15.
//  Copyright (c) 2015 sergey.sergeev. All rights reserved.
//

#pragma once

#include "resource.h"
#include "shader.h"
#include "std_extensions.h"

namespace gb
{
    enum e_vertex_attribute_type
    {
        e_vertex_attribute_type_position = 0,
        e_vertex_attribute_type_texcoord,
        e_vertex_attribute_type_normal,
        e_vertex_attribute_type_tangent,
        e_vertex_attribute_type_color,
        e_vertex_attribute_type_extra,
        e_vertex_attribute_type_extra_01,
        e_vertex_attribute_type_extra_02,
        e_vertex_attribute_type_max
    };
    
    struct vertex_attribute
    {
    protected:
        
        std::bitset<e_vertex_attribute_type_max> m_active_attributes;
        std::function<void*(e_vertex_attribute_type)> m_offset_of;
        
        vertex_attribute()
        {
            position.getter([]() {
                assert(false);
                return glm::vec3(0.f);
            });
            position.setter([](const glm::vec3& position) {
                assert(false);
            });
            
            texcoord.getter([]() {
                assert(false);
                return 0;
            });
            texcoord.setter([](glm::uint32 texcoord) {
                assert(false);
            });
            
            normal.getter([]() {
                assert(false);
                return 0;
            });
            normal.setter([](glm::uint32 normal) {
                assert(false);
            });
            
            tangent.getter([]() {
                assert(false);
                return 0;
            });
            tangent.setter([](glm::uint32 tangent) {
                assert(false);
            });
            
            color.getter([]() {
                assert(false);
                return glm::u8vec4(0);
            });
            color.setter([](const glm::u8vec4& color) {
                assert(false);
            });
            
            extra.getter([]() {
                assert(false);
                return glm::u8vec4(0);
            });
            extra.setter([](const glm::u8vec4& extra) {
                assert(false);
            });
            
            m_offset_of = [=](e_vertex_attribute_type type) {
                assert(false);
                return nullptr;
            };
            
            m_active_attributes.reset();
        };
        
    public:
        
        std::property_rw<glm::vec3>   position;
        std::property_rw<glm::uint32> texcoord;
        std::property_rw<glm::uint32> normal;
        std::property_rw<glm::uint32> tangent;
        std::property_rw<glm::u8vec4> color;
        std::property_rw<glm::u8vec4> extra;
        
        bool is_attribute_active(e_vertex_attribute_type type) const
        {
            return m_active_attributes[type] == true;
        };
        
        void* offset_of(e_vertex_attribute_type type) const
        {
            return m_offset_of(type);
        };
    };
    
    struct vertex_attribute_PTC : public vertex_attribute
    {
        glm::vec3 m_position;
        glm::uint32 m_texcoord;
        glm::u8vec4 m_color;
        
        vertex_attribute_PTC()
        {
            m_active_attributes.set(e_vertex_attribute_type_position);
            m_active_attributes.set(e_vertex_attribute_type_texcoord);
            m_active_attributes.set(e_vertex_attribute_type_color);
            
            position.getter([=]() {
                return m_position;
            });
            position.setter([=](const glm::vec3& position) {
                m_position = position;
            });
            
            texcoord.getter([=]() {
                return m_texcoord;
            });
            texcoord.setter([=](glm::uint32 texcoord) {
                m_texcoord = texcoord;
            });
            
            color.getter([=]() {
                return m_color;
            });
            color.setter([=](const glm::u8vec4& color) {
                m_color = color;
            });
            
            m_offset_of = [=](e_vertex_attribute_type type) -> void* {
                void* result = nullptr;
                switch(type)
                {
                    case e_vertex_attribute_type_position:
                    {
                        result = (void*)offsetof(vertex_attribute_PTC, m_position);
                    }
                        break;
                    case e_vertex_attribute_type_texcoord:
                    {
                        result = (void*)offsetof(vertex_attribute_PTC, m_texcoord);
                    }
                        break;
                    case e_vertex_attribute_type_color:
                    {
                        result = (void*)offsetof(vertex_attribute_PTC, m_color);
                    }
                        break;
                    default:
                        assert(false);
                        break;
                }
                assert(false);
                return result;
            };
        };
    };
    
    struct vertex_attribute_PTCE : public vertex_attribute
    {
        glm::vec3   m_position;
        glm::uint32 m_texcoord;
        glm::u8vec4 m_color;
        glm::u8vec4 m_extra;
        
        vertex_attribute_PTCE()
        {
            m_active_attributes.set(e_vertex_attribute_type_position);
            m_active_attributes.set(e_vertex_attribute_type_texcoord);
            m_active_attributes.set(e_vertex_attribute_type_color);
            m_active_attributes.set(e_vertex_attribute_type_extra);
            
            position.getter([=]() {
                return m_position;
            });
            position.setter([=](const glm::vec3& position) {
                m_position = position;
            });
            
            texcoord.getter([=]() {
                return m_texcoord;
            });
            texcoord.setter([=](glm::uint32 texcoord) {
                m_texcoord = texcoord;
            });
            
            color.getter([=]() {
                return m_color;
            });
            color.setter([=](const glm::u8vec4& color) {
                m_color = color;
            });
            
            extra.getter([=]() {
                return m_color;
            });
            extra.setter([=](const glm::u8vec4& color) {
                m_color = color;
            });
            
            m_offset_of = [=](e_vertex_attribute_type type) -> void* {
                void* result = nullptr;
                switch(type)
                {
                    case e_vertex_attribute_type_position:
                    {
                        result = (void*)offsetof(vertex_attribute_PTCE, m_position);
                    }
                        break;
                    case e_vertex_attribute_type_texcoord:
                    {
                        result = (void*)offsetof(vertex_attribute_PTCE, m_texcoord);
                    }
                        break;
                    case e_vertex_attribute_type_color:
                    {
                        result = (void*)offsetof(vertex_attribute_PTCE, m_color);
                    }
                        break;
                    case e_vertex_attribute_type_extra:
                    {
                        result = (void*)offsetof(vertex_attribute_PTCE, m_extra);
                    }
                        break;
                    default:
                        assert(false);
                        break;
                }
                assert(false);
                return result;
            };
        };
    };
    
    template<class T>
    class vbo final : public resource_transfering_data
    {
    public:
        
    private:
        
    protected:
        
        ui32 m_handle;
        ui32 m_version;
        
        T* m_data;
        ui32 m_allocated_size;
        ui32 m_used_size;
        
        ui32 m_mode;
        
        glm::vec2 m_min_bound;
        glm::vec2 m_max_bound;
        
    public:
        
        vbo(ui32 size, ui32 mode):
        m_allocated_size(size),
        m_used_size(0),
        m_mode(mode),
        m_min_bound(glm::vec2(0.f)),
        m_max_bound(glm::vec2(0.f)),
        m_version(0)
        {
            m_type = e_resource_transfering_data_type_vbo;
            
            assert(m_allocated_size != 0);
            gl_create_buffers(1, &m_handle);
            m_data = new T[m_allocated_size];
            memset(m_data, 0x0, sizeof(T) * m_allocated_size);
        };
        
        ~vbo()
        {
            gl_delete_buffers(1, &m_handle);
            delete[] m_data;
        };
        
        ui32 get_id() const
        {
            return m_handle;
        };

        ui32 get_version() const
        {
            return m_version;
        };
        
        ui32 get_allocated_size() const
        {
            return m_allocated_size;
        };
        
        ui32 get_used_size() const
        {
            return m_used_size;
        };
        
        T* lock() const
        {
            assert(m_data != nullptr);
            return m_data;
        };
        
        void unlock(bool is_bathing = false, ui32 size = 0)
        {
            assert(m_data != nullptr);
            assert(m_allocated_size != 0);
            m_used_size = size > 0 && size < m_allocated_size ? size : m_allocated_size;
            
#if !defined(__NO_RENDER__)
            
            if(!is_bathing)
            {
                gl_bind_buffer(GL_ARRAY_BUFFER, m_handle);
                gl_push_buffer_data(GL_ARRAY_BUFFER, sizeof(T) * m_used_size, m_data, m_mode);
            }
            
#endif
            
            m_min_bound = glm::vec2(INT16_MAX);
            m_max_bound = glm::vec2(INT16_MIN);
            
            for(i32 i = 0; i < m_used_size; ++i)
            {
                glm::vec3 position = m_data[i].position;
                glm::vec2 point = glm::vec2(position.x,
                                            position.y);
                m_min_bound = glm::min(point, m_min_bound);
                m_max_bound = glm::max(point, m_max_bound);
            }
            m_version++;
        }
        
        glm::vec2 get_min_bound() const
        {
            return m_min_bound;
        };
        
        glm::vec2 get_max_bound() const
        {
            return m_max_bound;
        };
        
        void bind(const std::array<i32, e_shader_attribute_max>& attributes) const
        {
#if !defined(__NO_RENDER__)
            
            if(m_used_size != 0)
            {
                gl_bind_buffer(GL_ARRAY_BUFFER, m_handle);
                if(attributes.at(e_shader_attribute_position) >= 0 &&
                   m_data[0].is_attribute_active(e_vertex_attribute_type_position))
                {
                    gl_enable_vertex_attribute(attributes.at(e_shader_attribute_position));
                    gl_bind_vertex_attribute(attributes.at(e_shader_attribute_position), 3, GL_FLOAT, GL_FALSE, // 12 bytes
                                             sizeof(T),
                                             m_data[0].offset_of(e_vertex_attribute_type_position));
                }
                if(attributes.at(e_shader_attribute_texcoord) >= 0 &&
                   m_data[0].is_attribute_active(e_vertex_attribute_type_texcoord))
                {
                    gl_enable_vertex_attribute(attributes.at(e_shader_attribute_texcoord));
                    gl_bind_vertex_attribute(attributes.at(e_shader_attribute_texcoord), 2, GL_UNSIGNED_SHORT, GL_TRUE, // 4 bytes = 16 bytes
                                             sizeof(T),
                                             m_data[0].offset_of(e_vertex_attribute_type_texcoord));
                }
                if(attributes.at(e_shader_attribute_normal) >= 0 &&
                   m_data[0].is_attribute_active(e_vertex_attribute_type_normal))
                {
                    gl_enable_vertex_attribute(attributes.at(e_shader_attribute_normal));
                    gl_bind_vertex_attribute(attributes.at(e_shader_attribute_normal), 4, GL_BYTE, GL_TRUE, // 4 bytes = 20 bytes
                                             sizeof(T),
                                             m_data[0].offset_of(e_vertex_attribute_type_normal));
                }
                if(attributes[e_shader_attribute_tangent] >= 0 &&
                   m_data[0].is_attribute_active(e_vertex_attribute_type_tangent))
                {
                    gl_enable_vertex_attribute(attributes.at(e_shader_attribute_tangent));
                    gl_bind_vertex_attribute(attributes.at(e_shader_attribute_tangent), 4, GL_BYTE, GL_TRUE, // 4 bytes = 24 bytes
                                             sizeof(T),
                                             m_data[0].offset_of(e_vertex_attribute_type_tangent));
                }
                if(attributes.at(e_shader_attribute_color) >= 0 &&
                   m_data[0].is_attribute_active(e_vertex_attribute_type_color))
                {
                    gl_enable_vertex_attribute(attributes.at(e_shader_attribute_color));
                    gl_bind_vertex_attribute(attributes.at(e_shader_attribute_color), 4, GL_UNSIGNED_BYTE, GL_TRUE, // 4 bytes = 28 bytes
                                             sizeof(T),
                                             m_data[0].offset_of(e_vertex_attribute_type_color));
                }
                if(attributes.at(e_shader_attribute_extra) >= 0 &&
                   m_data[0].is_attribute_active(e_vertex_attribute_type_extra))
                {
                    gl_enable_vertex_attribute(attributes.at(e_shader_attribute_extra));
                    gl_bind_vertex_attribute(attributes.at(e_shader_attribute_extra), 4, GL_UNSIGNED_BYTE, GL_FALSE,
                                             sizeof(T),
                                             m_data[0].offset_of(e_vertex_attribute_type_extra));
                }
            }
            
#endif
        };
        
        
        void unbind(const std::array<i32, e_shader_attribute_max>& attributes) const
        {
#if !defined(__NO_RENDER__)
            
            if(m_used_size != 0)
            {
                gl_bind_buffer(GL_ARRAY_BUFFER, m_handle);
                if(attributes.at(e_shader_attribute_position) >= 0)
                {
                    gl_disable_vertex_attribute(attributes.at(e_shader_attribute_position));
                }
                if(attributes.at(e_shader_attribute_texcoord) >= 0)
                {
                    gl_disable_vertex_attribute(attributes.at(e_shader_attribute_texcoord));
                }
                if(attributes.at(e_shader_attribute_normal) >= 0)
                {
                    gl_disable_vertex_attribute(attributes.at(e_shader_attribute_normal));
                }
                if(attributes.at(e_shader_attribute_tangent) >= 0)
                {
                    gl_disable_vertex_attribute(attributes.at(e_shader_attribute_tangent));
                }
                if(attributes.at(e_shader_attribute_color) >= 0)
                {
                    gl_disable_vertex_attribute(attributes.at(e_shader_attribute_color));
                }
                if(attributes.at(e_shader_attribute_extra) >= 0)
                {
                    gl_disable_vertex_attribute(attributes.at(e_shader_attribute_extra));
                }
                gl_bind_buffer(GL_ARRAY_BUFFER, NULL);
            }
            
#endif
        };
    };
};

#include "vbo.hpp"
