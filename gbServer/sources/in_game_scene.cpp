//
//  in_game_scene.cpp
//  gbDemo
//
//  Created by serhii serhiiv on 12/22/16.
//  Copyright © 2016 sergey.sergeev. All rights reserved.
//

#include "in_game_scene.h"
#include "game_transition.h"
#include "scene_fabricator.h"
#include "camera.h"
#include "level.h"
#include "game_commands_container.h"
#include "ces_box2d_body_component.h"
#include "anim_fabricator.h"
#include "animated_sprite.h"
#include "ces_ani_animation_system.h"
#include "ces_bullet_system.h"
#include "ces_character_controllers_system.h"
#include "vbo.h"
#include "ces_convex_hull_component.h"
#include "ces_transformation_extension.h"
#include "ces_network_system.h"
#include "server_character_controller.h"
#include "ces_server_component.h"
#include "ces_server_broadcast_component.h"
#include "command_client_connection_established.h"
#include "command_client_join.h"
#include "command_character_spawn.h"
#include "command_client_character_move.h"
#include "command_server_character_move.h"
#include "command_client_character_shoot.h"
#include "command_server_character_shoot.h"

namespace game
{
    in_game_scene::in_game_scene(const gb::game_transition_shared_ptr& transition) :
    gb::scene_graph(transition),
    m_level(nullptr)
    {
        
    }
    
    in_game_scene::~in_game_scene()
    {
        
    }
    
    void in_game_scene::create()
    {
        gb::scene_graph::create();
        
        auto character_controllers_system = std::make_shared<ces_character_controllers_system>();
        character_controllers_system->set_order(1);
        in_game_scene::get_transition()->add_system(character_controllers_system);
        
        auto bullet_system = std::make_shared<ces_bullet_system>();
        bullet_system->set_order(2);
        in_game_scene::get_transition()->add_system(bullet_system);
        
        m_camera = std::make_shared<gb::camera>(16, 16);
        in_game_scene::set_camera(m_camera);
        
        auto animation_system = std::make_shared<gb::anim::ces_ani_animation_system>();
        animation_system->init();
		animation_system->set_order(3);
        in_game_scene::get_transition()->add_system(animation_system);
        
        m_anim_fabricator = std::make_shared<gb::anim::anim_fabricator>(in_game_scene::get_fabricator());
        
        m_level = std::make_shared<game::level>();
        m_level->setup("ns_level_01.xml",
                     std::static_pointer_cast<gb::scene_graph>(shared_from_this()),
                     in_game_scene::get_fabricator(),
                     m_anim_fabricator,
                     glm::ivec2(1024),
                     glm::ivec2(64));
        in_game_scene::add_child(m_level);

        auto network_system = std::make_shared<gb::net::ces_network_system>();
        in_game_scene::get_transition()->add_system(network_system);
        
        network_system->register_command_callback(gb::net::command::k_command_client_join,
                                                  std::bind(&in_game_scene::on_client_join_command, this,
                                                            std::placeholders::_1));
        network_system->register_command_callback(gb::net::command::k_command_client_character_move,
                                                  std::bind(&in_game_scene::on_client_character_move_command, this,
                                                            std::placeholders::_1));
        network_system->register_command_callback(gb::net::command::k_command_client_character_shoot,
                                                  std::bind(&in_game_scene::on_client_character_shoot_commnad, this,
                                                            std::placeholders::_1));
        
        gb::net::ces_server_component_shared_ptr server_component = std::make_shared<gb::net::ces_server_component>();
        server_component->set_log_callback(std::bind(&in_game_scene::on_log_server_message, this,
                                                     std::placeholders::_1, std::placeholders::_2));
		server_component->set_connection_established_callback(std::bind(&in_game_scene::on_connection_established, this,
                                                                        std::placeholders::_1));
        server_component->set_connection_closed_callback(std::bind(&in_game_scene::on_connection_closed, this,
                                                                        std::placeholders::_1));
		server_component->start();
		in_game_scene::add_component(server_component);
    }

	void in_game_scene::on_log_server_message(const std::string& message, gb::ces_entity_const_shared_ptr entity)
	{
		std::cout<<message<<std::endl;
	}
    
    void in_game_scene::on_connection_closed(i32 udid)
    {
        auto character_controller = m_character_controllers.find(udid);
        if(character_controller != m_character_controllers.end())
        {
            m_character_controllers.erase(character_controller);
            in_game_scene::remove_child(character_controller->second);
        }
    }

	void in_game_scene::on_connection_established(i32 udid)
	{
		auto server_component = in_game_scene::get_component<gb::net::ces_server_component>();
		auto command_connection_established = std::make_shared<gb::net::command_client_connection_established>(udid);
		server_component->send_command(command_connection_established, udid);
    }
    
