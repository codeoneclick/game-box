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
    const f32 ces_box2d_world_component::k_box2d_world_scale = 1.f;
    ui32 ces_box2d_world_component::g_box2d_body_guid = 0;
    
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
        ces_entity_shared_ptr entity_01 = nullptr;
        ces_entity_shared_ptr entity_02 = nullptr;
        if(contact->GetFixtureA()->GetBody()->GetUserData())
        {
            ui32 entity_guid = *static_cast<ui32*>(contact->GetFixtureA()->GetBody()->GetUserData());
            entity_01 = ces_box2d_world_component::get_box2d_body_entity(entity_guid);
        }
        if(contact->GetFixtureB()->GetBody()->GetUserData())
        {
            ui32 entity_guid = *static_cast<ui32*>(contact->GetFixtureB()->GetBody()->GetUserData());
            entity_02 = ces_box2d_world_component::get_box2d_body_entity(entity_guid);
        }

        if(entity_01)
        {
            auto box2d_body_component = entity_01->get_component<ces_box2d_body_component>();
            if(box2d_body_component)
            {
                if(box2d_body_component->is_destructable_on_contact)
                {
                    contact->SetEnabled(false);
                }
                box2d_body_component->is_contacted = true;
                box2d_body_component->contacted_entity = entity_02;
            }
        }
        
        if(entity_02)
        {
            auto box2d_body_component = entity_02->get_component<ces_box2d_body_component>();
            if(box2d_body_component)
            {
                if(box2d_body_component->is_destructable_on_contact)
                {
                    contact->SetEnabled(false);
                }
                box2d_body_component->is_contacted = true;
                box2d_body_component->contacted_entity = entity_01;
            }
        }
    }
    
    void ces_box2d_world_component::EndContact(b2Contact* contact)
    {
        ces_entity_shared_ptr entity_01 = nullptr;
        ces_entity_shared_ptr entity_02 = nullptr;
        if(contact->GetFixtureA()->GetBody()->GetUserData())
        {
            ui32 entity_guid = *static_cast<ui32*>(contact->GetFixtureA()->GetBody()->GetUserData());
            entity_01 = ces_box2d_world_component::get_box2d_body_entity(entity_guid);
        }
        if(contact->GetFixtureB()->GetBody()->GetUserData())
        {
            ui32 entity_guid = *static_cast<ui32*>(contact->GetFixtureB()->GetBody()->GetUserData());
            entity_02 = ces_box2d_world_component::get_box2d_body_entity(entity_guid);
        }
        
        if(entity_01)
        {
            auto box2d_body_component = entity_01->get_component<ces_box2d_body_component>();
            if(box2d_body_component)
            {
                box2d_body_component->is_contacted = false;
                box2d_body_component->contacted_entity = nullptr;
            }
        }
        
        if(entity_02)
        {
            auto box2d_body_component = entity_02->get_component<ces_box2d_body_component>();
            if(box2d_body_component)
            {
                box2d_body_component->is_contacted = false;
                box2d_body_component->contacted_entity = nullptr;
            }
        }
    }
    
    ui32 ces_box2d_world_component::register_box2d_body_entity(const gb::ces_entity_shared_ptr& entity)
    {
        m_box2d_body_entities[++g_box2d_body_guid] = entity;
        return g_box2d_body_guid;
    }
    
    void ces_box2d_world_component::unregister_box2d_body_entity(ui32 guid)
    {
        auto it = m_box2d_body_entities.find(guid);
        if(it != m_box2d_body_entities.end())
        {
            m_box2d_body_entities.erase(it);
        }
    }
    
    gb::ces_entity_shared_ptr ces_box2d_world_component::get_box2d_body_entity(ui32 guid)
    {
        ces_entity_shared_ptr entity = nullptr;
        auto it = m_box2d_body_entities.find(guid);
        if(it != m_box2d_body_entities.end() && !it->second.expired())
        {
            entity = it->second.lock();
        }
        return entity;
    }
}
