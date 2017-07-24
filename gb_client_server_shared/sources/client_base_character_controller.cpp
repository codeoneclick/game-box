//
//  client_base_character_controller.cpp
//  gbDemo
//
//  Created by serhii serhiiv on 1/20/17.
//  Copyright © 2017 sergey.sergeev. All rights reserved.
//

#include "client_base_character_controller.h"
#include "ces_box2d_body_component.h"
#include "ces_transformation_2d_component.h"
#include "ces_character_controllers_component.h"
#include "ces_character_statistic_component.h"
#include "animated_sprite.h"
#include "ces_action_component.h"
#include "character.h"
#include "hit_bounds.h"
#include "scene_graph.h"
#include "thread_operation.h"
#include "information_bubble_controller.h"
#include "bloodprint_controller.h"
#include "footprint_controller.h"
#include "scene_fabricator.h"
#include "thread_operation.h"

namespace game
{
    const std::string client_base_character_controller::k_character_statistic_part = "character_statistic";
    const std::string client_base_character_controller::k_character_statistic_entity_filename = "character.statistic.2d.xml";
    
    client_base_character_controller::client_base_character_controller(const gb::scene_graph_shared_ptr& scene_graph,
                                                                       const gb::scene_fabricator_shared_ptr& scene_fabricator,
                                                                       const gb::anim::anim_fabricator_shared_ptr& anim_fabricator,
                                                                       const std::array<gb::game_object_2d_weak_ptr, level::e_level_layer_max>& layers) :
    m_scene_graph(scene_graph),
    m_scene_fabricator(scene_fabricator),
    m_anim_fabricator(anim_fabricator),
    m_layers(layers),
    m_spawn_point(0.f),
    m_footprint_previous_timestamp(std::chrono::steady_clock::now()),
    m_statistic_callback(nullptr),
    m_on_tap_on_character_callback(nullptr),
    m_dead_cooldown_timeinterval(10000.f),
    m_dead_timestamp(std::chrono::steady_clock::now())
    {
        ces_entity::add_deferred_component_constructor<gb::ces_action_component>();
        ces_entity::add_deferred_component_constructor<ces_character_statistic_component>();
        ces_entity::add_deferred_component_constructor<ces_character_controller_component>();
    }
    
    client_base_character_controller::~client_base_character_controller()
    {
        
    }
    
    void client_base_character_controller::setup_components()
    {
        auto action_component = ces_entity::get_component<gb::ces_action_component>();
        action_component->set_update_callback(std::bind(&client_base_character_controller::update, this,
                                                        std::placeholders::_1, std::placeholders::_2));
    }
    
    void client_base_character_controller::setup_controllers()
    {
        information_bubble_controller_shared_ptr information_bubble_controller = gb::ces_entity::construct<game::information_bubble_controller>(m_layers[level::e_level_layer_characters_top_statistic].lock(),
                                                                                                                                                m_scene_graph.lock(),
                                                                                                                                                m_scene_fabricator.lock());
        m_information_bubble_controller = information_bubble_controller;
        client_base_character_controller::add_child(information_bubble_controller);
        
        bloodprint_controller_shared_ptr bloodprint_controller = gb::ces_entity::construct<game::bloodprint_controller>(m_layers[level::e_level_layer_bloodprints].lock(),
                                                                                                                        m_scene_graph.lock(),
                                                                                                                        m_scene_fabricator.lock());
        m_bloodprint_controller = bloodprint_controller;
        client_base_character_controller::add_child(bloodprint_controller);
        
        footprint_controller_shared_ptr footprint_controller = gb::ces_entity::construct<game::footprint_controller>(m_layers[level::e_level_layer_footprints].lock(),
                                                                                                                     m_scene_graph.lock(),
                                                                                                                     m_scene_fabricator.lock());
        m_footprint_controller = footprint_controller;
        client_base_character_controller::add_child(footprint_controller);
        
        auto character_statistic = m_scene_fabricator.lock()->create_sprite(k_character_statistic_entity_filename);
        character_statistic->tag = k_character_statistic_part;
        character_statistic->size = glm::vec2(96.f);
        character_statistic->pivot = glm::vec2(.5f, .5f);
        character_statistic->color = glm::u8vec4(0, 255, 0, 255);
        m_character_statistic = character_statistic;
        m_layers[level::e_level_layer_characters_down_statistic].lock()->add_child(character_statistic);
        
        auto character_controller_component = client_base_character_controller::get_component<game::ces_character_controller_component>();
        character_controller_component->information_bubble_controller = information_bubble_controller;
        character_controller_component->bloodprint_controller = bloodprint_controller;
        character_controller_component->footprint_controller = footprint_controller;
        character_controller_component->character_statistic = character_statistic;
        character_controller_component->set_dead_callback(std::bind(&client_base_character_controller::on_died, this, std::placeholders::_1));
        character_controller_component->set_kill_callback(std::bind(&client_base_character_controller::on_killed, this, std::placeholders::_1, std::placeholders::_2));
        
        auto character_statistic_component = client_base_character_controller::get_component<ces_character_statistic_component>();
        character_statistic_component->set_on_health_changed_callback(std::bind(&client_base_character_controller::on_health_changed,
                                                                                this,
                                                                                std::placeholders::_1,
                                                                                std::placeholders::_2));
    }
    
