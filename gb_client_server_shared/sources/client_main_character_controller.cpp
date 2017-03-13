//
//  client_main_character_controller.cpp
//  gbDemo
//
//  Created by serhii serhiiv on 10/31/16.
//  Copyright © 2016 sergey.sergeev. All rights reserved.
//

#include "client_main_character_controller.h"
#include "fullscreen_joystick.h"
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

namespace game
{
    client_main_character_controller::client_main_character_controller(bool is_net_session,
																	   const gb::camera_shared_ptr& camera,
                                                                       const gb::game_object_shared_ptr& level,
                                                                       const gb::scene_graph_shared_ptr& scene_graph,
                                                                       const gb::scene_fabricator_shared_ptr& scene_fabricator,
                                                                       const gb::anim::anim_fabricator_shared_ptr& anim_fabricator) :
    game::client_base_character_controller(level, scene_graph, scene_fabricator, anim_fabricator),
	m_is_net_session(is_net_session),
    m_camera(camera),
	m_joystick(nullptr),
    m_joystick_delta(glm::vec2(0.f)),
    m_is_dragging(false),
    m_character_moving_callback(nullptr),
    m_server_adjust_position(glm::vec2(0.f)),
    m_server_adjust_rotation(0.f),
    m_client_tick(0)
    {
        m_received_client_tick = std::numeric_limits<ui64>::max();
        auto character_controller_component = client_main_character_controller::get_component<ces_character_controller_component>();
        character_controller_component->mode = ces_character_controller_component::e_mode::main;
    }
    
    client_main_character_controller::~client_main_character_controller()
    {
        
    }
    
