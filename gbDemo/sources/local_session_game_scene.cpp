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
#include "ces_battle_system.h"
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
#include "ability_button.h"
#include "attack_button.h"
#include "avatar_icon.h"

namespace game
{
    const f32 local_session_game_scene::k_character_sprite_size = 256.f;
    const f32 local_session_game_scene::k_character_avatar_sprite_size = 192.f;
    
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

		m_scene_size = glm::vec2(667, 375);
        
        auto character_controllers_system = std::make_shared<ces_character_controllers_system>();
        character_controllers_system->set_order(1);
        local_session_game_scene::get_transition()->add_system(character_controllers_system);
        
        auto battle_system = std::make_shared<ces_battle_system>();
        battle_system->set_order(2);
        local_session_game_scene::get_transition()->add_system(battle_system);
        
        auto ai_system = std::make_shared<ces_ai_system>();
        local_session_game_scene::get_transition()->add_system(ai_system);
        
        auto sound_system = std::make_shared<gb::al::ces_sound_system>();
        local_session_game_scene::get_transition()->add_system(sound_system);
        
        m_ui_fabricator = std::make_shared<gb::ui::ui_fabricator>(local_session_game_scene::get_fabricator());
        
        m_camera_2d = std::make_shared<gb::camera_2d>(m_scene_size.x,
			m_scene_size.y);
        local_session_game_scene::set_camera_2d(m_camera_2d);
        
        m_characters_3d_controller = std::make_shared<characters_3d_controller>(std::static_pointer_cast<gb::scene_graph>(shared_from_this()),
                                                                                local_session_game_scene::get_fabricator());
        
        local_session_game_scene::add_child(m_characters_3d_controller);
        
        auto animation_system = std::make_shared<gb::anim::ces_ani_animation_system>();
        animation_system->init();
        animation_system->set_order(3);
        local_session_game_scene::get_transition()->add_system(animation_system);
        
        m_anim_fabricator = std::make_shared<gb::anim::anim_fabricator>(local_session_game_scene::get_fabricator());
        
        m_action_console = m_ui_fabricator->create_action_console(glm::vec2(m_scene_size.x, 48.f),
                                                                  3);
        m_action_console->position = glm::vec2(0.f);
        local_session_game_scene::add_child(m_action_console);
        
        m_level = std::make_shared<game::level>();
        m_level->setup("ns_level_01.xml",
                       m_camera_2d,
                       std::static_pointer_cast<gb::scene_graph>(shared_from_this()),
                       local_session_game_scene::get_fabricator(),
                       m_anim_fabricator,
                       glm::ivec2(4096, 2048),
                       glm::ivec2(64));
        local_session_game_scene::add_child(m_level);
        ai_system->set_path_map(m_level->path_map);
        
        m_spawn_points[0] = glm::vec2(128.f, 128.f);
        m_spawn_points[1] = glm::vec2(128.f, 1024.f - 128.f);
        m_spawn_points[2] = glm::vec2(1024.f - 128.f, 128.f);
        m_spawn_points[3] = glm::vec2(1024.f - 128.f, 1024.f - 128.f);
        
        local_session_game_scene::preprocess_player("knight.2d.xml", "knight.top.3d.xml",
                                                    "knight.portrait.xml", "knight.front.3d.xml");
        
        local_session_game_scene::preprocess_enemy("ghoul.2d.xml", "ghoul.top.3d.xml",
                                                   "ghoul.portrait.xml", "ghoul.front.3d.xml");
        local_session_game_scene::preprocess_enemy("orc.2d.xml", "orc.top.3d.xml",
                                                   "orc.portrait.xml", "orc.front.3d.xml");
        local_session_game_scene::preprocess_enemy("ghost.2d.xml", "ghost.top.3d.xml",
                                                   "ghost.portrait.xml", "ghost.front.3d.xml");
        
        local_session_game_scene::deploy_player();
        local_session_game_scene::deploy_enemies();
        
