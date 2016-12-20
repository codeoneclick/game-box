//
//  client_menu_transition.cpp
//  gbDemo
//
//  Created by serhii serhiiv on 12/14/16.
//  Copyright © 2016 sergey.sergeev. All rights reserved.
//

#include "client_menu_transition.h"
#include "client_menu_scene.h"
#include "scene_fabricator.h"

namespace ns
{
    client_menu_transition::client_menu_transition(const std::string& guid, bool is_offscreen) :
    game_transition(guid, is_offscreen)
    {
        
    }
    
    client_menu_transition::~client_menu_transition(void)
    {
        
    }
    
    void client_menu_transition::create_scene()
    {
        m_scene = std::make_shared<client_menu_scene>(shared_from_this());
        gb::scene_fabricator_shared_ptr fabricator = std::make_shared<gb::scene_fabricator>();
        client_menu_transition::set_fabricator(fabricator);
    }
    
    void client_menu_transition::destroy_scene()
    {
        m_scene = nullptr;
    }
}
