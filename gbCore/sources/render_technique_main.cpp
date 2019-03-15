//
//  render_technique_main.cpp
//  gbCore
//
//  Created by sergey.sergeev on 8/17/15.
//  Copyright (c) 2015 sergey.sergeev. All rights reserved.
//

#include "render_technique_main.h"

#if USED_GRAPHICS_API != NO_GRAPHICS_API

#include "material.h"
#include "mesh_2d.h"
#include "shader.h"
#include "mesh_constructor.h"
#include "vk_device.h"
#include "vk_swap_chain.h"

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
    
    render_technique_main::~render_technique_main()
    {
        
    }
    
    void render_technique_main::bind()
    {
        gl::command::bind_frame_buffer(gl::constant::frame_buffer, m_frame_buffer);
        gl::command::bind_render_buffer(gl::constant::render_buffer, m_render_buffer);
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
    
    void render_technique_main::unbind()
    {
        if(m_material->get_shader()->is_loaded() &&
           m_material->get_shader()->is_commited())
        {
            m_quad->unbind(m_material->get_shader()->get_guid(), m_material->get_shader()->get_attributes());
            m_material->unbind();
        }
    }
    
    void render_technique_main::draw()
    {
        if(m_material->get_shader()->is_loaded() &&
           m_material->get_shader()->is_commited())
        {
            m_quad->draw();
        }
    }
}

#endif
