//
//  in_game_scene.cpp
//  gbDemo
//
//  Created by serhii serhiiv on 12/22/16.
//  Copyright © 2016 sergey.sergeev. All rights reserved.
//

#include "net_session_game_scene.h"
#include "ui_fabricator.h"
#include "game_transition.h"
#include "scene_fabricator.h"
#include "camera.h"
#include "button.h"
#include "joystick.h"
#include "game_commands_container.h"
#include "ces_box2d_body_component.h"
#include "ns_ui_commands.h"
#include "anim_fabricator.h"
#include "animated_sprite.h"
#include "light_source.h"
#include "client_main_character_controller.h"
#include "client_base_character_controller.h"
#include "ces_ani_animation_system.h"
#include "ces_character_controllers_system.h"
#include "vbo.h"
#include "ces_convex_hull_component.h"
#include "ces_shadow_component.h"
#include "ces_transformation_extension.h"
#include "ces_client_component.h"
#include "ces_network_system.h"
#include "command_client_connection_established.h"
#include "command_character_spawn.h"
#include "command_client_connection_established.h"
#include "command_character_spawn.h"
#include "command_client_character_move.h"
#include "command_server_character_move.h"

namespace game
{
	net_session_game_scene::net_session_game_scene(const gb::game_transition_shared_ptr& transition) :
    gb::scene_graph(transition)
    {
        m_current_character_udid = -1;
    }
    
	net_session_game_scene::~net_session_game_scene()
    {
        
    }
    
    void net_session_game_scene::create()
    {
        gb::scene_graph::create();
        
        auto network_system = std::make_shared<gb::net::ces_network_system>();
		net_session_game_scene::get_transition()->add_system(network_system);
        
        network_system->register_command_callback(gb::net::command::k_command_client_connection_established,
                                                  std::bind(&net_session_game_scene::on_connection_established_command, this,
                                                            std::placeholders::_1));
        
        network_system->register_command_callback(gb::net::command::k_command_character_spawn,
                                                  std::bind(&net_session_game_scene::on_character_spawn_command, this,
                                                            std::placeholders::_1));
        
        network_system->register_command_callback(gb::net::command::k_command_server_character_move,
                                                  std::bind(&net_session_game_scene::on_character_move_command, this,
                                                            std::placeholders::_1));
        
        auto character_controllers_system = std::make_shared<ces_character_controllers_system>();
        character_controllers_system->set_order(1);
		net_session_game_scene::get_transition()->add_system(character_controllers_system);
        
        gb::net::ces_client_component_shared_ptr client_component = std::make_shared<gb::net::ces_client_component>();
        //client_component->connect("35.156.69.254", 6868);
        //client_component->connect("178.151.163.50", 6868);
        //client_component->connect("127.0.0.1", 6868);
        client_component->connect("192.168.0.5", 6868);
		net_session_game_scene::add_component(client_component);
        
        m_ui_fabricator = std::make_shared<gb::ui::ui_fabricator>(net_session_game_scene::get_fabricator());
        
        m_camera = std::make_shared<gb::camera>(net_session_game_scene::get_transition()->get_screen_width(),
			net_session_game_scene::get_transition()->get_screen_height());
		net_session_game_scene::set_camera(m_camera);
        m_camera->set_position(glm::vec2(512.f, 512.f));
        
        auto animation_system = std::make_shared<gb::anim::ces_ani_animation_system>();
        animation_system->init();
		animation_system->set_order(3);
		net_session_game_scene::get_transition()->add_system(animation_system);
        
        m_anim_fabricator = std::make_shared<gb::anim::anim_fabricator>(net_session_game_scene::get_fabricator());
        
        auto level = std::make_shared<game::level>();
        level->setup("ns_level_01.xml",
                     std::static_pointer_cast<gb::scene_graph>(shared_from_this()),
                     net_session_game_scene::get_fabricator(),
                     m_anim_fabricator,
                     glm::ivec2(1024),
                     glm::ivec2(64));
        net_session_game_scene::add_child(level);
        m_level = level;
    }
    
