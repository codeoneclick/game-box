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
#include "ces_transformation_3d_component.h"
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
        const auto trail = m_scene_fabricator.lock()->create_trail(filename);
        const auto trail_component = trail->get_component<ces_trail_component>();
        auto position = start_position;
        position.y += .4f;
        trail_component->set_last_segment_position(position);
        trail_component->set_new_segment_position(position);
    
        const auto transformation_component = m_layer.lock()->get_component<ces_transformation_component>()->as_3d();
        auto angle = transformation_component->get_rotation().y;
        angle = glm::radians(angle);
        glm::vec3 start_direction;
        start_direction.x = cosf(-glm::wrap_radians(angle - M_PI_2));
        start_direction.y = 0.f;
        start_direction.z = sinf(-glm::wrap_radians(angle - M_PI_2));
        trail_component->set_start_segment_direction(start_direction);
        
        m_layer.lock()->add_child(trail);
        m_trails.push_back(trail);
    }
    
    void trail_controller::update(const gb::ces_entity_shared_ptr& entity, f32 deltatime)
    {
        gb::ces_entity_shared_ptr current_parent = parent;
        const auto transformation_component = current_parent->get_component<ces_transformation_component>()->as_3d();
        for (auto trail : m_trails)
        {
            const auto trail_component = trail.lock()->get_component<ces_trail_component>();
            auto position = transformation_component->get_absolute_position();
            position.y += .4f;
            trail_component->set_new_segment_position(position);
            auto angle = transformation_component->get_rotation().y;
            angle = glm::radians(angle);
            glm::vec3 start_direction;
            start_direction.x = cosf(-glm::wrap_radians(angle - M_PI_2));
            start_direction.y = 0.f;
            start_direction.z = sinf(-glm::wrap_radians(angle - M_PI_2));
            trail_component->set_start_segment_direction(start_direction);
        }
    }
}
