//
//  render_technique_ss.cpp
//  gbCore
//
//  Created by sergey.sergeev on 8/17/15.
//  Copyright (c) 2015 sergey.sergeev. All rights reserved.
//

#include "render_technique_ss.h"

#if USED_GRAPHICS_API != NO_GRAPHICS_API

#include "texture.h"
#include "material.h"
#include "mesh_2d.h"
#include "mesh_constructor.h"

namespace gb
{
    render_technique_ss::render_technique_ss(ui32 width, ui32 height, const std::string& name, const material_shared_ptr& material) :
    render_technique_base(width, height, name, 0),
    m_material(material)
    {
        ui32 color_attachment_id = 0;
        gl::command::create_textures(1, &color_attachment_id);
        gl::command::bind_texture(gl::constant::texture_2d, color_attachment_id);
        gl::command::texture_parameter_i(gl::constant::texture_2d, gl::constant::texture_min_filter, gl::constant::linear);
        gl::command::texture_parameter_i(gl::constant::texture_2d, gl::constant::texture_mag_filter, gl::constant::linear);
        gl::command::texture_parameter_i(gl::constant::texture_2d, gl::constant::texture_wrap_s, gl::constant::clamp_to_edge);
        gl::command::texture_parameter_i(gl::constant::texture_2d, gl::constant::texture_wrap_t, gl::constant::clamp_to_edge);
        gl::command::texture_image2d(gl::constant::texture_2d, 0, gl::constant::rgba_t, m_frame_width, m_frame_height, 0, gl::constant::rgba_t, gl::constant::ui8_t, NULL);
        
        gl::command::create_frame_buffers(1, &m_frame_buffer);
        gl::command::bind_frame_buffer(gl::constant::frame_buffer, m_frame_buffer);
        gl::command::attach_frame_buffer_texture2d(gl::constant::frame_buffer, gl::constant::color_attachment_0, gl::constant::texture_2d, color_attachment_id, 0);
        
        assert(gl::command::check_frame_buffer_status(gl::constant::frame_buffer) == gl::constant::frame_buffer_complete);
        
        std::string color_attachment_guid = m_name;
        color_attachment_guid.append(".color");
        m_color_attachment_texture = texture::construct(color_attachment_guid,
                                                        color_attachment_id,
                                                        m_frame_width,
                                                        m_frame_height);
        m_color_attachment_texture->set_wrap_mode(gl::constant::clamp_to_edge);

        m_quad = mesh_constructor::create_screen_quad();
    }
    
    render_technique_ss::~render_technique_ss()
    {
        gl::command::delete_frame_buffers(1, &m_frame_buffer);
    }
    
    std::shared_ptr<texture> render_technique_ss::get_color_attachment_texture() const
    {
        assert(m_color_attachment_texture != nullptr);
        return m_color_attachment_texture;
    }
    
    std::shared_ptr<material> render_technique_ss::get_material() const
    {
        assert(m_material != nullptr);
        return m_material;
    }
    
    void render_technique_ss::bind()
    {
        gl::command::bind_frame_buffer(gl::constant::frame_buffer, m_frame_buffer);
        gl::command::viewport(0, 0, m_frame_width, m_frame_height);
        
        gl::command::disable(gl::constant::depth_test);
        material::get_cached_parameters()->m_is_depth_test = false;
        gl::command::depth_mask(gl::constant::no);
        material::get_cached_parameters()->m_is_depth_mask = false;
        gl::command::disable(gl::constant::stencil_test);
        material::get_cached_parameters()->m_is_stencil_test = false;
        
        gl::command::clear_color(m_clear_color.r, m_clear_color.g, m_clear_color.b, m_clear_color.a);
        gl::command::clear(gl::constant::color_buffer_bit | gl::constant::depth_buffer_bit);

        if(m_material->get_shader()->is_loaded() &&
           m_material->get_shader()->is_commited())
        {

#if USED_GRAPHICS_API == VULKAN_API

			m_material->bind(m_quad->get_vbo()->get_vertex_input_state());

#elif USED_GRAPHICS_API == METAL_API
            
            m_material->bind(m_quad->get_vbo()->get_mtl_vertex_descriptor());
            
#else
            
			m_material->bind();

#endif

            m_quad->bind(m_material->get_shader()->get_guid(), m_material->get_shader()->get_attributes());
        }
    }
    
    void render_technique_ss::unbind()
    {
        if(m_material->get_shader()->is_loaded() &&
           m_material->get_shader()->is_commited())
        {
            m_quad->unbind(m_material->get_shader()->get_guid(), m_material->get_shader()->get_attributes());
            m_material->unbind();
        }
    }
    
    void render_technique_ss::draw()
    {
        if(m_material->get_shader()->is_loaded() &&
           m_material->get_shader()->is_commited())
        {
            m_quad->draw();
        }
    }
}

#endif
