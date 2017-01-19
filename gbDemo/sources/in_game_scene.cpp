//
//  in_game_scene.cpp
//  gbDemo
//
//  Created by serhii serhiiv on 12/22/16.
//  Copyright © 2016 sergey.sergeev. All rights reserved.
//

#include "in_game_scene.h"
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
#include "character_controller.h"
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

namespace ns
{
    in_game_scene::in_game_scene(const gb::game_transition_shared_ptr& transition) :
    gb::scene_graph(transition)
    {
        m_current_character_udid = std::numeric_limits<ui32>::max();
    }
    
    in_game_scene::~in_game_scene()
    {
        
    }
    
    void in_game_scene::create()
    {
        gb::scene_graph::create();
        
        auto network_system = std::make_shared<gb::net::ces_network_system>();
        in_game_scene::get_transition()->add_system(network_system);
        
        network_system->register_command_callback(gb::net::command_client_connection_established::class_guid(),
                                                  std::bind(&in_game_scene::on_connection_established_command, this,
                                                            std::placeholders::_1));
        
        network_system->register_command_callback(gb::net::command_character_spawn::class_guid(),
                                                  std::bind(&in_game_scene::on_character_spawn_command, this,
                                                            std::placeholders::_1));
        
        auto character_controllers_system = std::make_shared<ces_character_controllers_system>();
        character_controllers_system->set_order(1);
        in_game_scene::get_transition()->add_system(character_controllers_system);
        
        gb::net::ces_client_component_shared_ptr client_component = std::make_shared<gb::net::ces_client_component>();
        client_component->connect("192.168.0.97", 6868);
        in_game_scene::add_component(client_component);
        
        m_ui_fabricator = std::make_shared<gb::ui::ui_fabricator>(in_game_scene::get_fabricator());
        
        m_camera = std::make_shared<gb::camera>(in_game_scene::get_transition()->get_screen_width(),
                                                in_game_scene::get_transition()->get_screen_height());
        in_game_scene::set_camera(m_camera);
        m_camera->set_position(glm::vec2(512.f, 512.f));
        
        auto animation_system = std::make_shared<gb::anim::ces_ani_animation_system>();
        animation_system->init();
		animation_system->set_order(3);
        in_game_scene::get_transition()->add_system(animation_system);
        
        m_anim_fabricator = std::make_shared<gb::anim::anim_fabricator>(in_game_scene::get_fabricator());
        
        auto level = m_anim_fabricator->create_animated_sprite("ns_level_01.xml", "level");
        in_game_scene::add_child(level);
        level->position = glm::vec2(0.f, 0.f);
        level->goto_and_stop(0);
        level->is_shadow_caster = true;
        
        gb::vbo::vertex_attribute vertices[4];
        vertices[0].m_position = glm::vec3(0.f,    0.f,    0.f);
        vertices[1].m_position = glm::vec3(2048.f, 0.f,    0.f);
        vertices[2].m_position = glm::vec3(2048.f, 2048.f, 0.f);
        vertices[3].m_position = glm::vec3(0.f,    2048.f, 0.f);
        
        gb::ces_convex_hull_component_shared_ptr convex_hull_component = std::make_shared<gb::ces_convex_hull_component>();
        convex_hull_component->create(vertices, 4);
        level->add_component(convex_hull_component);
        
        gb::ces_shadow_component_shared_ptr shadow_component = std::make_shared<gb::ces_shadow_component>();
        level->add_component(shadow_component);
        
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
					glm::vec2 point1 = pt1.lock()->position;
					glm::vec2 point2 = pt2.lock()->position;
					glm::vec2 point3 = pt3.lock()->position;
					glm::vec2 point4 = pt4.lock()->position;

					pt1.lock()->visible = false;
					pt2.lock()->visible = false;
					pt3.lock()->visible = false;
					pt4.lock()->visible = false;

					gb::vbo::vertex_attribute vertices[4];
					vertices[0].m_position = glm::vec3(point1.x, point1.y, 0.f);
					vertices[1].m_position = glm::vec3(point2.x, point2.y, 0.f);
					vertices[2].m_position = glm::vec3(point3.x, point3.y, 0.f);
					vertices[3].m_position = glm::vec3(point4.x, point4.y, 0.f);
                    
                    glm::vec2 min_bound = glm::vec2(INT16_MAX);
                    glm::vec2 max_bound = glm::vec2(INT16_MIN);
                    
                    for(i32 i = 0; i < 4; ++i)
                    {
                        min_bound = glm::min(glm::vec2(vertices[i].m_position.x, vertices[i].m_position.y), min_bound);
                        max_bound = glm::max(glm::vec2(vertices[i].m_position.x, vertices[i].m_position.y), max_bound);
                    }
                    glm::vec2 size = max_bound - min_bound;
                    
                    glm::mat4 absolute_transformation = gb::ces_transformation_extension::get_absolute_transformation_in_ws(wall);
                    size = size - glm::vec2(absolute_transformation[3][0], absolute_transformation[3][1]);
                    
					gb::ces_convex_hull_component_shared_ptr convex_hull_component = std::make_shared<gb::ces_convex_hull_component>();
					convex_hull_component->create(vertices, 4);
					wall->add_component(convex_hull_component);

					gb::ces_shadow_component_shared_ptr shadow_component = std::make_shared<gb::ces_shadow_component>();
					wall->add_component(shadow_component);
                    
                    wall->is_luminous = true;

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
    }
    
