//
//  ces_box2d_world_component.cpp
//  gbCore
//
//  Created by serhii serhiiv on 11/1/16.
//  Copyright © 2016 sergey.sergeev. All rights reserved.
//

#include "ces_box2d_world_component.h"

namespace gb
{
    ces_box2d_world_component::ces_box2d_world_component()
    {
        m_box2d_world = std::make_shared<b2World>(b2Vec2(0.f, 0.f));
        m_box2d_world->SetContinuousPhysics(true);
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
        bounds.Set(b2Vec2(min_bound.x, min_bound.y), b2Vec2(max_bound.x, min_bound.y));
        m_box2d_body->CreateFixture(&bounds, 0);
        bounds.Set(b2Vec2(min_bound.x, max_bound.y), b2Vec2(max_bound.x, max_bound.y));
        m_box2d_body->CreateFixture(&bounds, 0);
        bounds.Set(b2Vec2(min_bound.x, max_bound.y), b2Vec2(min_bound.x, min_bound.y));
        m_box2d_body->CreateFixture(&bounds, 0);
        bounds.Set(b2Vec2(max_bound.x, max_bound.y), b2Vec2(max_bound.x, min_bound.y));
        m_box2d_body->CreateFixture(&bounds, 0);
    }
}