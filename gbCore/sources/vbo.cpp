//
//  vbo.cpp
//  gbCore
//
//  Created by sergey.sergeev on 8/13/15.
//  Copyright (c) 2015 sergey.sergeev. All rights reserved.
//

#include "vbo.h"

namespace gb
{
    std::set<uintptr_t> vertex_declaration::g_guids_container;
    
    vertex_declaration::vertex_declaration(i32 size) :
    m_allocated_size(size),
    m_used_size(0),
    m_data(nullptr)
    {
        allocated_size.getter([=]() {
            return m_allocated_size;
        });
        
        used_size.getter([=]() {
            return m_used_size;
        });
        
        used_size.setter([=](i32 size) {
            m_used_size = size;
        });
        
        data.getter([=]() {
            return m_data;
        });
    }
    
    vertex_declaration::~vertex_declaration()
    {
        delete[] m_data;
    }
    
    bool vertex_declaration::is_attribute_active(e_vertex_attribute_type type)
    {
        return m_active_attributes[type] == true;
    }
    
    vertex_declaration_PTC::vertex_declaration_PTC(i32 size) :
    gb::vertex_declaration(size)
    {
        m_data = new vertex_attribute_PTC[m_allocated_size];
    }
    
    vertex_declaration_PTC::~vertex_declaration_PTC()
    {
        
    }
    
    vertex_declaration_PTCE::vertex_declaration_PTCE(i32 size) :
    gb::vertex_declaration(size)
    {
        m_data = new vertex_attribute_PTCE[m_allocated_size];
    }
    
    vertex_declaration_PTCE::~vertex_declaration_PTCE()
    {
        
    }
    
    vbo::vbo(const vertex_declaration_shared_ptr& declaration, ui32 mode) :
    m_declaration(declaration),
    m_mode(mode),
    m_min_bound(glm::vec2(0.f)),
    m_max_bound(glm::vec2(0.f)),
    m_version(0)
    {
        m_type = e_resource_transfering_data_type_vbo;
        
        assert(declaration->allocated_size != 0);
        gl_create_buffers(1, &m_handle);
        
        allocated_size.getter([=]() {
            return m_declaration->allocated_size;
        });
        
        used_size.getter([=]() {
            return m_declaration->used_size;
        });
        
        used_size.setter([=](i32 size) {
            m_declaration->used_size = size;
        });
        
        min_bound.getter([=]() {
            return m_min_bound;
        });

        max_bound.getter([=]() {
            return m_max_bound;
        });
    }
    
    vbo::~vbo()
    {
        gl_delete_buffers(1, &m_handle);
    }
    
    vertex_attribute* vbo::lock() const
    {
        assert(m_declaration->data != nullptr);
        return m_declaration->data;
    }
    
    ui32 vbo::get_id() const
    {
        return m_handle;
    }
    
    ui32 vbo::get_version() const
    {
        return m_version;
    }
    
    void vbo::unlock(bool is_bathing, ui32 size)
    {
        assert(m_declaration->data != nullptr);
        assert(m_declaration->allocated_size != 0);
        m_declaration->used_size = size > 0 && size < m_declaration->allocated_size ? size : m_declaration->allocated_size;
        
#if !defined(__NO_RENDER__)
        
        if(!is_bathing)
        {
            gl_bind_buffer(GL_ARRAY_BUFFER, m_handle);
            gl_push_buffer_data(GL_ARRAY_BUFFER, sizeof(m_declaration->data[0]) * m_declaration->used_size, m_declaration->data, m_mode);
        }
        
#endif
        
        m_min_bound = glm::vec2(INT16_MAX);
        m_max_bound = glm::vec2(INT16_MIN);
        
        for(i32 i = 0; i < m_declaration->used_size; ++i)
        {
            glm::vec3 position = m_declaration->data[i].position;
            glm::vec2 point = glm::vec2(position.x, position.y);
            m_min_bound = glm::min(point, m_min_bound);
            m_max_bound = glm::max(point, m_max_bound);
        }
        
        m_version++;
    }
    
