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
    m_fabricator(nullptr),
    m_camera(nullptr)
    {
        ces_transformation_component_shared_ptr transformation_component = std::make_shared<ces_transformation_component>();
        ces_entity::add_component(transformation_component);
        transformation_component->set_z_order(0.f);
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
    
    void scene_graph::set_camera(const camera_shared_ptr& camera)
    {
        m_camera = camera;
    }
    
    camera_shared_ptr scene_graph::get_camera() const
    {
        return m_camera;
    }
    
    void scene_graph::updated_z_order_recursively(const ces_entity_shared_ptr& entity, f32 z_order)
    {
        ces_transformation_component_shared_ptr transformation_component = entity->get_component<ces_transformation_component>();
        if(transformation_component)
        {
            transformation_component->set_z_order(z_order);
        }
        std::list<ces_entity_shared_ptr> children = entity->children;
        for(const auto& child : children)
        {
            scene_graph::updated_z_order_recursively(child, z_order + ces_transformation_component::k_z_order_step);
        }
    }
    
    void scene_graph::add_child(const ces_entity_shared_ptr& child)
    {
        ces_entity::add_child(child);
        ces_transformation_component_shared_ptr transformation_component = child->get_component<ces_transformation_component>();
        f32 z_order = transformation_component->get_z_order();
        scene_graph::updated_z_order_recursively(child, z_order + ces_transformation_component::k_z_order_step);
    }
};
