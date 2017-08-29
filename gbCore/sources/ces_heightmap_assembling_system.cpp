//
//  ces_heightmap_assembling_system.cpp
//  gbCore
//
//  Created by serhii serhiiv on 8/24/17.
//  Copyright © 2017 sergey.sergeev. All rights reserved.
//

#include "ces_heightmap_assembling_system.h"
#include "ces_heightmap_container_component.h"
#include "ces_heightmap_bounding_boxes_component.h"
#include "heightmap_mmap.h"
#include "heightmap_constants.h"
#include "vbo.h"
#include "heightmap_geometry_generator.h"
#include "heightmap_textures_generator.h"
#include "glm_extensions.h"
#include "thread_operation.h"

namespace gb
{
    ces_heightmap_assembling_system::ces_heightmap_assembling_system()
    {
        ces_base_system::add_required_component_guid(m_heightmap_components_mask, ces_heightmap_container_component::class_guid());
        ces_base_system::add_required_components_mask(m_heightmap_components_mask);
    }
    
    ces_heightmap_assembling_system::~ces_heightmap_assembling_system()
    {
        
    }
    
    void ces_heightmap_assembling_system::on_feed_start(f32 dt)
    {
        
    }
    
    void ces_heightmap_assembling_system::on_feed(const ces_entity_shared_ptr& root, f32 dt)
    {
        ces_base_system::enumerate_entities_with_components(m_heightmap_components_mask, [this, dt](const ces_entity_shared_ptr& entity) {
            const auto& heightmap_container_component = entity->get_component<ces_heightmap_container_component>();
            if(!heightmap_container_component->get_is_generated() && !heightmap_container_component->get_is_generating())
            {
                heightmap_container_component->set_is_generating(true);
                
                gb::thread_operation_shared_ptr completion_operation = std::make_shared<gb::thread_operation>(gb::thread_operation::e_thread_operation_queue_main);
                completion_operation->set_execution_callback([entity]() {
                    const auto& heightmap_container_component = entity->get_component<ces_heightmap_container_component>();
                    heightmap_container_component->set_is_generated(true);
                });
                
                gb::thread_operation_shared_ptr generate_geometry_operation = std::make_shared<gb::thread_operation>(gb::thread_operation::e_thread_operation_queue_background);
                generate_geometry_operation->set_execution_callback([entity]() {
                    heightmap_geometry_generator::generate_geometry(entity);
                });
                completion_operation->add_dependency(generate_geometry_operation);
                
                gb::thread_operation_shared_ptr mmap_geometry_operation = std::make_shared<gb::thread_operation>(gb::thread_operation::e_thread_operation_queue_background);
                mmap_geometry_operation->set_execution_callback([entity]() {
                    const auto& heightmap_container_component = entity->get_component<ces_heightmap_container_component>();
                    heightmap_container_component->mmap_geometry(heightmap_container_component->get_mmap()->get_filename());
                    heightmap_geometry_generator::generate_tangent_space(entity);
                    heightmap_geometry_generator::generate_attaches_to_vbo(entity);
                    heightmap_geometry_generator::generate_bounding_boxes(entity);
                });
                completion_operation->add_dependency(mmap_geometry_operation);
                
                gb::thread_operation_shared_ptr generate_splatting_diffuse_textures_operation = std::make_shared<gb::thread_operation>(gb::thread_operation::e_thread_operation_queue_background);
                generate_splatting_diffuse_textures_operation->set_execution_callback([this, entity]() {
                    i32 current_progress = std::numeric_limits<i32>::min();
                    i32 total_progress = std::numeric_limits<i32>::max();
                    gb::thread_operation_shared_ptr operation = std::make_shared<gb::thread_operation>(gb::thread_operation::e_thread_operation_queue_main);
                    operation->set_execution_callback([entity, this, &current_progress, &total_progress]() {
                        heightmap_textures_generator::generate_splatting_diffuse_textures(entity, m_resource_accessor.lock(), [&current_progress, &total_progress](i32 current, i32 total){
                            current_progress = current;
                            total_progress = total;
                        });
                    });
                    operation->add_to_execution_queue();
                    while(current_progress != total_progress) {
                        std::this_thread::sleep_for(std::chrono::milliseconds(1));
                    };
                });
                completion_operation->add_dependency(generate_geometry_operation);
                completion_operation->add_to_execution_queue();
            }
        });
    }
    
    void ces_heightmap_assembling_system::on_feed_end(f32 dt)
    {
        
    }
    
    void ces_heightmap_assembling_system::set_resource_accessor(const resource_accessor_shared_ptr& resource_accessor)
    {
        m_resource_accessor = resource_accessor;
    }
}
