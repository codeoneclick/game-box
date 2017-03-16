//
//  ces_bullet_system.cpp
//  gbDemo
//
//  Created by serhii serhiiv on 2/3/17.
//  Copyright © 2017 sergey.sergeev. All rights reserved.
//

#include "ces_bullet_system.h"
#include "ces_bullet_component.h"
#include "ces_box2d_body_component.h"

namespace game
{
    ces_bullet_system::ces_bullet_system()
    {
        
    }
    
    ces_bullet_system::~ces_bullet_system()
    {
        
    }
    
    void ces_bullet_system::on_feed_start(f32 deltatime)
    {
        
    }
    
    void ces_bullet_system::on_feed(const gb::ces_entity_shared_ptr& entity, f32 deltatime)
    {
        ces_bullet_system::update_recursively(entity, deltatime);
    }
    
    void ces_bullet_system::on_feed_end(f32 deltatime)
    {
        
    }
    
    void ces_bullet_system::update_recursively(const gb::ces_entity_shared_ptr& entity, f32 deltatime)
    {
        std::shared_ptr<ces_bullet_component> bullet_component = entity->get_component<ces_bullet_component>();
        
        bool is_removed = false;
        if(bullet_component)
        {
            auto box2d_body_component = entity->get_component<gb::ces_box2d_body_component>();
            if(box2d_body_component->is_contacted)
            {
                entity->remove_from_parent();
                is_removed = true;
            }
        }
        
        if(!is_removed)
        {
            std::list<gb::ces_entity_shared_ptr> children = entity->children;
            for(const auto& child : children)
            {
                ces_bullet_system::update_recursively(child, deltatime);
            }
        }
    }
}

