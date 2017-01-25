//
//  in_game_scene.h
//  gbDemo
//
//  Created by serhii serhiiv on 12/22/16.
//  Copyright © 2016 sergey.sergeev. All rights reserved.
//

#pragma once

#include "scene_graph.h"
#include "ns_declarations.h"

namespace ns
{
    class in_game_scene : public gb::scene_graph
    {
    private:
        
        
    protected:
        
        gb::camera_shared_ptr m_camera;
		std::map<i32, server_character_controller_shared_ptr> m_character_controllers;
        
        void on_client_character_move_command(gb::net::command_const_shared_ptr command);
        void on_server_character_move(ui64 client_tick, i32 udid, const glm::vec2& velocity,
                                      const glm::vec2& position, f32 rotation);
        
        gb::anim::anim_fabricator_shared_ptr m_anim_fabricator;
        
		void on_log_server_message(const std::string& message, gb::ces_entity_const_shared_ptr entity);
		void on_connection_established(i32 udid);
        void on_connection_closed(i32 udid);
        
    public:
        
        in_game_scene(const gb::game_transition_shared_ptr& transition);
        ~in_game_scene();
        
        void create();
    };
};

