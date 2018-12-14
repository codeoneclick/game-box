//
//  render_technique_ws.cpp
//  gbCore
//
//  Created by sergey.sergeev on 8/17/15.
//  Copyright (c) 2015 sergey.sergeev. All rights reserved.
//

#include "render_technique_ws.h"

#if USED_GRAPHICS_API != NO_GRAPHICS_API

#include "texture.h"
#include "mesh_2d.h"
#include "material.h"

namespace gb
{
    render_technique_ws::render_technique_ws(ui32 width, ui32 height, const std::string& name, ui32 index,
                                             bool is_depth_compare_mode_enabled, i32 num_passes) :
    gb::render_technique_base(width, height, name, index),
    m_num_passes(std::max(num_passes, 1))
    {
        ui32 color_attachment_id;
        gl_create_textures(1, &color_attachment_id);
        gl_bind_texture(GL_TEXTURE_2D, color_attachment_id);
        gl_texture_parameter_i(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        gl_texture_parameter_i(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        gl_texture_parameter_i(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        gl_texture_parameter_i(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        gl_texture_image2d(GL_TEXTURE_2D, 0, GL_RGBA, m_frame_width, m_frame_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
        
        ui32 depth_attachment_id;
        gl_create_textures(1, &depth_attachment_id);
        gl_bind_texture(GL_TEXTURE_2D, depth_attachment_id);
        gl_texture_parameter_i(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        gl_texture_parameter_i(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        gl_texture_parameter_i(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        gl_texture_parameter_i(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        
        
        if(is_depth_compare_mode_enabled)
        {
#if defined(__IOS__) || defined(__TVOS__)
            
            gl_texture_parameter_i(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE_EXT, GL_COMPARE_REF_TO_TEXTURE_EXT);
            gl_texture_parameter_i(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC_EXT, GL_LEQUAL);
            
#else
            
            gl_texture_parameter_i(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
            gl_texture_parameter_i(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
            
#endif
        }

#if defined(__IOS__) || defined(__TVOS__)

	#if USED_GRAPHICS_API == OPENGL_30_API
		gl_texture_image2d(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, m_frame_width, m_frame_height, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_INT, NULL);
	#elif USED_GRAPHICS_API == OPENGL_20_API
		gl_texture_image2d(GL_TEXTURE_2D, 0, GL_DEPTH_STENCIL_OES, m_frame_width, m_frame_height, 0, GL_DEPTH_STENCIL_OES, GL_UNSIGNED_INT_24_8_OES, NULL);
	#endif

#else
        
	#if defined(__OSX__)
        gl_texture_image2d(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, m_frame_width, m_frame_height, 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, NULL);
	#endif

	#if defined(__WINOS__)
		gl_texture_image2d(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, m_frame_width, m_frame_height, 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, NULL);
	#endif

#endif
        
        gl_create_frame_buffers(1, &m_frame_buffer);
        gl_bind_frame_buffer(GL_FRAMEBUFFER, m_frame_buffer);
        gl_attach_frame_buffer_texture2d(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, color_attachment_id, 0);
        
#if defined(__OSX__)
        
        gl_attach_frame_buffer_texture2d(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, depth_attachment_id, 0);
        
#else
        
        gl_attach_frame_buffer_texture2d(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depth_attachment_id, 0);
        gl_attach_frame_buffer_texture2d(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_TEXTURE_2D, depth_attachment_id, 0);
        
#endif
        
        GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
        assert(status == GL_FRAMEBUFFER_COMPLETE);
        
        std::string color_attachment_guid = m_name;
        color_attachment_guid.append(".color");
        m_color_attachment_texture = texture::construct(color_attachment_guid,
                                                        color_attachment_id,
                                                        m_frame_width,
                                                        m_frame_height);
        
        m_color_attachment_texture->set_wrap_mode(GL_CLAMP_TO_EDGE);
        
        std::string depth_attachment_guid = m_name;
        depth_attachment_guid.append(".depth");
        m_depth_attachment_texture = texture::construct(depth_attachment_guid,
                                                        depth_attachment_id,
                                                        m_frame_width,
                                                        m_frame_height);
        m_depth_attachment_texture->set_wrap_mode(GL_CLAMP_TO_EDGE);
    }
    
    render_technique_ws::~render_technique_ws()
    {
        gl_delete_frame_buffers(1, &m_frame_buffer);
    }
    
    texture_shared_ptr render_technique_ws::get_color_attachment_texture() const
    {
        assert(m_color_attachment_texture);
        return m_color_attachment_texture;
    }
    
    texture_shared_ptr render_technique_ws::get_depth_attachment_texture() const
    {
        assert(m_depth_attachment_texture);
        return m_depth_attachment_texture;
    }
    
    void render_technique_ws::bind()
    {
        gl_bind_frame_buffer(GL_FRAMEBUFFER, m_frame_buffer);
        gl_viewport(0, 0, m_frame_width, m_frame_height);
        
        gl_enable(GL_DEPTH_TEST);
        material::get_cached_parameters()->m_is_depth_test = true;
        gl_depth_mask(GL_TRUE);
        material::get_cached_parameters()->m_is_depth_mask = true;
        gl_enable(GL_STENCIL_TEST);
        material::get_cached_parameters()->m_is_stencil_test = true;

        glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
        
        gl_clear_color(m_clear_color.r, m_clear_color.g, m_clear_color.b, m_clear_color.a);
        gl_clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    }
    
    void render_technique_ws::unbind()
    {
        gl_bind_frame_buffer(GL_FRAMEBUFFER, NULL);
    }
    
    void render_technique_ws::draw()
    {
        assert(false);
    }
    
    i32 render_technique_ws::get_num_passes() const
    {
        return m_num_passes;
    }
}

#endif
