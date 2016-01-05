//
//  ces_render_system.cpp
//  gbCore
//
//  Created by sergey.sergeev on 8/17/15.
//  Copyright (c) 2015 sergey.sergeev. All rights reserved.
//

#include "ces_render_system.h"
#include "render_pipeline.h"
#include "ces_system_types.h"
#include "ces_geometry_component.h"
#include "ces_material_component.h"
#include "ces_transformation_component.h"
#include "render_technique_ws.h"
#include "material.h"
#include "mesh.h"
#include "graphics_context.h"

namespace gb
{
    ces_render_system::ces_render_system(const std::shared_ptr<graphics_context>& graphic_context, bool is_offscreen)
    {
        m_type = e_ces_system_type_render;
        m_render_pipeline = std::make_shared<render_pipeline>(graphic_context, is_offscreen);
        m_matrix_p = glm::ortho(0.f,
                                static_cast<f32>(graphic_context->get_width()),
                                static_cast<f32>(graphic_context->get_height()),
                                0.f, -1.f, 1.f);
    }
    
    ces_render_system::~ces_render_system(void)
    {
        
    }
    
    std::shared_ptr<render_pipeline> ces_render_system::get_render_pipeline(void) const
    {
        return m_render_pipeline;
    }
    
    void ces_render_system::on_feed_start(f32 deltatime)
    {
        m_render_pipeline->on_draw_begin();
    }
    
    void ces_render_system::on_feed(const ces_entity_shared_ptr& entity, f32 deltatime)
    {
        std::map<std::string, std::shared_ptr<render_technique_ws>> ws_techniques = m_render_pipeline->get_ws_techniques();
        
        for(const auto& iterator : ws_techniques)
        {
            std::string technique_name = iterator.first;
            std::shared_ptr<render_technique_ws> technique = iterator.second;
            technique->bind();
            
            for(i32 technique_pass = 0; technique_pass < technique->get_num_passes(); ++technique_pass)
            {
                ces_render_system::draw_recursively(entity, technique_name, technique_pass);
            }

            technique->unbind();
        }
    }
    
    void ces_render_system::draw_recursively(const ces_entity_shared_ptr& entity, const std::string &technique_name, i32 technique_pass)
    {
        ces_material_component* material_component = unsafe_get_material_component(entity);
        ces_geometry_component* geometry_component = unsafe_get_geometry_component(entity);
        ces_transformation_component* transformation_component = unsafe_get_transformation_component(entity);
        
        if(material_component && geometry_component && transformation_component)
        {
            material_shared_ptr material = material_component->get_material(technique_name, technique_pass);
            mesh_shared_ptr mesh = geometry_component->get_mesh();
            if(material && material->get_shader()->is_commited() &&
               mesh && material_component->get_visible())
            {
                material_component->on_bind(technique_name, technique_pass, material);
                
                material->get_shader()->set_mat4(m_matrix_p, e_shader_uniform_mat_p);
                
                glm::mat4 matrix_m = glm::mat4(1.f);
                ces_entity_shared_ptr parent = entity->get_parent();
                
                while(parent)
                {
                    ces_transformation_component* transformation_component = unsafe_get_transformation_component(parent);
                    matrix_m = matrix_m * transformation_component->get_matrix_m();
                    parent = parent->get_parent();
                }
    
                matrix_m = matrix_m * transformation_component->get_matrix_m();
                
                material->get_shader()->set_mat4(matrix_m, e_shader_uniform_mat_m);
                
                mesh->bind(material->get_shader()->get_guid(), material->get_shader()->get_attributes());
                mesh->draw();
                mesh->unbind(material->get_shader()->get_guid(), material->get_shader()->get_attributes());
                
                material_component->on_unbind(technique_name, technique_pass, material);
            }
        }
        
        std::set<ces_entity_shared_ptr> children = entity->get_children();
        for(const auto& child : children)
        {
            ces_render_system::draw_recursively(child, technique_name, technique_pass);
        }
    }
    
    void ces_render_system::on_feed_end(f32 deltatime)
    {
        m_render_pipeline->on_draw_end();
    }
}