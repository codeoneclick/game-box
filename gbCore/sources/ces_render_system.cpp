//
//  ces_render_system.cpp
//  gbCore
//
//  Created by sergey.sergeev on 8/17/15.
//  Copyright (c) 2015 sergey.sergeev. All rights reserved.
//

#include "ces_render_system.h"

#if USED_GRAPHICS_API != NO_GRAPHICS_API

#include "render_pipeline.h"
#include "batching_pipeline.h"
#include "ces_geometry_component.h"
#include "ces_material_component.h"
#include "ces_transformation_2d_component.h"
#include "ces_transformation_3d_component.h"
#include "ces_light_compoment.h"
#include "ces_shadow_component.h"
#include "ces_light_mask_component.h"
#include "ces_convex_hull_component.h"
#include "ces_animation_3d_mixer_component.h"
#include "ces_render_target_component.h"
#include "ces_shader_uniforms_component.h"
#include "render_technique_ws.h"
#include "render_technique_ss.h"
#include "render_technique_main.h"
#include "material.h"
#include "mesh_2d.h"
#include "mesh_3d.h"
#include "vbo.h"
#include "camera_2d.h"
#include "camera_3d.h"
#include "frustum_3d.h"
#include "graphics_context.h"
#include "mesh_constructor.h"
#include "glm_extensions.h"
#include "ces_geometry_extension.h"
#include "mesh_constructor.h"
#include "ogl_graveyard_controller.h"
#include "render_target.h"
#include "ces_render_technique_uniforms_component.h"
#include "ces_camera_collision_component.h"

#if USED_GRAPHICS_API == METAL_API

#include "mtl_render_encoder.h"
#include "mtl_buffer.h"

#endif

#define k_camera_trashhold 64.f;

namespace gb
{
    static const std::string k_bones_uniform = "u_bones";
    
    ces_render_system::ces_render_system(const std::shared_ptr<graphics_context>& graphic_context, bool is_offscreen) :
    m_camera_2d_mesh(nullptr),
    m_camera_2d_bounds(0.f)
    {
        m_render_pipeline = std::make_shared<render_pipeline>(graphic_context, is_offscreen);
        
#if defined(__USE_BATCHING__)
        
        m_batching_pipeline = std::make_shared<batching_pipeline>();
        
#endif
        
#if USED_GRAPHICS_API != METAL_API
        
        m_ogl_graveyard_controller = std::make_shared<gb::ogl_graveyard_controller>();
        
#endif
        
        m_camera_2d_mesh = mesh_constructor::create_shape_quad();
        
        ces_base_system::add_required_component_guid(m_render_components_mask, ces_transformation_component::class_guid());
        ces_base_system::add_required_component_guid(m_render_components_mask, ces_geometry_component::class_guid());
        ces_base_system::add_required_component_guid(m_render_components_mask, ces_material_component::class_guid());
        ces_base_system::add_required_components_mask(m_render_components_mask);
        
        ces_base_system::add_required_component_guid(m_render_target_components_mask, ces_render_target_component::class_guid());
        ces_base_system::add_required_components_mask(m_render_target_components_mask);
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
        
#if USED_GRAPHICS_API != METAL_API
        
        m_ogl_graveyard_controller->update(dt);
        
#endif
        
        m_render_pipeline->on_draw_begin();
    }
    
    void ces_render_system::on_feed(const ces_entity_shared_ptr& root, f32 deltatime)
    {
        const auto render_technique_uniforms_component = root->get_component<ces_render_technique_uniforms_component>();
        if (render_technique_uniforms_component)
        {
            const auto ss_techniques = m_render_pipeline->get_ss_techniques();
            for (auto ss_technique_it : ss_techniques)
            {
                std::size_t name_position = ss_technique_it->get_name().find("_") + 1;
                assert(name_position < ss_technique_it->get_name().size());
                std::string technique_name = ss_technique_it->get_name().substr(name_position);
                const auto uniforms_wrapper = render_technique_uniforms_component->get_uniforms(technique_name);
                if (uniforms_wrapper)
                {
                    const auto uniforms = uniforms_wrapper->get_uniforms();
                    ss_technique_it->get_material()->set_custom_shader_uniforms(uniforms);
                    ss_technique_it->set_uniforms(uniforms_wrapper);
                }
            }
            
            const auto main_technique = m_render_pipeline->get_main_technique();
            std::size_t name_position = main_technique->get_name().find("_") + 1;
            assert(name_position < main_technique->get_name().size());
            std::string technique_name = main_technique->get_name().substr(name_position);
            const auto uniforms_wrapper = render_technique_uniforms_component->get_uniforms(technique_name);
            if (uniforms_wrapper)
            {
                const auto uniforms = uniforms_wrapper->get_uniforms();
                main_technique->get_material()->set_custom_shader_uniforms(uniforms);
                main_technique->set_uniforms(uniforms_wrapper);
            }
        }
    }
    
