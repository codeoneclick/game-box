//
//  client_base_character_controller.cpp
//  gbDemo
//
//  Created by serhii serhiiv on 1/20/17.
//  Copyright © 2017 sergey.sergeev. All rights reserved.
//

#include "client_base_character_controller.h"
#include "ces_box2d_body_component.h"
#include "ces_transformation_component.h"
#include "ces_character_controller_component.h"
#include "animated_sprite.h"
#include "ces_action_component.h"
#include "character.h"
#include "thread_operation.h"
#include "information_bubble_controller.h"
#include "bloodprint_controller.h"
#include "footprint_controller.h"

namespace game
{
    client_base_character_controller::client_base_character_controller(const gb::scene_graph_shared_ptr& scene_graph,
                                                                       const gb::scene_fabricator_shared_ptr& scene_fabricator,
                                                                       const gb::anim::anim_fabricator_shared_ptr& anim_fabricator,
                                                                       const std::array<gb::game_object_weak_ptr, level::e_level_layer_max>& layers) :
    m_scene_graph(scene_graph),
    m_scene_fabricator(scene_fabricator),
    m_anim_fabricator(anim_fabricator),
    m_layers(layers),
    m_spawn_point(0.f),
    m_footprint_previous_timestamp(std::chrono::steady_clock::now()),
    m_statistic_callback(nullptr),
    m_dead_cooldown_timeinterval(10000.f),
    m_dead_timestamp(std::chrono::steady_clock::now())
    {
        std::shared_ptr<gb::ces_action_component> action_component = std::make_shared<gb::ces_action_component>();
        action_component->set_update_callback(std::bind(&client_base_character_controller::update, this,
                                                        std::placeholders::_1, std::placeholders::_2));
        client_base_character_controller::add_component(action_component);
        
        std::shared_ptr<ces_character_controller_component> character_controller_component = std::make_shared<game::ces_character_controller_component>();
        client_base_character_controller::add_component(character_controller_component);
    }
    
    client_base_character_controller::~client_base_character_controller()
    {
        
    }
    
