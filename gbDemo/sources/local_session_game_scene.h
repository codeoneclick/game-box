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
        
        client_main_character_controller_shared_ptr m_main_character_controller;
        std::map<i32, client_base_character_controller_shared_ptr> m_base_character_controllers;
        std::map<i32, ai_character_controller_shared_ptr> m_ai_character_controllers;
        
    protected:
        
        gb::ui::ui_fabricator_shared_ptr m_ui_fabricator;
        gb::anim::anim_fabricator_shared_ptr m_anim_fabricator;
        gb::camera_shared_ptr m_camera;
        
    public:
        
		local_session_game_scene(const gb::game_transition_shared_ptr& transition);
        ~local_session_game_scene();
        
        void create();
    };
};

