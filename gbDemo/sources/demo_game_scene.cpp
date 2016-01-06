//
//  demo_game_scene.cpp
//  gbDemo
//
//  Created by sergey.sergeev on 8/27/15.
//  Copyright (c) 2015 sergey.sergeev. All rights reserved.
//

#include "demo_game_scene.h"
#include "game_transition.h"
#include "scene_fabricator.h"
#include "static_shape.h"

demo_game_scene::demo_game_scene(const gb::game_transition_shared_ptr& transition) :
gb::scene_graph(transition)
{

}

demo_game_scene::~demo_game_scene()
{
    
}

void demo_game_scene::create()
{
    gb::static_shape_shared_ptr static_shape = demo_game_scene::get_fabricator()->create_static_shape("shape_01.xml");
    static_shape->set_size(glm::vec2(128.f, 128.f));
    static_shape->set_position(glm::vec2(15.f, 25.f));
    demo_game_scene::add_child(static_shape);
}

