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
#include "ces_action_component.h"
#include "game_object.h"
#include "character.h"
#include "bullet.h"
#include "animated_sprite.h"
#include "scene_graph.h"
#include "glm_extensions.h"

namespace game
{
    server_character_controller::server_character_controller(ui32 udid,
                                                             const gb::scene_graph_shared_ptr& scene_graph,
                                                             const gb::scene_fabricator_shared_ptr& scene_fabricator,
                                                             const gb::anim::anim_fabricator_shared_ptr& anim_fabricator,
                                                             const std::array<gb::game_object_weak_ptr, level::e_level_layer_max>& layers) :
    m_udid(udid),
    m_scene_graph(scene_graph),
    m_scene_fabricator(scene_fabricator),
    m_anim_fabricator(anim_fabricator),
    m_layers(layers),
    m_server_character_move_callback(nullptr),
    m_spawn_point(0.f),
    m_dead_cooldown_timeinterval(10000.f),
    m_dead_timestamp(std::chrono::steady_clock::now())
    {
        std::shared_ptr<gb::ces_action_component> action_component = std::make_shared<gb::ces_action_component>();
        action_component->set_update_callback(std::bind(&server_character_controller::update, this,
                                                        std::placeholders::_1, std::placeholders::_2));
        server_character_controller::add_component(action_component);
        
        std::shared_ptr<ces_character_controller_component> character_controller_component = std::make_shared<game::ces_character_controller_component>();
        server_character_controller::add_component(character_controller_component);
    }
    
    server_character_controller::~server_character_controller()
    {
        
    }
    
    void server_character_controller::setup(const std::string& filename)
    {
        m_character = std::make_shared<character>();
        std::static_pointer_cast<character>(m_character)->setup(filename,
                                                                m_scene_graph.lock(),
                                                                m_scene_fabricator.lock(),
                                                                m_anim_fabricator.lock(),
                                                                true, glm::vec4(0.f, 1.f, 0.f, 1.f));
        server_character_controller::add_child(m_character);
        
        auto character_controller_component = server_character_controller::get_component<game::ces_character_controller_component>();
        character_controller_component->set_dead_callback(std::bind(&server_character_controller::on_dead, this, std::placeholders::_1));
        character_controller_component->set_health_changed_callback(std::bind(&server_character_controller::on_health_changed, this, std::placeholders::_1, std::placeholders::_2));
        character_controller_component->set_kill_callback(std::bind(&server_character_controller::on_kill, this, std::placeholders::_1, std::placeholders::_2));
    }

#define k_move_speed -16.f
#define k_move_speed_mult 100.f
#define k_shoot_speed 10000.f
    
    void server_character_controller::update(const gb::ces_entity_shared_ptr& entity, f32 dt)
    {
        gb::ces_box2d_body_component_shared_ptr box2d_body_component =
        server_character_controller::get_component<gb::ces_box2d_body_component>();
        
        glm::vec2 current_position = server_character_controller::position;
        f32 current_rotation = server_character_controller::rotation;
        glm::vec2 velocity = glm::vec2(.0f);
        
        ui64 last_move_revision = std::numeric_limits<ui64>::max();
        
        while(!m_client_character_move_history.empty())
        {
            client_character_move_timeprint move_timeprint = m_client_character_move_history.front();
            m_client_character_move_history.pop();
            
            last_move_revision = move_timeprint.m_move_revision;
            current_rotation = move_timeprint.m_move_angle;
            f32 dt = move_timeprint.m_dt;
            
            velocity += glm::vec2(-sinf(glm::radians(current_rotation)) * k_move_speed * dt * k_move_speed_mult,
                                  cosf(glm::radians(current_rotation)) * k_move_speed * dt * k_move_speed_mult);
            
        }
        box2d_body_component->velocity = velocity;
        
        server_character_controller::position = current_position;
        server_character_controller::rotation = current_rotation;
        
        if(m_server_character_move_callback && last_move_revision != std::numeric_limits<ui64>::max())
        {
            m_server_character_move_callback(last_move_revision, m_udid, velocity, current_position, current_rotation);
        }
        
        while(!m_client_character_shoot_history.empty())
        {
            client_character_shoot_timeprint shoot_timeprint = m_client_character_shoot_history.front();
            m_client_character_shoot_history.pop();
            
            ui64 shoot_revision = shoot_timeprint.m_shoot_revision;
            f32 shoot_angle = shoot_timeprint.m_shoot_angle;
            shoot_angle += 180.f;
            
            glm::vec2 shoot_position = server_character_controller::position;
            shoot_position += glm::vec2(-sinf(glm::radians(shoot_angle + 10.f)) * 64.f,
                                        cosf(glm::radians(shoot_angle + 10.f)) * 64.f);
            
            glm::vec2 shoot_velocity = glm::vec2(-sinf(glm::radians(shoot_angle)) * k_shoot_speed,
                                                 cosf(glm::radians(shoot_angle)) * k_shoot_speed);
            server_character_controller::on_shoot(shoot_position, shoot_angle, shoot_velocity);
            if(m_server_character_shoot_callback)
            {
                m_server_character_shoot_callback(shoot_revision, m_udid, shoot_velocity, current_position, shoot_angle);
            }
        }
    }
    
