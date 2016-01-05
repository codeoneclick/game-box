//
//  scene_graph.cpp
//  gbCore
//
//  Created by sergey.sergeev on 8/27/15.
//  Copyright (c) 2015 sergey.sergeev. All rights reserved.
//

#include "scene_graph.h"
#include "game_commands_container.h"

namespace gb
{
    scene_graph::scene_graph(const game_transition_shared_ptr& transition) :
    m_transition(transition),
    m_internal_commands(std::make_shared<game_commands_container>()),
    m_external_commands(nullptr),
    m_fabricator(nullptr)
    {
        
    }
    
    scene_graph::~scene_graph()
    {
        
    }
    
    game_transition_shared_ptr scene_graph::get_transition() const
    {
        return m_transition.lock();
    }
    
    void scene_graph::set_external_commands(const game_commands_container_shared_ptr& commands)
    {
        m_external_commands = commands;
    }
    
    game_commands_container_shared_ptr scene_graph::get_internal_commands() const
    {
        return m_internal_commands;
    }
    
    void scene_graph::set_fabricator(const scene_fabricator_shared_ptr& fabricator)
    {
        m_fabricator = fabricator;
    }
    
    scene_fabricator_shared_ptr scene_graph::get_fabricator() const
    {
        return m_fabricator;
    }
};