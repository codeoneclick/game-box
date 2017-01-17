//
//  mesh.cpp
//  gbCore
//
//  Created by sergey.sergeev on 8/13/15.
//  Copyright (c) 2015 sergey.sergeev. All rights reserved.
//

#include "mesh.h"
#include "resource_status.h"

namespace gb
{
#if !defined(__NO_RENDER__)

    mesh::mesh(const vbo_shared_ptr& vbo, const ibo_shared_ptr& ibo, GLenum mode) :

#else

	mesh::mesh(const vbo_shared_ptr& vbo, const ibo_shared_ptr& ibo, ui32 mode) :

#endif
    m_vbo(vbo),
    m_ibo(ibo),
    m_mode(mode)
    {
        
    }
    
    mesh::~mesh()
    {
        
    }
    
    vbo_shared_ptr mesh::get_vbo() const
    {
        return m_vbo;
    }
    
    ibo_shared_ptr mesh::get_ibo() const
    {
        return m_ibo;
    }
    
    ui32 mesh::get_id() const
    {
        return m_vbo->get_id();
    }
    
    void mesh::bind(const std::string& attributes_guid, const std::array<i32, e_shader_attribute_max>& attributes)
    {
        assert(attributes_guid.length() != 0);
        std::shared_ptr<vao> vao_state = m_vao_states[attributes_guid];
        if(!vao_state)
        {
            vao_state = std::make_shared<vao>();
            vao::bind(vao_state);
            m_vbo->bind(attributes);
            m_ibo->bind();
            m_vao_states[attributes_guid] = vao_state;
        }
        else
        {
            vao::bind(vao_state);
        }
    }
    
    void mesh::draw() const
    {
#if !defined(__NO_RENDER__)

        gl_draw_elements(m_mode, m_ibo->get_used_size(), GL_UNSIGNED_SHORT, NULL);

#endif
    }
    
    void mesh::draw(ui32 indices) const
    {
#if !defined(__NO_RENDER__)

        gl_draw_elements(m_mode, indices, GL_UNSIGNED_SHORT, NULL);

#endif
    }
    
    void mesh::unbind(const std::string& attributes_guid, const std::array<i32, e_shader_attribute_max>& attributes)
    {
        vao::unbind();
    }
}
