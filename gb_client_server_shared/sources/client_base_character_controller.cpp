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
    client_base_character_controller::client_base_character_controller(const gb::game_object_shared_ptr& character) :
    m_character(character)
    {
        std::shared_ptr<ces_character_controller_component> character_controller_component = std::make_shared<ces_character_controller_component>();
        character_controller_component->set_update_callback(std::bind(&client_base_character_controller::update, this,
                                                                      std::placeholders::_1, std::placeholders::_2));
        client_base_character_controller::add_component(character_controller_component);
    }
    
    client_base_character_controller::~client_base_character_controller()
    {
        
    }
    
    void client_base_character_controller::on_changed_server_transformation(const glm::vec2& velocity,
                                                                            const glm::vec2& position,
                                                                            f32 rotation,
                                                                            bool is_moving)
    {
        m_server_velocity = velocity;
        m_server_position = position;
        m_server_rotation = rotation;
        m_is_moving = is_moving;
    }
    
    void client_base_character_controller::update(const gb::ces_entity_shared_ptr& entity, f32 deltatime)
    {
        gb::ces_box2d_body_component_shared_ptr box2d_body_component =
        m_character->get_component<gb::ces_box2d_body_component>();
        b2Body* box2d_body = box2d_body_component->box2d_body;
        
        if(m_is_moving)
        {
            box2d_body->SetAwake(true);
            b2Vec2 velocity = b2Vec2(m_server_velocity.x,
                                     m_server_velocity.y);
            box2d_body->SetTransform(box2d_body->GetPosition(), m_server_rotation);
            box2d_body->SetLinearVelocity(velocity);
            
            std::list<gb::ces_entity_shared_ptr> children = m_character->children;
            for(const auto& child : children)
            {
                std::string part_name = child->tag;
                if(part_name == "feet" || part_name == "body")
                {
                    gb::anim::animated_sprite_shared_ptr part = std::static_pointer_cast<gb::anim::animated_sprite>(child);
                    part->goto_and_play("move");
                }
                if(part_name == "light_source")
                {
                    f32 current_rotation = m_character->rotation;
                    gb::game_object_shared_ptr light_source = std::static_pointer_cast<gb::game_object>(child);
                    light_source->rotation = -current_rotation;
                }
            }
        }
        else
        {
            box2d_body->SetLinearVelocity(b2Vec2(0.f, 0.f));
            
            std::list<gb::ces_entity_shared_ptr> children = m_character->children;
            for(const auto& child : children)
            {
                std::string part_name = child->tag;
                if(part_name == "feet" || part_name == "body")
                {
                    gb::anim::animated_sprite_shared_ptr part = std::static_pointer_cast<gb::anim::animated_sprite>(child);
                    part->goto_and_play("idle");
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
}