    void vbo::bind(const std::array<i32, e_shader_attribute_max>& attributes) const
    {
#if !defined(__NO_RENDER__)

        if(m_declaration->used_size != 0)
        {
            gl_bind_buffer(GL_ARRAY_BUFFER, m_handle);
            if(attributes.at(e_shader_attribute_position) >= 0 && m_declaration->is_attribute_active(e_vertex_attribute_type_position))
            {
                gl_enable_vertex_attribute(attributes.at(e_shader_attribute_position));
                gl_bind_vertex_attribute(attributes.at(e_shader_attribute_position), 3, GL_FLOAT, GL_FALSE, // 12 bytes
                                         sizeof(vertex_attribute),
                                         m_declaration->data[0].attribute_offset(e_vertex_attribute_type_position));
            }
            if(attributes.at(e_shader_attribute_texcoord) >= 0 && m_declaration->is_attribute_active(e_vertex_attribute_type_texcoord))
            {
                gl_enable_vertex_attribute(attributes.at(e_shader_attribute_texcoord));
                gl_bind_vertex_attribute(attributes.at(e_shader_attribute_texcoord), 2, GL_UNSIGNED_SHORT, GL_TRUE, // 4 bytes = 16 bytes
                                         sizeof(vertex_attribute),
                                         m_declaration->data[0].attribute_offset(e_vertex_attribute_type_texcoord));
            }
            if(attributes.at(e_shader_attribute_normal) >= 0 && m_declaration->is_attribute_active(e_vertex_attribute_type_normal))
            {
                gl_enable_vertex_attribute(attributes.at(e_shader_attribute_normal));
                gl_bind_vertex_attribute(attributes.at(e_shader_attribute_normal), 4, GL_BYTE, GL_TRUE, // 4 bytes = 20 bytes
                                      sizeof(vertex_attribute),
                                      m_declaration->data[0].attribute_offset(e_vertex_attribute_type_normal));
            }
            if(attributes[e_shader_attribute_tangent] >= 0 && m_declaration->is_attribute_active(e_vertex_attribute_type_tangent))
            {
                gl_enable_vertex_attribute(attributes.at(e_shader_attribute_tangent));
                gl_bind_vertex_attribute(attributes.at(e_shader_attribute_tangent), 4, GL_BYTE, GL_TRUE, // 4 bytes = 24 bytes
                                      sizeof(vertex_attribute),
                                      m_declaration->data[0].attribute_offset(e_vertex_attribute_type_tangent));
            }
            if(attributes.at(e_shader_attribute_color) >= 0 && m_declaration->is_attribute_active(e_vertex_attribute_type_color))
            {
                gl_enable_vertex_attribute(attributes.at(e_shader_attribute_color));
                gl_bind_vertex_attribute(attributes.at(e_shader_attribute_color), 4, GL_UNSIGNED_BYTE, GL_TRUE, // 4 bytes = 28 bytes
                                         sizeof(vertex_attribute),
                                         m_declaration->data[0].attribute_offset(e_vertex_attribute_type_color));
            }
            if(attributes.at(e_shader_attribute_extra) >= 0 && m_declaration->is_attribute_active(e_vertex_attribute_type_extra))
            {
                gl_enable_vertex_attribute(attributes.at(e_shader_attribute_extra));
                gl_bind_vertex_attribute(attributes.at(e_shader_attribute_extra), 4, GL_UNSIGNED_BYTE, GL_FALSE,
                                         sizeof(vertex_attribute),
                                         m_declaration->data[0].attribute_offset(e_vertex_attribute_type_extra));
            }
        }

#endif
    }
    
    void vbo::unbind(const std::array<i32, e_shader_attribute_max>& attributes) const
    {
#if !defined(__NO_RENDER__)

        if(m_declaration->used_size != 0)
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
    }
}
