//
//  ces_character_controllers_system.cpp
//  gbDemo
//
//  Created by serhii serhiiv on 10/31/16.
//  Copyright © 2016 sergey.sergeev. All rights reserved.
//

#include "ces_character_controllers_system.h"
#include "ces_character_controller_component.h"

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
        std::shared_ptr<ces_character_controller_component> character_controller_component = entity->get_component<ces_character_controller_component>();
        
        std::list<gb::ces_entity_shared_ptr> children = entity->children;
        for(const auto& child : children)
        {
            ces_character_controllers_system::update_recursively(child, deltatime);
        }
    }
}

