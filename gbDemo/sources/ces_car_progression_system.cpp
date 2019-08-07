//
//  ces_car_progression_system.cpp
//  gbDemo
//
//  Created by serhii.s on 5/22/19.
//  Copyright © 2019 sergey.sergeev. All rights reserved.
//

#include "ces_car_progression_system.h"
#include "ces_transformation_2d_component.h"
#include "ces_car_parts_component.h"
#include "ces_scene_state_automat_component.h"
#include "std_extensions.h"
#include "glm_extensions.h"
#include "game_object_3d.h"
#include "ces_geometry_component.h"
#include "ces_level_route_component.h"
#include "ces_car_ai_input_component.h"
#include "ces_car_model_component.h"
#include "ces_car_descriptor_component.h"
#include "ces_car_input_component.h"
#include "ces_car_drift_state_component.h"
#include "ces_box2d_world_component.h"
#include "ces_level_descriptor_component.h"

namespace game
{
    ces_car_progression_system::ces_car_progression_system()
    {
        ces_base_system::add_required_component_guid(m_level_components_mask, ces_level_route_component::class_guid());
        ces_base_system::add_required_components_mask(m_level_components_mask);
        
        ces_base_system::add_required_component_guid(m_cars_components_mask, ces_car_descriptor_component::class_guid());
        ces_base_system::add_required_components_mask(m_cars_components_mask);
    }
    
    void ces_car_progression_system::on_feed_start(f32 dt)
    {
        
    }
    