    void server_character_controller::on_client_character_move(ui64 move_revision, f32 move_angle, f32 dt)
    {
        client_character_move_timeprint move_timeprint;
        move_timeprint.m_move_revision = move_revision;
        move_timeprint.m_move_angle = move_angle;
        move_timeprint.m_dt = dt;
        m_client_character_move_history.push(move_timeprint);
    }

    void server_character_controller::server_character_controller::set_server_character_move_callback(const on_server_character_move_callback_t& callback)
    {
        m_server_character_move_callback = callback;
    }
    
    void server_character_controller::on_client_character_shoot(ui64 shoot_revision, f32 shoot_angle)
    {
        client_character_shoot_timeprint shoot_timeprint;
        shoot_timeprint.m_shoot_revision = shoot_revision;
        shoot_timeprint.m_shoot_angle = shoot_angle;
        m_client_character_shoot_history.push(shoot_timeprint);
    }
    
    void server_character_controller::set_server_character_shoot_callback(const on_server_character_shoot_callback_t& callback)
    {
        m_server_character_shoot_callback = callback;
    }
    
    void server_character_controller::set_spawn_point(const glm::vec2& spawn_point)
    {
        m_spawn_point = spawn_point;
    }
    
    void server_character_controller::on_health_changed(const gb::ces_entity_shared_ptr& entity, f32 health)
    {
        
    }
    
    void server_character_controller::on_dead(const gb::ces_entity_shared_ptr& entity)
    {
        m_dead_timestamp = std::chrono::steady_clock::now();
        gb::ces_box2d_body_component_shared_ptr box2d_body_component =
        server_character_controller::get_component<gb::ces_box2d_body_component>();
        box2d_body_component->velocity = glm::vec2(0.f);
        box2d_body_component->enabled = false;
    }
    
    void server_character_controller::on_kill(const gb::ces_entity_shared_ptr& owner, const gb::ces_entity_shared_ptr& target)
    {

    }
    
    void server_character_controller::on_revive()
    {
        server_character_controller::position = m_spawn_point;
        auto character_controller_component = server_character_controller::get_component<game::ces_character_controller_component>();
        character_controller_component->reset_health();
        gb::ces_box2d_body_component_shared_ptr box2d_body_component =
        server_character_controller::get_component<gb::ces_box2d_body_component>();
        box2d_body_component->enabled = true;
    }
    
    void server_character_controller::on_shoot(const glm::vec2& position, f32 rotation, const glm::vec2& velocity)
    {
        bullet_shared_ptr bullet = std::make_shared<game::bullet>();
        bullet->setup("ns_bullet_01.xml",
                      m_scene_graph.lock(),
                      m_scene_fabricator.lock(),
                      m_anim_fabricator.lock(),
                      shared_from_this());
        m_layers[level::e_level_layer_bullets].lock()->add_child(bullet);
        
        m_scene_graph.lock()->apply_box2d_physics(bullet, b2BodyType::b2_dynamicBody, [](gb::ces_box2d_body_component_const_shared_ptr component) {
            component->shape = gb::ces_box2d_body_component::circle;
            component->set_radius(8.f);
        });
        
        gb::ces_box2d_body_component_shared_ptr box2d_body_component =
        bullet->get_component<gb::ces_box2d_body_component>();
        box2d_body_component->is_destuctable_on_contact = true;
        
        bullet->position = position;
        bullet->rotation = rotation;
        box2d_body_component->velocity = velocity;
    }

}
