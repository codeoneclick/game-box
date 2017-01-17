//
//  demo_game_transition.cpp
//  gbDemo
//
//  Created by Sergey Sergeev on 8/13/15.
//  Copyright (c) 2015 sergey.sergeev. All rights reserved.
//

#include "demo_game_transition.h"
#include "demo_game_scene.h"
#include "scene_fabricator.h"

demo_game_transition::demo_game_transition(const std::string& guid, bool is_offscreen) :
game_transition(guid, is_offscreen)
{

}

demo_game_transition::~demo_game_transition(void)
{
    
}

void demo_game_transition::create_scene()
{
    m_scene = std::make_shared<demo_game_scene>(shared_from_this());
    gb::scene_fabricator_shared_ptr fabricator = std::make_shared<gb::scene_fabricator>();
    demo_game_transition::set_fabricator(fabricator);
}

void demo_game_transition::destroy_scene()
{
    m_scene = nullptr;
}