    void ces_car_progression_system::on_feed(const gb::ces_entity_shared_ptr& root, f32 dt)
    {
        ces_base_system::enumerate_entities_with_components(m_cars_components_mask, [this](const gb::ces_entity_shared_ptr& entity) {
            std::string tag = entity->tag;
            m_cars[tag] = entity;
            if (entity->get_component<ces_car_input_component>())
            {
                m_main_car = entity;
            }
        });
        
        ces_base_system::enumerate_entities_with_components(m_level_components_mask, [this](const gb::ces_entity_shared_ptr& entity) {
            m_level = entity;
        });
        
        if (!m_level.expired())
        {
            const auto level_route_component = m_level.lock()->get_component<ces_level_route_component>();
            const auto level_descriptor_component = m_level.lock()->get_component<ces_level_descriptor_component>();
            std::vector<glm::vec2> route = level_route_component->route;
            std::list<gb::ces_entity_weak_ptr> sorted_cars;
            for (auto weak_car_ptr_it : m_cars)
            {
                const auto weak_car_ptr = weak_car_ptr_it.second;
                if (!weak_car_ptr.expired())
                {
                    const auto car = std::static_pointer_cast<gb::game_object_3d>(weak_car_ptr.lock());
                    const auto car_descriptor_component = car->get_component<ces_car_descriptor_component>();
                    glm::vec3 car_position = car->position;
                    
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
                    
                    nearest_next_checkpoint_index = nearest_next_checkpoint_index % route.size();
                    i32 nearest_next_next_checkpoint_index = (nearest_next_checkpoint_index + 1) % route.size();
                    i32 checkpoint_passed_index = car_descriptor_component->checkpoint_passed_index;
                    glm::vec2 goal_position = route.at(nearest_next_checkpoint_index);
                    glm::vec2 next_goal_position = route.at(nearest_next_next_checkpoint_index);
                    
                    if (m_main_car.lock() == car)
                    {
                        std::vector<glm::vec2> slow_motion_triggers = level_route_component->slow_motion_triggers;
                        if (slow_motion_triggers.size() > 0)
                        {
                            i32 index = 0;
                            i32 nearest_slow_motion_trigger_index = 0;
                            f32 nearest_slow_motion_trigger_distance = glm::distance(glm::vec2(car_position.x, car_position.z), slow_motion_triggers.at(nearest_slow_motion_trigger_index));
                            for (auto slow_motion_trigger_it : slow_motion_triggers)
                            {
                                f32 distance = glm::distance(glm::vec2(car_position.x, car_position.z), slow_motion_trigger_it);
                                if (distance < nearest_slow_motion_trigger_distance)
                                {
                                    nearest_slow_motion_trigger_distance = distance;
                                    nearest_slow_motion_trigger_index = index;
                                }
                                index++;
                            }
                            car_descriptor_component->nearest_slow_motion_trigger_index = nearest_slow_motion_trigger_index;
                        }
                        
                        std::vector<glm::vec2> speed_up_triggers = level_route_component->speed_up_triggers;
                        if (speed_up_triggers.size() > 0)
                        {
                            index = 0;
                            i32 nearest_speed_up_trigger_index = 0;
                            f32 nearest_speed_up_trigger_distance = glm::distance(glm::vec2(car_position.x, car_position.z), speed_up_triggers.at(nearest_speed_up_trigger_index));
                            for (auto speed_up_trigger_it : speed_up_triggers)
                            {
                                f32 distance = glm::distance(glm::vec2(car_position.x, car_position.z), speed_up_trigger_it);
                                if (distance < nearest_speed_up_trigger_distance)
                                {
                                    nearest_speed_up_trigger_distance = distance;
                                    nearest_speed_up_trigger_index = index;
                                }
                                index++;
                            }
                            car_descriptor_component->nearest_speed_up_trigger_index = nearest_speed_up_trigger_index;
                        }
                    }
                    
                    if (checkpoint_passed_index != nearest_next_checkpoint_index)
                    {
                        if (nearest_next_checkpoint_index < checkpoint_passed_index && checkpoint_passed_index == route.size() - 1)
                        {
                            i32 lap_passed_index = car_descriptor_component->lap_passed_index;
                            lap_passed_index++;
                            car_descriptor_component->lap_passed_index = lap_passed_index;
                        }
                        car_descriptor_component->checkpoint_passed_index = nearest_next_checkpoint_index;
                        car_descriptor_component->distance_to_next_checkpoint = glm::distance(glm::vec2(car_position.x, car_position.z), goal_position);
                    }
                    
                    const auto car_drift_state_component = car->get_component<ces_car_drift_state_component>();
                    if (car_drift_state_component->is_drifting)
                    {
                        f32 last_drifring_time = car_drift_state_component->last_drifting_time;
                        f32 current_time = std::get_tick_count();
                        f32 delta = current_time - last_drifring_time;
                        car_drift_state_component->session_drifting_time = delta;
                    }
                    else
                    {
                        if (!car_drift_state_component->is_interrupted)
                        {
                            car_drift_state_component->apply_session_drifting_time_to_total();
                        }
                    }
                    sorted_cars.push_back(car);
                }
            }
            
            sorted_cars.sort([](const gb::ces_entity_weak_ptr& car_a, const gb::ces_entity_weak_ptr& car_b) {
                const auto car_descriptor_component_a = car_a.lock()->get_component<ces_car_descriptor_component>();
                const auto car_descriptor_component_b = car_b.lock()->get_component<ces_car_descriptor_component>();
                i32 lap_passed_index_a = car_descriptor_component_a->lap_passed_index;
                i32 checkpoint_passed_index_a = car_descriptor_component_a->checkpoint_passed_index;
                i32 lap_passed_index_b = car_descriptor_component_b->lap_passed_index;
                i32 checkpoint_passed_index_b = car_descriptor_component_b->checkpoint_passed_index;
                f32 distance_to_next_checkpoint_a = car_descriptor_component_a->distance_to_next_checkpoint;
                f32 distance_to_next_checkpoint_b = car_descriptor_component_b->distance_to_next_checkpoint;
                if (lap_passed_index_a != lap_passed_index_b)
                {
                    return lap_passed_index_a > lap_passed_index_b;
                }
                if (checkpoint_passed_index_a != checkpoint_passed_index_b)
                {
                    return checkpoint_passed_index_a > checkpoint_passed_index_b;
                }
                else
                {
                    return distance_to_next_checkpoint_a < distance_to_next_checkpoint_b;
                }
            });
            
            i32 place_index = 0;
            for (auto car_it : sorted_cars)
            {
                const auto car_descriptor_component = car_it.lock()->get_component<ces_car_descriptor_component>();
                car_descriptor_component->place = place_index + 1;
                place_index++;
            }
            
            if (!m_main_car.expired())
            {
                const auto main_car_descriptor_component = m_main_car.lock()->get_component<ces_car_descriptor_component>();
                for (auto weak_car_ptr_it : m_cars)
                {
                    const auto weak_car_ptr = weak_car_ptr_it.second;
                    if (!weak_car_ptr.expired())
                    {
                        const auto car = std::static_pointer_cast<gb::game_object_3d>(weak_car_ptr.lock());
                        if (car != m_main_car.lock())
                        {
                            const auto ai_car_descriptor_component = car->get_component<ces_car_descriptor_component>();
                            const auto ai_input_component = car->get_component<ces_car_ai_input_component>();
                            f32 complexity = level_descriptor_component->complexity;
                            if (main_car_descriptor_component->place < ai_car_descriptor_component->place)
                            {
                                ai_input_component->speed_multiplier = std::get_random_f(glm::mix(1.0f, 1.5f, complexity), glm::mix(1.2f, 2.f, complexity));
                            }
                            else
                            {
                                ai_input_component->speed_multiplier = std::get_random_f(glm::mix(.5f, .8f, complexity), glm::mix(.75f, 1.2f, complexity));
                            }
                        }
                    }
                }
            }
        }
    }
    
    void ces_car_progression_system::on_feed_end(f32 dt)
    {
        
    }
}
