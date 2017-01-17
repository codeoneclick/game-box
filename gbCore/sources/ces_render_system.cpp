//
//  ces_render_system.cpp
//  gbCore
//
//  Created by sergey.sergeev on 8/17/15.
//  Copyright (c) 2015 sergey.sergeev. All rights reserved.
//

#if !defined(__NO_RENDER__)

#include "ces_render_system.h"
#include "render_pipeline.h"
#include "batching_pipeline.h"
#include "ces_geometry_component.h"
#include "ces_material_component.h"
#include "ces_transformation_component.h"
#include "ces_light_compoment.h"
#include "ces_shadow_component.h"
#include "ces_light_mask_component.h"
#include "ces_convex_hull_component.h"
#include "render_technique_ws.h"
#include "material.h"
#include "mesh.h"
#include "camera.h"
#include "graphics_context.h"
#include "mesh_constructor.h"
#include "glm_extensions.h"
#include "ces_geometry_extension.h"

namespace gb
{
    static const std::string k_shadow_color_uniform = "u_shadow_color";
    static const std::string k_light_mask_vs_flag_uniform = "u_mask_flag_vs";
    static const std::string k_light_mask_fs_flag_uniform = "u_mask_flag_fs";
    static const glm::vec4 k_shadow_color_for_casters = glm::vec4(1.f);
    static const glm::vec4 k_shadow_color_for_receivers = glm::vec4(0.f, 1.f, 0.f, .75f);
    
    ces_render_system::ces_render_system(const std::shared_ptr<graphics_context>& graphic_context, bool is_offscreen)
    {
        m_render_pipeline = std::make_shared<render_pipeline>(graphic_context, is_offscreen);
        m_batching_pipeline = std::make_shared<batching_pipeline>();
    }
    
    ces_render_system::~ces_render_system()
    {
        
    }
    
    std::shared_ptr<render_pipeline> ces_render_system::get_render_pipeline() const
    {
        return m_render_pipeline;
    }
    
    void ces_render_system::on_feed_start(f32 deltatime)
    {
        m_render_pipeline->on_draw_begin();
    }
    
    void ces_render_system::on_feed(const ces_entity_shared_ptr& entity, f32 deltatime)
    {
        auto ws_techniques = m_render_pipeline->get_ws_techniques();
        
        for(const auto& technique : ws_techniques)
        {
            std::size_t name_position = technique->get_name().find("_") + 1;
            assert(name_position < technique->get_name().size());
            std::string technique_name = technique->get_name().substr(name_position);
            
            technique->bind();
            
            for(i32 technique_pass = 0; technique_pass < technique->get_num_passes(); ++technique_pass)
            {
                ces_render_system::draw_recursively_lights(entity, technique_name, technique_pass);
                ces_render_system::draw_recursively(entity, technique_name, technique_pass);
                m_batching_pipeline->draw();
            }

            technique->unbind();
        }
    }
    