    void ces_render_system::grab_visible_entities(const std::string &technique_name, const instance_draw_3d_entity_func_t& instance_draw_3d_entity_func)
    {
        auto technique_name_reference_it = m_visible_2d_entities.find(technique_name);
        if(technique_name_reference_it != m_visible_2d_entities.end())
        {
            technique_name_reference_it->second.clear();
        }
        
        const auto camera_3d = get_current_camera_3d();
        const auto camera_3d_position = camera_3d->get_position();
        const auto frustum_3d = camera_3d->get_frustum();
        
        ces_base_system::enumerate_entities_with_components(m_render_components_mask, [=](const ces_entity_shared_ptr& entity) {
            
            bool is_visible = entity->get_is_visible() && entity->get_is_visible_in_next_frame();
            if(is_visible)
            {
                const auto& material_component = entity->get_component<ces_material_component>();
                const auto& geometry_component = entity->get_component<ces_geometry_component>();
                
                const auto& material = material_component->get_material(technique_name);
                const auto& mesh = geometry_component->get_mesh();
                
                if(material && material->get_shader()->is_commited() && mesh)
                {
                    const auto& transformation_component = entity->get_component<ces_transformation_component>();
                    
                    if (transformation_component->is_2d())
                    {
                        is_visible = !transformation_component->is_in_camera_space();
                        if(!is_visible)
                        {
                            is_visible = glm::intersect(m_camera_2d_bounds, ces_geometry_extension::get_absolute_position_bounds(entity));
                        }
                    }
                    else if(transformation_component->is_3d())
                    {
                        if (transformation_component->is_in_camera_space())
                        {
                            ces_geometry_component::e_bounding_mode bounding_mode = geometry_component->bounding_mode;
                            if (bounding_mode == ces_geometry_component::e_box)
                            {
                                const auto max_bound = mesh->as_3d()->get_max_bound();
                                const auto min_bound = mesh->as_3d()->get_min_bound();
                                const auto mat_m = transformation_component->get_absolute_transformation();
                                i32 result = frustum_3d->is_bound_box_in_frustum(max_bound, min_bound, mat_m);
                                if((result == frustum_3d::e_frustum_bounds_result_inside ||
                                    result == frustum_3d::e_frustum_bounds_result_intersect))
                                {
                                    is_visible = true;
                                    const auto& camera_collision_component = entity->get_component<ces_camera_collision_component>();
                                    if (camera_collision_component)
                                    {
                                        glm::vec3 absolute_min_bound = glm::transform(min_bound, mat_m);
                                        glm::vec3 absolute_max_bound = glm::transform(max_bound, mat_m);
                                        if (camera_3d_position.x >= absolute_min_bound.x && camera_3d_position.x <= absolute_max_bound.x &&
                                            camera_3d_position.y >= absolute_min_bound.y && camera_3d_position.y <= absolute_max_bound.y &&
                                            camera_3d_position.z >= absolute_min_bound.z && camera_3d_position.z <= absolute_max_bound.z)
                                        {
                                            is_visible = false;
                                        }
                                    }
                                    
                                }
                                else
                                {
                                    is_visible = false;
                                }
                            }
                            if (bounding_mode == ces_geometry_component::e_radius)
                            {
                                const auto position = transformation_component->as_3d()->get_absolute_position();
                                const auto radius = geometry_component->get_bounding_radius();
                                i32 result = frustum_3d->is_sphere_in_frustum(position, radius);
                                if((result == frustum_3d::e_frustum_bounds_result_inside ||
                                    result == frustum_3d::e_frustum_bounds_result_intersect))
                                {
                                    is_visible = true;
                                }
                                else
                                {
                                    is_visible = false;
                                }
                            }
                        }
                    }
                    else
                    {
                        assert(false);
                    }
                    
                    if(is_visible)
                    {
                        if(transformation_component->is_2d())
                        {
                            f32 z_order = transformation_component->as_2d()->get_z_order();
                            m_visible_2d_entities[technique_name][z_order] = entity;
                        }
                        else if(transformation_component->is_3d())
                        {
                            if (instance_draw_3d_entity_func)
                            {
                                instance_draw_3d_entity_func(technique_name,
                                                             material_component,
                                                             geometry_component,
                                                             transformation_component,
                                                             entity->get_component<ces_shader_uniforms_component>());
                            }
                            else
                            {
                                m_visible_3d_entities[technique_name].push(entity);
                            }
                        }
                    }
                }
            }
        });
    }
    
