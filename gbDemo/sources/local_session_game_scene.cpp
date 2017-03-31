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
#include "joystick.h"
#include "action_console.h"
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
#include "level.h"

namespace game
{
	local_session_game_scene::local_session_game_scene(const gb::game_transition_shared_ptr& transition) :
    gb::scene_graph(transition)
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
        
        m_ui_fabricator = std::make_shared<gb::ui::ui_fabricator>(local_session_game_scene::get_fabricator());
        
        m_camera = std::make_shared<gb::camera>(local_session_game_scene::get_transition()->get_screen_width(),
			local_session_game_scene::get_transition()->get_screen_height());
		local_session_game_scene::set_camera(m_camera);
        
        auto animation_system = std::make_shared<gb::anim::ces_ani_animation_system>();
        animation_system->init();
        animation_system->set_order(3);
        local_session_game_scene::get_transition()->add_system(animation_system);
        
        m_anim_fabricator = std::make_shared<gb::anim::anim_fabricator>(local_session_game_scene::get_fabricator());
        
        m_action_console = m_ui_fabricator->create_action_console(glm::vec2(local_session_game_scene::get_transition()->get_screen_width(), 48.f),
                                                                  5);
        m_action_console->position = glm::vec2(0.f);
        local_session_game_scene::add_child(m_action_console);
        m_action_console->write("hello");
        m_action_console->write("world");
        
        auto level = std::make_shared<game::level>();
        level->setup("ns_level_01.xml",
                     std::static_pointer_cast<gb::scene_graph>(shared_from_this()),
                     local_session_game_scene::get_fabricator(),
                     m_anim_fabricator,
                     glm::ivec2(1024),
                     glm::ivec2(64));
        local_session_game_scene::add_child(level);
        ai_system->set_path_map(level->path_map);
        
        gb::ui::joystick_shared_ptr joystick_01 = m_ui_fabricator->create_joystick(glm::vec2(128.f));
        joystick_01->position = glm::vec2(32.f,
                                          local_session_game_scene::get_transition()->get_screen_height() - 160.f);
        joystick_01->tag = "joystick_01";
        local_session_game_scene::add_child(joystick_01);
        
        gb::ui::joystick_shared_ptr joystick_02 = m_ui_fabricator->create_joystick(glm::vec2(128.f));
        joystick_02->position = glm::vec2(local_session_game_scene::get_transition()->get_screen_width() - 160.f,
                                          local_session_game_scene::get_transition()->get_screen_height() - 160.f);
        joystick_02->tag = "joystick_02";
        local_session_game_scene::add_child(joystick_02);
        
        auto character_controller = std::make_shared<game::client_main_character_controller>(false,
                                                                                             m_camera,
                                                                                             std::static_pointer_cast<gb::scene_graph>(shared_from_this()),
                                                                                             local_session_game_scene::get_fabricator(),
                                                                                             m_anim_fabricator,
                                                                                             level->layers);
        level->get_layer(level::e_level_layer_characters)->add_child(character_controller);
        character_controller->setup("ns_character_01.xml");
        character_controller->position = glm::vec2(128.f, 128.f);
        character_controller->set_spawn_point(glm::vec2(128.f, 128.f));
        character_controller->rotation = 0.f;
        
        local_session_game_scene::apply_box2d_physics(character_controller, b2BodyType::b2_dynamicBody, [](gb::ces_box2d_body_component_const_shared_ptr component) {
            component->shape = gb::ces_box2d_body_component::circle;
            component->set_radius(32.f);
        });
        character_controller->set_move_joystick(joystick_01);
        character_controller->set_shoot_joystick(joystick_02);
        character_controller->set_map_size(glm::vec2(1024.f));
        m_main_character_controller = character_controller;
        m_main_character_controller->tag = "player";
        
        auto ai_character_controller = std::make_shared<game::ai_character_controller>(std::static_pointer_cast<gb::scene_graph>(shared_from_this()),
                                                                                       local_session_game_scene::get_fabricator(),
                                                                                       m_anim_fabricator,
                                                                                       level->layers);
        level->get_layer(level::e_level_layer_characters)->add_child(ai_character_controller);
        ai_character_controller->setup("ns_character_01.xml");
        ai_character_controller->position = glm::vec2(128.f, 1024.f - 128.f);
        ai_character_controller->set_spawn_point(glm::vec2(128.f, 1024.f - 128.f));
        ai_character_controller->rotation = 0.f;
        local_session_game_scene::apply_box2d_physics(ai_character_controller, b2BodyType::b2_dynamicBody, [](gb::ces_box2d_body_component_const_shared_ptr component) {
            component->shape = gb::ces_box2d_body_component::circle;
            component->set_radius(32.f);
        });
        m_ai_character_controllers[0] = ai_character_controller;
        m_ai_character_controllers[0]->tag = "bot 1";
    
        ai_character_controller = std::make_shared<game::ai_character_controller>(std::static_pointer_cast<gb::scene_graph>(shared_from_this()),
                                                                                  local_session_game_scene::get_fabricator(),
                                                                                  m_anim_fabricator,
                                                                                  level->layers);
        level->get_layer(level::e_level_layer_characters)->add_child(ai_character_controller);
        ai_character_controller->setup("ns_character_01.xml");
        ai_character_controller->position = glm::vec2(1024.f - 128.f, 128.f);
        ai_character_controller->set_spawn_point(glm::vec2(1024.f - 128.f, 128.f));
        ai_character_controller->rotation = 0.f;
        local_session_game_scene::apply_box2d_physics(ai_character_controller, b2BodyType::b2_dynamicBody, [](gb::ces_box2d_body_component_const_shared_ptr component) {
            component->shape = gb::ces_box2d_body_component::circle;
            component->set_radius(32.f);
        });
        m_ai_character_controllers[1] = ai_character_controller;
        m_ai_character_controllers[1]->tag = "bot 2";
        
        ai_character_controller = std::make_shared<game::ai_character_controller>(std::static_pointer_cast<gb::scene_graph>(shared_from_this()),
                                                                                  local_session_game_scene::get_fabricator(),
                                                                                  m_anim_fabricator,
                                                                                  level->layers);
        level->get_layer(level::e_level_layer_characters)->add_child(ai_character_controller);
        ai_character_controller->setup("ns_character_01.xml");
        ai_character_controller->position = glm::vec2(1024.f - 128.f, 1024.f - 128.f);
        ai_character_controller->set_spawn_point(glm::vec2(1024.f - 128.f, 1024.f - 128.f));
        ai_character_controller->rotation = 0.f;
        local_session_game_scene::apply_box2d_physics(ai_character_controller, b2BodyType::b2_dynamicBody, [](gb::ces_box2d_body_component_const_shared_ptr component) {
            component->shape = gb::ces_box2d_body_component::circle;
            component->set_radius(32.f);
        });
        m_ai_character_controllers[2] = ai_character_controller;
        m_ai_character_controllers[2]->tag = "bot 3";
    }
}
