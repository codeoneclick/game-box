//
//  net_session_menu_transition.cpp
//  gbDemo
//
//  Created by serhii serhiiv on 4/12/17.
//  Copyright © 2017 sergey.sergeev. All rights reserved.
//

#include "net_session_menu_transition.h"
#include "net_session_menu_scene.h"
#include "scene_fabricator.h"

namespace game
{
    net_session_menu_transition::net_session_menu_transition(const std::string& guid, bool is_offscreen) :
    game_transition(guid, is_offscreen)
    {
        
    }
    
    net_session_menu_transition::~net_session_menu_transition(void)
    {
        
    }
    
    void net_session_menu_transition::create_scene()
    {
        m_scene = std::make_shared<net_session_menu_scene>(shared_from_this());
        gb::scene_fabricator_shared_ptr fabricator = std::make_shared<gb::scene_fabricator>();
        net_session_menu_transition::set_fabricator(fabricator);
    }
    
    void net_session_menu_transition::destroy_scene()
    {
        m_scene = nullptr;
    }
}