    void client_base_character_controller::setup(const std::pair<gb::sprite_shared_ptr, gb::shape_3d_shared_ptr>& character_linkage)
    {
        m_character = gb::ces_entity::construct<character>();
        std::static_pointer_cast<character>(m_character)->setup(character_linkage,
                                                                m_scene_graph.lock(),
                                                                m_scene_fabricator.lock(),
                                                                true, glm::vec4(1.f, 1.f, 1.f, 1.f));
        client_base_character_controller::add_child(m_character);
        client_base_character_controller::setup_controllers();
        
        std::static_pointer_cast<character>(m_character)->set_on_death_effect_ended_callback(std::bind(&client_base_character_controller::on_death_effect_ended, this));
    }
    
    void client_base_character_controller::on_changed_server_transformation(const glm::vec2& velocity,
                                                                            const glm::vec2& position,
                                                                            f32 rotation)
    {
        m_server_velocity = velocity;
        m_server_position = position;
        m_server_rotation = rotation;
    }
    
    void client_base_character_controller::update(const gb::ces_entity_shared_ptr& entity, f32 dt)
    {

    }
    
    void client_base_character_controller::set_spawn_point(const glm::vec2& spawn_point)
    {
        m_spawn_point = spawn_point;
    }
    
    void client_base_character_controller::on_move()
    {
        std::static_pointer_cast<character>(m_character)->play_animation(character::animations::k_walk_animation, true);
    }
    
    void client_base_character_controller::on_idle()
    {
        std::static_pointer_cast<character>(m_character)->play_animation(character::animations::k_idle_animation, true);
    }
    
    void client_base_character_controller::on_health_changed(const gb::ces_entity_shared_ptr& attacker, f32 health)
    {
        glm::vec2 current_position = client_base_character_controller::position;
        f32 current_rotation = client_base_character_controller::rotation;
        
        if(!m_information_bubble_controller.expired())
        {
            auto information_bubble_controller = m_information_bubble_controller.lock();
            std::stringstream string_stream;
            string_stream<<-health;
            information_bubble_controller->push_bubble(string_stream.str(), glm::u8vec4(255, 0, 0, 255), current_position, current_rotation, 1);
        }
        
        if(!m_bloodprint_controller.expired())
        {
            auto attacker_transformation_component = attacker->get_component<gb::ces_transformation_2d_component>();
            auto current_transformation_component = client_base_character_controller::get_component<gb::ces_transformation_2d_component>();
            
            glm::vec2 executor_position = attacker_transformation_component->get_position();
            glm::vec2 target_position = current_transformation_component->get_position();
            glm::vec2 direction = glm::normalize(target_position - executor_position);

            auto bloodprint_controller = m_bloodprint_controller.lock();
            bloodprint_controller->push_bloodprint(glm::u8vec4(255, 0, 0, 255),
                                                   current_position + direction * std::get_random_f(16.f, 24.f),
                                                   current_rotation);
        }
    }
    
    void client_base_character_controller::on_health_updated()
    {
        gb::sprite_shared_ptr character_statistic = std::static_pointer_cast<gb::sprite>(m_character_statistic);
        auto character_statistic_component = client_base_character_controller::get_component<ces_character_statistic_component>();
        f32 current_health_percents = character_statistic_component->current_health_percents;
        character_statistic->color = glm::mix(glm::u8vec4(255, 0, 0, 255), glm::u8vec4(0, 255, 0, 255), current_health_percents);
    }
    
