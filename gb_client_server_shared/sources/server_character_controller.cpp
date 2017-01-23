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

namespace ns
{
    server_character_controller::server_character_controller(const gb::game_object_shared_ptr& character) :
    m_character(character),
    m_delta(glm::vec2(0.f)),
    m_is_moving(false)
    {
        
    }
    
    server_character_controller::~server_character_controller()
    {
        
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
        f32 rotation = 0.f;
        
        if(m_is_moving)
        {
            box2d_body->SetAwake(true);
            
            f32 current_move_speed = -100.f * m_delta.y;
            rotation = m_character->rotation;
            rotation -= 2.f * m_delta.x;
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
            m_character_moving_callback(velocity, position, rotation, m_is_moving);
        }
    }
    
    void server_character_controller::set_character_moving_callback(const on_character_moving_callback_t& callback)
    {
        m_character_moving_callback = callback;
    }
}