    void in_game_scene::on_client_join_command(gb::net::command_const_shared_ptr command)
    {
        auto current_command = std::static_pointer_cast<gb::net::command_client_join>(command);
        i32 udid = current_command->udid;
        auto server_component = in_game_scene::get_component<gb::net::ces_server_component>();
        auto command_character_spawn = std::make_shared<gb::net::command_character_spawn>(udid, glm::vec2(128.f, 128.f), 0.f);
        server_component->send_command(command_character_spawn, udid);
        
        for(const auto& character_controller :  m_character_controllers)
        {
            glm::vec2 position = character_controller.second->position;
            f32 rotation = character_controller.second->rotation;
            auto command_character_spawn = std::make_shared<gb::net::command_character_spawn>(character_controller.first,
                                                                                              position,
                                                                                              rotation);
            server_component->send_command(command_character_spawn, udid);
            
            command_character_spawn = std::make_shared<gb::net::command_character_spawn>(udid,
                                                                                         glm::vec2(128.f, 128.f),
                                                                                         0.f);
            server_component->send_command(command_character_spawn, character_controller.first);
        }
        
        auto character_controller = std::make_shared<game::server_character_controller>(udid,
                                                                                        std::static_pointer_cast<gb::scene_graph>(shared_from_this()),
                                                                                        in_game_scene::get_fabricator(),
                                                                                        m_anim_fabricator,
                                                                                        m_level->layers);
        character_controller->setup("ns_character_01.xml");
        character_controller->position = glm::vec2(128.f, 128.f);
        character_controller->set_spawn_point(glm::vec2(128.f, 128.f));
        character_controller->rotation = 0.f;
        character_controller->set_server_character_move_callback(std::bind(&in_game_scene::on_server_character_move,
                                                                           this, std::placeholders::_1,
                                                                           std::placeholders::_2,
                                                                           std::placeholders::_3,
                                                                           std::placeholders::_4,
                                                                           std::placeholders::_5));
        
        in_game_scene::apply_box2d_physics(character_controller, b2BodyType::b2_dynamicBody, [](gb::ces_box2d_body_component_const_shared_ptr component) {
            component->shape = gb::ces_box2d_body_component::circle;
            component->set_radius(32.f);
        });
        
        m_level->get_layer(level::e_level_layer_characters)->add_child(character_controller);
        m_character_controllers[udid] = character_controller;
    }
    
    void in_game_scene::on_client_character_move_command(gb::net::command_const_shared_ptr command)
    {
        gb::net::command_client_character_move_shared_ptr client_move_command =  std::static_pointer_cast<gb::net::command_client_character_move>(command);
        i32 udid = client_move_command->udid;
        const auto& iterator = m_character_controllers.find(udid);
        if(iterator != m_character_controllers.end())
        {
            ui64 move_revision = client_move_command->move_revision;
            f32 move_angle = client_move_command->move_angle;
            f32 dt = client_move_command->dt;
            iterator->second->on_client_character_move(move_revision, move_angle, dt);
        }
        else
        {
            assert(false);
        }
    }
    
    void in_game_scene::on_server_character_move(ui64 move_revision, i32 udid, const glm::vec2& velocity,
                                                 const glm::vec2& position, f32 rotation)
    {
        auto server_component = in_game_scene::get_component<gb::net::ces_server_component>();
        auto command_server_character_move = std::make_shared<gb::net::command_server_character_move>(move_revision,
                                                                                                      udid,
                                                                                                      velocity,
                                                                                                      position,
                                                                                                      rotation);
        server_component->send_command(command_server_character_move);
    }
    
    void in_game_scene::on_client_character_shoot_commnad(gb::net::command_const_shared_ptr command)
    {
        gb::net::command_client_character_shoot_shared_ptr current_command =  std::static_pointer_cast<gb::net::command_client_character_shoot>(command);
        i32 udid = current_command->udid;
        const auto& iterator = m_character_controllers.find(udid);
        if(iterator != m_character_controllers.end())
        {
            ui64 shoot_revision = current_command->shoot_revision;
            f32 shoot_angle = current_command->shoot_angle;
            iterator->second->on_client_character_shoot(shoot_revision, shoot_angle);
        }
        else
        {
            assert(false);
        }
    }
    
    void in_game_scene::on_server_character_shoot(ui64 shoot_revision, i32 udid, const glm::vec2& velocity,
                                                  const glm::vec2& position, f32 rotation)
    {
        auto server_component = in_game_scene::get_component<gb::net::ces_server_component>();
        auto command_server_character_shoot = std::make_shared<gb::net::command_server_character_shoot>(shoot_revision,
                                                                                                        udid,
                                                                                                        velocity,
                                                                                                        position,
                                                                                                        rotation);
        server_component->send_command(command_server_character_shoot);
    }

}
