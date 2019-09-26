//
//  in_game_transition.cpp
//  gbDemo
//
//  Created by serhii.s on 4/16/19.
//  Copyright © 2019 sergey.sergeev. All rights reserved.
//

#include "in_game_transition.h"
#include "in_game_scene.h"
#include "scene_fabricator.h"

namespace game
{
    in_game_transition::in_game_transition(const std::string& guid, const gb::window_impl_shared_ptr& window, bool is_offscreen) :
    game_transition(guid, window, is_offscreen)
    {
        
    }
    
    void in_game_transition::create_scene()
    {
        m_scene = gb::ces_entity::construct<game::in_game_scene>(shared_from_this());
        gb::scene_fabricator_shared_ptr fabricator = std::make_shared<gb::scene_fabricator>();
        in_game_transition::set_fabricator(fabricator);
    }
    
    void in_game_transition::destroy_scene()
    {
        m_scene = nullptr;
    }
}