    void in_game_scene::on_connection_established_command(gb::net::command_const_shared_ptr command)
    {
        gb::net::command_client_connection_established_shared_ptr current_command = std::static_pointer_cast<gb::net::command_client_connection_established>(command);
        m_current_character_udid = current_command->get_udid();
    }
    
    void in_game_scene::on_character_spawn_command(gb::net::command_const_shared_ptr command)
    {
        gb::net::command_character_spawn_shared_ptr current_command =  std::static_pointer_cast<gb::net::command_character_spawn>(command);
        ui32 current_udid = current_command->get_udid();
        if(current_udid == m_current_character_udid)
        {
            gb::light_source_shared_ptr light_source = in_game_scene::get_fabricator()->create_light_source("light_01.xml");
            light_source->radius = 512.f;
            light_source->color = glm::vec4(1.f, 1.f, 1.f, 1.f);
            light_source->tag = "light_source";
            
            gb::game_object_shared_ptr character_container = std::make_shared<gb::game_object>();
            character_container->position = glm::vec2(current_command->get_position_x(),
                                                      current_command->get_position_y());
            character_container->rotation = current_command->get_rotation();
            in_game_scene::add_child(character_container);
            
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
            character_container->add_child(light_source);
            
            gb::ui::joystick_shared_ptr joystick = m_ui_fabricator->create_joystick(glm::vec2(128.f, 128.f));
            joystick->position = glm::vec2(32.f, in_game_scene::get_transition()->get_screen_height() - (128.f + 32.f));
            joystick->tag = "joystick";
            in_game_scene::add_child(joystick);
            
            auto character_controller = std::make_shared<ns::character_controller>(m_camera, character_container);
            character_controller->set_joystick(joystick);
            in_game_scene::add_child(character_controller);
            
            m_character_controllers[current_udid] = character_controller;
            
            in_game_scene::enable_box2d_world(glm::vec2(0.f, 0.f), glm::vec2(2048.f, 2048.f));
            in_game_scene::apply_box2d_physics(character_container, b2BodyType::b2_dynamicBody, [](gb::ces_box2d_body_component_const_shared_ptr component) {
                component->shape = gb::ces_box2d_body_component::circle;
                component->set_radius(10.f);
            });
        }
        else
        {
            std::cerr<<"unimplemented"<<std::endl;
        }
    }
}
