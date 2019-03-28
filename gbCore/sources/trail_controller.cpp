//
//  trail_controller.cpp
//  gbCore
//
//  Created by serhii.s on 3/28/19.
//  Copyright © 2019 sergey.sergeev. All rights reserved.
//

#include "trail_controller.h"
#include "ces_action_component.h"
#include "scene_fabricator.h"
#include "ces_trail_component.h"
#include "ces_geometry_3d_component.h"
#include "mesh_3d.h"
#include "trail.h"

namespace gb
{
    trail_controller::trail_controller(const gb::ces_entity_shared_ptr& layer,
                                       const gb::scene_fabricator_shared_ptr& scene_fabricator)
    {
        m_layer = layer;
        m_scene_fabricator = scene_fabricator;
        ces_entity::add_deferred_component_constructor<gb::ces_action_component>();
    }
    
    trail_controller::~trail_controller()
    {
        
    }
    
    void trail_controller::setup_components()
    {
        const auto action_component = ces_entity::get_component<gb::ces_action_component>();
        action_component->set_update_callback(std::bind(&trail_controller::update, this,
                                                        std::placeholders::_1, std::placeholders::_2));
    }
    
    void trail_controller::push_trail(const std::string& filename,
                                      const glm::vec3& start_position)
    {
        
    }
    
    void trail_controller::update(const gb::ces_entity_shared_ptr& entity, f32 deltatime)
    {
        
    }
}
