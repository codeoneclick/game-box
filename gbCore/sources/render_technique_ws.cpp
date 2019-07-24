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

#if USED_GRAPHICS_API == METAL_API

#include "mtl_device.h"
#include "mtl_render_pass_descriptor.h"

#endif

namespace gb
{
    render_technique_ws::render_technique_ws(ui32 width, ui32 height, const std::string& name, ui32 index,
                                             bool is_depth_compare_mode_enabled, i32 num_passes) :
    gb::render_technique_base(width, height, name, index),
    m_num_passes(std::max(num_passes, 1))
    {
        
    }
    
    render_technique_ws_shared_ptr render_technique_ws::construct(const std::shared_ptr<ws_technique_configuration>& configuration)
    {
        render_technique_ws_shared_ptr render_technique = std::make_shared<render_technique_ws>(configuration->get_frame_width(),
                                                                                                configuration->get_frame_height(),
                                                                                                configuration->get_guid(),
                                                                                                configuration->get_order(),
                                                                                                configuration->get_is_depth_compare_mode_enabled(),
                                                                                                configuration->get_num_passes());
        
        ui32 color_attachment_id = 0;
        ui32 depth_attachment_id = 0;
        
        gl::command::create_textures(1, &color_attachment_id);
        gl::command::bind_texture(gl::constant::texture_2d, color_attachment_id);
        gl::command::texture_parameter_i(gl::constant::texture_2d, gl::constant::texture_min_filter, gl::constant::linear);
        gl::command::texture_parameter_i(gl::constant::texture_2d, gl::constant::texture_mag_filter, gl::constant::linear);
        gl::command::texture_parameter_i(gl::constant::texture_2d, gl::constant::texture_wrap_s, gl::constant::clamp_to_edge);
        gl::command::texture_parameter_i(gl::constant::texture_2d, gl::constant::texture_wrap_t, gl::constant::clamp_to_edge);
        gl::command::texture_image2d(gl::constant::texture_2d, 0, gl::constant::rgba_t,
                                     configuration->get_frame_width(),
                                     configuration->get_frame_height(), 0, gl::constant::rgba_t, gl::constant::ui8_t, NULL);
        
        gl::command::create_textures(1, &depth_attachment_id);
        gl::command::bind_texture(gl::constant::texture_2d, depth_attachment_id);
        gl::command::texture_parameter_i(gl::constant::texture_2d, gl::constant::texture_min_filter, gl::constant::linear);
        gl::command::texture_parameter_i(gl::constant::texture_2d, gl::constant::texture_mag_filter, gl::constant::linear);
        gl::command::texture_parameter_i(gl::constant::texture_2d, gl::constant::texture_wrap_s, gl::constant::clamp_to_edge);
        gl::command::texture_parameter_i(gl::constant::texture_2d, gl::constant::texture_wrap_t, gl::constant::clamp_to_edge);
        
        if(configuration->get_is_depth_compare_mode_enabled())
        {
#if defined(__IOS__) || defined(__TVOS__)
            
            gl::command::texture_parameter_i(gl::constant::texture_2d, gl::constant::texture_compare_mode, gl::constant::compare_ref_to_texture);
            gl::command::texture_parameter_i(gl::constant::texture_2d, gl::constant::texture_compare_func, gl::constant::lequal);
            
#else
            
            gl::command::texture_parameter_i(gl::constant::texture_2d, gl::constant::texture_compare_mode, gl::constant::compare_ref_to_texture);
            gl::command::texture_parameter_i(gl::constant::texture_2d, gl::constant::texture_compare_func, gl::constant::lequal);
            
#endif
        }
        
#if defined(__IOS__) || defined(__TVOS__)
        
#if USED_GRAPHICS_API == OPENGL_30_API
        
        gl::command::texture_image2d(gl::constant::texture_2d, 0, GL_DEPTH_COMPONENT24, m_frame_width, m_frame_height, 0, GL_DEPTH_COMPONENT, gl::constant::ui32_t, NULL);
        
#elif USED_GRAPHICS_API == OPENGL_20_API
        
        gl::command::texture_image2d(gl::constant::texture_2d, 0, GL_DEPTH_STENCIL_OES, m_frame_width, m_frame_height, 0, GL_DEPTH_STENCIL_OES, gl::constant::ui24_8_t, NULL);
        
#endif
        
#else
        
#if defined(__OSX__)
        
        gl::command::texture_image2d(gl::constant::texture_2d, 0, gl::constant::depth24_stencil8,
                                     configuration->get_frame_width(),
                                     configuration->get_frame_height(),
			0, gl::constant::depth_stencil, gl::constant::ui24_8_t, NULL);
        
#endif
        
#if defined(__WINOS__)
        
        gl::command::texture_image2d(gl::constant::texture_2d, 0, gl::constant::depth24_stencil8,
			configuration->get_frame_width(),
			configuration->get_frame_height(),
			0, gl::constant::depth_stencil, gl::constant::ui24_8_t, NULL);
#endif
        
#endif
        
        gl::command::create_frame_buffers(1, &render_technique->m_frame_buffer);
        gl::command::bind_frame_buffer(gl::constant::frame_buffer, render_technique->m_frame_buffer);
        gl::command::attach_frame_buffer_texture2d(gl::constant::frame_buffer, gl::constant::color_attachment_0, gl::constant::texture_2d, color_attachment_id, 0);
        
#if defined(__OSX__)
        
        gl::command::attach_frame_buffer_texture2d(gl::constant::frame_buffer, gl::constant::depth_stencil_attachment, gl::constant::texture_2d, depth_attachment_id, 0);
        
#else
        
        gl::command::attach_frame_buffer_texture2d(gl::constant::frame_buffer, gl::constant::depth_attachment, gl::constant::texture_2d, depth_attachment_id, 0);
        gl::command::attach_frame_buffer_texture2d(gl::constant::frame_buffer, gl::constant::stencil_attachment, gl::constant::texture_2d, depth_attachment_id, 0);
        
#endif
        
        ui32 status = gl::command::check_frame_buffer_status(gl::constant::frame_buffer);
        assert(status == gl::constant::frame_buffer_complete);
        
        std::string color_attachment_guid = configuration->get_guid();
        color_attachment_guid.append(".color");
        render_technique->m_color_attachment_texture = texture::construct(color_attachment_guid,
                                                        color_attachment_id,
                                                        configuration->get_frame_width(),
                                                        configuration->get_frame_height());
        
        std::string depth_attachment_guid = configuration->get_guid();
        depth_attachment_guid.append(".depth");
        render_technique->m_depth_attachment_texture = texture::construct(depth_attachment_guid,
                                                        depth_attachment_id,
                                                        configuration->get_frame_width(),
                                                        configuration->get_frame_height());
        
        render_technique->m_color_attachment_texture->set_wrap_mode(gl::constant::clamp_to_edge);
        render_technique->m_depth_attachment_texture->set_wrap_mode(gl::constant::clamp_to_edge);
        
#if USED_GRAPHICS_API == METAL_API
        
        render_technique->m_render_pass_descriptor = mtl_render_pass_descriptor::construct_ws_render_pass_descriptor(configuration);
        
#endif
        
        return render_technique;
    }
    
