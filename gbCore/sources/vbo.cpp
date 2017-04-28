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
    vbo::vbo(ui32 size, ui32 mode) :
    m_allocated_size(size),
    m_used_size(0),
    m_mode(mode),
    m_min_bound(glm::vec2(0.f)),
    m_max_bound(glm::vec2(0.f)),
    m_version(0)
    {
        assert(m_allocated_size != 0);
        gl_create_buffers(1, &m_handle);
        m_data = new vertex_attribute[m_allocated_size];
        memset(m_data, 0x0, sizeof(vertex_attribute) * m_allocated_size);
    }
    
    vbo::~vbo()
    {
        gl_delete_buffers(1, &m_handle);
        delete[] m_data;
    }
    
    ui32 vbo::get_allocated_size() const
    {
        return m_allocated_size;
    }
    
    ui32 vbo::get_used_size() const
    {
        return m_used_size;
    }
    
    glm::vec2 vbo::get_min_bound() const
    {
        return m_min_bound;
    }
    
    glm::vec2 vbo::get_max_bound() const
    {
        return m_max_bound;
    }
    
    vbo::vertex_attribute* vbo::lock() const
    {
        assert(m_data != nullptr);
        return m_data;
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
        assert(m_data != nullptr);
        assert(m_allocated_size != 0);
        m_used_size = size > 0 && size < m_allocated_size ? size : m_allocated_size;
        
#if !defined(__NO_RENDER__)
        
        if(!is_bathing)
        {
            gl_bind_buffer(GL_ARRAY_BUFFER, m_handle);
            gl_push_buffer_data(GL_ARRAY_BUFFER, sizeof(vertex_attribute) * m_used_size, m_data, m_mode);
        }
        
#endif
        
        m_min_bound = glm::vec2(INT16_MAX);
        m_max_bound = glm::vec2(INT16_MIN);
        
        for(i32 i = 0; i < m_used_size; ++i)
        {
            glm::vec2 point = glm::vec2(m_data[i].m_position.x, m_data[i].m_position.y);
            m_min_bound = glm::min(point, m_min_bound);
            m_max_bound = glm::max(point, m_max_bound);
        }
        
        m_version++;
    }
    
    void vbo::bind(const std::array<i32, e_shader_attribute_max>& attributes) const
    {
#if !defined(__NO_RENDER__)

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
            if(attributes.at(e_shader_attribute_color) >= 0)
            {
                gl_enable_vertex_attribute(attributes.at(e_shader_attribute_color));
                gl_bind_vertex_attribute(attributes.at(e_shader_attribute_color), 4, GL_UNSIGNED_BYTE, GL_TRUE,
                                         sizeof(vertex_attribute),
                                         (GLvoid*)offsetof(vertex_attribute, m_color));
            }
        }

#endif
    }
    
    void vbo::unbind(const std::array<i32, e_shader_attribute_max>& attributes) const
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
            if(attributes.at(e_shader_attribute_color) >= 0)
            {
                gl_disable_vertex_attribute(attributes.at(e_shader_attribute_color));
            }
            gl_bind_buffer(GL_ARRAY_BUFFER, NULL);
        }

#endif
    }
}
