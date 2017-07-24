//
//  ces_battle_system.cpp
//  gbDemo
//
//  Created by serhii serhiiv on 2/3/17.
//  Copyright © 2017 sergey.sergeev. All rights reserved.
//

#include "ces_battle_system.h"
#include "ces_hit_bounds_component.h"
#include "ces_box2d_body_component.h"
#include "ces_character_controllers_component.h"
#include "ces_character_statistic_component.h"
#include "std_extensions.h"

namespace game
{
    ces_battle_system::ces_battle_system()
    {
        
    }
    
    ces_battle_system::~ces_battle_system()
    {
        
    }
    
    void ces_battle_system::on_feed_start(f32 deltatime)
    {
        
    }
    
    void ces_battle_system::on_feed(const gb::ces_entity_shared_ptr& entity, f32 deltatime)
    {
        ces_battle_system::update_recursively(entity, deltatime);
    }
    
    void ces_battle_system::on_feed_end(f32 deltatime)
    {
        
    }
    
    void ces_battle_system::update_recursively(const gb::ces_entity_shared_ptr& entity, f32 deltatime)
    {
        /*auto hit_bounds_component = entity->get_component<ces_hit_bounds_component>();
        
        bool is_removed = false;
        if(hit_bounds_component)
        {
            auto hit_bounds_box2d_body_component = entity->get_component<gb::ces_box2d_body_component>();
            if(hit_bounds_box2d_body_component->is_contacted)
            {
                gb::ces_entity_shared_ptr target_entity = hit_bounds_box2d_body_component->contacted_entity;
                if(target_entity)
                {
                    gb::ces_entity_shared_ptr executor_entity = hit_bounds_component->owner;
                    
                    auto target_box2d_body_component = target_entity->get_component<gb::ces_box2d_body_component>();
                    target_box2d_body_component->contacted_entity = nullptr;
                    
                    auto target_controller_component = target_entity->get_component<ces_character_controllers_component>();
                    auto target_statistic_component = target_entity->get_component<ces_character_statistic_component>();
                    
                    auto executor_controller_component = executor_entity->get_component<ces_character_controllers_component>();
                    auto executor_statistic_component = executor_entity->get_component<ces_character_statistic_component>();
                    
                    if(target_controller_component && target_statistic_component &&
                       executor_controller_component && executor_statistic_component)
                    {
                        f32 current_damage = executor_statistic_component->current_damage;
                        target_statistic_component->on_health_changed(executor_entity, -std::get_random_i(current_damage - 1,
                                                                                                          current_damage + 1));
                        f32 current_health = target_statistic_component->current_health;
                        if(current_health <= 0.f)
                        {
                            target_controller_component->on_dead(entity);
                            executor_controller_component->on_kill(executor_entity, target_entity);
                        }
                    }
                }
                entity->remove_from_parent();
                is_removed = true;
            }
        }
        
        if(!is_removed)
        {
            std::vector<gb::ces_entity_shared_ptr> children = entity->children;
            for(const auto& child : children)
            {
                ces_battle_system::update_recursively(child, deltatime);
            }
        }*/
    }
}