    render_technique_ws::~render_technique_ws()
    {
        gl::command::delete_frame_buffers(1, &m_frame_buffer);
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
        
#if USED_GRAPHICS_API == METAL_API
        
        m_render_pass_descriptor->bind();
        
#endif
        
        gl::command::bind_frame_buffer(gl::constant::frame_buffer, m_frame_buffer);
        gl::command::viewport(0, 0, m_frame_width, m_frame_height);
        
        gl::command::enable(gl::constant::depth_test);
        material::get_cached_parameters()->set_is_depth_test(true);
        gl::command::depth_mask(gl::constant::yes);
        material::get_cached_parameters()->set_is_depth_mask(true);
        gl::command::enable(gl::constant::stencil_test);
        material::get_cached_parameters()->set_is_stencil_test(true);

        gl::command::stencil_operation(gl::constant::keep, gl::constant::keep, gl::constant::replace);
        
        gl::command::clear_color(m_clear_color.r, m_clear_color.g, m_clear_color.b, m_clear_color.a);
        gl::command::clear(gl::constant::color_buffer_bit | gl::constant::depth_buffer_bit | gl::constant::stencil_buffer_bit);
    }
    
    void render_technique_ws::unbind()
    {
        gl::command::bind_frame_buffer(gl::constant::frame_buffer, NULL);
        
#if USED_GRAPHICS_API == METAL_API
        
        m_render_pass_descriptor->unbind();
        
#endif
        
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
