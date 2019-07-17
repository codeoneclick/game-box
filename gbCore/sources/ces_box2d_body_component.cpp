//
//  ces_box2d_body_component.cpp
//  gbCore
//
//  Created by serhii serhiiv on 11/1/16.
//  Copyright © 2016 sergey.sergeev. All rights reserved.
//

#include "ces_box2d_body_component.h"
#include "ces_box2d_world_component.h"
#include "glm_extensions.h"

namespace gb
{
    void ces_box2d_body_component::box2d_shape_parameters::set_radius(f32 radius)
    {
        m_radius = radius * ces_box2d_world_component::k_box2d_world_scale;
    }
    
    void ces_box2d_body_component::box2d_shape_parameters::set_custom_vertices(const std::vector<b2Vec2>& vertices)
    {
        m_custom_vertices.clear();
        for(const auto& vertex : vertices)
        {
            m_custom_vertices.push_back(b2Vec2(vertex.x * ces_box2d_world_component::k_box2d_world_scale, vertex.y * ces_box2d_world_component::k_box2d_world_scale));
        }
    }
    
    ces_box2d_body_component::ces_box2d_body_component() :
    m_is_applied(false),
    m_box2d_body_definition(new b2BodyDef()),
    m_box2d_body(nullptr),
    m_is_contacted(false),
    m_is_destructable_on_contact(false),
    m_body_entity_guid(0)
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
            m_box2d_body->SetTransform(m_box2d_body->GetPosition(), glm::wrap_radians(glm::radians(value)));
        });
        
        linear_velocity.getter([=] {
            glm::vec2 velocity = glm::vec2(m_box2d_body->GetLinearVelocity().x, m_box2d_body->GetLinearVelocity().y);
            return velocity;
        });
        
        linear_velocity.setter([=](const glm::vec2& value) {
            /*m_box2d_body->ApplyLinearImpulse(b2Vec2(value.x * m_box2d_body->GetMass(),
                                                    value.y * m_box2d_body->GetMass()),
                                             m_box2d_body->GetWorldCenter(), true);*/
            m_box2d_body->SetLinearVelocity(b2Vec2(value.x, value.y));
            
        });
        
        angular_velocity.getter([=] {
            return m_box2d_body->GetAngularVelocity();
        });
        
        angular_velocity.setter([=](f32 value) {
            m_box2d_body->SetAngularVelocity(value);
        });
        
        box2d_body_definition.getter([=] {
            return m_box2d_body_definition;
        });
        
        box2d_body.getter([=] {
            return m_box2d_body;
        });
        
        box2d_body.setter([=] (b2Body* box2d_body) {
            if(!box2d_body)
            {
                if(m_box2d_body)
                {
                    m_box2d_body->GetWorld()->DestroyBody(m_box2d_body);
                }
            }
            else
            {
                m_box2d_body = box2d_body;
            }
        });
        
        is_contacted.getter([=] {
            return m_is_contacted;
        });
        
        is_contacted.setter([=] (bool is_contacted) {
            m_is_contacted = is_contacted;
        });
        
        is_destructable_on_contact.getter([=] {
            return m_is_destructable_on_contact;
        });
        
        is_destructable_on_contact.setter([=] (bool is_destructable_on_contact) {
            m_is_destructable_on_contact = is_destructable_on_contact;
        });
        
        contacted_entity.getter([=] {
            return m_contacted_entity.lock();
        });
        
        contacted_entity.setter([=] (const ces_entity_shared_ptr& contacted_entity) {
            m_contacted_entity = contacted_entity;
        });
        
        body_entity_guid.getter([=] {
            return m_body_entity_guid;
        });
        
        body_entity_guid.setter([=] (ui32 body_entity_guid) {
            m_body_entity_guid = body_entity_guid;
            assert(m_box2d_body);
            m_box2d_body->SetUserData(&m_body_entity_guid);
        });
        
        enabled.getter([=]() {
            return m_box2d_body->IsActive();
        });
        
        enabled.setter([=](bool value) {
            m_box2d_body->SetActive(value);
        });
        
        is_applied.getter([=]() {
            return m_is_applied;
        });
        
        is_applied.setter([=](bool value) {
            m_is_applied = value;
        });
    }
    
    ces_box2d_body_component::~ces_box2d_body_component()
    {
        ces_box2d_body_component::box2d_body = nullptr;
        delete m_box2d_body_definition;
    }
    
    void ces_box2d_body_component::set_deferred_box2d_component_setup(const ces_entity_shared_ptr& entity, b2BodyType body, const deferred_box2d_component_setup_t& callback)
    {
        m_deferred_box2d_component_setup = std::make_tuple(entity, body, callback);
    }
    
    const std::tuple<ces_entity_weak_ptr, b2BodyType, ces_box2d_body_component::deferred_box2d_component_setup_t>& ces_box2d_body_component::get_deferred_box2d_component_setup() const
    {
        return m_deferred_box2d_component_setup;
    }
    
    void ces_box2d_body_component::set_custom_box2d_body_setup(const custom_box2d_body_setup_t& callback)
    {
        m_custom_box2d_body_setup = callback;
    }
    
    ces_box2d_body_component::custom_box2d_body_setup_t ces_box2d_body_component::get_custom_box2d_body_setup() const
    {
        return m_custom_box2d_body_setup;
    }
    
    bool ces_box2d_body_component::is_custom_box2d_body_setup_exist() const
    {
        return m_custom_box2d_body_setup != nullptr;
    }
    
    void ces_box2d_body_component::add_shape_parameters(const box2d_shape_parameters& parameters)
    {
        m_shapes_parameters.emplace_back(parameters);
    }
    
    std::vector<ces_box2d_body_component::box2d_shape_parameters> ces_box2d_body_component::get_shapes_parameters() const
    {
        return m_shapes_parameters;
    }
    
    void ces_box2d_body_component::add_to_contact_ignore_list(const ces_entity_shared_ptr& entity)
    {
        m_contact_ignore_list.push_back(entity);
    }
    
    std::list<ces_entity_weak_ptr> ces_box2d_body_component::get_contact_ignore_list() const
    {
        /*std::remove_if(m_contact_ignore_list.begin(), m_contact_ignore_list.end(), [=](const ces_entity_weak_ptr& entity) {
            return entity.expired();
        });*/
        return m_contact_ignore_list;
    }
}
