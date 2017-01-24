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
#include "game_commands_container.h"
#include "ces_box2d_body_component.h"
#include "anim_fabricator.h"
#include "animated_sprite.h"
#include "ces_ani_animation_system.h"
#include "ces_character_controllers_system.h"
#include "vbo.h"
#include "ces_convex_hull_component.h"
#include "ces_transformation_extension.h"
#include "ces_network_system.h"
#include "server_character_controller.h"
#include "ces_server_component.h"
#include "ces_server_broadcast_component.h"
#include "command_client_connection_established.h"
#include "command_character_spawn.h"
#include "command_client_character_move.h"
#include "command_server_character_move.h"

namespace ns
{
    in_game_scene::in_game_scene(const gb::game_transition_shared_ptr& transition) :
    gb::scene_graph(transition)
    {
        
    }
    
    in_game_scene::~in_game_scene()
    {
        
    }
    
    void in_game_scene::create()
    {
        gb::scene_graph::create();
        
        m_camera = std::make_shared<gb::camera>(2048,
                                                2048);
        in_game_scene::set_camera(m_camera);
        
        auto animation_system = std::make_shared<gb::anim::ces_ani_animation_system>();
        animation_system->init();
		animation_system->set_order(3);
        in_game_scene::get_transition()->add_system(animation_system);
        
        auto character_controllers_system = std::make_shared<ces_character_controllers_system>();
        character_controllers_system->set_order(1);
        in_game_scene::get_transition()->add_system(character_controllers_system);
        
        m_anim_fabricator = std::make_shared<gb::anim::anim_fabricator>(in_game_scene::get_fabricator());
        
        auto level = m_anim_fabricator->create_animated_sprite("ns_level_01.xml", "level");
        in_game_scene::add_child(level);
        level->position = glm::vec2(0.f, 0.f);
        level->goto_and_stop(0);

        in_game_scene::enable_box2d_world(glm::vec2(0.f, 0.f), glm::vec2(2048.f, 2048.f));

		std::list<gb::ces_entity_shared_ptr> level_children = level->children;
		i32 level_children_count = static_cast<i32>(level_children.size());

		for (i32 i = 0; i < level_children_count; ++i)
		{
			std::stringstream str_stream;
			str_stream << "wall_" << i;
			auto weak_wall = level->get_named_part(str_stream.str());
			if (!weak_wall.expired())
			{
				auto wall = weak_wall.lock();
				auto pt1 = wall->get_named_part("pt_01");
				auto pt2 = wall->get_named_part("pt_02");
				auto pt3 = wall->get_named_part("pt_03");
				auto pt4 = wall->get_named_part("pt_04");

				if (pt1.lock() && pt2.lock() && pt3.lock() && pt4.lock())
				{
					pt1.lock()->visible = false;
					pt2.lock()->visible = false;
					pt3.lock()->visible = false;
					pt4.lock()->visible = false;

					std::vector<b2Vec2> box2d_vertices;
					box2d_vertices.push_back(b2Vec2(0.f, 0.f));
					box2d_vertices.push_back(b2Vec2(64.f, 0.f));
					box2d_vertices.push_back(b2Vec2(64.f, 64.f));
					box2d_vertices.push_back(b2Vec2(0.f, 64.f));

					in_game_scene::apply_box2d_physics(wall, b2BodyType::b2_staticBody, [box2d_vertices](gb::ces_box2d_body_component_const_shared_ptr component) {
						component->shape = gb::ces_box2d_body_component::custom_geometry_convex;
						component->set_custom_vertices(box2d_vertices);
					});
				}
            }
        }
        
        auto network_system = std::make_shared<gb::net::ces_network_system>();
        in_game_scene::get_transition()->add_system(network_system);
        
        network_system->register_command_callback(gb::net::command::k_command_client_character_move,
                                                  std::bind(&in_game_scene::on_client_character_move_command, this,
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
    
    void in_game_scene::on_connection_closed(ui32 udid)
    {
        auto character_controller = m_character_controllers.find(udid);
        if(character_controller != m_character_controllers.end())
        {
            m_character_controllers.erase(character_controller);
            in_game_scene::remove_child(character_controller->second);
        }
    }

	void in_game_scene::on_connection_established(ui32 udid)
	{
		auto server_component = in_game_scene::get_component<gb::net::ces_server_component>();
		auto command_connection_established = std::make_shared<gb::net::command_client_connection_established>(udid);
		server_component->send_command(command_connection_established, udid);
		auto command_character_spawn = std::make_shared<gb::net::command_character_spawn>(udid, 256.f, 256.f, 0.f);
		server_component->send_command(command_character_spawn, udid);
        
        for(const auto& character_controller :  m_character_controllers)
        {
            glm::vec2 position = character_controller.second->get_position();
            f32 rotation = character_controller.second->get_rotation();
            auto command_character_spawn = std::make_shared<gb::net::command_character_spawn>(character_controller.first,
                                                                                              position.x,
                                                                                              position.y,
                                                                                              rotation);
            server_component->send_command(command_character_spawn, udid);
            
            command_character_spawn = std::make_shared<gb::net::command_character_spawn>(udid,
                                                                                         256.f,
                                                                                         256.f,
                                                                                         0.f);
            server_component->send_command(command_character_spawn, character_controller.first);
        }
        
        gb::game_object_shared_ptr character_container = std::make_shared<gb::game_object>();
        character_container->position = glm::vec2(256.f,
                                                  256.f);
        character_container->rotation = 0.f;
        
        auto feet = m_anim_fabricator->create_animated_sprite("ns_character_01.xml", "feet_animation");
        feet->tag = "feet";
        feet->goto_and_play("idle");
        feet->is_luminous = true;
        feet->rotation = -90.f;
        character_container->add_child(feet);
        
        auto body = m_anim_fabricator->create_animated_sprite("ns_character_01.xml", "character_animation_shotgun");
        body->tag = "body";
        body->goto_and_play("idle");
        body->is_luminous = true;
        body->rotation = -90.f;
        character_container->add_child(body);
        
        in_game_scene::apply_box2d_physics(character_container, b2BodyType::b2_dynamicBody, [](gb::ces_box2d_body_component_const_shared_ptr component) {
            component->shape = gb::ces_box2d_body_component::circle;
            component->set_radius(10.f);
        });
        
        auto character_controller = std::make_shared<ns::server_character_controller>(udid);
        character_controller->set_character(character_container);
        character_controller->set_character_moving_callback(std::bind(&in_game_scene::on_server_character_move,
                                                                      this, std::placeholders::_1,
                                                                      std::placeholders::_2,
                                                                      std::placeholders::_3,
                                                                      std::placeholders::_4,
                                                                      std::placeholders::_5,
                                                                      std::placeholders::_6));
        in_game_scene::add_child(character_controller);
        m_character_controllers[udid] = character_controller;
    }
    
    void in_game_scene::on_client_character_move_command(gb::net::command_const_shared_ptr command)
    {
        gb::net::command_client_character_move_shared_ptr current_command =  std::static_pointer_cast<gb::net::command_client_character_move>(command);
        ui32 udid = current_command->get_udid();
        const auto& iterator = m_character_controllers.find(udid);
        if(iterator != m_character_controllers.end())
        {
            ui64 timestamp = current_command->get_timestamp();
            glm::vec2 delta = current_command->get_delta();
            bool is_moving = current_command->is_moving();
            iterator->second->on_changed_server_transformation(timestamp, delta, is_moving);
        }
        else
        {
            assert(false);
        }
    }
    
    void in_game_scene::on_server_character_move(ui64 timestamp, ui32 udid, const glm::vec2& velocity,
                                                 const glm::vec2& position, f32 rotation, bool is_moving)
    {
        auto server_component = in_game_scene::get_component<gb::net::ces_server_component>();
        auto command_server_character_move = std::make_shared<gb::net::command_server_character_move>(timestamp,
                                                                                                      udid,
                                                                                                      velocity,
                                                                                                      position,
                                                                                                      rotation,
                                                                                                      is_moving);
        //std::cout<<"udid: "<<udid<<" velocity: "<<velocity.x<<", "<<velocity.y<<" rotation: "<<rotation<<std::endl;
        server_component->send_command(command_server_character_move);
    }
}