    void ces_render_system::draw_2d_entities(const std::string &technique_name)
    {
        if(m_visible_2d_entities.find(technique_name) != m_visible_2d_entities.end())
        {
            for(const auto& entity_it : m_visible_2d_entities[technique_name])
            {
                auto entity_weak = entity_it.second;
                if(!entity_weak.expired())
                {
                    auto entity = entity_weak.lock();
                    
                    auto geometry_component = entity->get_component<ces_geometry_component>();
                    auto transformation_component = entity->get_component<ces_transformation_2d_component>();
                    auto material_component = entity->get_component<ces_material_component>();
                    
                    const auto shader_uniforms_component = entity->get_component<ces_shader_uniforms_component>();
                    if (shader_uniforms_component)
                    {
                        auto buffers = shader_uniforms_component->get_vertex_buffers();
                        for (const auto& buffer_it : buffers)
                        {
                            const auto uniforms = buffer_it.second->get_uniforms();
                            material_component->set_custom_shader_uniforms(uniforms, technique_name);
                        }
                        
                        buffers = shader_uniforms_component->get_fragment_buffers();
                        for (const auto& buffer_it : buffers)
                        {
                            const auto uniforms = buffer_it.second->get_uniforms();
                            material_component->set_custom_shader_uniforms(uniforms, technique_name);
                        }
                    }
                    
                    auto material = material_component->get_material(technique_name);
                    auto mesh = geometry_component->get_mesh();
                    
                    
#if USED_GRAPHICS_API == VULKAN_API
                    
                    material_component->on_bind(technique_name, mesh->get_vbo()->get_vertex_input_state(), material);
                    
#elif USED_GRAPHICS_API == METAL_API
                    
                    material_component->on_bind(technique_name, mesh->get_vbo()->get_mtl_vertex_descriptor(), material);
                    
#else
                    
                    material_component->on_bind(technique_name, material);
                    
#endif
                    
                    material->get_shader()->set_mat4(ces_base_system::get_current_camera_2d()->get_mat_p(), e_shader_uniform_mat_p);
                    if(transformation_component->is_in_camera_space())
                    {
                        material->get_shader()->set_mat4(ces_base_system::get_current_camera_2d()->get_mat_v(), e_shader_uniform_mat_v);
                    }
                    else
                    {
                        material->get_shader()->set_mat4(glm::mat4(1.f), e_shader_uniform_mat_v);
                    }
                    
                    glm::mat4 mat_m = transformation_component->get_absolute_transformation();
                    glm::mat4 mat_n = glm::transpose(glm::inverse(mat_m));
                    
#if defined(__USE_BATCHING__)
                    
                    if(material->get_is_batching())
                    {
                        m_batching_pipeline->batch(material, mesh, mat_m, transformation_component->get_absolute_matrix_version());
                    }
                    else
                    
#endif
                    
                    {
                        material->get_shader()->set_mat4(mat_m, e_shader_uniform_mat_m);
                        material->get_shader()->set_mat4(mat_n, e_shader_uniform_mat_n);
                        mesh->bind(material->get_shader()->get_guid(), material->get_shader()->get_attributes());
                        mesh->draw();
                        mesh->unbind(material->get_shader()->get_guid(), material->get_shader()->get_attributes());
                        
#if USED_GRAPHICS_API == METAL_API
                        
                        const auto vbo_mtl_buffer_id = mesh->get_vbo()->get_mtl_buffer_id();
                        const auto ibo_mtl_buffer_id = mesh->get_ibo()->get_mtl_buffer_id();
                        const auto mvp_uniforms = material->get_shader()->get_mvp_uniforms();
                        const auto render_encoder = material->get_render_encoder();
                        const auto mvp_uniforms_buffer_id = material->get_mvp_uniforms_buffer();
                        mvp_uniforms_buffer_id->update((void*)&mvp_uniforms, sizeof(shader_mvp_uniforms));
                        render_encoder->set_vertex_buffer(vbo_mtl_buffer_id, 0);
                        render_encoder->set_vertex_uniforms(mvp_uniforms_buffer_id, 1);
                        if (shader_uniforms_component)
                        {
                            auto buffers = shader_uniforms_component->get_vertex_buffers();
                            for (const auto& buffer_it : buffers)
                            {
                                void* custom_uniforms_value = buffer_it.second->get_values();
                                ui32 custom_uniforms_size = buffer_it.second->get_values_size();
                                const auto custom_uniforms_buffer_id = material->get_custom_uniform_buffer(custom_uniforms_size);
                                custom_uniforms_buffer_id->update(custom_uniforms_value, custom_uniforms_size);
                                
                                if (buffer_it.second->get_mode() == ces_shader_uniforms_component::e_shader_uniform_mode::e_vertex)
                                {
                                    render_encoder->set_vertex_uniforms(custom_uniforms_buffer_id, buffer_it.first);
                                }
                            }
                            
                            buffers = shader_uniforms_component->get_fragment_buffers();
                            for (const auto& buffer_it : buffers)
                            {
                                void* custom_uniforms_value = buffer_it.second->get_values();
                                ui32 custom_uniforms_size = buffer_it.second->get_values_size();
                                const auto custom_uniforms_buffer_id = material->get_custom_uniform_buffer(custom_uniforms_size);
                                custom_uniforms_buffer_id->update(custom_uniforms_value, custom_uniforms_size);
                                
                                if (buffer_it.second->get_mode() == ces_shader_uniforms_component::e_shader_uniform_mode::e_fragment)
                                {
                                    render_encoder->set_fragment_uniforms(custom_uniforms_buffer_id, buffer_it.first);
                                }
                            }
                        }
                        render_encoder->set_index_buffer(ibo_mtl_buffer_id, mesh->get_ibo()->get_used_size(), 0);
                        render_encoder->draw(technique_name);
                        
#endif
                        
                    }
                    material_component->on_unbind(technique_name, material);
                }
            }
        }
    }
    
