//
//  ces_deferred_lighting_system.cpp
//  gbCore
//
//  Created by sergey.sergeev on 1/11/16.
//  Copyright © 2016 sergey.sergeev. All rights reserved.
//

#if !defined(__NO_RENDER__)

#include "ces_deferred_lighting_system.h"
#include "ces_light_compoment.h"
#include "ces_geometry_component.h"
#include "ces_shadow_component.h"
#include "ces_transformation_2d_component.h"
#include "ces_convex_hull_component.h"
#include "ces_material_component.h"
#include "ces_light_mask_component.h"
#include "ces_luminous_component.h"
#include "ces_geometry_extension.h"
#include "mesh_2d.h"
#include "camera_2d.h"
#include "glm_extensions.h"
#include "thread_operation.h"

#define k_camera_trashhold 64.f;

namespace gb
{
    ces_deferred_lighting_system::ces_deferred_lighting_system()
    {
        ces_base_system::add_required_component_guid(m_light_components_mask, ces_light_compoment::class_guid());
        ces_base_system::add_required_components_mask(m_light_components_mask);
        
        ces_base_system::add_required_component_guid(m_shadow_components_mask, ces_shadow_component::class_guid());
        ces_base_system::add_required_components_mask(m_shadow_components_mask);
        
        m_inside_outside_process_thread_executed = true;
        m_inside_outside_process_thread = std::thread(&ces_deferred_lighting_system::inside_outside_process, this);
    }
    
    ces_deferred_lighting_system::~ces_deferred_lighting_system()
    {
        m_inside_outside_process_thread_executed = false;
        if(m_inside_outside_process_thread.joinable())
        {
            m_inside_outside_process_thread.join();
        }
    }
    
    void ces_deferred_lighting_system::on_feed_start(f32 deltatime)
    {
        m_camera_2d_bounds = ces_base_system::get_current_camera_2d()->bound;
        m_camera_2d_bounds.x -= k_camera_trashhold;
        m_camera_2d_bounds.y -= k_camera_trashhold;
        m_camera_2d_bounds.z += k_camera_trashhold;
        m_camera_2d_bounds.w += k_camera_trashhold;
    }
    
    void ces_deferred_lighting_system::on_feed(ces_entity_const_shared_ptr entity, f32 deltatime)
    {
        
    }
    
    void ces_deferred_lighting_system::on_feed_end(f32 deltatime)
    {
        std::list<ces_entity_weak_ptr> light_casters = m_references_to_required_entities.at(m_light_components_mask);
        std::list<ces_entity_weak_ptr> shadow_casters = m_references_to_required_entities.at(m_shadow_components_mask);
        
        for(const auto& weak_light_caster : light_casters)
        {
            if(!weak_light_caster.expired())
            {
                ces_entity_shared_ptr light_caster = weak_light_caster.lock();
                auto light_component = light_caster->get_component<ces_light_compoment>();
                light_component->cleanup();
                
                auto light_mask_component = light_caster->get_component<ces_light_mask_component>();
                light_mask_component->cleanup();
                
                auto light_caster_transformation_component = light_caster->get_component<ces_transformation_2d_component>();
                
                glm::mat4 light_caster_mat_m = light_caster_transformation_component->get_absolute_transformation();
                light_mask_component->center = glm::vec2(light_caster_mat_m[3][0],
                                                         light_caster_mat_m[3][1]);
                light_mask_component->radius = light_caster_transformation_component->get_scale().x;
                
                for(const auto& weak_shadow_caster : shadow_casters)
                {
                    if(!weak_shadow_caster.expired())
                    {
                        ces_entity_shared_ptr shadow_caster = weak_shadow_caster.lock();
                        
                        const auto& convex_hull_component = shadow_caster->get_component<ces_convex_hull_component>();
                        const auto& shadow_caster_transformation_component = shadow_caster->get_component<ces_transformation_2d_component>();
                        
                        const std::vector<glm::vec2>& oriented_vertices = convex_hull_component->get_absolute_transformed_oriented_vertices(shadow_caster_transformation_component->get_absolute_transformation(),
                                                                                                                                            shadow_caster_transformation_component->get_absolute_matrix_version());
                        light_mask_component->add_shadowcasters_geometry(oriented_vertices);
                    }
                }
                light_mask_component->update_mesh();
            }
        }
    }
    
    void ces_deferred_lighting_system::inside_outside_process()
    {
#if defined(__IOS__) || defined(__OSX__) || defined(__TVOS__)
        
        pthread_setname_np("gb.core.inside.outside.process");
        
#endif
        while(m_inside_outside_process_thread_executed)
        {
            std::list<ces_entity_weak_ptr> light_casters = m_references_to_required_entities.at(m_light_components_mask);
            if(!light_casters.empty())
            {
                for(const auto& weak_light_caster : light_casters)
                {
                    if(!weak_light_caster.expired())
                    {
                        ces_entity_shared_ptr light_caster = weak_light_caster.lock();
                        auto light_mask_component = light_caster->get_component<ces_light_mask_component>();
                        if(light_mask_component->is_inside_outside_requests_exist())
                        {
                            auto request = light_mask_component->pop_inside_outside_request();
                            const std::vector<ces_entity_weak_ptr>& entities_source = std::get<0>(request);
                            size_t entities_source_count = entities_source.size();
                            
                            std::vector<ces_entity_weak_ptr> entities_inside;
                            std::vector<ces_entity_weak_ptr> entities_outside;
                            
                            for(size_t i = 0; i < entities_source_count; ++i)
                            {
                                gb::mesh_2d_shared_ptr light_source_mesh = light_mask_component->get_mesh();
                                if(light_source_mesh)
                                {
                                    auto light_source_vbo = light_source_mesh->get_vbo();
                                    auto light_source_ibo = light_source_mesh->get_ibo();
                                    glm::mat4 light_source_mat_m = glm::mat4(1.f);
                                    
                                    for(auto entity_weak : entities_source)
                                    {
                                        if(!entity_weak.expired())
                                        {
                                            bool is_inside = false;
                                            auto entity = entity_weak.lock();
                                            auto entity_transformation_component = entity->get_component<gb::ces_transformation_2d_component>();
                                            auto entity_mesh = entity->get_component<gb::ces_geometry_component>()->get_mesh();
                                            if(entity_mesh)
                                            {
                                                is_inside = glm::intersect(m_camera_2d_bounds, gb::ces_geometry_extension::get_absolute_position_bounds(entity));
                                                if(is_inside)
                                                {
                                                    is_inside = gb::mesh_2d::intersect(entity_mesh->get_vbo(), entity_mesh->get_ibo(), entity_transformation_component->get_absolute_transformation(), true,
                                                                                       light_source_vbo, light_source_ibo, light_source_mat_m, false);
                                                }
                                            }
                                            if(is_inside)
                                            {
                                                entities_inside.push_back(entity);
                                            }
                                            else
                                            {
                                                entities_outside.push_back(entity);
                                            }
                                        }
                                    }
                                }
                            }
                            
                            auto callback = std::get<1>(request);
                            gb::thread_operation_shared_ptr operation = std::make_shared<gb::thread_operation>(gb::thread_operation::e_thread_operation_queue_main);
                            operation->set_execution_callback([entities_inside, entities_outside, callback]() {
                                callback(entities_inside, entities_outside);
                            });
                            operation->add_to_execution_queue();
                        }
                    }
                }
            }
            else
            {
                std::this_thread::sleep_for(std::chrono::milliseconds(1));
            }
        }
    }
}

#endif
