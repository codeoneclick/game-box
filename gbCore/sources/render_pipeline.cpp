//
//  render_pipeline.cpp
//  gbCore
//
//  Created by sergey.sergeev on 8/17/15.
//  Copyright (c) 2015 sergey.sergeev. All rights reserved.
//

#include "render_pipeline.h"
#include "graphics_context.h"
#include "render_technique_main.h"
#include "render_technique_ws.h"
#include "render_technique_ss.h"

namespace gb
{
    render_pipeline::render_pipeline(const graphics_context_shared_ptr& graphic_context, bool is_offscreen) :
    gb::render_techniques_importer(graphic_context, is_offscreen),
    gb::render_techniques_accessor()
    {
        
    }
    
    render_pipeline::~render_pipeline()
    {
        
    }
    
    void render_pipeline::on_draw_begin()
    {
        assert(m_graphics_context);
        m_graphics_context->make_current();
    }
    
    void render_pipeline::on_draw_end()
    {
        for(const auto& iterator : m_ss_render_techniques)
        {
            std::shared_ptr<render_technique_ss> technique = iterator.second;
            
            technique->bind();
            technique->draw();
            technique->unbind();
        }
        
        if(m_main_render_technique && !m_offscreen)
        {
            m_main_render_technique->bind();
            m_main_render_technique->draw();
            m_main_render_technique->unbind();
        }
        
        if(!m_offscreen)
        {
            m_graphics_context->draw();
        }
    }
    
    texture_shared_ptr render_pipeline::get_technique_texture(const std::string& technique_name)
    {
        std::string find_technique_name = technique_name;
        std::string::size_type location = technique_name.find(".depth");
        if (std::string::npos != location)
        {
            find_technique_name = std::string(technique_name, 0, location);
        }
        std::shared_ptr<texture> texture = m_ws_render_techniques.find(find_technique_name) != m_ws_render_techniques.end() ? std::string::npos == location ? m_ws_render_techniques.find(find_technique_name)->second->get_color_attachment_texture() : m_ws_render_techniques.find(find_technique_name)->second->get_depth_attachment_texture() : m_ss_render_techniques.find(find_technique_name) != m_ss_render_techniques.end() ? m_ss_render_techniques.find(find_technique_name)->second->get_color_attachment_texture() : nullptr;
        assert(texture != nullptr);
        return texture;
    }
    
    material_shared_ptr render_pipeline::get_technique_material(const std::string& technique_name)
    {
        material_shared_ptr material = m_ss_render_techniques.find(technique_name) != m_ss_render_techniques.end() ? m_ss_render_techniques.find(technique_name)->second->get_material(): nullptr;
        assert(material != nullptr);
        return material;
    }
    
    ui32 render_pipeline::get_screen_width()
    {
        assert(m_graphics_context != nullptr);
        return m_graphics_context->get_width();
    }
    
    ui32 render_pipeline::get_screen_height()
    {
        assert(m_graphics_context != nullptr);
        return m_graphics_context->get_height();
    }
    
    graphics_context_shared_ptr render_pipeline::get_graphics_context() const
    {
        return m_graphics_context;
    }
    
	const std::list<std::shared_ptr<render_technique_ws>>& render_pipeline::get_ws_techniques() const
    {
        return m_ordered_ws_render_techniques;
    }
}