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
#include "ces_transformation_2d_component.h"
#include "ces_light_compoment.h"
#include "ces_shadow_component.h"
#include "ces_light_mask_component.h"
#include "ces_convex_hull_component.h"
#include "ces_animation_3d_mixer_component.h"
#include "render_technique_ws.h"
#include "material.h"
#include "mesh_2d.h"
#include "vbo.h"
#include "camera_2d.h"
#include "camera_3d.h"
#include "graphics_context.h"
#include "mesh_constructor.h"
#include "glm_extensions.h"
#include "ces_geometry_extension.h"
#include "mesh_constructor.h"
#include "ogl_graveyard_controller.h"

#define k_camera_trashhold 64.f;

namespace gb
{
    static const std::string k_bones_uniform = "u_bones";
    static const std::string k_shadow_color_uniform = "u_shadow_color";
    static const std::string k_light_mask_vs_flag_uniform = "u_mask_flag_vs";
    static const std::string k_light_mask_fs_flag_uniform = "u_mask_flag_fs";
    static const glm::vec4 k_shadow_color_for_casters = glm::vec4(1.f);
    static const glm::vec4 k_shadow_color_for_receivers = glm::vec4(0.f, 1.f, 0.f, .75f);
    
    ces_render_system::ces_render_system(const std::shared_ptr<graphics_context>& graphic_context, bool is_offscreen) :
    m_camera_2d_mesh(nullptr),
    m_camera_2d_bounds(0.f)
    {
        m_render_pipeline = std::make_shared<render_pipeline>(graphic_context, is_offscreen);
        m_batching_pipeline = std::make_shared<batching_pipeline>();
        m_ogl_graveyard_controller = std::make_shared<gb::ogl_graveyard_controller>();
        m_camera_2d_mesh = mesh_constructor::create_shape_quad();
    }
    
    ces_render_system::~ces_render_system()
    {
        
    }
    
    std::shared_ptr<render_pipeline> ces_render_system::get_render_pipeline() const
    {
        return m_render_pipeline;
    }
    
