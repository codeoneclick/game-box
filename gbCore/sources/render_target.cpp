//
//  render_target.cpp
//  gbCore
//
//  Created by sergey.sergeev on 11/6/15.
//  Copyright © 2015 sergey.sergeev. All rights reserved.
//

#include "render_target.h"
#include "graphics_context.h"

namespace gb
{
    render_target::render_target(const std::shared_ptr<graphics_context>& graphics_context, GLint format, ui32 width, ui32 height) :
    m_graphics_context(graphics_context),
    m_size(glm::ivec2(width, height)),
    m_format(format)
    {
        gl_create_textures(1, &m_color_attachment);
        gl_bind_texture(GL_TEXTURE_2D, m_color_attachment);
        gl_texture_parameter_i(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        gl_texture_parameter_i(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        gl_texture_parameter_i(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        gl_texture_parameter_i(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        gl_texture_image2d(GL_TEXTURE_2D, 0, format,
                           m_size.x,
                           m_size.y,
                           0, format, GL_UNSIGNED_BYTE, NULL);
        
        gl_create_render_buffers(1, &m_depth_attachment);
        gl_bind_render_buffer(GL_RENDERBUFFER, m_depth_attachment);
        gl_create_render_buffer_storage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16,
                                        m_size.x,
                                        m_size.y);
        
        gl_create_frame_buffers(1, &m_frame_buffer);
        gl_bind_frame_buffer(GL_FRAMEBUFFER, m_frame_buffer);
        gl_attach_frame_buffer_texture2d(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_color_attachment, 0);
        gl_attach_frame_buffer_render_buffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_depth_attachment);
        assert(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);
        
#if defined(__PBO__)
        
        ui32 size = m_size.x * m_size.y;
        if(m_format == GL_RGBA)
        {
            size *= 4;
        }
        else if(m_format == GL_RGB)
        {
            size *= 3;
        }
        
        gl_create_buffers(1, &m_pixel_buffer);
        gl_bind_buffer(GL_PIXEL_PACK_BUFFER, m_pixel_buffer);
        gl_push_buffer_data(GL_PIXEL_PACK_BUFFER, size, NULL, GL_STREAM_READ);
        gl_bind_buffer(GL_PIXEL_PACK_BUFFER, 0);
        
#endif
    }
    
    render_target::~render_target()
    {
        if(m_frame_buffer != 0)
        {
            gl_delete_frame_buffers(1, &m_frame_buffer);
        }
        if(m_color_attachment != 0)
        {
            gl_delete_textures(1, &m_color_attachment);
        }
        if(m_depth_attachment != 0)
        {
            gl_delete_render_buffers(1, &m_depth_attachment);
        }
        
#if defined(__PBO__)
        
        if(m_pixel_buffer != 0)
        {
            gl_delete_buffers(1, &m_pixel_buffer);
        }
        
#endif
    }
    
    void render_target::clear()
    {
        gl_clear_color(0.f, 0.f, 0.f, 1.f);
        gl_clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }
    void render_target::begin()
    {
        gl_bind_frame_buffer(GL_FRAMEBUFFER, m_frame_buffer);
        gl_viewport(0, 0, m_size.x, m_size.y);
    }
    
    void render_target::end(ui8* data)
    {
        if(data)
        {
#if defined(__PBO__)
            
            glReadBuffer(GL_COLOR_ATTACHMENT0);
            gl_bind_buffer(GL_PIXEL_PACK_BUFFER, m_pixel_buffer);
            glReadPixels(0, 0, m_size.x, m_size.y, m_format, GL_UNSIGNED_BYTE, NULL);
            
            ui8* pointer = static_cast<ui8*>(glMapBuffer(GL_PIXEL_PACK_BUFFER, GL_READ_ONLY));
            if (pointer)
            {
                ui32 size = m_size.x * m_size.y;
                if(m_format == GL_RGBA)
                {
                    size *= 4;
                }
                else if(m_format == GL_RGB)
                {
                    size *= 3;
                }
                
                memcpy(data, pointer, size);
                glUnmapBuffer(GL_PIXEL_PACK_BUFFER);
            }
            
#else
            
            glReadPixels(0, 0, m_size.x, m_size.y, m_format, GL_UNSIGNED_BYTE, data);
            
            
#endif
        }
        
        assert(m_graphics_context != nullptr);
        
        gl_bind_frame_buffer(GL_FRAMEBUFFER, m_graphics_context->get_frame_buffer());
        gl_viewport(0, 0, m_graphics_context->get_width(), m_graphics_context->get_height());
    }
}