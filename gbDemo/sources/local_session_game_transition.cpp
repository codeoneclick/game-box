//
//  in_game_transition.cpp
//  gbDemo
//
//  Created by serhii serhiiv on 12/22/16.
//  Copyright © 2016 sergey.sergeev. All rights reserved.
//

#include "local_session_game_transition.h"
#include "local_session_game_scene.h"
#include "scene_fabricator.h"

namespace game
{
	local_session_game_transition::local_session_game_transition(const std::string& guid, bool is_offscreen) :
    game_transition(guid, is_offscreen)
    {
        
    }
    
	local_session_game_transition::~local_session_game_transition(void)
    {
        
    }
    
    void local_session_game_transition::create_scene()
    {
        m_scene = gb::ces_entity::construct<game::local_session_game_scene>(shared_from_this());
        gb::scene_fabricator_shared_ptr fabricator = std::make_shared<gb::scene_fabricator>();
		local_session_game_transition::set_fabricator(fabricator);
    }
    
    void local_session_game_transition::destroy_scene()
    {
        m_scene = nullptr;
    }
}
