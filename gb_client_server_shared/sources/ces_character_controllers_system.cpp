//
//  ces_character_controllers_system.cpp
//  gbDemo
//
//  Created by serhii serhiiv on 10/31/16.
//  Copyright © 2016 sergey.sergeev. All rights reserved.
//

#include "ces_character_controllers_system.h"
#include "ces_character_controller_component.h"
#include "ces_box2d_body_component.h"
#include "ces_bullet_component.h"
#include "ces_geometry_component.h"
#include "ces_light_mask_component.h"
#include "ces_transformation_2d_component.h"
#include "ces_geometry_extension.h"
#include "std_extensions.h"
#include "glm_extensions.h"

#if !defined(__NO_RENDER__)

#include "information_bubble_controller.h"
#include "bloodprint_controller.h"
#include "footprint_controller.h"
#include "information_bubble.h"
#include "bloodprint.h"
#include "footprint.h"

#endif

#include "camera_2d.h"

#define k_camera_trashhold 64.f;

namespace game
{
    ces_character_controllers_system::ces_character_controllers_system()
    {
        m_visibility_process_threa_executed = true;
        m_visibility_process_thread = std::thread(&ces_character_controllers_system::process_entities_visibility, this);
    }
    
    ces_character_controllers_system::~ces_character_controllers_system()
    {
        m_visibility_process_threa_executed = false;
        if(m_visibility_process_thread.joinable())
        {
            m_visibility_process_thread.join();
        }
    }
    
    void ces_character_controllers_system::on_feed_start(f32 deltatime)
    {
        m_camera_2d_bounds = ces_base_system::get_current_camera_2d()->bound;
        m_camera_2d_bounds.x -= k_camera_trashhold;
        m_camera_2d_bounds.y -= k_camera_trashhold;
        m_camera_2d_bounds.z += k_camera_trashhold;
        m_camera_2d_bounds.w += k_camera_trashhold;
    }
    
    void ces_character_controllers_system::on_feed(const gb::ces_entity_shared_ptr& entity, f32 deltatime)
    {
        ces_character_controllers_system::update_recursively(entity, deltatime);
    }
    
    void ces_character_controllers_system::on_feed_end(f32 deltatime)
    {
        
    }
    
    void ces_character_controllers_system::process_entities_visibility()
    {
        
#if defined(__IOS__) || defined(__OSX__) || defined(__TVOS__)
        
        pthread_setname_np("gb.core.character.system");
        
#endif
        while(m_visibility_process_threa_executed)
        {
            volatile size_t entities_count = m_visibility_unprocessed_entities.size();
            if(entities_count != 0)
            {
                size_t iterations = entities_count - 1;
                auto iterator = m_visibility_unprocessed_entities.begin();
                for(size_t i = 0; i < iterations; ++i)
                {
                    std::list<gb::ces_entity_weak_ptr> visibility_unprocessed_entities = *iterator;
                    iterator++;
                    
                    auto main_character = m_main_character.lock();
                    gb::ces_entity_shared_ptr light_source_entity = main_character->get_child("light_source", true);
                    gb::mesh_2d_shared_ptr light_source_mesh = light_source_entity->get_component<gb::ces_light_mask_component>()->get_mesh();
                    
                    if(light_source_mesh)
                    {
                        auto light_source_vbo = light_source_mesh->get_vbo();
                        auto light_source_ibo = light_source_mesh->get_ibo();
                        glm::mat4 light_source_mat_m = glm::mat4(1.f);
                        
                        for(auto entity_weak : visibility_unprocessed_entities)
                        {
                            if(!entity_weak.expired())
                            {
                                bool is_visible = false;
                                auto entity = entity_weak.lock();
                                auto entity_transformation_component = entity->get_component<gb::ces_transformation_2d_component>();
                                auto entity_mesh = entity->get_component<gb::ces_geometry_component>()->get_mesh();
                                if(entity_mesh)
                                {
                                    is_visible = glm::intersect(m_camera_2d_bounds, gb::ces_geometry_extension::get_absolute_position_bounds(entity));
                                    if(is_visible)
                                    {
                                        is_visible = gb::mesh_2d::intersect(entity_mesh->get_vbo(), entity_mesh->get_ibo(), entity_transformation_component->get_absolute_transformation(), true,
                                                                            light_source_vbo, light_source_ibo, light_source_mat_m, false);
                                    }
                                }
                                entity->visible = is_visible;
                                assert(entity != m_main_character.lock());
                            }
                        }
                    }
                }
                std::lock_guard<std::mutex> guard(m_visibility_process_mutex);
                for(size_t i = 0; i < iterations; ++i)
                {
                    m_visibility_unprocessed_entities.pop_front();
                }
            }
            else
            {
                std::this_thread::sleep_for(std::chrono::milliseconds(1));
            }
        }
    }
    
