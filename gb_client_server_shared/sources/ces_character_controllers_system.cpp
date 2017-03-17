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
            auto character_box2d_body_component = entity->get_component<gb::ces_box2d_body_component>();
            if(character_box2d_body_component && character_box2d_body_component->is_contacted)
            {
                gb::ces_entity* raw_contacted_entity = character_box2d_body_component->contacted_entity;
                if(raw_contacted_entity)
                {
                    auto bullet_component = raw_contacted_entity->get_component<ces_bullet_component>();
                    if(bullet_component)
                    {
                        auto bullet_box2d_body_component = raw_contacted_entity->get_component<gb::ces_box2d_body_component>();
                        bullet_box2d_body_component->contacted_entity = nullptr;
                        character_controller_component->add_health(-std::get_random_i(30, 40));
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

