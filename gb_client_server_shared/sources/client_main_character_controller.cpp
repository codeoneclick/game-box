//
//  client_main_character_controller.cpp
//  gbDemo
//
//  Created by serhii serhiiv on 10/31/16.
//  Copyright © 2016 sergey.sergeev. All rights reserved.
//

#include "client_main_character_controller.h"
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

namespace ns
{
    client_main_character_controller::client_main_character_controller(bool is_net_session,
																	   const gb::camera_shared_ptr& camera,
                                                                       const gb::scene_graph_shared_ptr& scene_graph,
                                                                       const gb::scene_fabricator_shared_ptr& scene_fabricator,
                                                                       const gb::anim::anim_fabricator_shared_ptr& anim_fabricator) :
    ns::client_base_character_controller(scene_graph, scene_fabricator, anim_fabricator),
	m_is_net_session(is_net_session),
    m_camera(camera),
	m_joystick(nullptr),
	m_shoot_button(nullptr),
    m_joystick_delta(glm::vec2(0.f)),
    m_is_dragging(false),
    m_character_moving_callback(nullptr),
    m_server_adjust_position(glm::vec2(0.f)),
    m_server_adjust_rotation(0.f),
    m_client_tick(0)
    {
        m_received_client_tick = std::numeric_limits<ui64>::max();
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

	void client_main_character_controller::set_shoot_button(const gb::ui::button_shared_ptr& button)
	{
		m_shoot_button = button;
		m_shoot_button->set_on_pressed_callback(std::bind(&client_main_character_controller::on_shoot_button_pressed, this,
			std::placeholders::_1));
	}

	void client_main_character_controller::on_shoot_button_pressed(const gb::ces_entity_shared_ptr& entity)
	{
		bullet_shared_ptr bullet = std::make_shared<ns::bullet>();
		bullet->setup("ns_bullet_01.xml",
			m_scene_graph.lock(),
			m_scene_fabricator.lock(),
			m_anim_fabricator.lock());
		m_scene_graph.lock()->add_child(bullet);
		
		f32 current_rotation = client_base_character_controller::rotation;
		current_rotation += 180.f;
		glm::vec2 current_position = client_base_character_controller::position;

		m_scene_graph.lock()->apply_box2d_physics(bullet, b2BodyType::b2_dynamicBody, [](gb::ces_box2d_body_component_const_shared_ptr component) {
			component->shape = gb::ces_box2d_body_component::circle;
			component->set_radius(8.f);
		});

		gb::ces_box2d_body_component_shared_ptr box2d_body_component =
			bullet->get_component<gb::ces_box2d_body_component>();
		b2Body* box2d_body = box2d_body_component->box2d_body;

		box2d_body->SetAwake(true);
		f32 current_move_speed = 100000.f;

		b2Vec2 velocity = b2Vec2(-sinf(glm::radians(current_rotation)) * current_move_speed,
			cosf(glm::radians(current_rotation)) * current_move_speed);
		box2d_body->SetTransform(b2Vec2(current_position.x, current_position.y), current_rotation);
		box2d_body->SetLinearVelocity(velocity);
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
    
    void client_main_character_controller::synchronize_transformations(ui64 client_tick,
                                                                       const glm::vec2& position,
                                                                       const f32 rotation)
    {
        m_received_client_tick = client_tick;
        m_server_adjust_position = position;
        m_server_adjust_rotation = rotation;
    }
    
#define k_move_speed -5000.f
#define k_rotate_speed 2.f
    
    void client_main_character_controller::update(const gb::ces_entity_shared_ptr& entity, f32 deltatime)
    {
        gb::ces_box2d_body_component_shared_ptr box2d_body_component =
        client_base_character_controller::get_component<gb::ces_box2d_body_component>();
        b2Body* box2d_body = box2d_body_component->box2d_body;
		bool is_synchronized = true;

		if (m_is_net_session)
		{
			is_synchronized = client_main_character_controller::check_synchronization(m_received_client_tick,
				m_server_adjust_position,
				m_server_adjust_rotation);
		}
        
        f32 current_rotation = client_base_character_controller::rotation;
        glm::vec2 current_position = glm::vec2(box2d_body->GetPosition().x, box2d_body->GetPosition().y);
        
        if(m_is_dragging)
        {
            box2d_body->SetAwake(true);
            glm::vec2 delta = m_joystick_delta;
            f32 current_move_speed = k_move_speed * delta.y;
            current_rotation -= k_rotate_speed * delta.x;
            current_rotation = glm::wrap_degrees(current_rotation);
            
            if(!is_synchronized && m_is_net_session)
            {
                current_rotation = glm::mix_angles_degrees(current_rotation, m_server_adjust_rotation, .5f);
                current_position = glm::mix(current_position, m_server_adjust_position, .5f);
            }
            
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
                    gb::game_object_shared_ptr light_source = std::static_pointer_cast<gb::game_object>(child);
                    light_source->rotation = -current_rotation;
                }
            }
            
            m_camera->set_position(current_position * -1.f);
            m_camera->set_rotation(-current_rotation);
        }
        else
        {
            box2d_body->SetAwake(false);
			if (m_is_net_session)
			{
				current_rotation = glm::wrap_degrees(current_rotation);
				current_rotation = glm::mix_angles_degrees(current_rotation, m_server_adjust_rotation, .5f);
				current_position = glm::mix(current_position, m_server_adjust_position, .5f);
			}
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
                    gb::game_object_shared_ptr light_source = std::static_pointer_cast<gb::game_object>(child);
                    light_source->rotation = -current_rotation;
                }
            }
            
            m_camera->set_position(current_position * -1.f);
            m_camera->set_rotation(-current_rotation);
        }
        
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