    void ces_character_controllers_system::update_recursively(const gb::ces_entity_shared_ptr& entity, f32 deltatime)
    {
        auto character_controller_component = entity->get_component<ces_character_controller_component>();
        
        if(character_controller_component)
        {
            std::string character_key = entity->tag;
            if(character_controller_component->mode == ces_character_controller_component::e_mode::ai)
            {
                m_ai_characters[character_key] = entity;
                gb::ces_entity_shared_ptr light_source_entity = entity->get_child("light_source", true);
                light_source_entity->visible = false;
            }
            else if(character_controller_component->mode == ces_character_controller_component::e_mode::main)
            {
                m_main_character = entity;
            }
            m_all_characters[character_key] = entity;
            
#if !defined(__NO_RENDER__)
            
            if(!m_main_character.expired())
            {
                auto main_character = m_main_character.lock();
                gb::ces_entity_shared_ptr light_source_entity = main_character->get_child("light_source", true);
                gb::mesh_2d_shared_ptr light_source_mesh = light_source_entity->get_component<gb::ces_light_mask_component>()->get_mesh();
                std::list<gb::ces_entity_weak_ptr> visibility_unprocessed_entities;
        
                if(light_source_mesh)
                {
                    if(entity != m_main_character.lock())
                    {
                        gb::ces_entity_shared_ptr body_entity = entity->get_child("body", true);
                        gb::ces_entity_shared_ptr feet_entity = entity->get_child("feet", true);
                        visibility_unprocessed_entities.push_back(body_entity);
                        visibility_unprocessed_entities.push_back(feet_entity);
                    }
                    
                    auto character_controller_component = entity->get_component<ces_character_controller_component>();
                    footprint_controller_shared_ptr footprint_controller = character_controller_component->footprint_controller;
                    const std::list<game::footprint_weak_ptr>& footprints = footprint_controller->get_footprints();
                    for(auto footprint_weak : footprints)
                    {
                        if(!footprint_weak.expired())
                        {
                            auto footprint_entity = footprint_weak.lock()->get_child("footprint");
                            visibility_unprocessed_entities.push_back(footprint_entity);
                        }
                    }
                    
                    bloodprint_controller_shared_ptr bloodprint_controller = character_controller_component->bloodprint_controller;
                    const std::list<game::bloodprint_weak_ptr>& bloodprints = bloodprint_controller->get_bloodprints();
                    for(auto bloodprint_weak : bloodprints)
                    {
                        if(!bloodprint_weak.expired())
                        {
                            auto bloodprint_entity = bloodprint_weak.lock()->get_child("bloodprint");
                            visibility_unprocessed_entities.push_back(bloodprint_entity);
                        }
                    }
                    
                    information_bubble_controller_shared_ptr information_bubbles_controller = character_controller_component->information_bubble_controller;
                    const std::list<game::information_bubble_weak_ptr>& information_bubbles = information_bubbles_controller->get_information_bubbles();
                    for(auto information_bubble_weak : information_bubbles)
                    {
                        if(!information_bubble_weak.expired())
                        {
                            auto information_bubble_entity = information_bubble_weak.lock()->get_child("information_bubble");
                            visibility_unprocessed_entities.push_back(information_bubble_entity);
                        }
                    }
                }
                if(visibility_unprocessed_entities.size() != 0)
                {
                    std::lock_guard<std::mutex> guard(m_visibility_process_mutex);
                    m_visibility_unprocessed_entities.push_back(visibility_unprocessed_entities);
                }
            }
            
#endif
            
        }
        
        std::list<gb::ces_entity_shared_ptr> children = entity->children;
        for(const auto& child : children)
        {
            ces_character_controllers_system::update_recursively(child, deltatime);
        }
    }
}

