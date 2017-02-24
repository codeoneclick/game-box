//
//  ces_box2d_body_component.cpp
//  gbCore
//
//  Created by serhii serhiiv on 11/1/16.
//  Copyright © 2016 sergey.sergeev. All rights reserved.
//

#include "ces_box2d_body_component.h"
#include "ces_box2d_world_component.h"

namespace gb
{
    ces_box2d_body_component::ces_box2d_body_component() :
    m_box2d_body_definition(new b2BodyDef()),
    m_box2d_body(nullptr),
    m_shape(current_geometry_convex),
    m_radius(1.f)
    {
        position.getter([=] {
            glm::vec2 position = glm::vec2(m_box2d_body->GetPosition().x, m_box2d_body->GetPosition().y);
            return position / ces_box2d_world_component::k_box2d_world_scale;
        });
        
        position.setter([=](const glm::vec2& value) {
            m_box2d_body->SetTransform(b2Vec2(value.x * ces_box2d_world_component::k_box2d_world_scale, value.y * ces_box2d_world_component::k_box2d_world_scale), m_box2d_body->GetAngle());
        });
        
        rotation.getter([=] {
            f32 rotation = m_box2d_body->GetAngle();
            return rotation;
        });
        
        rotation.setter([=](f32 value) {
            m_box2d_body->SetTransform(m_box2d_body->GetPosition(), value);
        });
        
        velocity.getter([=] {
            glm::vec2 velocity = glm::vec2(m_box2d_body->GetLinearVelocity().x, m_box2d_body->GetLinearVelocity().y);
            return velocity;
        });
        
        velocity.setter([=](const glm::vec2& value) {
            m_box2d_body->SetLinearVelocity(b2Vec2(value.x, value.y));
            m_box2d_body->SetAngularVelocity(0.f);
        });
        
        box2d_body_definition.getter([=] {
            return m_box2d_body_definition;
        });
        
        box2d_body.getter([=] {
            return m_box2d_body;
        });
        
        box2d_body.setter([=] (b2Body* box2d_body) {
            m_box2d_body = box2d_body;
        });
        
        shape.getter([=] {
            return m_shape;
        });
        
        shape.setter([=] (e_shape shape) {
            m_shape = shape;
        });
    }
    
    ces_box2d_body_component::~ces_box2d_body_component()
    {
        
    }
    
    void ces_box2d_body_component::set_radius(f32 radius)
    {
        m_radius = radius * ces_box2d_world_component::k_box2d_world_scale;
    }
    
    f32 ces_box2d_body_component::get_radius() const
    {
        return m_radius;
    }
    
    void ces_box2d_body_component::set_custom_vertices(const std::vector<b2Vec2>& vertices)
    {
        m_custom_vertices.clear();
        for(const auto& vertex : vertices)
        {
            m_custom_vertices.push_back(b2Vec2(vertex.x * ces_box2d_world_component::k_box2d_world_scale, vertex.y * ces_box2d_world_component::k_box2d_world_scale));
        }
    }
    
    const std::vector<b2Vec2>& ces_box2d_body_component::get_custom_vertices() const
    {
        return m_custom_vertices;
    }
}
