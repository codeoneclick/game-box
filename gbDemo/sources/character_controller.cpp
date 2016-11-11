//
//  character_controller.cpp
//  gbDemo
//
//  Created by serhii serhiiv on 10/31/16.
//  Copyright © 2016 sergey.sergeev. All rights reserved.
//

#include "character_controller.h"
#include "joystick.h"
#include "ces_character_controller_component.h"
#include "ces_box2d_body_component.h"
#include "ces_transformation_component.h"
#include "game_object.h"
#include "camera.h"
#include "character_navigator.h"

character_controller::character_controller(const gb::camera_shared_ptr& camera, const gb::game_object_shared_ptr& character) :
m_camera(camera),
m_character(character),
m_joystick_angle(0.f),
m_is_dragging(false)
{
    std::shared_ptr<ces_character_controller_component> character_controller_component = std::make_shared<ces_character_controller_component>();
    character_controller_component->set_update_callback(std::bind(&character_controller::update, this,
                                                                  std::placeholders::_1, std::placeholders::_2));
    character_controller::add_component(character_controller_component);
    
    m_navigator = std::make_shared<character_navigator>(100.f, 75.f, 0.f, .1f);
}

character_controller::~character_controller()
{
    
}

void character_controller::set_joystick(const gb::ui::joystick_shared_ptr& joystick)
{
    m_joystick = joystick;
    m_joystick->set_on_dragging_callback(std::bind(&character_controller::on_joystick_dragging, this,
                                                   std::placeholders::_1, std::placeholders::_2));
    m_joystick->set_on_end_dragging_callback(std::bind(&character_controller::on_joystick_end_dragging, this,
                                                       std::placeholders::_1));
}

void character_controller::on_joystick_dragging(const gb::ces_entity_shared_ptr& joystick, const glm::vec2& delta)
{
    m_joystick_angle = atan2(delta.y, delta.x) - atan2(1.f, 0.f);
    m_is_dragging = true;
}

void character_controller::on_joystick_end_dragging(const gb::ces_entity_shared_ptr& joystick)
{
    m_is_dragging = false;
}

void character_controller::update(const gb::ces_entity_shared_ptr& entity, f32 deltatime)
{
    gb::ces_box2d_body_component_shared_ptr box2d_body_component = m_character->get_component<gb::ces_box2d_body_component>();
    b2Body* box2d_body = box2d_body_component->box2d_body;
    
    if(m_is_dragging)
    {
        box2d_body->SetAwake(true);
        
        box2d_body->SetTransform(box2d_body->GetPosition(), glm::degrees(m_joystick_angle));
        
        f32 current_rotation = box2d_body->GetAngle();
        b2Vec2 velocity = b2Vec2(sinf(glm::radians(current_rotation)) * 100.f, -cosf(glm::radians(current_rotation)) * 100.f);
        box2d_body->SetLinearVelocity(velocity);
    }
    else
    {
        box2d_body->SetLinearVelocity(b2Vec2(0.f, 0.f));
    }
}