    void ces_render_system::draw_recursively(const ces_entity_shared_ptr& entity, const std::string &technique_name, i32 technique_pass)
    {
        bool is_in_frustum = true;
        ces_geometry_component_shared_ptr geometry_component = entity->get_component<ces_geometry_component>();
        ces_transformation_component_shared_ptr transformation_component = entity->get_component<ces_transformation_component>();
		auto convex_hull_component = entity->get_component<ces_convex_hull_component>();

        if(convex_hull_component && transformation_component)
        {
            glm::mat4 absolute_transformation = transformation_component->get_absolute_transformation();
            glm::vec2 min_bound = geometry_component->get_mesh()->get_vbo()->get_min_bound() + glm::vec2(absolute_transformation[3][0],
                                                                                                         absolute_transformation[3][1]);
            glm::vec2 max_bound = geometry_component->get_mesh()->get_vbo()->get_max_bound() + glm::vec2(absolute_transformation[3][0],
                                                                                                         absolute_transformation[3][1]);
            glm::vec4 game_object_bounds = glm::vec4(min_bound, max_bound);
            glm::vec4 camera_bounds = ces_base_system::get_current_camera()->bound;
			is_in_frustum = glm::intersect(game_object_bounds, camera_bounds) || !transformation_component->is_in_camera_space();
        }
        
        auto light_component = entity->get_unsafe_component<ces_light_compoment>();
        if(!light_component && is_in_frustum)
        {
            auto material_component = entity->get_unsafe_component<ces_material_component>();
            
            if(material_component && geometry_component && transformation_component)
            {
                material_shared_ptr material = material_component->get_material(technique_name, technique_pass);
                mesh_shared_ptr mesh = geometry_component->get_mesh();
                if(material && material->get_shader()->is_commited() && mesh && entity->visible)
                {
                    material->set_custom_shader_uniform(k_shadow_color_for_casters, k_shadow_color_uniform);
                    
                    material_component->on_bind(technique_name, technique_pass, material);
                    
                    material->get_shader()->set_mat4(ces_base_system::get_current_camera()->get_mat_p(), e_shader_uniform_mat_p);
                    if(transformation_component->is_in_camera_space())
                    {
                        material->get_shader()->set_mat4(ces_base_system::get_current_camera()->get_mat_v(), e_shader_uniform_mat_v);
                    }
                    else
                    {
                        material->get_shader()->set_mat4(glm::mat4(1.f), e_shader_uniform_mat_v);
                    }
                    
                    glm::mat4 mat_m = transformation_component->get_absolute_transformation();
                    
                    if(material->get_is_batching())
                    {
                        m_batching_pipeline->batch(material, mesh, mat_m, transformation_component->get_absolute_matrix_version());
                    }
                    else
                    {
                        material->get_shader()->set_mat4(mat_m, e_shader_uniform_mat_m);
                        mesh->bind(material->get_shader()->get_guid(), material->get_shader()->get_attributes());
                        mesh->draw();
                        mesh->unbind(material->get_shader()->get_guid(), material->get_shader()->get_attributes());
                    }
                    
                    material_component->on_unbind(technique_name, technique_pass, material);
                }
            }
        }
        
        std::list<ces_entity_shared_ptr> children = entity->children;
        for(const auto& child : children)
        {
            ces_render_system::draw_recursively(child, technique_name, technique_pass);
        }
    }
    
