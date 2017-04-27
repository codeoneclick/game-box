//
//  client_main_character_controller.cpp
//  gbDemo
//
//  Created by serhii serhiiv on 10/31/16.
//  Copyright © 2016 sergey.sergeev. All rights reserved.
//

#include "client_main_character_controller.h"
#include "fullscreen_joystick.h"
#include "joystick.h"
#include "button.h"
#include "ces_character_controller_component.h"
#include "ces_box2d_body_component.h"
#include "ces_transformation_component.h"
#include "game_object.h"
#include "animated_sprite.h"
#include "camera.h"
#include "bullet.h"
#include "scene_graph.h"
#include "glm_extensions.h"
#include "ces_transformation_extension.h"
#include "footprint_controller.h"
#include "information_bubble_controller.h"

#define k_footprint_timeinterval 333.f

namespace game
{
    client_main_character_controller::client_main_character_controller(bool is_net_session,
																	   const gb::camera_shared_ptr& camera,
                                                                       const gb::scene_graph_shared_ptr& scene_graph,
                                                                       const gb::scene_fabricator_shared_ptr& scene_fabricator,
                                                                       const gb::anim::anim_fabricator_shared_ptr& anim_fabricator,
                                                                       const std::array<gb::game_object_weak_ptr, level::e_level_layer_max>& layers) :
    game::client_base_character_controller(scene_graph, scene_fabricator, anim_fabricator, layers),
	m_is_net_session(is_net_session),
    m_camera(camera),
    m_character_move_callback(nullptr),
    m_character_shoot_callback(nullptr),
    m_server_adjust_position(glm::vec2(0.f)),
    m_server_adjust_rotation(0.f),
    m_move_revision(0),
    m_shoot_revision(0),
    m_move_joystick(nullptr),
    m_shoot_joystick(nullptr),
    m_move_joystick_delta(0.f),
    m_shoot_joystick_delta(0.f),
    m_move_joystick_angle(0.f),
    m_shoot_joystick_angle(0.f),
    m_move_joystick_dragging(false),
    m_shoot_joystick_dragging(false),
    m_map_size(0.f),
    m_dead_cooldown_callback(nullptr),
    m_is_move_interacted(false),
    m_is_shoot_interacted(false)
    {
        m_server_adjust_move_revision = 0;
        auto character_controller_component = client_main_character_controller::get_component<ces_character_controller_component>();
        character_controller_component->mode = ces_character_controller_component::e_mode::main;
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
    }
    
    void client_main_character_controller::set_character_move_callback(const on_character_move_callback_t& callback)
    {
        m_character_move_callback = callback;
    }
    
    void client_main_character_controller::set_character_shoot_callback(const on_character_shoot_callback_t& callback)
    {
        m_character_shoot_callback = callback;
    }
    
    void client_main_character_controller::on_joystick_dragging(const gb::ces_entity_shared_ptr& joystick,
                                                                const glm::vec2& delta, f32 angle)
    {
        if(joystick == m_move_joystick)
        {
            m_move_joystick_angle = glm::wrap_degrees(glm::degrees(angle));
            m_move_joystick_delta = delta;
            m_move_joystick_dragging = true;
        }
        else if(joystick == m_shoot_joystick)
        {
            m_shoot_joystick_angle = glm::wrap_degrees(glm::degrees(angle));
            m_shoot_joystick_delta = delta;
            m_shoot_joystick_dragging = true;
        }
        else
        {
            assert(false);
        }
    }
    
