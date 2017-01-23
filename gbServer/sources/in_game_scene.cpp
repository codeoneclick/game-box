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
#include "ces_server_component.h"
#include "ces_server_broadcast_component.h"
#include "command_client_connection_established.h"
#include "command_character_spawn.h"

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
        
        auto anim_fabricator = std::make_shared<gb::anim::anim_fabricator>(in_game_scene::get_fabricator());
        
        auto level = anim_fabricator->create_animated_sprite("ns_level_01.xml", "level");
        in_game_scene::add_child(level);
        level->position = glm::vec2(0.f, 0.f);
        level->goto_and_stop(0);

        in_game_scene::enable_box2d_world(glm::vec2(0.f, 0.f), glm::vec2(2048.f, 2048.f));

		std::list<gb::ces_entity_shared_ptr> level_children = level->children;
		i32 level_children_count = level_children.size();

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

		gb::net::ces_server_component_shared_ptr server_component = std::make_shared<gb::net::ces_server_component>();
		server_component->set_log_callback(std::bind(&in_game_scene::on_log_server_message, this,
			std::placeholders::_1, std::placeholders::_2));
		server_component->set_connection_established_callback(std::bind(&in_game_scene::on_connection_established, this, std::placeholders::_1));
		server_component->start();
		in_game_scene::add_component(server_component);
    }

	void in_game_scene::on_log_server_message(const std::string& message, gb::ces_entity_const_shared_ptr entity)
	{
		std::cout<<message<<std::endl;
	}

	void in_game_scene::on_connection_established(ui32 udid)
	{
		auto server_component = in_game_scene::get_component<gb::net::ces_server_component>();
		auto command_connection_established = std::make_shared<gb::net::command_client_connection_established>(udid);
		server_component->send_command(command_connection_established, udid);
		auto command_character_spawn = std::make_shared<gb::net::command_character_spawn>(udid, 128.f, 256.f, 0.f);
		server_component->send_command(command_character_spawn, udid);
	}
}
