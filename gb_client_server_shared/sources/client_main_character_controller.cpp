//
//  client_main_character_controller.cpp
//  gbDemo
//
//  Created by serhii serhiiv on 10/31/16.
//  Copyright © 2016 sergey.sergeev. All rights reserved.
//

#include "client_main_character_controller.h"
#include "button.h"
#include "ces_character_controller_component.h"
#include "ces_character_statistic_component.h"
#include "ces_box2d_body_component.h"
#include "ces_transformation_2d_component.h"
#include "game_object_2d.h"
#include "animated_sprite.h"
#include "camera_2d.h"
#include "bullet.h"
#include "character.h"
#include "scene_graph.h"
#include "glm_extensions.h"
#include "ces_transformation_extension.h"
#include "footprint_controller.h"
#include "information_bubble_controller.h"
#include "pathfinder.h"
#include "path_map.h"

#define k_footprint_timeinterval 333.f

namespace game
{
    client_main_character_controller::client_main_character_controller(bool is_net_session,
																	   const gb::camera_2d_shared_ptr& camera,
                                                                       const gb::scene_graph_shared_ptr& scene_graph,
                                                                       const gb::scene_fabricator_shared_ptr& scene_fabricator,
                                                                       const gb::anim::anim_fabricator_shared_ptr& anim_fabricator,
                                                                       const std::array<gb::game_object_2d_weak_ptr, level::e_level_layer_max>& layers) :
    game::client_base_character_controller(scene_graph, scene_fabricator, anim_fabricator, layers),
	m_is_net_session(is_net_session),
    m_camera(camera),
    m_character_move_callback(nullptr),
    m_character_shoot_callback(nullptr),
    m_server_adjust_position(glm::vec2(0.f)),
    m_server_adjust_rotation(0.f),
    m_move_revision(0),
    m_shoot_revision(0),
    m_map_size(0.f),
    m_dead_cooldown_callback(nullptr),
    m_is_move_interacted(false),
    m_is_shoot_interacted(false),
    m_path_map(nullptr),
    m_pathfinder(std::make_shared<pathfinder>())
    {
        m_server_adjust_move_revision = 0;
        auto character_controller_component = client_main_character_controller::get_component<ces_character_controller_component>();
        character_controller_component->mode = ces_character_controller_component::e_mode::main;
        
        auto character_statistic_component = client_main_character_controller::get_component<ces_character_statistic_component>();
        character_statistic_component->setup(100.f, 1000.f, 2000.f, 10.f);
    }
    
    client_main_character_controller::~client_main_character_controller()
    {
        
    }
    
#define k_shoot_speed 10000.f
    
    void client_main_character_controller::on_shoot()
    {
        bullet_shared_ptr bullet = std::make_shared<game::bullet>();
        bullet->setup("ns_bullet_01.xml",
                      m_scene_graph.lock(),
                      m_scene_fabricator.lock(),
                      m_anim_fabricator.lock(),
                      shared_from_this());
        bullet->attach_sound("sound_01.mp3", bullet::k_create_state);
        bullet->on_create();
        m_layers[level::e_level_layer_bullets].lock()->add_child(bullet);
        
        f32 current_rotation = client_base_character_controller::rotation;
        current_rotation += 180.f;
        glm::vec2 current_position = client_base_character_controller::position;
        current_position += glm::vec2(-sinf(glm::radians(current_rotation + 10.f)) * 64.f,
                                      cosf(glm::radians(current_rotation + 10.f)) * 64.f);
        
        
        m_scene_graph.lock()->apply_box2d_physics(bullet, b2BodyType::b2_dynamicBody, [](gb::ces_box2d_body_component_const_shared_ptr component) {
            component->shape = gb::ces_box2d_body_component::circle;
            component->set_radius(8.f);
        });
        
        gb::ces_box2d_body_component_shared_ptr box2d_body_component =
        bullet->get_component<gb::ces_box2d_body_component>();
        box2d_body_component->is_destuctable_on_contact = true;
        
        glm::vec2 velocity = glm::vec2(-sinf(glm::radians(current_rotation)) * k_shoot_speed,
                                       cosf(glm::radians(current_rotation)) * k_shoot_speed);
        bullet->position = current_position;
        bullet->rotation = current_rotation;
        box2d_body_component->velocity = velocity;
        
        std::static_pointer_cast<character>(m_character)->play_animation(character::animations::k_attack_animation);
    }
    