        m_dead_cooldown_label = m_ui_fabricator->create_textfield(glm::vec2(m_scene_size.x, 32.f), "respawn in: ");
        m_dead_cooldown_label->position = glm::vec2(m_scene_size.x * .5f,
			m_scene_size.y * .5f);
        m_dead_cooldown_label->set_background_color(glm::u8vec4(64, 64, 64, 196));
        m_dead_cooldown_label->tag = "dead_cooldown_label";
        m_dead_cooldown_label->visible = false;
        auto dead_cooldown_label_transformation_component = m_dead_cooldown_label->get_component<gb::ces_transformation_2d_component>();
        dead_cooldown_label_transformation_component->set_is_in_camera_space(false);
        m_level->get_layer(level::e_level_layer_characters_top_statistic)->add_child(m_dead_cooldown_label);
        
        
        m_player_avatar = std::make_shared<avatar_icon>();
        m_player_avatar->setup(m_player_character_avatar_linkage,
                               std::static_pointer_cast<gb::scene_graph>(shared_from_this()),
                               local_session_game_scene::get_fabricator(),
                               m_anim_fabricator);
        m_player_avatar->position = glm::vec2(8.f);
        m_level->get_layer(level::e_level_layer_characters_top_statistic)->add_child(m_player_avatar);
        
        m_enemy_avatar = std::make_shared<avatar_icon>();
        m_enemy_avatar->setup(m_enemy_character_avatar_linkages[0],
                              std::static_pointer_cast<gb::scene_graph>(shared_from_this()),
                              local_session_game_scene::get_fabricator(),
                              m_anim_fabricator);
        m_enemy_avatar->position = glm::vec2(m_scene_size.x - 88.f,
                                             8.f);
        m_level->get_layer(level::e_level_layer_characters_top_statistic)->add_child(m_enemy_avatar);
        m_enemy_avatar->visible = false;
        
        m_abilities_buttons[0] = std::make_shared<ability_button>();
        m_abilities_buttons[0]->setup("", std::static_pointer_cast<gb::scene_graph>(shared_from_this()),
                                      local_session_game_scene::get_fabricator(),
                                      m_anim_fabricator);
        m_abilities_buttons[0]->position = glm::vec2(m_scene_size.x - 80.f,
			m_scene_size.y - 128.f);
        m_level->get_layer(level::e_level_layer_characters_top_statistic)->add_child(m_abilities_buttons[0]);
        
        m_abilities_buttons[1] = std::make_shared<ability_button>();
        m_abilities_buttons[1]->setup("", std::static_pointer_cast<gb::scene_graph>(shared_from_this()),
                                      local_session_game_scene::get_fabricator(),
                                      m_anim_fabricator);
        m_abilities_buttons[1]->position = glm::vec2(m_scene_size.x - 80.f,
			m_scene_size.y - 208.f);
        m_level->get_layer(level::e_level_layer_characters_top_statistic)->add_child(m_abilities_buttons[1]);
        
        m_abilities_buttons[2] = std::make_shared<ability_button>();
        m_abilities_buttons[2]->setup("", std::static_pointer_cast<gb::scene_graph>(shared_from_this()),
                                      local_session_game_scene::get_fabricator(),
                                      m_anim_fabricator);
        m_abilities_buttons[2]->position = glm::vec2(m_scene_size.x - 80.f,
			m_scene_size.y - 288.f);
        m_level->get_layer(level::e_level_layer_characters_top_statistic)->add_child(m_abilities_buttons[2]);
        
