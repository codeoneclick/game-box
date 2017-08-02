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
        
    protected:
        
        gb::ui::ui_fabricator_shared_ptr m_ui_fabricator;
        gb::anim::anim_fabricator_shared_ptr m_anim_fabricator;
        gameplay_fabricator_shared_ptr m_gameplay_fabricator;
        gameplay_ui_fabricator_shared_ptr m_gameplay_ui_fabricator;
        gb::camera_2d_shared_ptr m_camera_2d;
        
    public:
        
		local_session_game_scene(const gb::game_transition_shared_ptr& transition);
        ~local_session_game_scene();
        
        void create();
    };
};

