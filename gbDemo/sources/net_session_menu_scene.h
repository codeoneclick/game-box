//
//  net_session_menu_scene.h
//  gbDemo
//
//  Created by serhii serhiiv on 4/12/17.
//  Copyright © 2017 sergey.sergeev. All rights reserved.
//

#pragma once

#include "scene_graph.h"
#include "ns_declarations.h"

namespace game
{
    class net_session_menu_scene : public gb::scene_graph
    {
    private:
        
        i32 m_session_udid;
        
    protected:
        
        gb::ui::ui_fabricator_shared_ptr m_ui_fabricator;
        gb::camera_shared_ptr m_camera;
        gb::ui::textfield_weak_ptr m_public_server_status_label;
        
        void on_goto_net_game_scene(gb::ces_entity_const_shared_ptr entity);
        void on_goto_main_menu_scene(gb::ces_entity_const_shared_ptr entity);
        
        void on_connection_established_command(gb::net::command_const_shared_ptr command);

    public:
        
        net_session_menu_scene(const gb::game_transition_shared_ptr& transition);
        ~net_session_menu_scene();
        
        void create();
    };
};

