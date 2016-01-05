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
#include "ces_entity.h"
#include "mesh.h"
#include "ces_render_component.h"
#include "ces_geometry_component.h"
#include "ces_particle_emitter_component.h"

namespace gb
{
    render_pipeline::render_pipeline(const graphics_context_shared_ptr& graphic_context, bool is_offscreen) :
    gb::render_techniques_importer(graphic_context, is_offscreen),
    gb::render_techniques_accessor()
    {
        
    }
    
    render_pipeline::~render_pipeline(void)
    {
        
    }
    
    void render_pipeline::on_draw_begin()
    {
        assert(m_graphics_context);
        m_graphics_context->make_current();
    }
    
    void render_pipeline::on_draw(const ces_entity_shared_ptr &entity)
    {
        for(const auto& iterator : m_ws_render_techniques)
        {
            std::shared_ptr<render_technique_ws> technique = iterator.second;
            
            std::size_t name_position = iterator.first.find("_") + 1;
            assert(name_position < iterator.first.size());
            std::string technique_name = iterator.first.substr(name_position);
            
            ces_render_component_shared_ptr render_component =
            std::static_pointer_cast<ces_render_component>(entity->get_component(e_ces_component_type_render));
            ces_geometry_component_shared_ptr geometry_component =
            std::static_pointer_cast<ces_geometry_component>(entity->get_component(e_ces_component_type_geometry));
            ces_particle_emitter_component_shared_ptr particle_emitter_component =
            std::static_pointer_cast<ces_particle_emitter_component>(entity->get_component(e_ces_component_type_particle_emitter));
            assert(render_component);
            
            for(i32 technique_pass = 0; technique_pass < technique->get_num_passes(); ++technique_pass)
            {
                material_shared_ptr using_material = render_component->get_material(technique_name, technique_pass);
                mesh_shared_ptr mesh = nullptr;
                if(geometry_component)
                {
                    mesh = geometry_component->get_mesh();
                }
                else if(particle_emitter_component)
                {
                    mesh = particle_emitter_component->get_mesh();
                }
                
                if(using_material && using_material->get_shader()->is_commited() &&
                   mesh && mesh->is_commited() && render_component->get_visible())
                {
                    technique->add_entity(entity, technique_pass, using_material);
                }
            }
        }
    }
    
    void render_pipeline::on_draw_end()
    {
        for(const auto& iterator : m_ws_render_techniques)
        {
            std::shared_ptr<render_technique_ws> technique = iterator.second;
            technique->bind();
            technique->draw();
            technique->unbind();
        }
        
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
    
    std::shared_ptr<texture> render_pipeline::get_technique_texture(const std::string& technique_name)
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
    
    std::shared_ptr<material> render_pipeline::get_technique_material(const std::string& technique_name)
    {
        std::shared_ptr<material> material = m_ss_render_techniques.find(technique_name) != m_ss_render_techniques.end() ? m_ss_render_techniques.find(technique_name)->second->get_material(): nullptr;
        assert(material != nullptr);
        return material;
    }
    
    ui32 render_pipeline::get_screen_width(void)
    {
        assert(m_graphics_context != nullptr);
        return m_graphics_context->get_width();
    }
    
    ui32 render_pipeline::get_screen_height(void)
    {
        assert(m_graphics_context != nullptr);
        return m_graphics_context->get_height();
    }
    
    graphics_context_shared_ptr render_pipeline::get_graphics_context(void) const
    {
        return m_graphics_context;
    }
}