    void ces_render_system::draw_recursively_lights(const ces_entity_shared_ptr& entity, const std::string &technique_name, i32 technique_pass)
    {
        bool is_in_frustum = false;
        ces_geometry_component_shared_ptr geometry_component = entity->get_component<ces_geometry_component>();
        ces_transformation_component_shared_ptr transformation_component = entity->get_component<ces_transformation_component>();
        if(geometry_component && transformation_component)
        {
            glm::mat4 absolute_transformation = transformation_component->get_absolute_transformation();
            glm::vec2 min_bound = geometry_component->get_mesh()->get_vbo()->get_min_bound() + glm::vec2(absolute_transformation[3][0],
                                                                                                         absolute_transformation[3][1]);
            glm::vec2 max_bound = geometry_component->get_mesh()->get_vbo()->get_max_bound() + glm::vec2(absolute_transformation[3][0],
                                                                                                         absolute_transformation[3][1]);
            glm::vec4 game_object_bounds = glm::vec4(min_bound, max_bound);
            glm::vec4 camera_bounds = ces_base_system::get_current_camera()->bound;
            is_in_frustum = glm::intersect(game_object_bounds, camera_bounds);
        }
        
        auto light_component = entity->get_unsafe_component<ces_light_compoment>();
        if(light_component && is_in_frustum)
        {
            auto material_component = entity->get_unsafe_component<ces_material_component>();
            auto light_mask_component = entity->get_unsafe_component<ces_light_mask_component>();
            
            if(material_component && geometry_component && transformation_component)
            {
                material_shared_ptr material = material_component->get_material(technique_name, technique_pass);
                
                mesh_shared_ptr light_main_mesh = geometry_component->get_mesh();
                mesh_shared_ptr light_mask_mesh = light_mask_component->get_mask_mesh();
                mesh_shared_ptr screed_quad_mesh = mesh_constructor::create_screen_quad();
                
                if(material && entity->visible && material->get_shader()->is_commited() &&
                   light_main_mesh && light_mask_mesh)
                {
                    auto draw_light_mask = [=]() {
                        
                        gl_color_mask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
                        gl_depth_mask(GL_FALSE);
                        
                        material->set_stencil_function(GL_ALWAYS);
                        material->set_stencil_function_parameter_1(1);
                        material->set_stencil_function_parameter_2(255);
                        material->set_stencil_mask_parameter(1);

                        material->set_custom_shader_uniform(0, k_light_mask_vs_flag_uniform);
                        material->set_custom_shader_uniform(1, k_light_mask_fs_flag_uniform);
                        
                        material_component->on_bind(technique_name, technique_pass, material);
                        
                        material->get_shader()->set_mat4(ces_base_system::get_current_camera()->get_mat_p(), e_shader_uniform_mat_p);
                        material->get_shader()->set_mat4(ces_base_system::get_current_camera()->get_mat_v(), e_shader_uniform_mat_v);
                        material->get_shader()->set_mat4(glm::mat4(1.f), e_shader_uniform_mat_m);
                        
                        light_mask_mesh->bind(material->get_shader()->get_guid(), material->get_shader()->get_attributes());
                        light_mask_mesh->draw();
                        light_mask_mesh->unbind(material->get_shader()->get_guid(), material->get_shader()->get_attributes());
                        
                        material_component->on_unbind(technique_name, technique_pass, material);
                        
                        std::list<ces_entity_weak_ptr> luminous_entities = light_component->get_luminous_entities();
                        for(const auto& weak_luminous_entity : luminous_entities)
                        {
                            if(!weak_luminous_entity.expired())
                            {
                                auto luminous_entity = weak_luminous_entity.lock();
                                auto luminous_transformation_component = luminous_entity->get_component<ces_transformation_component>();
                                auto luminous_geometry_component = luminous_entity->get_component<ces_geometry_component>();
                                auto luminous_material_component = luminous_entity->get_component<ces_material_component>();
                                
                                material_shared_ptr luminous_material = luminous_material_component->get_material(technique_name, technique_pass);
                                
                                if(luminous_material && luminous_material->get_shader()->is_commited())
                                {
                                    mesh_shared_ptr luminous_mesh = luminous_geometry_component->get_mesh();
                                    
                                    luminous_material_component->on_bind(technique_name, technique_pass, luminous_material);
                                    
                                    glm::mat4 mat_m = luminous_transformation_component->get_absolute_transformation();
                                    
                                    luminous_material->get_shader()->set_mat4(mat_m, e_shader_uniform_mat_m);
                                    
                                    luminous_mesh->bind(luminous_material->get_shader()->get_guid(),
                                                        luminous_material->get_shader()->get_attributes());
                                    luminous_mesh->draw();
                                    luminous_mesh->unbind(luminous_material->get_shader()->get_guid(),
                                                          luminous_material->get_shader()->get_attributes());
                                    
                                    luminous_material_component->on_unbind(technique_name, technique_pass, luminous_material);
                                }
                            }
                        }
                        
                        gl_color_mask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
                        gl_depth_mask(GL_TRUE);
                    };

                    auto draw_light = [=]() {
                        
                        glm::mat4 mat_m = transformation_component->get_absolute_transformation();
                        
                        material->set_stencil_function(GL_EQUAL);
                        material->set_stencil_function_parameter_1(1);
                        material->set_stencil_function_parameter_2(255);
                        material->set_stencil_mask_parameter(0);
                        
                        material->set_blending_function_source(GL_SRC_ALPHA);
                        material->set_blending_function_destination(GL_ONE);
                        
                        material->set_custom_shader_uniform(0, k_light_mask_vs_flag_uniform);
                        material->set_custom_shader_uniform(0, k_light_mask_fs_flag_uniform);
                        
                        material_component->on_bind(technique_name, technique_pass, material);
                        
                        material->get_shader()->set_mat4(mat_m, e_shader_uniform_mat_m);
                        
                        light_main_mesh->bind(material->get_shader()->get_guid(), material->get_shader()->get_attributes());
                        light_main_mesh->draw();
                        light_main_mesh->unbind(material->get_shader()->get_guid(), material->get_shader()->get_attributes());
                        
                        material_component->on_unbind(technique_name, technique_pass, material);
                    };
                    
                    auto clear_light_mask = [=]() {
                        
                        gl_color_mask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
                        gl_depth_mask(GL_FALSE);
                        
                        material->set_stencil_function(GL_ALWAYS);
                        material->set_stencil_function_parameter_1(0);
                        material->set_stencil_function_parameter_2(255);
                        material->set_stencil_mask_parameter(1);
                        
                        material->set_custom_shader_uniform(1, k_light_mask_vs_flag_uniform);
                        material->set_custom_shader_uniform(1, k_light_mask_fs_flag_uniform);

                        material_component->on_bind(technique_name, technique_pass, material);

                        material->get_shader()->set_mat4(glm::mat4(1.f), e_shader_uniform_mat_m);
                        
                        screed_quad_mesh->bind(material->get_shader()->get_guid(), material->get_shader()->get_attributes());
                        screed_quad_mesh->draw();
                        screed_quad_mesh->unbind(material->get_shader()->get_guid(), material->get_shader()->get_attributes());
                        
                        gl_color_mask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
                        gl_depth_mask(GL_TRUE);
                        
                        material_component->on_unbind(technique_name, technique_pass, material);
                    };
                    
                    draw_light_mask();
                    draw_light();
                    clear_light_mask();
                }
            }
        }
        
        std::list<ces_entity_shared_ptr> children = entity->children;
        for(const auto& child : children)
        {
            ces_render_system::draw_recursively_lights(child, technique_name, technique_pass);
        }
    }
    
