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
#include "camera_2d.h"
#include "camera_3d.h"
#include "button.h"
#include "joystick.h"
#include "textfield.h"
#include "action_console.h"
#include "game_commands_container.h"
#include "ces_box2d_body_component.h"
#include "ns_ui_commands.h"
#include "anim_fabricator.h"
#include "animated_sprite.h"
#include "light_source_2d.h"
#include "client_main_character_controller.h"
#include "client_base_character_controller.h"
#include "ces_ani_animation_system.h"
#include "ces_character_controllers_system.h"
#include "ces_transformation_2d_component.h"
#include "ces_bullet_system.h"
#include "ces_sound_system.h"
#include "vbo.h"
#include "ces_convex_hull_component.h"
#include "ces_shadow_component.h"
#include "ces_transformation_extension.h"
#include "ces_ai_system.h"
#include "ai_character_controller.h"
#include "path_map.h"
#include "level.h"
#include "label.h"
#include "shape_3d.h"
#include "characters_3d_controller.h"

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
        
        auto sound_system = std::make_shared<gb::al::ces_sound_system>();
        local_session_game_scene::get_transition()->add_system(sound_system);
        
        m_ui_fabricator = std::make_shared<gb::ui::ui_fabricator>(local_session_game_scene::get_fabricator());
        
        m_camera_2d = std::make_shared<gb::camera_2d>(local_session_game_scene::get_transition()->get_screen_width(),
                                                      local_session_game_scene::get_transition()->get_screen_height());
        local_session_game_scene::set_camera_2d(m_camera_2d);
        
        m_characters_3d_controller = std::make_shared<characters_3d_controller>(std::static_pointer_cast<gb::scene_graph>(shared_from_this()),
                                                                                local_session_game_scene::get_fabricator());
        
        local_session_game_scene::add_child(m_characters_3d_controller);
        
        auto animation_system = std::make_shared<gb::anim::ces_ani_animation_system>();
        animation_system->init();
        animation_system->set_order(3);
        local_session_game_scene::get_transition()->add_system(animation_system);
        
        m_anim_fabricator = std::make_shared<gb::anim::anim_fabricator>(local_session_game_scene::get_fabricator());
        
        m_action_console = m_ui_fabricator->create_action_console(glm::vec2(local_session_game_scene::get_transition()->get_screen_width(), 48.f),
                                                                  3);
        m_action_console->position = glm::vec2(0.f);
        local_session_game_scene::add_child(m_action_console);
        
        auto main_character_linkage = m_characters_3d_controller->create_character("ghoul.2d.xml", "ghoul.top.3d.xml",
                                                                                   glm::vec2(256.f), characters_3d_controller::e_view_type_top);
        main_character_linkage.second->set_custom_animation_fps(60);
        
        auto ai_character_linkage_bot_01 = m_characters_3d_controller->create_character("orc.2d.xml", "orc.top.3d.xml",
                                                                                        glm::vec2(256.f), characters_3d_controller::e_view_type_top);
        ai_character_linkage_bot_01.second->set_custom_animation_fps(60);
        
        auto ai_character_linkage_bot_02 = m_characters_3d_controller->create_character("orc.2d.xml", "orc.top.3d.xml",
                                                                                        glm::vec2(256.f), characters_3d_controller::e_view_type_top);
        ai_character_linkage_bot_02.second->set_custom_animation_fps(60);
        
        auto ai_character_linkage_bot_03 = m_characters_3d_controller->create_character("orc.2d.xml", "orc.top.3d.xml",
                                                                                        glm::vec2(256.f), characters_3d_controller::e_view_type_top);
        ai_character_linkage_bot_03.second->set_custom_animation_fps(60);
        
        
        auto main_character_portrait_linkage = m_characters_3d_controller->create_character("ghoul.portrait.xml", "ghoul.front.3d.xml",
                                                                                            glm::vec2(192.f), characters_3d_controller::e_view_type_front);
        
        auto level = std::make_shared<game::level>();
        level->setup("ns_level_01.xml",
                     m_camera_2d,
                     std::static_pointer_cast<gb::scene_graph>(shared_from_this()),
                     local_session_game_scene::get_fabricator(),
                     m_anim_fabricator,
                     glm::ivec2(1024),
                     glm::ivec2(64));
        local_session_game_scene::add_child(level);
        ai_system->set_path_map(level->path_map);
        
        main_character_portrait_linkage.first->position = glm::vec2(local_session_game_scene::get_transition()->get_screen_width() - 96.f, 96.f);
        auto transformation_component = main_character_portrait_linkage.first->get_component<gb::ces_transformation_2d_component>();
        transformation_component->set_is_in_camera_space(false);
        main_character_portrait_linkage.second->set_custom_animation_fps(60);
        main_character_portrait_linkage.second->play_animation("search", true);
        level->get_layer(level::e_level_layer_characters_top_statistic)->add_child(main_character_portrait_linkage.first);
        
        m_main_character_controller = std::make_shared<game::client_main_character_controller>(false,
                                                                                               m_camera_2d,
                                                                                               std::static_pointer_cast<gb::scene_graph>(shared_from_this()),
                                                                                               local_session_game_scene::get_fabricator(),
                                                                                               m_anim_fabricator,
                                                                                               level->layers);
        m_main_character_controller->setup(main_character_linkage);
        m_main_character_controller->position = glm::vec2(128.f, 128.f);
        m_main_character_controller->set_spawn_point(glm::vec2(128.f, 128.f));
        m_main_character_controller->rotation = 0.f;
        
        local_session_game_scene::apply_box2d_physics(m_main_character_controller, b2BodyType::b2_dynamicBody, [](gb::ces_box2d_body_component_const_shared_ptr component) {
            component->shape = gb::ces_box2d_body_component::circle;
            component->set_radius(32.f);
        });
        
        m_main_character_controller->set_map_size(glm::vec2(1024.f));
        m_main_character_controller->tag = "player";
        m_main_character_controller->set_statistic_callback(std::bind(&local_session_game_scene::on_statistic_message, this, std::placeholders::_1));
        m_main_character_controller->set_dead_cooldown_callback(std::bind(&local_session_game_scene::on_dead_cooldown, this, std::placeholders::_1, std::placeholders::_2));
        m_main_character_controller->set_path_map(level->path_map);
        level->set_on_touch_level_callback(std::bind(&client_main_character_controller::on_touch_level_at_position, m_main_character_controller, std::placeholders::_1));
        level->get_layer(level::e_level_layer_characters)->add_child(m_main_character_controller);

        auto ai_character_controller = std::make_shared<game::ai_character_controller>(std::static_pointer_cast<gb::scene_graph>(shared_from_this()),
                                                                                       local_session_game_scene::get_fabricator(),
                                                                                       m_anim_fabricator,
                                                                                       level->layers);
        ai_character_controller->setup(ai_character_linkage_bot_01);
        ai_character_controller->position = glm::vec2(128.f, 1024.f - 128.f);
        ai_character_controller->set_spawn_point(glm::vec2(128.f, 1024.f - 128.f));
        ai_character_controller->rotation = 0.f;
        local_session_game_scene::apply_box2d_physics(ai_character_controller, b2BodyType::b2_dynamicBody, [](gb::ces_box2d_body_component_const_shared_ptr component) {
            component->shape = gb::ces_box2d_body_component::circle;
            component->set_radius(32.f);
        });
        m_ai_character_controllers[0] = ai_character_controller;
        m_ai_character_controllers[0]->tag = "bot";
        m_ai_character_controllers[0]->set_statistic_callback(std::bind(&local_session_game_scene::on_statistic_message, this, std::placeholders::_1));
        level->get_layer(level::e_level_layer_characters)->add_child(ai_character_controller);
        
        ai_character_controller = std::make_shared<game::ai_character_controller>(std::static_pointer_cast<gb::scene_graph>(shared_from_this()),
                                                                                  local_session_game_scene::get_fabricator(),
                                                                                  m_anim_fabricator,
                                                                                  level->layers);
        
        ai_character_controller->setup(ai_character_linkage_bot_02);
        ai_character_controller->position = glm::vec2(1024.f - 128.f, 128.f);
        ai_character_controller->set_spawn_point(glm::vec2(1024.f - 128.f, 128.f));
        ai_character_controller->rotation = 0.f;
        local_session_game_scene::apply_box2d_physics(ai_character_controller, b2BodyType::b2_dynamicBody, [](gb::ces_box2d_body_component_const_shared_ptr component) {
            component->shape = gb::ces_box2d_body_component::circle;
            component->set_radius(32.f);
        });
        m_ai_character_controllers[1] = ai_character_controller;
        m_ai_character_controllers[1]->tag = "bot 2";
        m_ai_character_controllers[1]->set_statistic_callback(std::bind(&local_session_game_scene::on_statistic_message, this, std::placeholders::_1));
        level->get_layer(level::e_level_layer_characters)->add_child(ai_character_controller);
        

        ai_character_controller = std::make_shared<game::ai_character_controller>(std::static_pointer_cast<gb::scene_graph>(shared_from_this()),
                                                                                  local_session_game_scene::get_fabricator(),
                                                                                  m_anim_fabricator,
                                                                                  level->layers);
        ai_character_controller->setup(ai_character_linkage_bot_03);
        ai_character_controller->position = glm::vec2(1024.f - 128.f, 1024.f - 128.f);
        ai_character_controller->set_spawn_point(glm::vec2(1024.f - 128.f, 1024.f - 128.f));
        ai_character_controller->rotation = 0.f;
        local_session_game_scene::apply_box2d_physics(ai_character_controller, b2BodyType::b2_dynamicBody, [](gb::ces_box2d_body_component_const_shared_ptr component) {
            component->shape = gb::ces_box2d_body_component::circle;
            component->set_radius(32.f);
        });
        m_ai_character_controllers[2] = ai_character_controller;
        m_ai_character_controllers[2]->tag = "bot 3";
        m_ai_character_controllers[2]->set_statistic_callback(std::bind(&local_session_game_scene::on_statistic_message, this, std::placeholders::_1));
        level->get_layer(level::e_level_layer_characters)->add_child(ai_character_controller);
        
        m_dead_cooldown_label = m_ui_fabricator->create_textfield(glm::vec2(local_session_game_scene::get_transition()->get_screen_width(), 32.f), "respawn in: ");
        m_dead_cooldown_label->position = glm::vec2(local_session_game_scene::get_transition()->get_screen_width() * .5f,
                                                    local_session_game_scene::get_transition()->get_screen_height() * .5f);
        m_dead_cooldown_label->set_background_color(glm::u8vec4(64, 64, 64, 196));
        m_dead_cooldown_label->tag = "dead_cooldown_label";
        m_dead_cooldown_label->visible = false;
        auto dead_cooldown_label_transformation_component = m_dead_cooldown_label->get_component<gb::ces_transformation_2d_component>();
        dead_cooldown_label_transformation_component->set_is_in_camera_space(false);
        level->get_layer(level::e_level_layer_characters_top_statistic)->add_child(m_dead_cooldown_label);
    }
    
    void local_session_game_scene::on_statistic_message(const std::string& message)
    {
        m_action_console->write(message);
    }
    
    void local_session_game_scene::on_dead_cooldown(i32 seconds, i32 milliseconds)
    {
        m_dead_cooldown_label->visible = seconds != 0 && milliseconds != 0;
        std::stringstream string_stream;
        string_stream<<"respawn in: "<<seconds;
        m_dead_cooldown_label->set_text(string_stream.str());
        glm::vec2 dead_cooldown_label_size = m_dead_cooldown_label->size;
        m_dead_cooldown_label->position = glm::vec2(local_session_game_scene::get_transition()->get_screen_width() * .5f - dead_cooldown_label_size.x * .5f,
                                                    local_session_game_scene::get_transition()->get_screen_height() * .5f - 16.f);
    }
}
