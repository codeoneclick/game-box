//
//  render_technique_ss.cpp
//  gbCore
//
//  Created by sergey.sergeev on 8/17/15.
//  Copyright (c) 2015 sergey.sergeev. All rights reserved.
//

#include "render_technique_ss.h"
#include "texture.h"
#include "material.h"
#include "mesh.h"
#include "mesh_constructor.h"

namespace gb
{
    render_technique_ss::render_technique_ss(ui32 width, ui32 height, const std::string& name, const std::shared_ptr<material>& material) :
    render_technique_base(width, height, name, 0),
    m_material(material)
    {
        ui32 color_attachment_id;
        gl_create_textures(1, &color_attachment_id);
        gl_bind_texture(GL_TEXTURE_2D, color_attachment_id);
        gl_texture_parameter_i(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        gl_texture_parameter_i(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        gl_texture_parameter_i(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        gl_texture_parameter_i(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        gl_texture_image2d(GL_TEXTURE_2D, 0, GL_RGBA, m_frame_width, m_frame_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
        
        gl_create_frame_buffers(1, &m_frame_buffer);
        gl_bind_frame_buffer(GL_FRAMEBUFFER, m_frame_buffer);
        gl_attach_frame_buffer_texture2d(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, color_attachment_id, 0);
        
        assert(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);
        
        std::string color_attachment_guid = m_name;
        color_attachment_guid.append(".color");
        m_color_attachment_texture = texture::construct(color_attachment_guid,
                                                        color_attachment_id,
                                                        m_frame_width,
                                                        m_frame_height);
        
        m_color_attachment_texture->set_wrap_mode(GL_CLAMP_TO_EDGE);
        
        m_quad = mesh_constructor::create_screen_quad();
    }
    
    render_technique_ss::~render_technique_ss(void)
    {
        
    }
    
    std::shared_ptr<texture> render_technique_ss::get_color_attachment_texture(void) const
    {
        assert(m_color_attachment_texture != nullptr);
        return m_color_attachment_texture;
    }
    
    std::shared_ptr<material> render_technique_ss::get_material(void) const
    {
        assert(m_material != nullptr);
        return m_material;
    }
    
    void render_technique_ss::bind(void)
    {
        gl_bind_frame_buffer(GL_FRAMEBUFFER, m_frame_buffer);
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
    
    void render_technique_ss::unbind(void)
    {
        if(m_material->get_shader()->is_loaded() &&
           m_material->get_shader()->is_commited())
        {
            m_quad->unbind(m_material->get_shader()->get_guid(), m_material->get_shader()->get_attributes());
            m_material->unbind();
        }
    }
    
    void render_technique_ss::draw(void)
    {
        if(m_material->get_shader()->is_loaded() &&
           m_material->get_shader()->is_commited())
        {
            m_quad->draw();
        }
    }
}