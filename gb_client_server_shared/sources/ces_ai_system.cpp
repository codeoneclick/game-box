//
//  ces_ai_system.cpp
//  gbDemo
//
//  Created by serhii serhiiv on 3/9/17.
//  Copyright © 2017 sergey.sergeev. All rights reserved.
//

#include "ces_ai_system.h"
#include "ces_transformation_2d_component.h"
#include "ces_level_controllers_component.h"
#include "ces_level_path_grid_component.h"
#include "ces_character_controllers_component.h"
#include "ces_character_statistic_component.h"
#include "ces_car_parts_component.h"
#include "ces_scene_state_automat_component.h"
#include "ces_character_pathfinder_component.h"
#include "ces_character_animation_component.h"
#include "std_extensions.h"
#include "glm_extensions.h"
#include "ces_geometry_extension.h"
#include "pathfinder.h"
#include "path_map.h"
#include "mesh_2d.h"
#include "vbo.h"
#include "ibo.h"
#include "camera_2d.h"
#include "ai_move_action.h"
#include "ai_attack_action.h"
#include "ai_chase_action.h"
#include "ai_actions_processor.h"
#include "game_object_3d.h"

#include "ces_light_mask_component.h"
#include "ces_transformation_2d_component.h"
#include "ces_geometry_component.h"
#include "ces_level_route_component.h"
#include "ces_ai_car_input_component.h"
#include "ces_car_model_component.h"

namespace game
{
    ces_ai_system::ces_ai_system()
    {
        ces_base_system::add_required_component_guid(m_track_components_mask, ces_level_route_component::class_guid());
        ces_base_system::add_required_components_mask(m_track_components_mask);
        
        ces_base_system::add_required_component_guid(m_ai_cars_components_mask, ces_ai_car_input_component::class_guid());
        ces_base_system::add_required_components_mask(m_ai_cars_components_mask);
    }
    
    void ces_ai_system::on_feed_start(f32 dt)
    {
        
    }
    
    void ces_ai_system::on_feed(const gb::ces_entity_shared_ptr& root, f32 dt)
    {
        ces_base_system::enumerate_entities_with_components(m_ai_cars_components_mask, [this](const gb::ces_entity_shared_ptr& entity) {
            std::string tag = entity->tag;
            m_ai_cars[tag] = entity;
        });
        
        ces_base_system::enumerate_entities_with_components(m_track_components_mask, [this](const gb::ces_entity_shared_ptr& entity) {
            m_track = entity;
        });
        
        if (!m_track.expired())
        {
            const auto track_route_component = m_track.lock()->get_component<ces_level_route_component>();
            std::vector<glm::vec2> route = track_route_component->route;
           
            for (auto weak_car_ptr_it : m_ai_cars)
            {
                const auto weak_car_ptr = weak_car_ptr_it.second;
                if (!weak_car_ptr.expired())
                {
                    const auto car = std::static_pointer_cast<gb::game_object_3d>(weak_car_ptr.lock());
                    const auto car_ai_input_component = car->get_component<ces_ai_car_input_component>();
                    const auto car_model_component = car->get_component<ces_car_model_component>();
                    glm::vec3 car_position = car->position;
                    glm::vec3 car_rotation = car->rotation;
                    
                    i32 nearest_next_checkpoint_index = 0;
                    f32 nearest_next_checkpoint_distance = glm::distance(glm::vec2(car_position.x, car_position.z), route.at(nearest_next_checkpoint_index));
                    
                    i32 index = 0;
                    for (auto route_it : route)
                    {
                        f32 distance = glm::distance(glm::vec2(car_position.x, car_position.z), route_it);
                        if (distance < nearest_next_checkpoint_distance)
                        {
                            nearest_next_checkpoint_distance = distance;
                            nearest_next_checkpoint_index = index;
                        }
                        index++;
                    }
                    
                    i32 nearest_checkpoint_index = (nearest_next_checkpoint_index + 1) % route.size();
                    nearest_next_checkpoint_index = (nearest_next_checkpoint_index + 2) % route.size();
                    const auto next_position = route.at(nearest_checkpoint_index);
                    auto goal_position = route.at(nearest_next_checkpoint_index);
                    f32 delta_angle = fabsf(glm::degrees(atan2(goal_position.x - next_position.x, goal_position.y - next_position.y)));
                    goal_position = glm::mix(next_position, goal_position, 1.f - (delta_angle / 360.f));
                   
                    f32 steer_angle = atan2(goal_position.x - car_position.x, goal_position.y - car_position.z);
                    steer_angle -= glm::wrap_radians(car_rotation.y);
                    f32 speed_multiply = 1.f;
                    
                    if (steer_angle < 0.f)
                    {
                        steer_angle += M_PI * 2.f;
                        speed_multiply = .5f;
                    }
                    
                    if (steer_angle > M_PI)
                    {
                        steer_angle -= M_PI * 2.f;
                        speed_multiply = .5f;
                    }
                    
                    f32 distance = glm::distance(glm::vec2(car_position.x, car_position.z), glm::vec2(goal_position.x, goal_position.y));
                    car_ai_input_component->updated = true;
                    car_ai_input_component->brake = (1.f - speed_multiply) * 200.f;
                    car_ai_input_component->steer_angle = steer_angle;
                    car_ai_input_component->throttle = (car_model_component->get_max_force() * (distance / 10.f)) * speed_multiply;
                }
            }
        }
    }
    
    void ces_ai_system::on_feed_end(f32 dt)
    {
        
    }
}