    void ces_render_system::on_feed_start(f32 dt)
    {
        m_camera_2d_bounds = ces_base_system::get_current_camera_2d()->bound;
        m_camera_2d_bounds.x -= k_camera_trashhold;
        m_camera_2d_bounds.y -= k_camera_trashhold;
        m_camera_2d_bounds.z += k_camera_trashhold;
        m_camera_2d_bounds.w += k_camera_trashhold;
        vbo::vertex_attribute_P* vertices = m_camera_2d_mesh->get_vbo()->lock<vbo::vertex_attribute_P>();;
        
        vertices[0].m_position.x = m_camera_2d_bounds.x;
        vertices[0].m_position.y = m_camera_2d_bounds.y;
        
        vertices[1].m_position.x = m_camera_2d_bounds.x;
        vertices[1].m_position.y = m_camera_2d_bounds.w;
        
        vertices[2].m_position.x = m_camera_2d_bounds.z;
        vertices[2].m_position.y = m_camera_2d_bounds.y;
        
        vertices[3].m_position.x = m_camera_2d_bounds.z;
        vertices[3].m_position.y = m_camera_2d_bounds.w;
        
        m_ogl_graveyard_controller->update(dt);
        
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
            
            for(i32 technique_pass = 0; technique_pass < technique->get_num_passes(); ++technique_pass)
            {
                ces_render_system::grab_visible_lights_recursively(entity, technique_name, technique_pass);
                ces_render_system::grab_visible_entities_recursively(entity, technique_name, technique_pass);
            }
        }
    }
    
    void ces_render_system::grab_visible_entities_recursively(const ces_entity_shared_ptr& entity, const std::string &technique_name, i32 technique_pass)
    {
        bool is_visible = entity->visible && entity->visible_in_next_frame;
        if(is_visible)
        {
            auto light_component = entity->get_component<ces_light_compoment>();
            if(!light_component)
            {
                auto geometry_component = entity->get_component<ces_geometry_component>();
                auto transformation_component = entity->get_component<ces_transformation_2d_component>();
                auto material_component = entity->get_component<ces_material_component>();
                
                if(geometry_component && transformation_component && material_component)
                {
                    auto material = material_component->get_material(technique_name, technique_pass);
                    auto mesh = geometry_component->get_mesh();
                    if(material && material->get_shader()->is_commited() && mesh)
                    {
                        is_visible = !transformation_component->is_in_camera_space();
                        if(!is_visible)
                        {
                            is_visible = glm::intersect(m_camera_2d_bounds, ces_geometry_extension::get_absolute_position_bounds(entity));
                        }
                        
                        if(is_visible)
                        {
                            m_visible_entities[technique_name][technique_pass].push(entity);
                        }
                    }
                }
            }
        }
        
        std::list<ces_entity_shared_ptr> children = entity->children;
        for(const auto& child : children)
        {
            ces_render_system::grab_visible_entities_recursively(child, technique_name, technique_pass);
        }
    }
    
    void ces_render_system::grab_visible_lights_recursively(const ces_entity_shared_ptr& entity, const std::string &technique_name, i32 technique_pass)
    {
        bool is_visible = entity->visible && entity->visible_in_next_frame;
        if(is_visible)
        {
            auto light_component = entity->get_component<ces_light_compoment>();
            if(light_component)
            {
                auto geometry_component = entity->get_component<ces_geometry_component>();
                auto transformation_component = entity->get_component<ces_transformation_2d_component>();
                auto material_component = entity->get_component<ces_material_component>();
                if(geometry_component && transformation_component && material_component)
                {
                    auto material = material_component->get_material(technique_name, technique_pass);
                    auto mesh = geometry_component->get_mesh();
                    
                    auto light_mask_component = entity->get_component<ces_light_mask_component>();
                    auto light_mask_mesh = light_mask_component->get_mesh();
                    
                    if(material && material->get_shader()->is_commited() && mesh && light_mask_mesh)
                    {
                        glm::mat4 absolute_transformation = transformation_component->get_absolute_transformation();
                        
                        is_visible = !transformation_component->is_in_camera_space();
                        if(!is_visible)
                        {
                            is_visible = gb::mesh_2d::intersect(mesh->get_vbo(), mesh->get_ibo(), absolute_transformation, true,
                                                                m_camera_2d_mesh->get_vbo(), m_camera_2d_mesh->get_ibo(), glm::mat4(1.f), false);
                        }
                        
                        if(is_visible)
                        {
                            m_visible_lights[technique_name][technique_pass].push(entity);
                        }
                    }
                }
            }
        }
        
        std::list<ces_entity_shared_ptr> children = entity->children;
        for(const auto& child : children)
        {
            ces_render_system::grab_visible_lights_recursively(child, technique_name, technique_pass);
        }
    }
    
    void ces_render_system::draw_entities(const std::string &technique_name, i32 technique_pass)
    {
        if(m_visible_entities.find(technique_name) != m_visible_entities.end())
        {
            if(m_visible_entities[technique_name].find(technique_pass) != m_visible_entities[technique_name].end())
            {
                while(!m_visible_entities[technique_name][technique_pass].empty())
                {
                    auto entity_weak = m_visible_entities[technique_name][technique_pass].front();
                    if(!entity_weak.expired())
                    {
                        auto entity = entity_weak.lock();
                        
                        auto geometry_component = entity->get_component<ces_geometry_component>();
                        auto transformation_component = entity->get_component<ces_transformation_2d_component>();
                        auto material_component = entity->get_component<ces_material_component>();
                        
                        auto material = material_component->get_material(technique_name, technique_pass);
                        auto mesh = geometry_component->get_mesh();
                        
                        material->set_custom_shader_uniform(k_shadow_color_for_casters, k_shadow_color_uniform);
                        material_component->on_bind(technique_name, technique_pass, material);
                        
                        material->get_shader()->set_mat4(ces_base_system::get_current_camera_2d()->get_mat_p(), e_shader_uniform_mat_p);
                        if(transformation_component->is_in_camera_space())
                        {
                            material->get_shader()->set_mat4(ces_base_system::get_current_camera_2d()->get_mat_v(), e_shader_uniform_mat_v);
                        }
                        else
                        {
                            material->get_shader()->set_mat4(glm::mat4(1.f), e_shader_uniform_mat_v);
                        }
                        
                        auto animation_3d_mixer_component = entity->get_component<ces_animation_3d_mixer_component>();
                        if(animation_3d_mixer_component)
                        {
                            material->get_shader()->set_mat4(ces_base_system::get_current_camera_3d()->get_mat_p(), e_shader_uniform_mat_p);
                            material->get_shader()->set_mat4(ces_base_system::get_current_camera_3d()->get_mat_v(), e_shader_uniform_mat_v);
                            material->get_shader()->set_custom_mat4_array(animation_3d_mixer_component->get_transformations(), animation_3d_mixer_component->get_transformation_size(), k_bones_uniform);
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
                    m_visible_entities[technique_name][technique_pass].pop();
                }
            }
        }
    }
    
    void ces_render_system::draw_lights(const std::string &technique_name, i32 technique_pass)
    {
        if(m_visible_lights.find(technique_name) != m_visible_lights.end())
        {
            if(m_visible_lights[technique_name].find(technique_pass) != m_visible_lights[technique_name].end())
            {
                while(!m_visible_lights[technique_name][technique_pass].empty())
                {
                    auto entity_weak = m_visible_lights[technique_name][technique_pass].front();
                    if(!entity_weak.expired())
                    {
                        auto entity = entity_weak.lock();
                        
                        auto geometry_component = entity->get_component<ces_geometry_component>();
                        auto transformation_component = entity->get_component<ces_transformation_2d_component>();
                        auto material_component = entity->get_component<ces_material_component>();
                        auto light_mask_component = entity->get_component<ces_light_mask_component>();
                        
                        auto material = material_component->get_material(technique_name, technique_pass);
                        auto mesh = geometry_component->get_mesh();
                        
                        auto light_mask_mesh = light_mask_component->get_mesh();
                        auto screed_quad_mesh = mesh_constructor::create_screen_quad();
                        
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
                            
                            material->get_shader()->set_mat4(ces_base_system::get_current_camera_2d()->get_mat_p(), e_shader_uniform_mat_p);
                            material->get_shader()->set_mat4(ces_base_system::get_current_camera_2d()->get_mat_v(), e_shader_uniform_mat_v);
                            material->get_shader()->set_mat4(glm::mat4(1.f), e_shader_uniform_mat_m);
                            
                            light_mask_mesh->bind(material->get_shader()->get_guid(), material->get_shader()->get_attributes());
                            light_mask_mesh->draw();
                            light_mask_mesh->unbind(material->get_shader()->get_guid(), material->get_shader()->get_attributes());
                            
                            material_component->on_unbind(technique_name, technique_pass, material);
                            
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
                            
                            mesh->bind(material->get_shader()->get_guid(), material->get_shader()->get_attributes());
                            mesh->draw();
                            mesh->unbind(material->get_shader()->get_guid(), material->get_shader()->get_attributes());
                            
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
                    
                    m_visible_lights[technique_name][technique_pass].pop();
                }
            }
        }
    }
    
    void ces_render_system::on_feed_end(f32 dt)
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
                ces_render_system::draw_lights(technique_name, technique_pass);
                ces_render_system::draw_entities(technique_name, technique_pass);
                m_batching_pipeline->draw();
            }
            technique->unbind();
        }
        m_render_pipeline->on_draw_end();
    }
}

#endif