    void ces_render_system::draw_3d_entities(const std::string &technique_name)
    {
        if(m_visible_3d_entities.find(technique_name) != m_visible_3d_entities.end())
        {
            while(!m_visible_3d_entities[technique_name].empty())
            {
                auto entity_weak = m_visible_3d_entities[technique_name].front();
                if(!entity_weak.expired())
                {
                    auto entity = entity_weak.lock();
                    
                    auto geometry_component = entity->get_component<ces_geometry_component>();
                    auto transformation_component = entity->get_component<ces_transformation_3d_component>();
                    auto material_component = entity->get_component<ces_material_component>();
                    
                    const auto shader_uniforms_component = entity->get_component<ces_shader_uniforms_component>();
                    if (shader_uniforms_component)
                    {
                        auto buffers = shader_uniforms_component->get_vertex_buffers();
                        for (const auto& buffer_it : buffers)
                        {
                            const auto uniforms = buffer_it.second->get_uniforms();
                            material_component->set_custom_shader_uniforms(uniforms, technique_name);
                        }
                        
                        buffers = shader_uniforms_component->get_fragment_buffers();
                        for (const auto& buffer_it : buffers)
                        {
                            const auto uniforms = buffer_it.second->get_uniforms();
                            material_component->set_custom_shader_uniforms(uniforms, technique_name);
                        }
                    }
                    
                    auto material = material_component->get_material(technique_name);
                    auto mesh = geometry_component->get_mesh();
                    
#if USED_GRAPHICS_API == VULKAN_API
                    
                    material_component->on_bind(technique_name, mesh->get_vbo()->get_vertex_input_state(), material);
                    
#elif USED_GRAPHICS_API == METAL_API
                    
                    material_component->on_bind(technique_name, mesh->get_vbo()->get_mtl_vertex_descriptor(), material);
                    
#else
                    
                    material_component->on_bind(technique_name, material);
                    
#endif
                    
                    material->get_shader()->set_mat4(ces_base_system::get_current_camera_3d()->get_mat_p(), e_shader_uniform_mat_p);
                    material->get_shader()->set_mat4(ces_base_system::get_current_camera_3d()->get_mat_v(), e_shader_uniform_mat_v);
                    
                    auto animation_3d_mixer_component = entity->get_component<ces_animation_3d_mixer_component>();
                    if(animation_3d_mixer_component)
                    {
                        material->get_shader()->set_custom_mat4_array(animation_3d_mixer_component->get_transformations(), animation_3d_mixer_component->get_transformation_size(), k_bones_uniform);
                    }
                    
                    glm::mat4 mat_m = transformation_component->get_absolute_transformation();
                    glm::mat4 mat_n = glm::transpose(glm::inverse(mat_m));
                    
#if defined(__USE_BATCHING__)
                    
                    if(material->get_is_batching())
                    {
                        m_batching_pipeline->batch(material, mesh, mat_m, transformation_component->get_matrix_m_version());
                    }
                    else
                    
#endif
                    
                    {
                        material->get_shader()->set_mat4(mat_m, e_shader_uniform_mat_m);
                        material->get_shader()->set_mat4(mat_n, e_shader_uniform_mat_n);
                        mesh->bind(material->get_shader()->get_guid(), material->get_shader()->get_attributes());
                        mesh->draw();
                        mesh->unbind(material->get_shader()->get_guid(), material->get_shader()->get_attributes());
                        
#if USED_GRAPHICS_API == METAL_API
                        
                        const auto vbo_mtl_buffer_id = mesh->get_vbo()->get_mtl_buffer_id();
                        const auto ibo_mtl_buffer_id = mesh->get_ibo()->get_mtl_buffer_id();
                        const auto mvp_uniforms = material->get_shader()->get_mvp_uniforms();
                        const auto render_encoder = material->get_render_encoder();
                        const auto mvp_uniforms_buffer_id = material->get_mvp_uniforms_buffer();
                        mvp_uniforms_buffer_id->update((void*)&mvp_uniforms, sizeof(shader_mvp_uniforms));
                        render_encoder->set_vertex_buffer(vbo_mtl_buffer_id, 0);
                        render_encoder->set_vertex_uniforms(mvp_uniforms_buffer_id, 1);
                        if (shader_uniforms_component)
                        {
                            auto buffers = shader_uniforms_component->get_vertex_buffers();
                            for (const auto& buffer_it : buffers)
                            {
                                void* custom_uniforms_value = buffer_it.second->get_values();
                                ui32 custom_uniforms_size = buffer_it.second->get_values_size();
                                const auto custom_uniforms_buffer_id = material->get_custom_uniform_buffer(custom_uniforms_size);
                                custom_uniforms_buffer_id->update(custom_uniforms_value, custom_uniforms_size);
                                
                                if (buffer_it.second->get_mode() == ces_shader_uniforms_component::e_shader_uniform_mode::e_vertex)
                                {
                                    render_encoder->set_vertex_uniforms(custom_uniforms_buffer_id, buffer_it.first);
                                }
                            }
                            
                            buffers = shader_uniforms_component->get_fragment_buffers();
                            for (const auto& buffer_it : buffers)
                            {
                                void* custom_uniforms_value = buffer_it.second->get_values();
                                ui32 custom_uniforms_size = buffer_it.second->get_values_size();
                                const auto custom_uniforms_buffer_id = material->get_custom_uniform_buffer(custom_uniforms_size);
                                custom_uniforms_buffer_id->update(custom_uniforms_value, custom_uniforms_size);
                                
                                if (buffer_it.second->get_mode() == ces_shader_uniforms_component::e_shader_uniform_mode::e_fragment)
                                {
                                    render_encoder->set_fragment_uniforms(custom_uniforms_buffer_id, buffer_it.first);
                                }
                            }
                        }
                        render_encoder->set_index_buffer(ibo_mtl_buffer_id, mesh->get_ibo()->get_used_size(), 0);
                        render_encoder->draw(technique_name);
                        
#endif
                        
                    }
                    material_component->on_unbind(technique_name, material);
                }
                m_visible_3d_entities[technique_name].pop();
            }
        }
    }
    