    void client_base_character_controller::on_died(const gb::ces_entity_shared_ptr& owner)
    {
        m_dead_timestamp = std::chrono::steady_clock::now();
        gb::ces_box2d_body_component_shared_ptr box2d_body_component =
        client_base_character_controller::get_component<gb::ces_box2d_body_component>();
        box2d_body_component->velocity = glm::vec2(0.f);
        box2d_body_component->enabled = false;
        std::static_pointer_cast<character>(m_character)->play_animation(character::animations::k_die_animation, false);
        
        gb::sprite_shared_ptr character_statistic = std::static_pointer_cast<gb::sprite>(m_character_statistic);
        character_statistic->color = glm::u8vec4(0, 0, 0, 0);
        
        client_base_character_controller::bring_to_back();
    }
    
    void client_base_character_controller::on_killed(const gb::ces_entity_shared_ptr& owner, const gb::ces_entity_shared_ptr& target)
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
        
        auto character_statistic_component = client_base_character_controller::get_component<ces_character_statistic_component>();
        f32 max_health = character_statistic_component->max_health;
        character_statistic_component->current_health = max_health;
        
        gb::ces_box2d_body_component_shared_ptr box2d_body_component =
        client_base_character_controller::get_component<gb::ces_box2d_body_component>();
        box2d_body_component->enabled = true;
        
        gb::thread_operation_shared_ptr operation = std::make_shared<gb::thread_operation>(gb::thread_operation::e_thread_operation_queue_main);
        operation->set_execution_callback([=]() {
            gb::sprite_shared_ptr character_statistic = std::static_pointer_cast<gb::sprite>(m_character_statistic);
            character_statistic->color = glm::u8vec4(255, 0, 0, 255);
            
            client_base_character_controller::bring_to_front();
            std::static_pointer_cast<character>(m_character)->remove_death_effect();
        });
        operation->add_to_execution_queue();
    }
    
    void client_base_character_controller::set_statistic_callback(const statistic_callback_t& callback)
    {
        m_statistic_callback = callback;
    }
    
    void client_base_character_controller::set_on_tap_on_character_callback(const on_tap_on_character_callback_t& callback)
    {
        m_on_tap_on_character_callback = callback;
        std::static_pointer_cast<character>(m_character)->set_on_tap_on_character_callback([=](const gb::ces_entity_shared_ptr&) {
            auto character_statistic_component = client_base_character_controller::get_component<ces_character_statistic_component>();
            if(!character_statistic_component->is_dead)
            {
                if(m_on_tap_on_character_callback)
                {
                    m_on_tap_on_character_callback(shared_from_this());
                }
            }
        });
    }
    
    void client_base_character_controller::on_animation_ended(const std::string& animation_name, bool is_looped)
    {
        if(animation_name == character::animations::k_attack_animation)
        {
            client_base_character_controller::create_hit_bounds();
        }
        if(animation_name == character::animations::k_die_animation)
        {
            std::static_pointer_cast<character>(m_character)->apply_death_effect();
        }
    }
    
    void client_base_character_controller::create_hit_bounds()
    {
        auto hit_bounds = gb::ces_entity::construct<game::hit_bounds>();
        hit_bounds->setup("character.hit_bounds.xml",
                          m_scene_graph.lock(),
                          m_scene_fabricator.lock(),
                          m_anim_fabricator.lock(),
                          shared_from_this());
        m_layers[level::e_level_layer_hit_bounds].lock()->add_child(hit_bounds);
        
        f32 current_rotation = client_base_character_controller::rotation;
        current_rotation += 180.f;
        glm::vec2 current_position = client_base_character_controller::position;
        current_position += glm::vec2(-sinf(glm::radians(current_rotation + 10.f)) * 64.f,
                                      cosf(glm::radians(current_rotation + 10.f)) * 64.f);
        
        m_scene_graph.lock()->apply_box2d_physics(hit_bounds, b2BodyType::b2_dynamicBody, [](gb::ces_box2d_body_component_const_shared_ptr component) {
            component->shape = gb::ces_box2d_body_component::circle;
            component->set_radius(8.f);
        });
        
        gb::ces_box2d_body_component_shared_ptr box2d_body_component =
        hit_bounds->get_component<gb::ces_box2d_body_component>();
        box2d_body_component->is_destructable_on_contact = true;
        
        b2Body *body = box2d_body_component->box2d_body;
        body->SetBullet(true);
        
        glm::vec2 velocity = glm::vec2(-sinf(glm::radians(current_rotation)) * std::numeric_limits<i16>::max(),
                                       cosf(glm::radians(current_rotation)) * std::numeric_limits<i16>::max());
        hit_bounds->position = current_position;
        hit_bounds->rotation = current_rotation;
        box2d_body_component->velocity = velocity;
    }
    
    void client_base_character_controller::on_death_effect_ended()
    {
        client_base_character_controller::position = glm::vec2(std::numeric_limits<i16>::min());
    }
}

