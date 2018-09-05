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
        
        gb::ui::ui_fabricator_shared_ptr m_ui_fabricator;
		gb::anim::anim_fabricator_shared_ptr m_anim_fabricator;
		gameplay_fabricator_shared_ptr m_gameplay_fabricator;

        gb::camera_2d_shared_ptr m_camera_2d;
        
		void on_goto_ui_editor_scene(gb::ces_entity_const_shared_ptr entity);
        void on_goto_local_session(gb::ces_entity_const_shared_ptr entity);
        void on_goto_net_session(gb::ces_entity_const_shared_ptr entity);
        void on_goto_net_menu_scene(gb::ces_entity_const_shared_ptr entity);
        
    public:
        
        main_menu_scene(const gb::game_transition_shared_ptr& transition);
        ~main_menu_scene();
        
        void create() override;
    };
};