    void client_main_character_controller::set_character_move_callback(const on_character_move_callback_t& callback)
    {
        m_character_move_callback = callback;
    }
    
    void client_main_character_controller::set_character_shoot_callback(const on_character_shoot_callback_t& callback)
    {
        m_character_shoot_callback = callback;
    }
    
    void client_main_character_controller::synchronize_transformations(ui64 move_revision,
                                                                       const glm::vec2& position,
                                                                       const f32 rotation)
    {
        m_server_adjust_move_revision = move_revision;
        m_server_adjust_position = position;
        m_server_adjust_rotation = rotation;
    }
    
    void client_main_character_controller::set_map_size(const glm::vec2& map_size)
    {
        m_map_size = map_size;
    }
    
#define k_move_synchronization_trashhold 32.f
#define k_trashhold_distance 16.f
    
    void client_main_character_controller::update(const gb::ces_entity_shared_ptr& entity, f32 dt)
    {
        auto character_statistic_component = client_main_character_controller::get_component<ces_character_statistic_component>();
        if(!character_statistic_component->is_dead)
        {
            gb::ces_box2d_body_component_shared_ptr box2d_body_component =
            client_base_character_controller::get_component<gb::ces_box2d_body_component>();
            bool is_synchronized = true;
            
            if (m_is_net_session)
            {
                is_synchronized = client_main_character_controller::validate_move_synchronization(m_server_adjust_move_revision,
                                                                                                  m_server_adjust_position,
                                                                                                  m_server_adjust_rotation);
            }
            
            glm::vec2 current_position = client_base_character_controller::position;
            f32 current_rotation = client_base_character_controller::rotation;
            glm::vec2 camera_position = m_camera->get_position();
            
            if(!m_move_path.empty())
            {
                glm::vec2 goal_position = m_move_path.front();
                
                f32 distance = glm::distance(current_position, goal_position);
                if(distance <= k_trashhold_distance)
                {
                    m_move_path.pop();
                }
                else
                {
                    glm::vec2 direction = glm::normalize(goal_position - current_position);
                    f32 goal_rotation = atan2f(direction.x, -direction.y);
                    goal_rotation = glm::wrap_degrees(glm::degrees(goal_rotation));
                    f32 move_speed = character_statistic_component->current_move_speed;
                    f32 current_move_speed = -move_speed * dt;
                    
                    glm::vec2 velocity = glm::vec2(-sinf(glm::radians(goal_rotation)) * current_move_speed,
                                                   cosf(glm::radians(goal_rotation)) * current_move_speed);
                    
                    current_rotation = goal_rotation;
                    box2d_body_component->velocity = velocity;
                    
                    client_base_character_controller::on_move();
                    
                    std::chrono::steady_clock::time_point current_timestamp = std::chrono::steady_clock::now();
                    f32 deltatime = std::chrono::duration_cast<std::chrono::milliseconds>(current_timestamp - m_footprint_previous_timestamp).count();
                    if(deltatime > k_footprint_timeinterval)
                    {
                        m_footprint_previous_timestamp = current_timestamp;
                        m_footprint_controller.lock()->push_footprint(glm::u8vec4(255, 255, 255, 255),
                                                                      current_position, current_rotation);
                    }
                }
            }
            else
            {
                if(!is_synchronized && m_is_net_session)
                {
                    current_rotation = glm::mix_angles_degrees(current_rotation, m_server_adjust_rotation, .5f);
                    current_position = glm::mix(current_position, m_server_adjust_position, .5f);
                }
                
                box2d_body_component->velocity = glm::vec2(0.f);
                client_base_character_controller::on_idle();
            }
            
            client_base_character_controller::position = current_position;
            client_base_character_controller::rotation = current_rotation;
            m_character_statistic->position = current_position;
            
            camera_position = glm::mix(camera_position, current_position * -1.f, .5f);
            glm::ivec2 viewport_size = m_camera->viewport_size;
            glm::vec2 camera_pivot = m_camera->pivot;
            camera_position = glm::clamp(camera_position,
                                         glm::vec2(-m_map_size.x + static_cast<f32>(viewport_size.x) * camera_pivot.x,
                                                   -m_map_size.y + static_cast<f32>(viewport_size.y) * camera_pivot.y),
                                         glm::vec2(static_cast<f32>(viewport_size.x) * -camera_pivot.x,
                                                   static_cast<f32>(viewport_size.y) * -camera_pivot.y));
            m_camera->set_position(camera_position);
            
            if(m_character_move_callback && m_is_net_session && m_is_move_interacted)
            {
                //m_character_move_callback(m_move_revision, m_move_joystick_angle, dt);
                client_character_move_history_point history_point;
                history_point.m_move_revision = m_move_revision++;
                history_point.m_position = current_position;
                history_point.m_rotation = current_rotation;
                m_client_character_move_history.push_back(history_point);
                m_is_move_interacted = false;
            }
            
            client_base_character_controller::on_health_updated();
        }
        else
        {
            glm::vec2 current_position = client_base_character_controller::position;
            glm::vec2 camera_position = m_camera->get_position();
            
            camera_position = glm::mix(camera_position, current_position * -1.f, .5f);
            glm::ivec2 viewport_size = m_camera->viewport_size;
            glm::vec2 camera_pivot = m_camera->pivot;
            camera_position = glm::clamp(camera_position,
                                         glm::vec2(-m_map_size.x + static_cast<f32>(viewport_size.x) * camera_pivot.x,
                                                   -m_map_size.y + static_cast<f32>(viewport_size.y) * camera_pivot.y),
                                         glm::vec2(static_cast<f32>(viewport_size.x) * -camera_pivot.x,
                                                   static_cast<f32>(viewport_size.y) * -camera_pivot.y));
            m_camera->set_position(camera_position);
            
            std::chrono::steady_clock::time_point current_timestamp = std::chrono::steady_clock::now();
            f32 deltatime = std::chrono::duration_cast<std::chrono::milliseconds>(current_timestamp - m_dead_timestamp).count();
            if(deltatime < m_dead_cooldown_timeinterval)
            {
                if(m_dead_cooldown_callback)
                {
                    m_dead_cooldown_callback(static_cast<i32>((m_dead_cooldown_timeinterval - deltatime) / 1000.f), static_cast<i32>(m_dead_cooldown_timeinterval - deltatime) % 1000);
                }
            }
            else
            {
                if(m_dead_cooldown_callback)
                {
                    m_dead_cooldown_callback(0, 0);
                }
                client_base_character_controller::on_revive();
            }
        }
    }
    
