//
//  demo_game_scene.h
//  gbDemo
//
//  Created by sergey.sergeev on 8/27/15.
//  Copyright (c) 2015 sergey.sergeev. All rights reserved.
//

#ifndef demo_game_scene_h
#define demo_game_scene_h

#include "scene_graph.h"
#include "ui_declarations.h"
#include "ed_declarations.h"
#include "cs_declarations.h"

class demo_game_scene : public gb::scene_graph
{
private:
    
protected:
    
    gb::ui::ui_fabricator_shared_ptr m_ui_fabricator;
    gb::ed::ed_fabricator_shared_ptr m_ed_fabricator;
    
    cs::camera_controller_shared_ptr m_camera_controller;
    
public:
    
    demo_game_scene(const gb::game_transition_shared_ptr& transition);
    ~demo_game_scene();
    
    void create();
};

#endif
