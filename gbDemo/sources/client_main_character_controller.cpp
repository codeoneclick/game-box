//
//  client_main_character_controller.cpp
//  gbDemo
//
//  Created by serhii serhiiv on 10/31/16.
//  Copyright © 2016 sergey.sergeev. All rights reserved.
//

#include "client_main_character_controller.h"
#include "joystick.h"
#include "ces_character_controller_component.h"
#include "ces_box2d_body_component.h"
#include "ces_transformation_component.h"
#include "game_object.h"
#include "animated_sprite.h"
#include "camera.h"

namespace ns
{
    client_main_character_controller::client_main_character_controller(const gb::camera_shared_ptr& camera,
                                                                       const gb::game_object_shared_ptr& character) :
    client_base_character_controller(character),
    m_camera(camera),
    m_joystick_delta(glm::vec2(0.f)),
    m_is_dragging(false),
    m_character_moving_callback(nullptr)
    {
        
    }
    
    client_main_character_controller::~client_main_character_controller()
    {
        
    }
    
    void client_main_character_controller::set_joystick(const gb::ui::joystick_shared_ptr& joystick)
    {
        m_joystick = joystick;
        m_joystick->set_on_dragging_callback(std::bind(&client_main_character_controller::on_joystick_dragging, this,
                                                       std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
        m_joystick->set_on_end_dragging_callback(std::bind(&client_main_character_controller::on_joystick_end_dragging, this,
                                                           std::placeholders::_1));
    }
    
    void client_main_character_controller::set_character_moving_callback(const on_character_moving_callback_t& callback)
    {
        m_character_moving_callback = callback;
    }
    
    void client_main_character_controller::on_joystick_dragging(const gb::ces_entity_shared_ptr& joystick,
                                                                const glm::vec2& delta, f32 angle)
    {
        m_joystick_delta = delta;
        m_is_dragging = true;
    }
    
    void client_main_character_controller::on_joystick_end_dragging(const gb::ces_entity_shared_ptr& joystick)
    {
        m_joystick_delta = glm::vec2(0.f);
        m_is_dragging = false;
    }
    
    void client_main_character_controller::update(const gb::ces_entity_shared_ptr& entity, f32 deltatime)
    {
        gb::ces_box2d_body_component_shared_ptr box2d_body_component =
        m_character->get_component<gb::ces_box2d_body_component>();
        b2Body* box2d_body = box2d_body_component->box2d_body;
        
        if(m_is_dragging)
        {
            box2d_body->SetAwake(true);
        
            f32 current_move_speed = -100.f * m_joystick_delta.y;
            f32 current_rotation = m_character->rotation;
            current_rotation -= 2.f * m_joystick_delta.x;
            b2Vec2 velocity = b2Vec2(-sinf(glm::radians(current_rotation)) * current_move_speed,
                                     cosf(glm::radians(current_rotation)) * current_move_speed);
            box2d_body->SetTransform(box2d_body->GetPosition(), current_rotation);
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
            
            glm::vec2 current_position = m_character->position;
            m_camera->set_position(current_position * -1.f);
            m_camera->set_rotation(-current_rotation);
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
            
            glm::vec2 current_position = m_character->position;
            f32 current_rotation = m_character->rotation;
            m_camera->set_position(current_position * -1.f);
            m_camera->set_rotation(-current_rotation);
        }
        
        if(m_character_moving_callback)
        {
            m_character_moving_callback(m_joystick_delta, m_is_dragging);
        }
    }
}