    bool client_main_character_controller::validate_move_synchronization(ui64 move_revision, const glm::vec2& position, f32 rotation)
    {
        bool is_synchronized = true;
        m_client_character_move_history.remove_if([=](const client_character_move_history_point& history_point) {
            if(history_point.m_move_revision < move_revision)
            {
                return true;
            }
            return false;
        });
        
        for(const auto& history_point : m_client_character_move_history)
        {
            if(history_point.m_move_revision == move_revision)
            {
                glm::vec2 history_position = history_point.m_position;
                f32 position_delta = glm::length(position - history_position);
                std::cout<<"position delta: "<<position_delta<<std::endl;
                if(position_delta > k_move_synchronization_trashhold)
                {
                    std::cout<<"unsynchronized"<<std::endl;
                    is_synchronized = false;
                    break;
                }
            }
        }
        return is_synchronized;
    }
    
    void client_main_character_controller::set_dead_cooldown_callback(const on_dead_cooldown_callback_t& callback)
    {
        m_dead_cooldown_callback = callback;
    }
    
    void client_main_character_controller::set_path_map(const path_map_shared_ptr& path_map)
    {
        m_path_map = path_map;
    }
    
    void client_main_character_controller::on_touch_level_at_position(const glm::vec2& end_position)
    {
        glm::vec2 start_position = client_base_character_controller::position;
        m_move_path = game::pathfinder::find_path(start_position, end_position,
                                                  m_pathfinder, m_path_map);
        client_main_character_controller::on_tap_on_character(nullptr);
    }
    
    void client_main_character_controller::on_tap_on_character(const gb::ces_entity_shared_ptr& entity)
    {
        m_selected_character_entity = entity;
        if(m_on_tap_on_character_callback)
        {
            m_on_tap_on_character_callback(entity);
        }
    }
}

