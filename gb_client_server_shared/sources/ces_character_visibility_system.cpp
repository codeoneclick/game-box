//
//  ces_character_visibility_system.cpp
//  gbDemo
//
//  Created by serhii serhiiv on 10/31/16.
//  Copyright © 2016 sergey.sergeev. All rights reserved.
//

#include "ces_character_visibility_system.h"
#include "ces_character_controllers_component.h"
#include "ces_character_parts_component.h"
#include "ces_box2d_body_component.h"
#include "ces_geometry_component.h"
#include "ces_light_mask_component.h"
#include "ces_transformation_2d_component.h"
#include "ces_character_statistic_component.h"
#include "ces_geometry_extension.h"
#include "std_extensions.h"
#include "glm_extensions.h"

#if USED_GRAPHICS_API != NO_GRAPHICS_API

#include "information_bubble_controller.h"
#include "bloodprint_controller.h"
#include "footprint_controller.h"
#include "information_bubble.h"
#include "bloodprint.h"
#include "footprint.h"
#include "sprite.h"

#endif

namespace game
{
    ces_character_visibility_system::ces_character_visibility_system()
    {
        ces_base_system::add_required_component_guid(m_character_components_mask, ces_character_controllers_component::class_guid());
        ces_base_system::add_required_component_guid(m_character_components_mask, ces_character_statistic_component::class_guid());
        ces_base_system::add_required_components_mask(m_character_components_mask);
    }
    
    ces_character_visibility_system::~ces_character_visibility_system()
    {
    }
    
    void ces_character_visibility_system::on_feed_start(f32 dt)
    {
    }
    
    void ces_character_visibility_system::on_feed(const gb::ces_entity_shared_ptr& entity, f32 dt)
    {
        
    }
    
    void ces_character_visibility_system::on_feed_end(f32 dt)
    {
        ces_base_system::enumerate_entities_with_components(m_character_components_mask, [this](const gb::ces_entity_shared_ptr& entity) {
            
            std::string character_key = entity->tag;
            auto character_statistic_component = entity->get_component<ces_character_statistic_component>();
            if(character_statistic_component->mode == ces_character_statistic_component::e_mode::e_mode_player)
            {
                m_main_character = entity;
            }
            m_all_characters[character_key] = entity;
        });
        
        if(!m_main_character.expired())
        {
            auto main_character = m_main_character.lock();
            auto character_controller_component = main_character->get_component<ces_character_controllers_component>();
            
            footprint_controller_shared_ptr footprint_controller = character_controller_component->footprint_controller;
            const std::vector<game::footprint_weak_ptr>& footprints = footprint_controller->get_footprints();
            for(auto footprint_weak : footprints)
            {
                if(!footprint_weak.expired())
                {
                    auto footprint_entity = footprint_weak.lock()->get_child("footprint");
                    footprint_entity->visible = true;
                }
            }
            
            bloodprint_controller_shared_ptr bloodprint_controller = character_controller_component->bloodprint_controller;
            const std::vector<game::bloodprint_weak_ptr>& bloodprints = bloodprint_controller->get_bloodprints();
            for(auto bloodprint_weak : bloodprints)
            {
                if(!bloodprint_weak.expired())
                {
                    auto bloodprint_entity = bloodprint_weak.lock()->get_child("bloodprint");
                    bloodprint_entity->visible = true;
                }
            }
            
            information_bubble_controller_shared_ptr information_bubbles_controller = character_controller_component->information_bubble_controller;
            const std::vector<game::information_bubble_weak_ptr>& information_bubbles = information_bubbles_controller->get_information_bubbles();
            for(auto information_bubble_weak : information_bubbles)
            {
                if(!information_bubble_weak.expired())
                {
                    auto information_bubble_entity = information_bubble_weak.lock()->get_child("information_bubble");
                    information_bubble_entity->visible = true;
                }
            }
            
            auto light_source_entity = main_character->get_child(ces_character_parts_component::parts::k_light_source_part, true);
            auto light_mask_component = light_source_entity->get_component<gb::ces_light_mask_component>();
            std::vector<gb::ces_entity_weak_ptr> visibility_unprocessed_entities;
            
            for(const auto& weak_character : m_all_characters)
            {
                if(!weak_character.second.expired() && weak_character.second.lock() != m_main_character.lock())
                {
                    auto ai_character = weak_character.second.lock();
                    auto bounds_entity = ai_character->get_child(ces_character_parts_component::parts::k_bounds_part, true);
                    visibility_unprocessed_entities.push_back(bounds_entity);
                    auto character_controller_component = ai_character->get_component<ces_character_controllers_component>();
                    
                    footprint_controller_shared_ptr footprint_controller = character_controller_component->footprint_controller;
                    const std::vector<game::footprint_weak_ptr>& footprints = footprint_controller->get_footprints();
                    for(auto footprint_weak : footprints)
                    {
                        if(!footprint_weak.expired())
                        {
                            auto footprint_entity = footprint_weak.lock()->get_child("footprint");
                            visibility_unprocessed_entities.push_back(footprint_entity);
                        }
                    }
                    
                    bloodprint_controller_shared_ptr bloodprint_controller = character_controller_component->bloodprint_controller;
                    const std::vector<game::bloodprint_weak_ptr>& bloodprints = bloodprint_controller->get_bloodprints();
                    for(auto bloodprint_weak : bloodprints)
                    {
                        if(!bloodprint_weak.expired())
                        {
                            auto bloodprint_entity = bloodprint_weak.lock()->get_child("bloodprint");
                            visibility_unprocessed_entities.push_back(bloodprint_entity);
                        }
                    }
                    
                    information_bubble_controller_shared_ptr information_bubbles_controller = character_controller_component->information_bubble_controller;
                    const std::vector<game::information_bubble_weak_ptr>& information_bubbles = information_bubbles_controller->get_information_bubbles();
                    for(auto information_bubble_weak : information_bubbles)
                    {
                        if(!information_bubble_weak.expired())
                        {
                            auto information_bubble_entity = information_bubble_weak.lock()->get_child("information_bubble");
                            visibility_unprocessed_entities.push_back(information_bubble_entity);
                        }
                    }
                }
            }
            light_mask_component->push_inside_outside_request(visibility_unprocessed_entities, [](const std::vector<gb::ces_entity_weak_ptr>& entities_inside, const std::vector<gb::ces_entity_weak_ptr>& entities_outside) {
                for(const auto& weak_entity : entities_inside)
                {
                    if(!weak_entity.expired())
                    {
                        auto entity = weak_entity.lock();
                        entity->visible = true;
                        std::string tag = entity->tag;
                        if(tag == ces_character_parts_component::parts::k_bounds_part)
                        {
                            gb::ces_entity_shared_ptr parent = entity->parent;
                            parent->visible = true;
                            auto character_statistic_component = parent->get_component<ces_character_statistic_component>();
                            character_statistic_component->get_health_status_entity()->visible = true;
                        }
                    }
                }
                for(const auto& weak_entity : entities_outside)
                {
                    if(!weak_entity.expired())
                    {
                        auto entity = weak_entity.lock();
                        entity->visible = false;
                        std::string tag = entity->tag;
                        if(tag == ces_character_parts_component::parts::k_bounds_part)
                        {
                            gb::ces_entity_shared_ptr parent = entity->parent;
                            parent->visible = false;
                            auto character_statistic_component = parent->get_component<ces_character_statistic_component>();
                            character_statistic_component->get_health_status_entity()->visible = false;
                        }
                    }
                }
            });
        }
    }
}

