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
    m_server_character_move_callback(nullptr)
    {
        gb::ces_transformation_component_shared_ptr transformation_component = std::make_shared<gb::ces_transformation_component>();
        ces_entity::add_component(transformation_component);

        std::shared_ptr<ces_character_controller_component> character_controller_component = std::make_shared<ces_character_controller_component>();
        character_controller_component->set_update_callback(std::bind(&server_character_controller::update, this,
                                                                      std::placeholders::_1, std::placeholders::_2));
        server_character_controller::add_component(character_controller_component);
        
        position.getter([=]() {
            glm::vec2 position = m_character->position;
            return position;
        });
        
        rotation.getter([=]() {
            f32 rotation = m_character->rotation;
            return rotation;
        });
    }
    
    server_character_controller::~server_character_controller()
    {
        
    }
    
    void server_character_controller::set_character(const gb::game_object_shared_ptr& character)
    {
        m_character = character;
        server_character_controller::add_child(m_character);
    }
    
    void server_character_controller::on_client_character_move(ui64 client_tick, const glm::vec2 &delta, bool is_move)
    {
        client_character_move_history_point history_point;
        history_point.m_client_tick = client_tick;
        history_point.m_delta = delta;
        history_point.m_is_move = is_move;
        m_client_character_move_history.push(history_point);
    }
    
#define k_move_speed -100.f
#define k_rotate_speed 2.f
    
    void server_character_controller::update(const gb::ces_entity_shared_ptr& entity, f32 deltatime)
    {
        gb::ces_box2d_body_component_shared_ptr box2d_body_component =
        m_character->get_component<gb::ces_box2d_body_component>();
        b2Body* box2d_body = box2d_body_component->box2d_body;
        
        glm::vec2 velocity = glm::vec2(0.f);
        glm::vec2 position = m_character->position;
        f32 rotation = m_character->rotation;
        ui64 last_client_tick = std::numeric_limits<ui64>::max();
        bool is_client_move = false;
        
        if(!m_client_character_move_history.empty())
        {
            while(!m_client_character_move_history.empty())
            {
                client_character_move_history_point history_point = m_client_character_move_history.front();
                m_client_character_move_history.pop();
                
                last_client_tick = history_point.m_client_tick;
                is_client_move = history_point.m_is_move;
                
                glm::vec2 delta = history_point.m_delta;
                f32 move_speed = k_move_speed * delta.y;
                rotation -= k_rotate_speed * delta.x;
                rotation = glm::wrap_degrees(rotation);
                velocity += glm::vec2(-sinf(glm::radians(rotation)) * move_speed,
                                      cosf(glm::radians(rotation)) * move_speed);
                
            }
            b2Vec2 b2_velocity = b2Vec2(velocity.x,
                                        velocity.y);
            
            box2d_body->SetAwake(true);
            box2d_body->SetTransform(box2d_body->GetPosition(), rotation);
            box2d_body->SetLinearVelocity(b2_velocity);
        }
        else
        {
            box2d_body->SetAwake(false);
            rotation = glm::wrap_degrees(rotation);
            box2d_body->SetLinearVelocity(b2Vec2(0.f, 0.f));
            box2d_body->SetTransform(box2d_body->GetPosition(), box2d_body->GetAngle());
        }
        
        if(m_server_character_move_callback && last_client_tick != std::numeric_limits<ui64>::max())
        {
            m_server_character_move_callback(last_client_tick, m_udid, velocity, position, rotation, is_client_move);
        }
    }
    
    void server_character_controller::set_server_character_move_callback(const on_server_character_move_callback_t& callback)
    {
        m_server_character_move_callback = callback;
    }
}