    void ces_render_system::instance_draw_3d_entity(const std::string& technique_name,
                                                    const ces_material_component_shared_ptr& material_component,
                                                    const ces_geometry_component_shared_ptr& geometry_component,
                                                    const ces_transformation_component_shared_ptr& transformation_component,
                                                    const ces_shader_uniforms_component_shared_ptr& shader_uniforms_component)
    {
        if (shader_uniforms_component)
        {
            const auto& vertex_buffers = shader_uniforms_component->get_vertex_buffers();
            for (const auto& buffer_it : vertex_buffers)
            {
                const auto& uniforms = buffer_it.second->get_uniforms();
                material_component->set_custom_shader_uniforms(uniforms, technique_name);
            }
            
            const auto& fragment_buffers = shader_uniforms_component->get_fragment_buffers();
            for (const auto& buffer_it : fragment_buffers)
            {
                const auto uniforms = buffer_it.second->get_uniforms();
                material_component->set_custom_shader_uniforms(uniforms, technique_name);
            }
        }
        
        const auto& material = material_component->get_material(technique_name);
        const auto& mesh = geometry_component->get_mesh();
        
#if USED_GRAPHICS_API == VULKAN_API
        
        material_component->on_bind(technique_name, mesh->get_vbo()->get_vertex_input_state(), material);
        
#elif USED_GRAPHICS_API == METAL_API
        
        material_component->on_bind(technique_name, mesh->get_vbo()->get_mtl_vertex_descriptor(), material);
        
#else
        
        material_component->on_bind(technique_name, material);
        
#endif
        
        material->get_shader()->set_mat4(ces_base_system::get_current_camera_3d()->get_mat_p(), e_shader_uniform_mat_p);
        material->get_shader()->set_mat4(ces_base_system::get_current_camera_3d()->get_mat_v(), e_shader_uniform_mat_v);
        
        glm::mat4 mat_m = transformation_component->get_absolute_transformation();
        glm::mat4 mat_n = glm::transpose(glm::inverse(mat_m));
        
#if defined(__USE_BATCHING__)
        
        if(material->get_is_batching())
        {
            m_batching_pipeline->batch(material, mesh, mat_m, transformation_component->get_matrix_m_version());
        }
        else
        
#endif
        
        {
            material->get_shader()->set_mat4(mat_m, e_shader_uniform_mat_m);
            material->get_shader()->set_mat4(mat_n, e_shader_uniform_mat_n);
            mesh->bind(material->get_shader()->get_guid(), material->get_shader()->get_attributes());
            mesh->draw();
            mesh->unbind(material->get_shader()->get_guid(), material->get_shader()->get_attributes());
            
#if USED_GRAPHICS_API == METAL_API
            
            const auto vbo_mtl_buffer_id = mesh->get_vbo()->get_mtl_buffer_id();
            const auto ibo_mtl_buffer_id = mesh->get_ibo()->get_mtl_buffer_id();
            const auto mvp_uniforms = material->get_shader()->get_mvp_uniforms();
            const auto render_encoder = material->get_render_encoder();
            const auto mvp_uniforms_buffer_id = material->get_mvp_uniforms_buffer();
            mvp_uniforms_buffer_id->update((void*)&mvp_uniforms, sizeof(shader_mvp_uniforms));
            render_encoder->set_vertex_buffer(vbo_mtl_buffer_id, 0);
            render_encoder->set_vertex_uniforms(mvp_uniforms_buffer_id, 1);
            if (shader_uniforms_component)
            {
                auto buffers = shader_uniforms_component->get_vertex_buffers();
                for (const auto& buffer_it : buffers)
                {
                    void* custom_uniforms_value = buffer_it.second->get_values();
                    ui32 custom_uniforms_size = buffer_it.second->get_values_size();
                    const auto custom_uniforms_buffer_id = material->get_custom_uniform_buffer(custom_uniforms_size);
                    custom_uniforms_buffer_id->update(custom_uniforms_value, custom_uniforms_size);
                    
                    if (buffer_it.second->get_mode() == ces_shader_uniforms_component::e_shader_uniform_mode::e_vertex)
                    {
                        render_encoder->set_vertex_uniforms(custom_uniforms_buffer_id, buffer_it.first);
                    }
                }
                
                buffers = shader_uniforms_component->get_fragment_buffers();
                for (const auto& buffer_it : buffers)
                {
                    void* custom_uniforms_value = buffer_it.second->get_values();
                    ui32 custom_uniforms_size = buffer_it.second->get_values_size();
                    const auto custom_uniforms_buffer_id = material->get_custom_uniform_buffer(custom_uniforms_size);
                    custom_uniforms_buffer_id->update(custom_uniforms_value, custom_uniforms_size);
                    
                    if (buffer_it.second->get_mode() == ces_shader_uniforms_component::e_shader_uniform_mode::e_fragment)
                    {
                        render_encoder->set_fragment_uniforms(custom_uniforms_buffer_id, buffer_it.first);
                    }
                }
            }
            render_encoder->set_index_buffer(ibo_mtl_buffer_id, mesh->get_ibo()->get_used_size(), 0);
            render_encoder->draw(technique_name);
            
#endif
            
        }
        material_component->on_unbind(technique_name, material);
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
            
            ces_render_system::grab_visible_entities(technique_name, std::bind(&ces_render_system::instance_draw_3d_entity, this,
                                                                               std::placeholders::_1,
                                                                               std::placeholders::_2,
                                                                               std::placeholders::_3,
                                                                               std::placeholders::_4,
                                                                               std::placeholders::_5));
            ces_render_system::draw_2d_entities(technique_name);
            ces_render_system::draw_3d_entities(technique_name);
            
#if defined(__USE_BATCHING__)
            
            m_batching_pipeline->draw();
            
#endif
            
            technique->unbind();
        }
        
