//
//  in_game_scene.cpp
//  gbDemo
//
//  Created by serhii serhiiv on 12/22/16.
//  Copyright © 2016 sergey.sergeev. All rights reserved.
//

#include "local_session_game_scene.h"
#include "ui_fabricator.h"
#include "game_transition.h"
#include "scene_fabricator.h"
#include "camera.h"
#include "button.h"
#include "fullscreen_joystick.h"
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
#include "ces_bullet_system.h"
#include "vbo.h"
#include "ces_convex_hull_component.h"
#include "ces_shadow_component.h"
#include "ces_transformation_extension.h"
#include "ces_ai_system.h"
#include "ai_character_controller.h"
#include "path_map.h"

namespace game
{
	local_session_game_scene::local_session_game_scene(const gb::game_transition_shared_ptr& transition) :
    gb::scene_graph(transition),
    m_level_size(1024.f)
    {
    }
    
	local_session_game_scene::~local_session_game_scene()
    {
        
    }
    
    void local_session_game_scene::create()
    {
        gb::scene_graph::create();
        
        auto character_controllers_system = std::make_shared<ces_character_controllers_system>();
        character_controllers_system->set_order(1);
		local_session_game_scene::get_transition()->add_system(character_controllers_system);
        
        auto bullet_system = std::make_shared<ces_bullet_system>();
        bullet_system->set_order(2);
        local_session_game_scene::get_transition()->add_system(bullet_system);
        
        auto ai_system = std::make_shared<ces_ai_system>();
        local_session_game_scene::get_transition()->add_system(ai_system);
        
        path_map_shared_ptr path_map = std::make_shared<game::path_map>(glm::ivec2(16), glm::vec2(64.f));
        for(i32 i = 0; i < 16; ++i)
        {
            for(i32 j = 0; j < 16; ++j)
            {
                path_map->set_path_passable(i, j, true);
            }
        }
        path_map->update();
        ai_system->set_path_map(path_map);
        
        m_ui_fabricator = std::make_shared<gb::ui::ui_fabricator>(local_session_game_scene::get_fabricator());
        
        m_camera = std::make_shared<gb::camera>(local_session_game_scene::get_transition()->get_screen_width(),
			local_session_game_scene::get_transition()->get_screen_height());
		local_session_game_scene::set_camera(m_camera);
        
        auto animation_system = std::make_shared<gb::anim::ces_ani_animation_system>();
        animation_system->init();
		animation_system->set_order(3);
		local_session_game_scene::get_transition()->add_system(animation_system);
        
        m_anim_fabricator = std::make_shared<gb::anim::anim_fabricator>(local_session_game_scene::get_fabricator());
        
        auto level = m_anim_fabricator->create_animated_sprite("ns_level_01.xml", "level");
		local_session_game_scene::add_child(level);
        level->position = glm::vec2(0.f, 0.f);
        level->goto_and_stop(0);
        level->is_shadow_caster = true;
        
        gb::vbo::vertex_attribute vertices[4];
        vertices[0].m_position = glm::vec3(0.f, 0.f, 0.f);
        vertices[1].m_position = glm::vec3(m_level_size, 0.f, 0.f);
        vertices[2].m_position = glm::vec3(m_level_size, m_level_size, 0.f);
        vertices[3].m_position = glm::vec3(0.f, m_level_size, 0.f);
        
        gb::ces_convex_hull_component_shared_ptr convex_hull_component = std::make_shared<gb::ces_convex_hull_component>();
        convex_hull_component->create(vertices, 4);
        level->add_component(convex_hull_component);
        
        gb::ces_shadow_component_shared_ptr shadow_component = std::make_shared<gb::ces_shadow_component>();
        level->add_component(shadow_component);
        
		std::list<gb::ces_entity_shared_ptr> level_children = level->children;
		size_t level_children_count = level_children.size();
        
		local_session_game_scene::enable_box2d_world(glm::vec2(0.f, 0.f), glm::vec2(m_level_size, m_level_size));

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
                    
                    local_session_game_scene::apply_box2d_physics(wall, b2BodyType::b2_staticBody, [box2d_vertices](gb::ces_box2d_body_component_const_shared_ptr component) {
                        component->shape = gb::ces_box2d_body_component::custom_geometry_convex;
                        component->set_custom_vertices(box2d_vertices);
                    });
                }
            }
        }
        
        gb::ui::fullscreen_joystick_shared_ptr joystick = m_ui_fabricator->create_fullscreen_joystick(glm::vec2(local_session_game_scene::get_transition()->get_screen_width(),
                                                                                                                local_session_game_scene::get_transition()->get_screen_height()),
                                                                                                      glm::vec2(local_session_game_scene::get_transition()->get_screen_width() * .5f,
                                                                                                                local_session_game_scene::get_transition()->get_screen_height() - 128.f));
        joystick->tag = "joystick";
        local_session_game_scene::add_child(joystick);
        
        gb::game_object_shared_ptr bullets_layer = std::make_shared<gb::game_object>();
        m_layer = bullets_layer;
        local_session_game_scene::add_child(bullets_layer);
        
        auto character_controller = std::make_shared<game::client_main_character_controller>(false,
                                                                                             m_camera,
                                                                                             m_layer.lock(),
                                                                                             std::static_pointer_cast<gb::scene_graph>(shared_from_this()),
                                                                                             local_session_game_scene::get_fabricator(),
                                                                                             m_anim_fabricator);
        character_controller->setup("ns_character_01.xml");
        character_controller->position = glm::vec2(256.f, 256.f);
        character_controller->rotation = 0.f;
        
        local_session_game_scene::apply_box2d_physics(character_controller, b2BodyType::b2_dynamicBody, [](gb::ces_box2d_body_component_const_shared_ptr component) {
            component->shape = gb::ces_box2d_body_component::circle;
            component->set_radius(32.f);
        });
        character_controller->set_joystick(joystick);
        local_session_game_scene::add_child(character_controller);
        
        m_main_character_controller = character_controller;
        
        auto ai_character_controller = std::make_shared<game::ai_character_controller>(m_layer.lock(),
                                                                                       std::static_pointer_cast<gb::scene_graph>(shared_from_this()),
                                                                                       local_session_game_scene::get_fabricator(),
                                                                                       m_anim_fabricator);
        ai_character_controller->setup("ns_character_01.xml");
        ai_character_controller->position = glm::vec2(128.f, 128.f);
        ai_character_controller->rotation = 0.f;
        local_session_game_scene::apply_box2d_physics(ai_character_controller, b2BodyType::b2_dynamicBody, [](gb::ces_box2d_body_component_const_shared_ptr component) {
            component->shape = gb::ces_box2d_body_component::circle;
            component->set_radius(32.f);
        });
        local_session_game_scene::add_child(ai_character_controller);
        
        m_ai_character_controllers[0] = ai_character_controller;
    }
}
