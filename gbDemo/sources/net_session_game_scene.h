//
//  net_session_game_scene.h
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
    class net_session_game_scene : public gb::scene_graph
    {
    private:
        
        void on_connection_established_command(gb::net::command_const_shared_ptr command);
        void on_character_spawn_command(gb::net::command_const_shared_ptr command);
        void on_character_move_command(gb::net::command_const_shared_ptr command);
        void on_character_shoot_command(gb::net::command_const_shared_ptr command);
        
        void on_main_character_move(ui64 move_revision, f32 move_angle, f32 dt);
        void on_main_character_shoot(ui64 shoot_revision, f32 shoot_angle);
        
        i32 m_current_character_udid;
        client_main_character_controller_shared_ptr m_main_character_controller;
        std::map<i32, client_base_character_controller_shared_ptr> m_base_character_controllers;
        
    protected:
        
        gb::ui::ui_fabricator_shared_ptr m_ui_fabricator;
        gb::anim::anim_fabricator_shared_ptr m_anim_fabricator;
        gb::camera_shared_ptr m_camera;
        level_weak_ptr m_level;
        
    public:
        
		net_session_game_scene(const gb::game_transition_shared_ptr& transition);
        ~net_session_game_scene();
        
        void create();
    };
};