    void client_main_character_controller::on_joystick_end_dragging(const gb::ces_entity_shared_ptr& joystick)
    {
        if(joystick == m_move_joystick)
        {
            m_move_joystick_delta = glm::vec2(0.f);
            m_move_joystick_dragging = false;
            m_is_move_interacted = true;
        }
        else if(joystick == m_shoot_joystick)
        {
            m_shoot_joystick_delta = glm::vec2(0.f);
            m_shoot_joystick_dragging = false;
        }
        else
        {
            assert(false);
        }
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
    
#define k_move_speed -16.f
#define k_move_speed_mult 100.f
#define k_move_synchronization_trashhold 32.f
    
    void client_main_character_controller::update(const gb::ces_entity_shared_ptr& entity, f32 dt)
    {
        auto character_controller_component = client_main_character_controller::get_component<ces_character_controller_component>();
        if(!character_controller_component->is_dead)
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
            
            if(m_move_joystick_dragging)
            {
                current_rotation = m_move_joystick_angle;
                
                if(!is_synchronized && m_is_net_session)
                {
                    current_rotation = glm::mix_angles_degrees(current_rotation, m_server_adjust_rotation, .5f);
                    current_position = glm::mix(current_position, m_server_adjust_position, .5f);
                    m_move_joystick_dragging = current_rotation;
                }
                
                glm::vec2 velocity = glm::vec2(-sinf(glm::radians(current_rotation)) * k_move_speed * dt * k_move_speed_mult,
                                               cosf(glm::radians(current_rotation)) * k_move_speed * dt * k_move_speed_mult);
                
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
                m_is_move_interacted = true;
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
            
            if(m_shoot_joystick_dragging)
            {
                current_rotation = m_shoot_joystick_angle;
                static std::chrono::steady_clock::time_point previous_timestamp = std::chrono::steady_clock::now();
                std::chrono::steady_clock::time_point current_timestamp = std::chrono::steady_clock::now();
                f32 deltatime = std::chrono::duration_cast<std::chrono::milliseconds>(current_timestamp - previous_timestamp).count();
                if(deltatime > 100.f)
                {
                    previous_timestamp = current_timestamp;
                    client_main_character_controller::on_shoot();
                    if(m_character_shoot_callback && m_is_net_session)
                    {
                        m_character_shoot_callback(m_shoot_revision, m_shoot_joystick_angle);
                    }
                }
            }
            
            client_base_character_controller::position = current_position;
            client_base_character_controller::rotation = current_rotation;
            
            camera_position = glm::mix(camera_position, current_position * -1.f, .5f);
            glm::ivec2 screen_size = m_camera->screen_size;
            glm::vec2 camera_pivot = m_camera->pivot;
            camera_position = glm::clamp(camera_position,
                                         glm::vec2(-m_map_size.x + static_cast<f32>(screen_size.x) * camera_pivot.x,
                                                   -m_map_size.y + static_cast<f32>(screen_size.y) * camera_pivot.y),
                                         glm::vec2(static_cast<f32>(screen_size.x) * -camera_pivot.x,
                                                   static_cast<f32>(screen_size.y) * -camera_pivot.y));
            m_camera->set_position(camera_position);
            
            if(m_character_move_callback && m_is_net_session && m_is_move_interacted)
            {
                m_character_move_callback(m_move_revision, m_move_joystick_angle, dt);
                client_character_move_history_point history_point;
                history_point.m_move_revision = m_move_revision++;
                history_point.m_position = current_position;
                history_point.m_rotation = current_rotation;
                m_client_character_move_history.push_back(history_point);
                m_is_move_interacted = false;
            }
        }
        else
        {
            glm::vec2 current_position = client_base_character_controller::position;
            glm::vec2 camera_position = m_camera->get_position();
            
            camera_position = glm::mix(camera_position, current_position * -1.f, .5f);
            glm::ivec2 screen_size = m_camera->screen_size;
            glm::vec2 camera_pivot = m_camera->pivot;
            camera_position = glm::clamp(camera_position,
                                         glm::vec2(-m_map_size.x + static_cast<f32>(screen_size.x) * camera_pivot.x,
                                                   -m_map_size.y + static_cast<f32>(screen_size.y) * camera_pivot.y),
                                         glm::vec2(static_cast<f32>(screen_size.x) * -camera_pivot.x,
                                                   static_cast<f32>(screen_size.y) * -camera_pivot.y));
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
    
    void client_main_character_controller::set_move_joystick(const gb::ui::joystick_shared_ptr& joystick)
    {
        m_move_joystick = joystick;
        m_move_joystick->set_on_dragging_callback(std::bind(&client_main_character_controller::on_joystick_dragging, this,
                                                            std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
        m_move_joystick->set_on_end_dragging_callback(std::bind(&client_main_character_controller::on_joystick_end_dragging, this,
                                                                std::placeholders::_1));
    }
    
    void client_main_character_controller::set_shoot_joystick(const gb::ui::joystick_shared_ptr& joystick)
    {
        m_shoot_joystick = joystick;
        m_shoot_joystick->set_on_dragging_callback(std::bind(&client_main_character_controller::on_joystick_dragging, this,
                                                             std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
        m_shoot_joystick->set_on_end_dragging_callback(std::bind(&client_main_character_controller::on_joystick_end_dragging, this,
                                                                 std::placeholders::_1));
    }
    
    void client_main_character_controller::set_dead_cooldown_callback(const on_dead_cooldown_callback_t& callback)
    {
        m_dead_cooldown_callback = callback;
    }
}

