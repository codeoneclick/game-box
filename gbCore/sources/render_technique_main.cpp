//
//  render_technique_main.cpp
//  gbCore
//
//  Created by sergey.sergeev on 8/17/15.
//  Copyright (c) 2015 sergey.sergeev. All rights reserved.
//

#include "render_technique_main.h"
#include "material.h"
#include "mesh.h"
#include "shader.h"
#include "mesh_constructor.h"

namespace gb
{
    render_technique_main::render_technique_main(ui32 width, ui32 height, const material_shared_ptr& material, ui32 frame_buffer, ui32 render_buffer) :
    render_technique_base (width, height, "render.technique.main", 0),
    m_render_buffer(render_buffer),
    m_material(material)
    {
        assert(m_material != nullptr);
        m_frame_buffer = frame_buffer;
        m_quad = mesh_constructor::create_screen_quad();
    }
    
    render_technique_main::~render_technique_main(void)
    {
        
    }
    
    void render_technique_main::bind(void)
    {
        gl_bind_frame_buffer(GL_FRAMEBUFFER, m_frame_buffer);
        gl_bind_render_buffer(GL_RENDERBUFFER, m_render_buffer);
        gl_viewport(0, 0, m_frame_width, m_frame_height);
        
        gl_disable(GL_DEPTH_TEST);
        material::get_cached_parameters()->m_is_depth_test = false;
        gl_depth_mask(GL_FALSE);
        material::get_cached_parameters()->m_is_depth_mask = false;
        gl_disable(GL_STENCIL_TEST);
        material::get_cached_parameters()->m_is_stencil_test = false;
        
        gl_clear_color(m_clear_color.r, m_clear_color.g, m_clear_color.b, m_clear_color.a);
        gl_clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        if(m_material->get_shader()->is_loaded() &&
           m_material->get_shader()->is_commited())
        {
            m_material->bind();
            m_quad->bind(m_material->get_shader()->get_guid(), m_material->get_shader()->get_attributes());
        }
    }
    
    void render_technique_main::unbind(void)
    {
        if(m_material->get_shader()->is_loaded() &&
           m_material->get_shader()->is_commited())
        {
            m_quad->unbind(m_material->get_shader()->get_guid(), m_material->get_shader()->get_attributes());
            m_material->unbind();
        }
    }
    
    void render_technique_main::draw(void)
    {
        if(m_material->get_shader()->is_loaded() &&
           m_material->get_shader()->is_commited())
        {
            m_quad->draw();
        }
    }
}