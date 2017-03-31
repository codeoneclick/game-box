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
#include "ces_transformation_component.h"
#include "ces_geometry_extension.h"
#include "information_bubble_controller.h"
#include "bloodprint_controller.h"
#include "footprint_controller.h"
#include "std_extensions.h"
#include "glm_extensions.h"
#include "information_bubble.h"
#include "bloodprint.h"
#include "footprint.h"
#include "camera.h"

#define k_camera_trashhold 64.f;

namespace game
{
    ces_character_controllers_system::ces_character_controllers_system()
    {
        
    }
    
    ces_character_controllers_system::~ces_character_controllers_system()
    {
        
    }
    
    void ces_character_controllers_system::on_feed_start(f32 deltatime)
    {
        m_camera_bounds = ces_base_system::get_current_camera()->bound;
        m_camera_bounds.x -= k_camera_trashhold;
        m_camera_bounds.y -= k_camera_trashhold;
        m_camera_bounds.z += k_camera_trashhold;
        m_camera_bounds.w += k_camera_trashhold;
    }
    
    void ces_character_controllers_system::on_feed(const gb::ces_entity_shared_ptr& entity, f32 deltatime)
    {
        ces_character_controllers_system::update_recursively(entity, deltatime);
    }
    
    void ces_character_controllers_system::on_feed_end(f32 deltatime)
    {
        
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
            }
            else if(character_controller_component->mode == ces_character_controller_component::e_mode::main)
            {
                m_main_character = entity;
            }
            m_all_characters[character_key] = entity;
            
            if(!m_main_character.expired())
            {
                auto main_character = m_main_character.lock();
                gb::ces_entity_shared_ptr light_source_entity = main_character->get_child("light_source", true);
                gb::mesh_shared_ptr light_source_mesh = light_source_entity->get_component<gb::ces_light_mask_component>()->get_mesh();
                
                if(light_source_mesh)
                {
                    auto character_controller_component = entity->get_component<ces_character_controller_component>();
                    
                    footprint_controller_shared_ptr footprint_controller = character_controller_component->footprint_controller;
                    const std::list<game::footprint_weak_ptr>& footprints = footprint_controller->get_footprints();
                    for(auto footprint_weak : footprints)
                    {
                        if(!footprint_weak.expired())
                        {
                            bool is_visible = false;
                            auto footprint_entity = footprint_weak.lock()->get_child("footprint");
                            auto footprint_transformation_component = footprint_entity->get_component<gb::ces_transformation_component>();
                            auto footprint_mesh = footprint_entity->get_component<gb::ces_geometry_component>()->get_mesh();
                            if(footprint_mesh)
                            {
                                is_visible = glm::intersect(m_camera_bounds, gb::ces_geometry_extension::get_absolute_position_bounds(footprint_entity));
                                if(is_visible)
                                {
                                    is_visible = gb::mesh::intersect(footprint_mesh->get_vbo(), footprint_mesh->get_ibo(), footprint_transformation_component->get_absolute_transformation(), true,
                                                                     light_source_mesh->get_vbo(), light_source_mesh->get_ibo(), glm::mat4(1.f), false);
                                }
                            }
                            footprint_entity->visible = is_visible;
                        }
                    }
                    
                    bloodprint_controller_shared_ptr bloodprint_controller = character_controller_component->bloodprint_controller;
                    const std::list<game::bloodprint_weak_ptr>& bloodprints = bloodprint_controller->get_bloodprints();
                    for(auto bloodprint_weak : bloodprints)
                    {
                        if(!bloodprint_weak.expired())
                        {
                            bool is_visible = false;
                            auto bloodprint_entity = bloodprint_weak.lock()->get_child("bloodprint");
                            auto bloodprint_transformation_component = bloodprint_entity->get_component<gb::ces_transformation_component>();
                            auto bloodprint_mesh = bloodprint_entity->get_component<gb::ces_geometry_component>()->get_mesh();
                            if(bloodprint_mesh)
                            {
                                is_visible = glm::intersect(m_camera_bounds, gb::ces_geometry_extension::get_absolute_position_bounds(bloodprint_entity));
                                if(is_visible)
                                {
                                    is_visible = gb::mesh::intersect(bloodprint_mesh->get_vbo(), bloodprint_mesh->get_ibo(), bloodprint_transformation_component->get_absolute_transformation(), true,
                                                                     light_source_mesh->get_vbo(), light_source_mesh->get_ibo(), glm::mat4(1.f), false);
                                }
                            }
                            bloodprint_entity->visible = is_visible;
                        }
                    }
                    
                    information_bubble_controller_shared_ptr information_bubbles_controller = character_controller_component->information_bubble_controller;
                    const std::list<game::information_bubble_weak_ptr>& information_bubbles = information_bubbles_controller->get_information_bubbles();
                    for(auto information_bubble_weak : information_bubbles)
                    {
                        if(!information_bubble_weak.expired())
                        {
                            bool is_visible = false;
                            auto information_bubble_entity = information_bubble_weak.lock()->get_child("information_bubble");
                            auto information_bubble_transformation_component = information_bubble_entity->get_component<gb::ces_transformation_component>();
                            auto information_bubble_mesh = information_bubble_entity->get_component<gb::ces_geometry_component>()->get_mesh();
                            if(information_bubble_mesh)
                            {
                                is_visible = glm::intersect(m_camera_bounds, gb::ces_geometry_extension::get_absolute_position_bounds(information_bubble_entity));
                                if(is_visible)
                                {
                                    is_visible = gb::mesh::intersect(information_bubble_mesh->get_vbo(), information_bubble_mesh->get_ibo(), information_bubble_transformation_component->get_absolute_transformation(), true,
                                                                     light_source_mesh->get_vbo(), light_source_mesh->get_ibo(), glm::mat4(1.f), false);
                                }
                            }
                            information_bubble_entity->visible = is_visible;
                        }
                    }
                }
            }
            
            auto character_box2d_body_component = entity->get_component<gb::ces_box2d_body_component>();
            if(character_box2d_body_component && character_box2d_body_component->is_contacted)
            {
                gb::ces_entity_shared_ptr contacted_entity = character_box2d_body_component->contacted_entity;
                if(contacted_entity)
                {
                    auto bullet_component = contacted_entity->get_component<ces_bullet_component>();
                    if(bullet_component)
                    {
                        auto bullet_box2d_body_component = contacted_entity->get_component<gb::ces_box2d_body_component>();
                        bullet_box2d_body_component->contacted_entity = nullptr;
                        character_controller_component->add_health(entity, -std::get_random_i(30, 40));
                    }
                }
            }
            
            f32 current_health = character_controller_component->health;
            if(current_health <= 0.f)
            {
                character_controller_component->on_spawn(entity);
            }
        }
        
        std::list<gb::ces_entity_shared_ptr> children = entity->children;
        for(const auto& child : children)
        {
            ces_character_controllers_system::update_recursively(child, deltatime);
        }
    }
}

