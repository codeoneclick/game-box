//
//  client_base_character_controller.cpp
//  gbDemo
//
//  Created by serhii serhiiv on 1/20/17.
//  Copyright © 2017 sergey.sergeev. All rights reserved.
//

#include "client_base_character_controller.h"
#include "ces_character_controller_component.h"
#include "ces_box2d_body_component.h"
#include "ces_transformation_component.h"
#include "game_object.h"
#include "animated_sprite.h"

namespace ns
{
    client_base_character_controller::client_base_character_controller()
    {
        gb::ces_transformation_component_shared_ptr transformation_component = std::make_shared<gb::ces_transformation_component>();
        ces_entity::add_component(transformation_component);

        std::shared_ptr<ces_character_controller_component> character_controller_component = std::make_shared<ces_character_controller_component>();
        character_controller_component->set_update_callback(std::bind(&client_base_character_controller::update, this,
                                                                      std::placeholders::_1, std::placeholders::_2));
        client_base_character_controller::add_component(character_controller_component);
    }
    
    client_base_character_controller::~client_base_character_controller()
    {
        
    }
    
    void client_base_character_controller::set_character(const gb::game_object_shared_ptr& character)
    {
        m_character = character;
        client_base_character_controller::add_child(m_character);
    }
    
    void client_base_character_controller::on_changed_server_transformation(const glm::vec2& velocity,
                                                                            const glm::vec2& position,
                                                                            f32 rotation)
    {
        m_server_velocity = velocity;
        m_server_position = position;
        m_server_rotation = rotation;
    }
    
    void client_base_character_controller::update(const gb::ces_entity_shared_ptr& entity, f32 deltatime)
    {
        gb::ces_box2d_body_component_shared_ptr box2d_body_component =
        m_character->get_component<gb::ces_box2d_body_component>();
        b2Body* box2d_body = box2d_body_component->box2d_body;
        
        box2d_body->SetAwake(true);
        b2Vec2 velocity = b2Vec2(m_server_velocity.x,
                                 m_server_velocity.y);
        
        glm::vec2 current_position = glm::vec2(box2d_body->GetPosition().x, box2d_body->GetPosition().y);
        current_position = glm::mix(current_position, m_server_position, .5f);
        
        box2d_body->SetTransform(b2Vec2(current_position.x, current_position.y), m_server_rotation);
        box2d_body->SetLinearVelocity(velocity);
        
        std::list<gb::ces_entity_shared_ptr> children = m_character->children;
        for(const auto& child : children)
        {
            std::string part_name = child->tag;
            if(part_name == "feet" || part_name == "body")
            {
                gb::anim::animated_sprite_shared_ptr part = std::static_pointer_cast<gb::anim::animated_sprite>(child);
                if(fabsf(m_server_velocity.x) > 0.f || fabsf(m_server_velocity.y) > 0.f)
                {
                    part->goto_and_play("move");
                }
                else
                {
                    part->goto_and_play("idle");
                }
            }
            if(part_name == "light_source")
            {
                f32 current_rotation = m_character->rotation;
                gb::game_object_shared_ptr light_source = std::static_pointer_cast<gb::game_object>(child);
                light_source->rotation = -current_rotation;
            }
        }
    }
}

