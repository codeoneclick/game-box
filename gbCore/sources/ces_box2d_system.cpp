//
//  ces_box2d_system.cpp
//  gbCore
//
//  Created by serhii serhiiv on 11/1/16.
//  Copyright © 2016 sergey.sergeev. All rights reserved.
//

#include "ces_box2d_system.h"
#include "ces_box2d_body_component.h"
#include "ces_box2d_world_component.h"
#include "ces_transformation_component.h"
#include "ces_transformation_extension.h"

namespace gb
{
    ces_box2d_system::ces_box2d_system()
    {
        
    }
    
    ces_box2d_system::~ces_box2d_system()
    {
        
    }
    
    void ces_box2d_system::on_feed_start(f32 deltatime)
    {
        
    }
    
    void ces_box2d_system::on_feed(const ces_entity_shared_ptr& root, f32 deltatime)
    {
        ces_box2d_world_component_shared_ptr box2d_world_component = root->get_component<ces_box2d_world_component>();
        if (box2d_world_component) {
            std::shared_ptr<b2World> box2d_world = box2d_world_component->box2d_world;
            box2d_world->Step(1.f / 60.f, 1, 1);
            ces_box2d_system::update_recursively(root, deltatime);
        }
    }
    
    void ces_box2d_system::on_feed_end(f32 deltatime)
    {
        
    }
    
    void ces_box2d_system::update_recursively(const ces_entity_shared_ptr& entity, f32 deltatime)
    {
        ces_box2d_body_component_shared_ptr box2d_body_component = entity->get_component<ces_box2d_body_component>();
        if (box2d_body_component)
        {
            if(box2d_body_component->is_contacted && box2d_body_component->is_destuctable_on_contact)
            {
                b2Body* box2d_body = box2d_body_component->box2d_body;
                b2World* box2d_world = box2d_body->GetWorld();
                box2d_world->DestroyBody(box2d_body);
            }
            else
            {
                ces_transformation_component_shared_ptr transformation_component = entity->get_component<ces_transformation_component>();
                transformation_component->set_position(box2d_body_component->position);
                transformation_component->set_rotation(box2d_body_component->rotation);
                ces_transformation_extension::update_absolute_transformation_recursively(entity);
            }
        }
        
        std::list<ces_entity_shared_ptr> children = entity->children;
        for(const auto& child : children)
        {
            ces_box2d_system::update_recursively(child, deltatime);
        }
    }
}
