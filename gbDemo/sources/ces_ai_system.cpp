//
//  ces_ai_system.cpp
//  gbDemo
//
//  Created by serhii serhiiv on 3/9/17.
//  Copyright © 2017 sergey.sergeev. All rights reserved.
//

#include "ces_ai_system.h"
#include "ces_ai_component.h"

namespace game
{
    ces_ai_system::ces_ai_system()
    {
        
    }
    
    ces_ai_system::~ces_ai_system()
    {
        
    }
    
    void ces_ai_system::on_feed_start(f32 deltatime)
    {
        
    }
    
    void ces_ai_system::on_feed(const gb::ces_entity_shared_ptr& entity, f32 deltatime)
    {
        ces_ai_system::update_recursively(entity, deltatime);
    }
    
    void ces_ai_system::on_feed_end(f32 deltatime)
    {
        
    }
    
    void ces_ai_system::update_recursively(const gb::ces_entity_shared_ptr& entity, f32 deltatime)
    {
        std::shared_ptr<ces_ai_component> ai_component = entity->get_component<ces_ai_component>();
        
        if(ai_component)
        {
            
        }
        
        std::list<gb::ces_entity_shared_ptr> children = entity->children;
        for(const auto& child : children)
        {
            ces_ai_system::update_recursively(child, deltatime);
        }
    }
}