        ces_base_system::enumerate_entities_with_components(m_render_target_components_mask, [](const ces_entity_shared_ptr& entity) {
            auto render_target_component = entity->get_component<ces_render_target_component>();
            auto screen_quad_mesh = mesh_constructor::create_screen_quad();
            auto render_target = render_target_component->get_render_target();
            auto material = render_target_component->get_material();
            if(render_target_component->get_on_render_begin_callback() != nullptr)
            {
                render_target_component->get_on_render_begin_callback()(entity);
            }
            render_target->begin();
            render_target->clear();
            
#if USED_GRAPHICS_API == VULKAN_API
            
            material->bind(screen_quad_mesh->get_vbo()->get_vertex_input_state());
            
#elif USED_GRAPHICS_API == METAL_API
            
            material->bind(screen_quad_mesh->get_vbo()->get_mtl_vertex_descriptor());
            
#else
            
            material->bind();
            
#endif
            
            screen_quad_mesh->bind(material->get_shader()->get_guid(), material->get_shader()->get_attributes());
            
            screen_quad_mesh->draw();
            
            screen_quad_mesh->unbind(material->get_shader()->get_guid(), material->get_shader()->get_attributes());
            material->unbind();
            
            render_target->end();
            
            if(render_target_component->get_on_render_end_callback() != nullptr)
            {
                render_target_component->get_on_render_end_callback()(entity);
            }
        });
        
        m_render_pipeline->on_draw_end();
    }
}

#endif
