//
//  server_character_controller.cpp
//  gbDemo
//
//  Created by serhii serhiiv on 1/20/17.
//  Copyright © 2017 sergey.sergeev. All rights reserved.
//

#include "server_character_controller.h"
#include "ces_character_controller_component.h"
#include "ces_box2d_body_component.h"
#include "ces_transformation_component.h"
#include "game_object.h"
#include "animated_sprite.h"
#include "glm_extensions.h"

namespace ns
{
    server_character_controller::server_character_controller(ui32 udid) :
    m_udid(udid),
    m_delta(glm::vec2(0.f)),
    m_is_moving(false)
    {
        gb::ces_transformation_component_shared_ptr transformation_component = std::make_shared<gb::ces_transformation_component>();
        ces_entity::add_component(transformation_component);

        std::shared_ptr<ces_character_controller_component> character_controller_component = std::make_shared<ces_character_controller_component>();
        character_controller_component->set_update_callback(std::bind(&server_character_controller::update, this,
                                                                      std::placeholders::_1, std::placeholders::_2));
        server_character_controller::add_component(character_controller_component);
    }
    
    server_character_controller::~server_character_controller()
    {
        
    }
    
    void server_character_controller::set_character(const gb::game_object_shared_ptr& character)
    {
        m_character = character;
        server_character_controller::add_child(m_character);
    }
    
    void server_character_controller::on_changed_server_transformation(const glm::vec2& delta,
                                                                       bool is_moving)
    {
        m_delta = delta;
        m_is_moving = is_moving;
    }
    
    void server_character_controller::update(const gb::ces_entity_shared_ptr& entity, f32 deltatime)
    {
        gb::ces_box2d_body_component_shared_ptr box2d_body_component =
        m_character->get_component<gb::ces_box2d_body_component>();
        b2Body* box2d_body = box2d_body_component->box2d_body;
        
        glm::vec2 velocity = glm::vec2(0.f);
        glm::vec2 position = m_character->position;
        f32 rotation = m_character->rotation;
        
        if(m_is_moving)
        {
            box2d_body->SetAwake(true);
            glm::vec2 delta = m_delta * deltatime * 60.f;
            f32 current_move_speed = -100.f * delta.y;
            rotation -= 2.f * delta.x;
            rotation = glm::wrap_degrees(rotation);
            b2Vec2 b2_velocity = b2Vec2(-sinf(glm::radians(rotation)) * current_move_speed,
                                     cosf(glm::radians(rotation)) * current_move_speed);
            box2d_body->SetTransform(box2d_body->GetPosition(), rotation);
            box2d_body->SetLinearVelocity(b2_velocity);
            velocity.x = b2_velocity.x;
            velocity.y = b2_velocity.y;
        }
        else
        {
            box2d_body->SetLinearVelocity(b2Vec2(0.f, 0.f));
        }
        
        if(m_character_moving_callback)
        {
            m_character_moving_callback(m_udid, velocity, position, rotation, m_is_moving);
        }
    }
    
    void server_character_controller::set_character_moving_callback(const on_character_moving_callback_t& callback)
    {
        m_character_moving_callback = callback;
    }
    
    glm::vec2 server_character_controller::get_position() const
    {
        glm::vec2 position = m_character->position;
        return position;
    }
    
    f32 server_character_controller::get_rotation() const
    {
        f32 rotation = m_character->rotation;
        return rotation;
    }
}
