//
//  server_menu_scene.h
//  gbDemo
//
//  Created by serhii serhiiv on 12/14/16.
//  Copyright © 2016 sergey.sergeev. All rights reserved.
//

#pragma once

#include "scene_graph.h"
#include "ns_declarations.h"

namespace ns
{
    class server_menu_scene : public gb::scene_graph
    {
    private:
        
        
    protected:
        
        gb::ui::ui_fabricator_shared_ptr m_ui_fabricator;
        gb::camera_shared_ptr m_camera;
        
    public:
        
        server_menu_scene(const gb::game_transition_shared_ptr& transition);
        ~server_menu_scene();
        
        void create();
    };
};

