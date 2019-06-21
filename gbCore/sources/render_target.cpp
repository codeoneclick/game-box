//
//  render_target.cpp
//  gbCore
//
//  Created by sergey.sergeev on 11/6/15.
//  Copyright © 2015 sergey.sergeev. All rights reserved.
//

#include "render_target.h"

#if USED_GRAPHICS_API != NO_GRAPHICS_API

#include "graphics_context.h"
#include "texture.h"

namespace gb
{
    render_target::render_target(i32 format, const glm::ivec2& size, const texture_shared_ptr& custom_attachment) :
    m_size(size),
    m_format(format),
    m_is_custom_color_attachment(custom_attachment != nullptr)
    {
        if(!m_is_custom_color_attachment)
        {
            gl::command::create_textures(1, &m_color_attachment);
            gl::command::bind_texture(gl::constant::texture_2d, m_color_attachment);
            gl::command::texture_parameter_i(gl::constant::texture_2d, gl::constant::texture_min_filter, gl::constant::linear);
            gl::command::texture_parameter_i(gl::constant::texture_2d, gl::constant::texture_mag_filter, gl::constant::linear);
            gl::command::texture_parameter_i(gl::constant::texture_2d, gl::constant::texture_wrap_s, gl::constant::clamp_to_edge);
            gl::command::texture_parameter_i(gl::constant::texture_2d, gl::constant::texture_wrap_t, gl::constant::clamp_to_edge);
            gl::command::texture_image2d(gl::constant::texture_2d, 0, format,
                               m_size.x,
                               m_size.y,
                               0, format, gl::constant::ui8_t, NULL);
        }
        else
        {
            m_color_attachment = custom_attachment->get_texture_id();
        }
        
        gl::command::create_render_buffers(1, &m_depth_attachment);
        gl::command::bind_render_buffer(gl::constant::render_buffer, m_depth_attachment);
        gl::command::create_render_buffer_storage(gl::constant::render_buffer, gl::constant::depth_component16,
                                        m_size.x,
                                        m_size.y);
        
        gl::command::create_frame_buffers(1, &m_frame_buffer);
        gl::command::bind_frame_buffer(gl::constant::frame_buffer, m_frame_buffer);
        gl::command::attach_frame_buffer_texture2d(gl::constant::frame_buffer, gl::constant::color_attachment_0, gl::constant::texture_2d, m_color_attachment, 0);
        gl::command::attach_frame_buffer_render_buffer(gl::constant::frame_buffer, gl::constant::depth_attachment, gl::constant::render_buffer, m_depth_attachment);
        assert(gl::command::check_frame_buffer_status(gl::constant::frame_buffer) == gl::constant::frame_buffer_complete);
        
#if defined(__PBO__)
        
        ui32 size = m_size.x * m_size.y;
        if(m_format == gl::constant::rgba_t)
        {
            size *= 4;
        }
        else if(m_format == gl::constant::rgb_t)
        {
            size *= 3;
        }
        
        gl::command::create_buffers(1, &m_pixel_buffer);
        gl::command::bind_buffer(GL_PIXEL_PACK_BUFFER, m_pixel_buffer);
        gl::command::push_buffer_data(GL_PIXEL_PACK_BUFFER, size, NULL, GL_STREAM_READ);
        gl::command::bind_buffer(GL_PIXEL_PACK_BUFFER, 0);
        
#endif
        
    }
    
    render_target::~render_target()
    {
        if(m_frame_buffer != 0)
        {
            gl::command::delete_frame_buffers(1, &m_frame_buffer);
        }
        if(m_color_attachment != 0 && !m_is_custom_color_attachment)
        {
            gl::command::delete_textures(1, &m_color_attachment);
        }
        if(m_depth_attachment != 0)
        {
            gl::command::delete_render_buffers(1, &m_depth_attachment);
        }
        
#if defined(__PBO__)
        
        if(m_pixel_buffer != 0)
        {
            gl::command::delete_buffers(1, &m_pixel_buffer);
        }
        
#endif
    }
    
    void render_target::clear()
    {
        gl::command::clear_color(0.f, 0.f, 0.f, 1.f);
        gl::command::clear(gl::constant::color_buffer_bit | gl::constant::depth_buffer_bit);
    }
    
    void render_target::begin()
    {
        gl::command::bind_frame_buffer(gl::constant::frame_buffer, m_frame_buffer);
        gl::command::viewport(0, 0, m_size.x, m_size.y);
    }
    
    void render_target::end(ui8* data)
    {
        if(data)
        {
            render_target::grab(data);
        }
    }
    
    void render_target::grab(ui8* data)
    {
        if(data)
        {
            
#if defined(__PBO__)
            
            glReadBuffer(gl::constant::color_attachment_0);
            gl::command::bind_buffer(GL_PIXEL_PACK_BUFFER, m_pixel_buffer);
            glReadPixels(0, 0, m_size.x, m_size.y, m_format, gl::constant::ui8_t, NULL);
            
            ui8* pointer = static_cast<ui8*>(glMapBuffer(GL_PIXEL_PACK_BUFFER, GL_READ_ONLY));
            if (pointer)
            {
                ui32 size = m_size.x * m_size.y;
                if(m_format == gl::constant::rgba_t)
                {
                    size *= 4;
                }
                else if(m_format == gl::constant::rgb_t)
                {
                    size *= 3;
                }
                
                memcpy(data, pointer, size);
                glUnmapBuffer(GL_PIXEL_PACK_BUFFER);
            }
            
#else
            
            gl::command::read_pixels(0, 0, m_size.x, m_size.y, m_format, gl::constant::ui8_t, data);
            
            
#endif
        }
    }
    
    texture_shared_ptr render_target::grab_color_attachment()
    {
        m_is_custom_color_attachment = true;
        texture_shared_ptr texture = texture::construct("color_attachment",
                                                        m_color_attachment, m_size.x, m_size.y);
        texture->set_wrap_mode(gl::constant::clamp_to_edge);
        texture->set_mag_filter(gl::constant::linear);
        texture->set_min_filter(gl::constant::linear);
        return texture;
    }
}

#endif
