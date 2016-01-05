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
    vbo::vbo(ui32 size, GLenum mode, vertex_attribute* mmap) :
    m_allocated_size(size),
    m_used_size(0),
    m_mode(mode),
    m_is_mmap(mmap != nullptr)
    {
        m_type = e_resource_transfering_data_type_vb;
        
        assert(m_allocated_size != 0);
        gl_create_buffers(1, &m_handle);
        
        if(mmap == nullptr)
        {
            m_data = new vertex_attribute[m_allocated_size];
            memset(m_data, 0x0, sizeof(vertex_attribute) * m_allocated_size);
        }
        else
        {
            m_data = mmap;
        }
    }
    
    vbo::~vbo(void)
    {
        gl_delete_buffers(1, &m_handle);
        
        if(!m_is_mmap)
        {
            delete[] m_data;
        }
    }
    
    ui32 vbo::get_allocated_size(void) const
    {
        return m_allocated_size;
    }
    
    ui32 vbo::get_used_size(void) const
    {
        return m_used_size;
    }
    
    vbo::vertex_attribute* vbo::lock(void) const
    {
        assert(m_data != nullptr);
        return m_data;
    }
    
    void vbo::unlock(ui32 size)
    {
        assert(m_data != nullptr);
        assert(m_allocated_size != 0);
        m_used_size = size > 0 && size < m_allocated_size ? size : m_allocated_size;
        gl_bind_buffer(GL_ARRAY_BUFFER, m_handle);
        gl_push_buffer_data(GL_ARRAY_BUFFER, sizeof(vertex_attribute) * m_used_size, m_data, m_mode);
    }
    
    void vbo::bind(const std::array<i32, e_shader_attribute_max>& attributes) const
    {
        if(m_used_size != 0)
        {
            gl_bind_buffer(GL_ARRAY_BUFFER, m_handle);
            if(attributes.at(e_shader_attribute_position) >= 0)
            {
                gl_enable_vertex_attribute(attributes.at(e_shader_attribute_position));
                gl_bind_vertex_attribute(attributes.at(e_shader_attribute_position), 3, GL_FLOAT, GL_FALSE,
                                         sizeof(vertex_attribute),
                                         (GLvoid*)offsetof(vertex_attribute, m_position));
            }
            if(attributes.at(e_shader_attribute_texcoord) >= 0)
            {
                gl_enable_vertex_attribute(attributes.at(e_shader_attribute_texcoord));
                gl_bind_vertex_attribute(attributes.at(e_shader_attribute_texcoord), 2, GL_UNSIGNED_SHORT, GL_TRUE,
                                         sizeof(vertex_attribute),
                                         (GLvoid*)offsetof(vertex_attribute, m_texcoord));
            }
            if(attributes.at(e_shader_attribute_normal) >= 0)
            {
                gl_enable_vertex_attribute(attributes.at(e_shader_attribute_normal));
                gl_bind_vertex_attribute(attributes.at(e_shader_attribute_normal), 4, GL_BYTE, GL_TRUE,
                                         sizeof(vertex_attribute),
                                         (GLvoid*)offsetof(vertex_attribute, m_normal));
            }
            if(attributes[e_shader_attribute_tangent] >= 0)
            {
                gl_enable_vertex_attribute(attributes.at(e_shader_attribute_tangent));
                gl_bind_vertex_attribute(attributes.at(e_shader_attribute_tangent), 4, GL_BYTE, GL_TRUE,
                                         sizeof(vertex_attribute),
                                         (GLvoid*)offsetof(vertex_attribute, m_tangent));
            }
            if(attributes.at(e_shader_attribute_color) >= 0)
            {
                gl_enable_vertex_attribute(attributes.at(e_shader_attribute_color));
                gl_bind_vertex_attribute(attributes.at(e_shader_attribute_color), 4, GL_UNSIGNED_BYTE, GL_FALSE,
                                         sizeof(vertex_attribute),
                                         (GLvoid*)offsetof(vertex_attribute, m_color));
            }
            if(attributes.at(e_shader_attribute_extra) >= 0)
            {
                gl_enable_vertex_attribute(attributes.at(e_shader_attribute_extra));
                gl_bind_vertex_attribute(attributes.at(e_shader_attribute_extra), 4, GL_UNSIGNED_BYTE, GL_FALSE,
                                         sizeof(vertex_attribute),
                                         (GLvoid*)offsetof(vertex_attribute, m_extra));
            }
        }
    }
    
    void vbo::unbind(const std::array<i32, e_shader_attribute_max>& attributes) const
    {
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
    }
}