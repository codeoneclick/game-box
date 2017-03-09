//
//  ces_box2d_world_component.cpp
//  gbCore
//
//  Created by serhii serhiiv on 11/1/16.
//  Copyright © 2016 sergey.sergeev. All rights reserved.
//

#include "ces_box2d_world_component.h"
#include "ces_box2d_body_component.h"
#include "ces_entity.h"

namespace gb
{
    const f32 ces_box2d_world_component::k_box2d_world_scale = .1f;
    
    ces_box2d_world_component::ces_box2d_world_component()
    {
        m_box2d_world = std::make_shared<b2World>(b2Vec2(0.f, 0.f));
        m_box2d_world->SetContinuousPhysics(true);
        m_box2d_world->SetContactListener(this);
        m_box2d_body_definition.position = b2Vec2(0.f, 0.f);
        m_box2d_body = m_box2d_world->CreateBody(&m_box2d_body_definition);
        
        box2d_world.getter([=] {
            return m_box2d_world;
        });
    }
    
    ces_box2d_world_component::~ces_box2d_world_component()
    {
        
    }

    void ces_box2d_world_component::set_world_bounds(const glm::vec2& min_bound, const glm::vec2& max_bound)
    {
        b2EdgeShape bounds;
        bounds.Set(b2Vec2(min_bound.x * k_box2d_world_scale, min_bound.y * k_box2d_world_scale), b2Vec2(max_bound.x * k_box2d_world_scale, min_bound.y * k_box2d_world_scale));
        m_box2d_body->CreateFixture(&bounds, 0);
        bounds.Set(b2Vec2(min_bound.x * k_box2d_world_scale, max_bound.y * k_box2d_world_scale), b2Vec2(max_bound.x * k_box2d_world_scale, max_bound.y * k_box2d_world_scale));
        m_box2d_body->CreateFixture(&bounds, 0);
        bounds.Set(b2Vec2(min_bound.x * k_box2d_world_scale, max_bound.y * k_box2d_world_scale), b2Vec2(min_bound.x * k_box2d_world_scale, min_bound.y * k_box2d_world_scale));
        m_box2d_body->CreateFixture(&bounds, 0);
        bounds.Set(b2Vec2(max_bound.x * k_box2d_world_scale, max_bound.y * k_box2d_world_scale), b2Vec2(max_bound.x * k_box2d_world_scale, min_bound.y * k_box2d_world_scale));
        m_box2d_body->CreateFixture(&bounds, 0);
    }
    
    void ces_box2d_world_component::BeginContact(b2Contact* contact)
    {
        auto entity_01 = contact->GetFixtureA()->GetBody()->GetUserData();
        if(entity_01)
        {
            auto box2d_body_component = static_cast<ces_entity *>(entity_01)->get_component<ces_box2d_body_component>();
            if(box2d_body_component)
            {
                box2d_body_component->is_contacted = true;
            }
        }
        
        auto entity_02 = contact->GetFixtureB()->GetBody()->GetUserData();
        if(entity_02)
        {
            auto box2d_body_component = static_cast<ces_entity *>(entity_02)->get_component<ces_box2d_body_component>();
            if(box2d_body_component)
            {
                box2d_body_component->is_contacted = true;
            }
        }
    }
    
    void ces_box2d_world_component::EndContact(b2Contact* contact)
    {

    }
}