    void ces_render_system::draw_shadow(const ces_entity_shared_ptr& entity, const std::string &technique_name, i32 technique_pass)
    {
        auto material_component = entity->get_unsafe_component<ces_material_component>();
        material_shared_ptr material = material_component->get_material(technique_name, technique_pass);
        
        auto shadow_component = entity->get_unsafe_component<ces_shadow_component>();
        mesh_shared_ptr mesh = shadow_component->get_shadow_mesh();
        
        material->set_custom_shader_uniform(k_shadow_color_for_receivers, k_shadow_color_uniform);
        
        material_component->on_bind(technique_name, technique_pass, material);
        
        material->get_shader()->set_mat4(ces_base_system::get_current_camera()->get_mat_p(), e_shader_uniform_mat_p);
        material->get_shader()->set_mat4(ces_base_system::get_current_camera()->get_mat_v(), e_shader_uniform_mat_v);
        
        glm::mat4 matrix_m = glm::mat4(1.f);
        material->get_shader()->set_mat4(matrix_m, e_shader_uniform_mat_m);
        
        mesh->bind(material->get_shader()->get_guid(),
                   material->get_shader()->get_attributes());
        mesh->draw();
        mesh->unbind(material->get_shader()->get_guid(),
                     material->get_shader()->get_attributes());
        
        material_component->on_unbind(technique_name, technique_pass, material);
    }
    
    void ces_render_system::on_feed_end(f32 deltatime)
    {
        m_render_pipeline->on_draw_end();
    }
}

#endif
