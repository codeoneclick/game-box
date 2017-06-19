//
//  local_session_game_scene.h
//  gbDemo
//
//  Created by serhii serhiiv on 12/22/16.
//  Copyright © 2016 sergey.sergeev. All rights reserved.
//

#pragma once

#include "scene_graph.h"
#include "ns_declarations.h"

namespace game
{
    class local_session_game_scene : public gb::scene_graph
    {
    private:
        
        level_shared_ptr m_level;
        
        client_main_character_controller_shared_ptr m_player_character_controller;
        std::map<i32, ai_character_controller_shared_ptr> m_enemy_character_controllers;
        
        std::pair<gb::sprite_shared_ptr, gb::shape_3d_shared_ptr> m_player_character_linkage;
        std::map<i32, std::pair<gb::sprite_shared_ptr, gb::shape_3d_shared_ptr>> m_enemy_character_linkages;
        
        std::pair<gb::sprite_shared_ptr, gb::shape_3d_shared_ptr> m_player_character_avatar_linkage;
        std::map<i32, std::pair<gb::sprite_shared_ptr, gb::shape_3d_shared_ptr>> m_enemy_character_avatar_linkages;
        
        std::map<i32, ability_button_shared_ptr> m_abilities_buttons;
        attack_button_shared_ptr m_attack_button;
        avatar_icon_shared_ptr m_player_avatar;
        avatar_icon_shared_ptr m_enemy_avatar;
        
        static const i32 k_max_characters = 4;
        static const f32 k_character_sprite_size;
        static const f32 k_character_avatar_sprite_size;
        
        std::array<glm::vec2, k_max_characters> m_spawn_points;
        
        void preprocess_player(const std::string& character_sprite_filename,
                               const std::string& character_shape_3d_filename,
                               const std::string& character_avatar_sprite_filename,
                               const std::string& character_avatar_shape_3d_filename);
        
        void preprocess_enemy(const std::string& character_sprite_filename,
                              const std::string& character_shape_3d_filename,
                              const std::string& character_avatar_sprite_filename,
                              const std::string& character_avatar_shape_3d_filename);
        
        void deploy_player();
        void deploy_enemies();
        
        
    protected:
        
        gb::ui::ui_fabricator_shared_ptr m_ui_fabricator;
        gb::anim::anim_fabricator_shared_ptr m_anim_fabricator;
        gb::camera_2d_shared_ptr m_camera_2d;
        gb::ui::action_console_shared_ptr m_action_console;
        gb::ui::textfield_shared_ptr m_dead_cooldown_label;
        gb::ui::joystick_shared_ptr m_move_joystick;
        gb::ui::joystick_shared_ptr m_shoot_joystick;
        
        characters_3d_controller_shared_ptr m_characters_3d_controller;
        
        void on_statistic_message(const std::string& message);
        void on_tap_on_character(const gb::ces_entity_shared_ptr& entity);
        void on_tap_on_map_at_position(const glm::vec2& position);
        
        void on_player_character_death_cooldown(i32 seconds, i32 milliseconds);
        void on_player_character_died(const gb::ces_entity_shared_ptr& owner);
        void on_player_character_killed(const gb::ces_entity_shared_ptr& owner, const gb::ces_entity_shared_ptr& target);
        
    public:
        
		local_session_game_scene(const gb::game_transition_shared_ptr& transition);
        ~local_session_game_scene();
        
        void create();
    };
};

