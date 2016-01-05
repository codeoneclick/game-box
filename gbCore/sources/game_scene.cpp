//
//  game_scene.cpp
//  gbCore
//
//  Created by sergey.sergeev on 8/27/15.
//  Copyright (c) 2015 sergey.sergeev. All rights reserved.
//

#include "game_scene.h"
#include "game_commands_container.h"

namespace gb
{
    game_scene::game_scene(const game_transition_shared_ptr& transition) :
    m_transition(transition),
    m_internal_commands(std::make_shared<game_commands_container>()),
    m_external_commands(nullptr)
    {
        
    }
    
    game_scene::~game_scene()
    {
        
    }
    
    game_transition_shared_ptr game_scene::get_transition() const
    {
        return m_transition.lock();
    }
    
    void game_scene::set_external_commands(const game_commands_container_shared_ptr& commands)
    {
        m_external_commands = commands;
    }
    
    game_commands_container_shared_ptr game_scene::get_internal_commands() const
    {
        return m_internal_commands;
    }
};