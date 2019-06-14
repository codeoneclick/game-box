//
//  main_menu_scene.h
//  gbDemo
//
//  Created by serhii serhiiv on 12/14/16.
//  Copyright © 2016 sergey.sergeev. All rights reserved.
//

#pragma once

#include "scene_graph.h"
#include "ns_declarations.h"

namespace game
{
    class main_menu_scene : public gb::scene_graph
    {
    private:
        
    protected:
        
        gb::ui::ui_fabricator_shared_ptr m_ui_base_fabricator = nullptr;
        
		gameplay_fabricator_shared_ptr m_gameplay_fabricator = nullptr;;
        gameplay_ui_fabricator_shared_ptr m_gameplay_ui_fabricator = nullptr;

        gb::camera_3d_shared_ptr m_camera_3d;
        
		void on_goto_ui_editor_scene(gb::ces_entity_const_shared_ptr entity);
        void on_goto_in_game_scene(gb::ces_entity_const_shared_ptr entity);
        void on_play_rewarded_video(gb::ces_entity_const_shared_ptr entity);
        void on_goto_net_menu_scene(gb::ces_entity_const_shared_ptr entity);
        
    public:
        
        main_menu_scene(const gb::game_transition_shared_ptr& transition);
        ~main_menu_scene();
        
        void create() override;
    };
};
