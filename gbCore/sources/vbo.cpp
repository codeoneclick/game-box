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
    
    vbo::vertex_declaration::vertex_declaration(ui32 size, vertex_attribute* external_data) :
    m_size(size),
    m_data(nullptr)
    {
        if(!external_data)
        {
            m_data = new vertex_attribute[m_size];
            memset(m_data, 0x0, sizeof(vertex_attribute) * m_size);
        }
        else
        {
            m_data = external_data;
        }
        m_is_external_data = external_data != nullptr;
    }
    
    vbo::vertex_declaration::~vertex_declaration()
    {
        if(!m_is_external_data)
        {
            delete[] m_data;
        }
    }
    
    vbo::vertex_attribute* vbo::vertex_declaration::get_data() const
    {
        return m_data;
    }
    
    ui32 vbo::vertex_declaration::get_size() const
    {
        return m_size;
    }
    
    vbo::vertex_declaration_PTC::vertex_declaration_PTC(ui32 size, vertex_attribute* external_data) :
    vertex_declaration(size, external_data)
    {
        
    }
    
    vbo::vertex_declaration_PTC::~vertex_declaration_PTC()
    {
        
    }
    
    void vbo::vertex_declaration_PTC::bind(const std::array<i32, e_shader_attribute_max>& attributes) const
    {
#if !defined(__NO_RENDER__)
        
        if(attributes.at(e_shader_attribute_position) >= 0)
        {
            gl_enable_vertex_attribute(attributes.at(e_shader_attribute_position));
            gl_bind_vertex_attribute(attributes.at(e_shader_attribute_position), 3, GL_FLOAT, GL_FALSE,
                                     sizeof(vertex_attribute_PTC),
                                     (GLvoid*)offsetof(vertex_attribute_PTC, m_position));
        }
        if(attributes.at(e_shader_attribute_texcoord) >= 0)
        {
            gl_enable_vertex_attribute(attributes.at(e_shader_attribute_texcoord));
            gl_bind_vertex_attribute(attributes.at(e_shader_attribute_texcoord), 2, GL_UNSIGNED_SHORT, GL_TRUE,
                                     sizeof(vertex_attribute_PTC),
                                     (GLvoid*)offsetof(vertex_attribute_PTC, m_texcoord));
        }
        if(attributes.at(e_shader_attribute_color) >= 0)
        {
            gl_enable_vertex_attribute(attributes.at(e_shader_attribute_color));
            gl_bind_vertex_attribute(attributes.at(e_shader_attribute_color), 4, GL_UNSIGNED_BYTE, GL_TRUE,
                                     sizeof(vertex_attribute_PTC),
                                     (GLvoid*)offsetof(vertex_attribute_PTC, m_color));
        }
        
#endif
    }
    
    void vbo::vertex_declaration_PTC::unbind(const std::array<i32, e_shader_attribute_max>& attributes) const
    {
#if !defined(__NO_RENDER__)
        
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
        
#endif
        
    }
    
    vbo::vertex_declaration_PT4B::vertex_declaration_PT4B(ui32 size, vertex_attribute* external_data) :
    vertex_declaration(size, external_data)
    {
        
    }
    
    vbo::vertex_declaration_PT4B::~vertex_declaration_PT4B()
    {
        
    }
    
    void vbo::vertex_declaration_PT4B::bind(const std::array<i32, e_shader_attribute_max>& attributes) const
    {
#if !defined(__NO_RENDER__)
        
        if(attributes.at(e_shader_attribute_position) >= 0)
        {
            gl_enable_vertex_attribute(attributes.at(e_shader_attribute_position));
            gl_bind_vertex_attribute(attributes.at(e_shader_attribute_position), 3, GL_FLOAT, GL_FALSE,
                                     sizeof(vertex_attribute_PT4B),
                                     (GLvoid*)offsetof(vertex_attribute_PT4B, m_position));
        }
        if(attributes.at(e_shader_attribute_texcoord) >= 0)
        {
            gl_enable_vertex_attribute(attributes.at(e_shader_attribute_texcoord));
            gl_bind_vertex_attribute(attributes.at(e_shader_attribute_texcoord), 2, GL_UNSIGNED_SHORT, GL_TRUE,
                                     sizeof(vertex_attribute_PT4B),
                                     (GLvoid*)offsetof(vertex_attribute_PT4B, m_texcoord));
        }
        if(attributes.at(e_shader_attribute_color) >= 0)
        {
            gl_enable_vertex_attribute(attributes.at(e_shader_attribute_color));
            gl_bind_vertex_attribute(attributes.at(e_shader_attribute_color), 4, GL_UNSIGNED_BYTE, GL_TRUE,
                                     sizeof(vertex_attribute_PT4B),
                                     (GLvoid*)offsetof(vertex_attribute_PT4B, m_bone_weights));
        }
        if(attributes.at(e_shader_attribute_extra) >= 0)
        {
            gl_enable_vertex_attribute(attributes.at(e_shader_attribute_extra));
            gl_bind_vertex_attribute(attributes.at(e_shader_attribute_extra), 4, GL_UNSIGNED_BYTE, GL_FALSE,
                                     sizeof(vertex_attribute_PT4B),
                                     (GLvoid*)offsetof(vertex_attribute_PT4B, m_bone_ids));
        }
        
#endif
    }
    
    void vbo::vertex_declaration_PT4B::unbind(const std::array<i32, e_shader_attribute_max>& attributes) const
    {
#if !defined(__NO_RENDER__)
        
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
        if(attributes.at(e_shader_attribute_extra) >= 0)
        {
            gl_disable_vertex_attribute(attributes.at(e_shader_attribute_extra));
        }
        
#endif
    }
    
    vbo::vertex_declaration_PTNTCE::vertex_declaration_PTNTCE(ui32 size, vertex_attribute* external_data) :
    vertex_declaration(size, external_data)
    {
        
    }
    
    vbo::vertex_declaration_PTNTCE::~vertex_declaration_PTNTCE()
    {
        
    }
    
    void vbo::vertex_declaration_PTNTCE::bind(const std::array<i32, e_shader_attribute_max>& attributes) const
    {
#if !defined(__NO_RENDER__)
        
        if(attributes.at(e_shader_attribute_position) >= 0)
        {
            gl_enable_vertex_attribute(attributes.at(e_shader_attribute_position));
            gl_bind_vertex_attribute(attributes.at(e_shader_attribute_position), 3, GL_FLOAT, GL_FALSE,
                                     sizeof(vertex_attribute_PTC),
                                     (GLvoid*)offsetof(vertex_attribute_PTC, m_position));
        }
        if(attributes.at(e_shader_attribute_texcoord) >= 0)
        {
            gl_enable_vertex_attribute(attributes.at(e_shader_attribute_texcoord));
            gl_bind_vertex_attribute(attributes.at(e_shader_attribute_texcoord), 2, GL_UNSIGNED_SHORT, GL_TRUE,
                                     sizeof(vertex_attribute_PTC),
                                     (GLvoid*)offsetof(vertex_attribute_PTC, m_texcoord));
        }
        if(attributes.at(e_shader_attribute_color) >= 0)
        {
            gl_enable_vertex_attribute(attributes.at(e_shader_attribute_color));
            gl_bind_vertex_attribute(attributes.at(e_shader_attribute_color), 4, GL_UNSIGNED_BYTE, GL_TRUE,
                                     sizeof(vertex_attribute_PTC),
                                     (GLvoid*)offsetof(vertex_attribute_PTC, m_color));
        }
        
#endif
    }
    
    void vbo::vertex_declaration_PTNTCE::unbind(const std::array<i32, e_shader_attribute_max>& attributes) const
    {
#if !defined(__NO_RENDER__)
        
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
        
#endif
        
    }
    
    std::queue<ui32> vbo::m_handlers_graveyard;
    std::mutex vbo::m_graveyard_mutex;
    
    vbo::vbo(const std::shared_ptr<vertex_declaration>& declaration, ui32 mode, bool is_using_batch) :
    m_declaration(declaration),
    m_handle(0),
    m_allocated_size(declaration->get_size()),
    m_used_size(0),
    m_mode(mode),
    m_is_using_batch(is_using_batch),
    m_min_bound(glm::vec2(0.f)),
    m_max_bound(glm::vec2(0.f)),
    m_version(0)
    {
        m_type = e_resource_transfering_data_type_vbo;
        assert(m_allocated_size != 0);
        
        if(!m_is_using_batch)
        {
            gl_create_buffers(1, &m_handle);
        }
    }
    
    vbo::~vbo()
    {
        if(!m_is_using_batch)
        {
            vbo::add_to_graveyard(m_handle);
        }
    }
    
    void vbo::add_to_graveyard(ui32 handler)
    {
        std::lock_guard<std::mutex> guard(m_graveyard_mutex);
        m_handlers_graveyard.push(handler);
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
    
    ui32 vbo::get_id() const
    {
        return m_handle;
    }
    
    ui32 vbo::get_version() const
    {
        return m_version;
    }
    
    bool vbo::is_using_batch() const
    {
        return m_is_using_batch;
    }
    
    void vbo::unlock(ui32 size, bool submit_to_vram)
    {
        assert(m_declaration->get_data() != nullptr);
        assert(m_allocated_size != 0);
        m_used_size = size > 0 && size < m_allocated_size ? size : m_allocated_size;
        
#if !defined(__NO_RENDER__)
        
        if(!m_is_using_batch && submit_to_vram)
        {
            gl_bind_buffer(GL_ARRAY_BUFFER, m_handle);
            gl_push_buffer_data(GL_ARRAY_BUFFER, sizeof(vertex_attribute) * m_used_size, m_declaration->get_data(), m_mode);
        }
        
#endif
        
        m_min_bound = glm::vec2(INT16_MAX);
        m_max_bound = glm::vec2(INT16_MIN);
        
        for(i32 i = 0; i < m_used_size; ++i)
        {
            glm::vec2 point = glm::vec2(((vbo::vertex_attribute_P*)m_declaration->get_data())[i].m_position.x,
                                        ((vbo::vertex_attribute_P*)m_declaration->get_data())[i].m_position.y);
            m_min_bound = glm::min(point, m_min_bound);
            m_max_bound = glm::max(point, m_max_bound);
        }
        
        m_version++;
    }
    
    void vbo::bind(const std::array<i32, e_shader_attribute_max>& attributes) const
    {
#if !defined(__NO_RENDER__)

        if(m_used_size != 0 && !m_is_using_batch)
        {
            gl_bind_buffer(GL_ARRAY_BUFFER, m_handle);
            m_declaration->bind(attributes);
        }

#endif
    }
    
    void vbo::unbind(const std::array<i32, e_shader_attribute_max>& attributes) const
    {
#if !defined(__NO_RENDER__)

        if(m_used_size != 0 && !m_is_using_batch)
        {
            gl_bind_buffer(GL_ARRAY_BUFFER, m_handle);
            m_declaration->unbind(attributes);
            gl_bind_buffer(GL_ARRAY_BUFFER, NULL);
        }

#endif
    }
}
