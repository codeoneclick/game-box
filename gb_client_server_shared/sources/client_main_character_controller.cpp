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
#include "glm_extensions.h"

namespace ns
{
    client_main_character_controller::client_main_character_controller(const gb::camera_shared_ptr& camera) :
    m_camera(camera),
    m_joystick_delta(glm::vec2(0.f)),
    m_is_dragging(false),
    m_character_moving_callback(nullptr),
    m_synchronization_callback(nullptr),
    m_server_adjust_position(glm::vec2(0.f)),
    m_server_adjust_rotation(0.f),
    m_timestamp(0)
    {
        m_last_server_timestamp = std::numeric_limits<ui64>::max();
    }
    
    client_main_character_controller::~client_main_character_controller()
    {
        
    }
    
    void client_main_character_controller::set_character(const gb::game_object_shared_ptr& character)
    {
        client_base_character_controller::set_character(character);
        m_server_adjust_position = character->position;
        m_server_adjust_rotation = character->rotation;
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
    
    void client_main_character_controller::set_synchronization_callback(const on_synchronization_callback_t& callback)
    {
        m_synchronization_callback = callback;
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
    
    void client_main_character_controller::synchronize_transformations(ui64 timestamp, const glm::vec2& position, const f32 rotation)
    {
        m_last_server_timestamp = timestamp;
        
        m_server_adjust_position = position;
        m_server_adjust_rotation = rotation;
    }
    
    void client_main_character_controller::update(const gb::ces_entity_shared_ptr& entity, f32 deltatime)
    {
        gb::ces_box2d_body_component_shared_ptr box2d_body_component =
        m_character->get_component<gb::ces_box2d_body_component>();
        b2Body* box2d_body = box2d_body_component->box2d_body;
        
        bool is_synchronized = true;
        
        if(m_synchronization_callback && m_last_server_timestamp != std::numeric_limits<ui64>::max())
        {
            is_synchronized = m_synchronization_callback(m_last_server_timestamp,
                                                         m_server_adjust_position,
                                                         m_server_adjust_rotation);
            m_last_server_timestamp = std::numeric_limits<ui64>::max();
        }
        
        if(m_is_dragging)
        {
            box2d_body->SetAwake(true);
            glm::vec2 delta = m_joystick_delta * deltatime * 60.f;
            f32 current_move_speed = -100.f * delta.y;
            f32 current_rotation = m_character->rotation;
            current_rotation -= 2.f * delta.x;
            current_rotation = glm::wrap_degrees(current_rotation);
            current_rotation = glm::mix_angles_degrees(current_rotation, m_server_adjust_rotation, .5f);
            
            glm::vec2 current_position = glm::vec2(box2d_body->GetPosition().x, box2d_body->GetPosition().y);
            current_position = glm::mix(current_position, m_server_adjust_position, .5f);
            
            b2Vec2 velocity = b2Vec2(-sinf(glm::radians(current_rotation)) * current_move_speed,
                                     cosf(glm::radians(current_rotation)) * current_move_speed);
            box2d_body->SetTransform(b2Vec2(current_position.x, current_position.y), current_rotation);
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
            
            m_camera->set_position(current_position * -1.f);
            m_camera->set_rotation(-current_rotation);
            
            //std::cout<<"velocity: "<<velocity.x<<", "<<velocity.y<<" rotation: "<<current_rotation<<std::endl;
        }
        else
        {
            box2d_body->SetAwake(false);
            f32 current_rotation = m_character->rotation;
            current_rotation = glm::wrap_degrees(current_rotation);
            current_rotation = glm::mix_angles_degrees(current_rotation, m_server_adjust_rotation, .5f);
            
            glm::vec2 current_position = glm::vec2(box2d_body->GetPosition().x, box2d_body->GetPosition().y);
            current_position = glm::mix(current_position, m_server_adjust_position, .5f);
            box2d_body->SetTransform(b2Vec2(current_position.x, current_position.y), current_rotation);
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
            
            m_camera->set_position(current_position * -1.f);
            m_camera->set_rotation(-current_rotation);
            //std::cout<<"velocity: 0, 0"<<" rotation: "<<current_rotation<<std::endl;
        }
        
        if(m_character_moving_callback)
        {
            m_character_moving_callback(m_timestamp, m_joystick_delta, m_is_dragging);
            m_timestamp++;
        }
    }
    
    glm::vec2 client_main_character_controller::get_position() const
    {
        glm::vec2 position = m_character->position;
        return position;
    }
    
    f32 client_main_character_controller::get_rotation() const
    {
        f32 rotation = m_character->rotation;
        return rotation;
    }
}