    void client_base_character_controller::setup(const std::string& filename)
    {
        m_character = std::make_shared<character>();
        std::static_pointer_cast<character>(m_character)->setup(filename,
                                                                m_scene_graph.lock(),
                                                                m_scene_fabricator.lock(),
                                                                m_anim_fabricator.lock(),
                                                                true, glm::vec4(0.f, 1.f, 0.f, 1.f));
        client_base_character_controller::add_child(m_character);
        
        information_bubble_controller_shared_ptr information_bubble_controller = std::make_shared<game::information_bubble_controller>(m_layers[level::e_level_layer_information_bubbles].lock(),
                                                                                                                                       m_scene_graph.lock(),
                                                                                                                                       m_scene_fabricator.lock());
        m_information_bubble_controller = information_bubble_controller;
        client_base_character_controller::add_child(information_bubble_controller);
        
        bloodprint_controller_shared_ptr bloodprint_controller = std::make_shared<game::bloodprint_controller>(m_layers[level::e_level_layer_bloodprints].lock(),
                                                                                                               m_scene_graph.lock(),
                                                                                                               m_scene_fabricator.lock());
        m_bloodprint_controller = bloodprint_controller;
        client_base_character_controller::add_child(bloodprint_controller);
        
        footprint_controller_shared_ptr footprint_controller = std::make_shared<game::footprint_controller>(m_layers[level::e_level_layer_footprints].lock(),
                                                                                                            m_scene_graph.lock(),
                                                                                                            m_scene_fabricator.lock());
        m_footprint_controller = footprint_controller;
        client_base_character_controller::add_child(footprint_controller);
        
        auto character_controller_component = client_base_character_controller::get_component<game::ces_character_controller_component>();
        character_controller_component->information_bubble_controller = information_bubble_controller;
        character_controller_component->bloodprint_controller = bloodprint_controller;
        character_controller_component->footprint_controller = footprint_controller;
        character_controller_component->set_dead_callback(std::bind(&client_base_character_controller::on_dead, this, std::placeholders::_1));
        character_controller_component->set_health_changed_callback(std::bind(&client_base_character_controller::on_health_changed, this, std::placeholders::_1, std::placeholders::_2));
        character_controller_component->set_kill_callback(std::bind(&client_base_character_controller::on_kill, this, std::placeholders::_1, std::placeholders::_2));
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
        client_base_character_controller::get_component<gb::ces_box2d_body_component>();
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
                f32 current_rotation = client_base_character_controller::rotation;
                gb::game_object_shared_ptr light_source = std::static_pointer_cast<gb::game_object>(child);
                light_source->rotation = -current_rotation;
            }
        }
    }
    
    
    void client_base_character_controller::set_spawn_point(const glm::vec2& spawn_point)
    {
        m_spawn_point = spawn_point;
    }
    
    void client_base_character_controller::on_move()
    {
        std::list<gb::ces_entity_shared_ptr> children = m_character->children;
        for(const auto& child : children)
        {
            std::string part_name = child->tag;
            if(part_name == "feet" || part_name == "body")
            {
                gb::anim::animated_sprite_shared_ptr part = std::static_pointer_cast<gb::anim::animated_sprite>(child);
                part->goto_and_play("move");
            }
        }
    }
    
    void client_base_character_controller::on_idle()
    {
        std::list<gb::ces_entity_shared_ptr> children = m_character->children;
        for(const auto& child : children)
        {
            std::string part_name = child->tag;
            if(part_name == "feet" || part_name == "body")
            {
                gb::anim::animated_sprite_shared_ptr part = std::static_pointer_cast<gb::anim::animated_sprite>(child);
                part->goto_and_play("idle");
            }
        }
    }
    
    void client_base_character_controller::on_health_changed(const gb::ces_entity_shared_ptr& entity, f32 health)
    {
        if(!m_information_bubble_controller.expired())
        {
            auto information_bubble_controller = m_information_bubble_controller.lock();
            glm::vec2 current_position = client_base_character_controller::position;
            std::stringstream string_stream;
            string_stream<<-health;
            information_bubble_controller->push_bubble(string_stream.str(), glm::u8vec4(255, 0, 0, 255), current_position);
        }
        
        if(!m_bloodprint_controller.expired())
        {
            auto bloodprint_controller = m_bloodprint_controller.lock();
            glm::vec2 current_position = client_base_character_controller::position;
            f32 current_rotation = client_base_character_controller::rotation;
            bloodprint_controller->push_bloodprint(glm::u8vec4(255, 0, 0, 255), current_position, current_rotation);
        }
    }
    
    void client_base_character_controller::on_dead(const gb::ces_entity_shared_ptr& entity)
    {
        m_dead_timestamp = std::chrono::steady_clock::now();
        gb::ces_box2d_body_component_shared_ptr box2d_body_component =
        client_base_character_controller::get_component<gb::ces_box2d_body_component>();
        box2d_body_component->velocity = glm::vec2(0.f);
        box2d_body_component->enabled = false;
        client_base_character_controller::on_idle();
    }
    
    void client_base_character_controller::on_kill(const gb::ces_entity_shared_ptr& owner, const gb::ces_entity_shared_ptr& target)
    {
        std::string owner_name = owner->tag;
        std::string targer_name = target->tag;
        std::stringstream string_stream;
        string_stream<<owner_name<<" - "<<targer_name;
        if(m_statistic_callback)
        {
            m_statistic_callback(string_stream.str());
        }
    }
    
    void client_base_character_controller::on_revive()
    {
        client_base_character_controller::position = m_spawn_point;
        auto character_controller_component = client_base_character_controller::get_component<game::ces_character_controller_component>();
        character_controller_component->reset_health();
        gb::ces_box2d_body_component_shared_ptr box2d_body_component =
        client_base_character_controller::get_component<gb::ces_box2d_body_component>();
        box2d_body_component->enabled = true;
    }
    
    void client_base_character_controller::set_statistic_callback(const statistic_callback_t& callback)
    {
        m_statistic_callback = callback;
    }
}