    void client_main_character_controller::set_joystick(const gb::ui::fullscreen_joystick_shared_ptr& joystick)
    {
        m_joystick = joystick;
        m_joystick->set_on_dragging_callback(std::bind(&client_main_character_controller::on_joystick_dragging, this,
                                                       std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
        m_joystick->set_on_end_dragging_callback(std::bind(&client_main_character_controller::on_joystick_end_dragging, this,
                                                           std::placeholders::_1));
        
        m_joystick->set_on_double_tap_callback(std::bind(&client_main_character_controller::on_shoot_button_pressed, this,
                                                         std::placeholders::_1));
    }
    
    void client_main_character_controller::on_shoot_button_pressed(const gb::ces_entity_shared_ptr& entity)
    {
        if(!m_layer.expired())
        {
            bullet_shared_ptr bullet = std::make_shared<game::bullet>();
            bullet->setup("ns_bullet_01.xml",
                          m_scene_graph.lock(),
                          m_scene_fabricator.lock(),
                          m_anim_fabricator.lock());
            m_layer.lock()->add_child(bullet);
            
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
            f32 current_move_speed = 100000.f;
            
            glm::vec2 velocity = glm::vec2(-sinf(glm::radians(current_rotation)) * current_move_speed,
                                           cosf(glm::radians(current_rotation)) * current_move_speed);
            bullet->position = current_position;
            bullet->rotation = current_rotation;
            box2d_body_component->velocity = velocity;
        }
        else
        {
            assert(false);
        }
    }
    
    void client_main_character_controller::set_character_moving_callback(const on_character_moving_callback_t& callback)
    {
        m_character_moving_callback = callback;
    }
    
    void client_main_character_controller::on_joystick_dragging(const gb::ces_entity_shared_ptr& joystick,
                                                                const glm::vec2& delta, f32 angle)
    {
        angle = glm::wrap_degrees(glm::degrees(angle));
        m_joystick_delta = delta;
        m_joystick_delta.y = angle >= 90.f && angle <= 270.f ? 0.f : m_joystick_delta.y;
        //m_joystick_delta.x = angle >= 90.f && angle <= 270.f ? -m_joystick_delta.x : m_joystick_delta.x;
        m_is_dragging = true;
    }
    
    void client_main_character_controller::on_joystick_end_dragging(const gb::ces_entity_shared_ptr& joystick)
    {
        m_joystick_delta = glm::vec2(0.f);
        m_is_dragging = false;
    }
    
    void client_main_character_controller::synchronize_transformations(ui64 client_tick,
                                                                       const glm::vec2& position,
                                                                       const f32 rotation)
    {
        m_received_client_tick = client_tick;
        m_server_adjust_position = position;
        m_server_adjust_rotation = rotation;
    }
    
#define k_move_speed -1000.f
#define k_rotate_speed 100.f
    
    void client_main_character_controller::update(const gb::ces_entity_shared_ptr& entity, f32 deltatime)
    {
        gb::ces_box2d_body_component_shared_ptr box2d_body_component =
        client_base_character_controller::get_component<gb::ces_box2d_body_component>();
		bool is_synchronized = true;

		if (m_is_net_session)
		{
			is_synchronized = client_main_character_controller::check_synchronization(m_received_client_tick,
				m_server_adjust_position,
				m_server_adjust_rotation);
		}
        
        glm::vec2 current_position = client_base_character_controller::position;
        f32 current_rotation = client_base_character_controller::rotation;
        
        glm::vec2 camera_position = m_camera->get_position();
        f32 camera_rotation = m_camera->get_rotation();
        camera_rotation = camera_rotation;
        
        if(m_is_dragging)
        {
            glm::vec2 delta = m_joystick_delta;
            f32 current_move_speed = k_move_speed * delta.y * deltatime;
            current_rotation -= k_rotate_speed * delta.x * deltatime;
            current_rotation = current_rotation;
            
            if(!is_synchronized && m_is_net_session)
            {
                current_rotation = glm::mix_angles_degrees(current_rotation, m_server_adjust_rotation, .5f);
                current_position = glm::mix(current_position, m_server_adjust_position, .5f);
            }
            
            glm::vec2 velocity = glm::vec2(-sinf(glm::radians(current_rotation)) * current_move_speed,
                                           cosf(glm::radians(current_rotation)) * current_move_speed);
            
            client_base_character_controller::position = current_position;
            client_base_character_controller::rotation = current_rotation;
            box2d_body_component->velocity = velocity;
            
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
                    gb::game_object_shared_ptr light_source = std::static_pointer_cast<gb::game_object>(child);
                    light_source->rotation = -current_rotation;
                }
            }
        }
        else
        {
			if (m_is_net_session)
			{
				current_rotation = glm::mix_angles_degrees(current_rotation, m_server_adjust_rotation, .5f);
				current_position = glm::mix(current_position, m_server_adjust_position, .5f);
			}
            
            client_base_character_controller::position = current_position;
            client_base_character_controller::rotation = current_rotation;
            box2d_body_component->velocity = glm::vec2(0.f);
            
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
                    gb::game_object_shared_ptr light_source = std::static_pointer_cast<gb::game_object>(child);
                    light_source->rotation = -current_rotation;
                }
            }
        }
        m_camera->set_position(glm::mix(camera_position, current_position * -1.f, .5f));
        m_camera->set_rotation(glm::mix_angles_degrees(camera_rotation, current_rotation * -1.f, .5f));
        
        if(m_character_moving_callback && m_is_net_session)
        {
            m_character_moving_callback(m_client_tick, m_joystick_delta);
            client_character_move_history_point history_point;
            history_point.m_client_tick = m_client_tick;
            history_point.m_position = current_position;
            history_point.m_rotation = current_rotation;
            m_client_character_move_history.push_back(history_point);
            m_client_tick++;
        }
    }
    
    bool client_main_character_controller::check_synchronization(ui64 client_tick, const glm::vec2& position, f32 rotation)
    {
        bool is_synchronized = true;
        m_client_character_move_history.remove_if([=, &is_synchronized](const client_character_move_history_point& history_point) {
            if(history_point.m_client_tick < client_tick)
            {
                return true;
            }
            if(history_point.m_client_tick == client_tick)
            {
                glm::vec2 history_position = history_point.m_position;
                f32 history_rotation = history_point.m_rotation;
                f32 position_delta = glm::length(position - history_position);
                f32 rotation_delta = fabsf(rotation - history_rotation);
                std::cout<<"position delta: "<<position_delta<<std::endl;
                std::cout<<"rotation delta: "<<rotation_delta<<std::endl;
                if(position_delta > 16.f && rotation_delta > 5.f)
                {
                    is_synchronized = false;
                }
                return true;
            }
            return false;
        });
        return is_synchronized;
    }
}

