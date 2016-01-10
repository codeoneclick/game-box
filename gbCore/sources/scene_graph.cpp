//
//  scene_graph.cpp
//  gbCore
//
//  Created by sergey.sergeev on 8/27/15.
//  Copyright (c) 2015 sergey.sergeev. All rights reserved.
//

#include "scene_graph.h"
#include "game_commands_container.h"
#include "ces_transformation_component.h"
#include "ces_scene_component.h"

namespace gb
{
    scene_graph::scene_graph(const game_transition_shared_ptr& transition) :
    m_transition(transition),
    m_internal_commands(std::make_shared<game_commands_container>()),
    m_external_commands(nullptr),
    m_fabricator(nullptr)
    {
        ces_transformation_component_shared_ptr transformation_component = std::make_shared<ces_transformation_component>();
        ces_entity::add_component(transformation_component);
    }
    
    scene_graph::~scene_graph()
    {
        
    }
    
    void scene_graph::create()
    {
        ces_scene_component_shared_ptr scene_component = std::make_shared<ces_scene_component>();
        scene_component->set_scene(std::static_pointer_cast<scene_graph>(shared_from_this()));
        ces_entity::add_component(scene_component);
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