        m_attack_button = std::make_shared<attack_button>();
        m_attack_button->setup("", std::static_pointer_cast<gb::scene_graph>(shared_from_this()),
                               local_session_game_scene::get_fabricator(),
                               m_anim_fabricator);
        m_attack_button->position = glm::vec2(16.f,
			m_scene_size.y - 80.f);
        m_attack_button->set_on_tap_on_attack_callback(std::bind(&client_main_character_controller::on_tap_on_attack_button,
                                                                 m_player_character_controller, std::placeholders::_1));
        m_level->get_layer(level::e_level_layer_characters_top_statistic)->add_child(m_attack_button);
    }
    
    void local_session_game_scene::on_statistic_message(const std::string& message)
    {
        m_action_console->write(message);
    }
    
    void local_session_game_scene::on_player_character_death_cooldown(i32 seconds, i32 milliseconds)
    {
        m_dead_cooldown_label->visible = seconds != 0 && milliseconds != 0;
        std::stringstream string_stream;
        string_stream<<"respawn in: "<<seconds;
        m_dead_cooldown_label->set_text(string_stream.str());
        glm::vec2 dead_cooldown_label_size = m_dead_cooldown_label->size;
        m_dead_cooldown_label->position = glm::vec2(m_scene_size.x * .5f - dead_cooldown_label_size.x * .5f,
                                                    m_scene_size.y * .5f - 16.f);
    }
    
    void local_session_game_scene::preprocess_player(const std::string& character_sprite_filename,
                                                     const std::string& character_shape_3d_filename,
                                                     const std::string& character_avatar_sprite_filename,
                                                     const std::string& character_avatar_shape_3d_filename)
    {
        m_player_character_linkage = m_characters_3d_controller->create_character(character_sprite_filename, character_shape_3d_filename,
                                                                                  glm::vec2(k_character_sprite_size), characters_3d_controller::e_view_type_top);
        m_player_character_linkage.second->set_custom_animation_fps(60);
        
        m_player_character_avatar_linkage = m_characters_3d_controller->create_character(character_avatar_sprite_filename, character_avatar_shape_3d_filename,
                                                                                         glm::vec2(k_character_avatar_sprite_size), characters_3d_controller::e_view_type_front);
        m_player_character_avatar_linkage.second->set_custom_animation_fps(60);
        m_player_character_avatar_linkage.second->play_animation("search", true);
    }
    
    void local_session_game_scene::preprocess_enemy(const std::string& character_sprite_filename,
                                                    const std::string& character_shape_3d_filename,
                                                    const std::string& character_avatar_sprite_filename,
                                                    const std::string& character_avatar_shape_3d_filename)
    {
        i32 index = static_cast<i32>(m_enemy_character_linkages.size());
        
        m_enemy_character_linkages[index]  = m_characters_3d_controller->create_character(character_sprite_filename, character_shape_3d_filename,
                                                                                          glm::vec2(k_character_sprite_size), characters_3d_controller::e_view_type_top);
        m_enemy_character_linkages[index].second->set_custom_animation_fps(60);
        
        m_enemy_character_avatar_linkages[index] = m_characters_3d_controller->create_character(character_avatar_sprite_filename, character_avatar_shape_3d_filename,
                                                                                                glm::vec2(k_character_avatar_sprite_size), characters_3d_controller::e_view_type_front);
        m_enemy_character_avatar_linkages[index].second->set_custom_animation_fps(60);
        m_enemy_character_avatar_linkages[index].second->play_animation("search", true);
    }
    
    void local_session_game_scene::deploy_player()
    {
        m_player_character_controller = std::make_shared<game::client_main_character_controller>(false,
                                                                                                 m_camera_2d,
                                                                                                 std::static_pointer_cast<gb::scene_graph>(shared_from_this()),
                                                                                                 local_session_game_scene::get_fabricator(),
                                                                                                 m_anim_fabricator,
                                                                                                 m_level->layers);
        m_player_character_controller->setup(m_player_character_linkage);
        m_player_character_controller->position = m_spawn_points[0];
        m_player_character_controller->set_spawn_point(m_spawn_points[0]);
        m_player_character_controller->rotation = 0.f;
        
        local_session_game_scene::apply_box2d_physics(m_player_character_controller, b2BodyType::b2_dynamicBody, [](gb::ces_box2d_body_component_const_shared_ptr component) {
            component->shape = gb::ces_box2d_body_component::circle;
            component->set_radius(32.f);
        });
        
        m_player_character_controller->set_map_size(glm::vec2(4096.f, 2048.f));
        m_player_character_controller->tag = "player";
        m_player_character_controller->set_statistic_callback(std::bind(&local_session_game_scene::on_statistic_message,
                                                                        this,
                                                                        std::placeholders::_1));
        m_player_character_controller->set_on_death_cooldown_callback(std::bind(&local_session_game_scene::on_player_character_death_cooldown,
                                                                                this,
                                                                                std::placeholders::_1,
                                                                                std::placeholders::_2));
        m_player_character_controller->set_on_died_callback(std::bind(&local_session_game_scene::on_player_character_died,
                                                                                this,
                                                                                std::placeholders::_1));
        m_player_character_controller->set_on_killed_callback(std::bind(&local_session_game_scene::on_player_character_killed,
                                                                                this,
                                                                                std::placeholders::_1,
                                                                                std::placeholders::_2));
        m_player_character_controller->set_path_map(m_level->path_map);
        m_level->set_on_tap_on_map_callback(std::bind(&local_session_game_scene::on_tap_on_map_at_position,
                                                      this,
                                                      std::placeholders::_1));
        m_level->get_layer(level::e_level_layer_characters)->add_child(m_player_character_controller);
    }
    
    void local_session_game_scene::deploy_enemies()
    {
        assert(m_enemy_character_linkages.size() <= k_max_characters - 1);
        i32 index = 1;
        for(const auto& enemy_character_linkage : m_enemy_character_linkages)
        {
            m_enemy_character_controllers[index - 1] = std::make_shared<game::ai_character_controller>(std::static_pointer_cast<gb::scene_graph>(shared_from_this()),
                                                                                                       local_session_game_scene::get_fabricator(),
                                                                                                       m_anim_fabricator,
                                                                                                       m_level->layers);
            m_enemy_character_controllers[index - 1]->setup(enemy_character_linkage.second);
            m_enemy_character_controllers[index - 1]->position = m_spawn_points[index];
            m_enemy_character_controllers[index - 1]->set_spawn_point(m_spawn_points[index]);
            m_enemy_character_controllers[index - 1]->rotation = 0.f;
            local_session_game_scene::apply_box2d_physics(m_enemy_character_controllers[index - 1], b2BodyType::b2_dynamicBody, [](gb::ces_box2d_body_component_const_shared_ptr component) {
                component->shape = gb::ces_box2d_body_component::circle;
                component->set_radius(32.f);
            });
            
            std::stringstream tag_string_stream;
            tag_string_stream<<"enemy "<<index;
            m_enemy_character_controllers[index - 1]->tag = tag_string_stream.str();
            m_enemy_character_controllers[index - 1]->set_statistic_callback(std::bind(&local_session_game_scene::on_statistic_message, this, std::placeholders::_1));
            m_enemy_character_controllers[index - 1]->set_on_tap_on_character_callback(std::bind(&local_session_game_scene::on_tap_on_character, this, std::placeholders::_1));
            m_level->get_layer(level::e_level_layer_characters)->add_child(m_enemy_character_controllers[index - 1]);
            
            index++;
        }
    }
    
    void local_session_game_scene::on_tap_on_character(const gb::ces_entity_shared_ptr& entity)
    {
        if(entity)
        {
            m_enemy_avatar->visible = true;
            for(const auto& enemy_character_controller : m_enemy_character_controllers)
            {
                if(entity == enemy_character_controller.second)
                {
                    m_enemy_avatar->set_avatar(m_enemy_character_avatar_linkages[enemy_character_controller.first]);
                    m_player_character_controller->on_tap_on_character(enemy_character_controller.second);
                    break;
                }
            }
        }
        else
        {
            m_enemy_avatar->visible = false;
            m_player_character_controller->on_tap_on_character(nullptr);
        }
    }
    
    void local_session_game_scene::on_tap_on_map_at_position(const glm::vec2& position)
    {
        m_enemy_avatar->visible = false;
        m_player_character_controller->on_tap_on_map_at_position(position);
    }
    
    void local_session_game_scene::on_player_character_died(const gb::ces_entity_shared_ptr& owner)
    {
        m_enemy_avatar->visible = false;
    }
    
    void local_session_game_scene::on_player_character_killed(const gb::ces_entity_shared_ptr& owner,
                                                              const gb::ces_entity_shared_ptr& target)
    {
        m_enemy_avatar->visible = false;
    }
}
