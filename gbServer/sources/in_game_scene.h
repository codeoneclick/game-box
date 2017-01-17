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
        character_controller_shared_ptr m_character_controller;
		void on_log_server_message(const std::string& message, gb::ces_entity_const_shared_ptr entity);
        
    public:
        
        in_game_scene(const gb::game_transition_shared_ptr& transition);
        ~in_game_scene();
        
        void create();
    };
};

