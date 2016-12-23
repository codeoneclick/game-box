//
//  in_game_transition.cpp
//  gbDemo
//
//  Created by serhii serhiiv on 12/22/16.
//  Copyright © 2016 sergey.sergeev. All rights reserved.
//

#include "in_game_transition.h"
#include "in_game_scene.h"
#include "scene_fabricator.h"

namespace ns
{
    in_game_transition::in_game_transition(const std::string& guid, bool is_offscreen) :
    game_transition(guid, is_offscreen)
    {
        
    }
    
    in_game_transition::~in_game_transition(void)
    {
        
    }
    
    void in_game_transition::create_scene()
    {
        m_scene = std::make_shared<in_game_scene>(shared_from_this());
        gb::scene_fabricator_shared_ptr fabricator = std::make_shared<gb::scene_fabricator>();
        in_game_transition::set_fabricator(fabricator);
    }
    
    void in_game_transition::destroy_scene()
    {
        m_scene = nullptr;
    }
}