    void net_session_game_scene::on_connection_established_command(gb::net::command_const_shared_ptr command)
    {
        gb::net::command_client_connection_established_shared_ptr current_command = std::static_pointer_cast<gb::net::command_client_connection_established>(command);
        m_current_character_udid = current_command->udid;
    }
    
    void net_session_game_scene::on_character_spawn_command(gb::net::command_const_shared_ptr command)
    {
        gb::net::command_character_spawn_shared_ptr current_command =  std::static_pointer_cast<gb::net::command_character_spawn>(command);
        ui32 current_udid = current_command->udid;
        
        if(current_udid == m_current_character_udid)
        {
            gb::ui::joystick_shared_ptr joystick = m_ui_fabricator->create_joystick(glm::vec2(128.f, 128.f));
            joystick->position = glm::vec2(32.f, net_session_game_scene::get_transition()->get_screen_height() - (128.f + 32.f));
            joystick->tag = "joystick";
            net_session_game_scene::add_child(joystick);
            
            auto character_controller = std::make_shared<game::client_main_character_controller>(true,
                                                                                                 m_camera,
                                                                                                 std::static_pointer_cast<gb::scene_graph>(shared_from_this()),
                                                                                                 net_session_game_scene::get_fabricator(),
                                                                                                 m_anim_fabricator,
                                                                                                 m_level.lock()->layers);
            character_controller->setup("ns_character_01.xml");
            character_controller->position = current_command->position;;
            character_controller->rotation = current_command->rotation;
            
            net_session_game_scene::apply_box2d_physics(character_controller, b2BodyType::b2_dynamicBody, [](gb::ces_box2d_body_component_const_shared_ptr component) {
                component->shape = gb::ces_box2d_body_component::circle;
                component->set_radius(32.f);
            });
            net_session_game_scene::add_child(character_controller);
            
            m_main_character_controller = character_controller;
            m_main_character_controller->set_character_moving_callback(std::bind(&net_session_game_scene::on_main_character_move,
                                                                                 this, std::placeholders::_1,
                                                                                 std::placeholders::_2));
        }
        else
        {
            auto character_controller = std::make_shared<game::client_base_character_controller>(std::static_pointer_cast<gb::scene_graph>(shared_from_this()),
                                                                                                 net_session_game_scene::get_fabricator(),
                                                                                                 m_anim_fabricator,
                                                                                                 m_level.lock()->layers);
            character_controller->setup("ns_character_01.xml");
            net_session_game_scene::apply_box2d_physics(character_controller, b2BodyType::b2_dynamicBody, [](gb::ces_box2d_body_component_const_shared_ptr component) {
                component->shape = gb::ces_box2d_body_component::circle;
                component->set_radius(32.f);
            });
            net_session_game_scene::add_child(character_controller);
            m_base_character_controllers[current_udid] = character_controller;
        }
    }
    
    void net_session_game_scene::on_main_character_move(ui64 timestamp, const glm::vec2& delta)
    {
        gb::net::command_client_character_move_shared_ptr command =
        std::make_shared<gb::net::command_client_character_move>(timestamp, m_current_character_udid, delta);
        auto client_component = net_session_game_scene::get_component<gb::net::ces_client_component>();
        client_component->send_command(command);
    }
    
    void net_session_game_scene::on_character_move_command(gb::net::command_const_shared_ptr command)
    {
        gb::net::command_server_character_move_shared_ptr current_command =  std::static_pointer_cast<gb::net::command_server_character_move>(command);
        ui32 current_udid = current_command->udid;
        if(current_udid != m_current_character_udid)
        {
            auto character_controller = m_base_character_controllers.find(current_udid);
            assert(character_controller != m_base_character_controllers.end());
            if(character_controller != m_base_character_controllers.end())
            {
                glm::vec2 velocity = current_command->velocity;
                glm::vec2 position = current_command->position;
                f32 rotation = current_command->rotation;
                character_controller->second->on_changed_server_transformation(velocity,
                                                                               position,
                                                                               rotation);
            }
        }
        else
        {
            ui64 client_tick = current_command->client_tick;
            glm::vec2 position = current_command->position;
            f32 rotation = current_command->rotation;
            m_main_character_controller->synchronize_transformations(client_tick, position, rotation);
        }
    }
}
