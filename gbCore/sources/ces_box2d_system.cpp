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
#include "ces_transformation_2d_component.h"
#include "ces_transformation_extension.h"

namespace gb
{
    ces_box2d_system::ces_box2d_system()
    {
        ces_base_system::add_required_component_guid(m_box2d_components_mask, ces_box2d_body_component::class_guid());
        ces_base_system::add_required_components_mask(m_box2d_components_mask);
    }
    
    ces_box2d_system::~ces_box2d_system()
    {
        
    }
    
    void ces_box2d_system::on_feed_start(f32 dt)
    {
        
    }
    
    void ces_box2d_system::on_feed(const ces_entity_shared_ptr& root, f32 dt)
    {
        auto box2d_world_component = root->get_component<ces_box2d_world_component>();
        if (box2d_world_component)
        {
            std::shared_ptr<b2World> box2d_world = box2d_world_component->box2d_world;
            box2d_world->Step(1.f / 60.f, 1, 1);
            
            ces_base_system::enumerate_entities_with_components(m_box2d_components_mask, [box2d_world_component](const ces_entity_shared_ptr& entity) {

                auto box2d_body_component = entity->get_component<ces_box2d_body_component>();
                if (box2d_body_component)
                {
                    if(box2d_body_component->is_contacted && box2d_body_component->is_destructable_on_contact)
                    {
                        box2d_world_component->unregister_box2d_body_entity(box2d_body_component->body_entity_guid);
                        entity->remove_component(box2d_body_component);
                    }
                    else
                    {
                        b2BodyDef* body_definition = box2d_body_component->box2d_body_definition;
                        if(body_definition->type != b2_staticBody)
                        {
                            auto transformation_component = entity->get_component<ces_transformation_2d_component>();
                            transformation_component->set_position(box2d_body_component->position);
                            transformation_component->set_rotation(box2d_body_component->rotation);
                            ces_transformation_extension::update_absolute_transformation_recursively(entity);
                        }
                    }
                }
            });
        }
    }
    
    void ces_box2d_system::on_feed_end(f32 dt)
    {
        
    }
}
