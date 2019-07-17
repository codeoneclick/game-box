//
//  ces_ai_system.cpp
//  gbDemo
//
//  Created by serhii serhiiv on 3/9/17.
//  Copyright © 2017 sergey.sergeev. All rights reserved.
//

#include "ces_ai_system.h"
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

namespace game
{
    ces_ai_system::ces_ai_system()
    {
        ces_base_system::add_required_component_guid(m_level_components_mask, ces_level_route_component::class_guid());
        ces_base_system::add_required_components_mask(m_level_components_mask);
        
        ces_base_system::add_required_component_guid(m_ai_cars_components_mask, ces_car_ai_input_component::class_guid());
        ces_base_system::add_required_components_mask(m_ai_cars_components_mask);
    }
    
    void ces_ai_system::on_feed_start(f32 dt)
    {
        
    }
    
    void ces_ai_system::on_feed(const gb::ces_entity_shared_ptr& root, f32 dt)
    {
        m_ai_cars.clear();
        
        ces_base_system::enumerate_entities_with_components(m_ai_cars_components_mask, [this](const gb::ces_entity_shared_ptr& entity) {
            std::string tag = entity->tag;
            m_ai_cars[tag] = entity;
        });
        
        ces_base_system::enumerate_entities_with_components(m_level_components_mask, [this](const gb::ces_entity_shared_ptr& entity) {
            m_level = entity;
        });
        
        if (!m_level.expired())
        {
            const auto level_route_component = m_level.lock()->get_component<ces_level_route_component>();
            std::vector<glm::vec2> route = level_route_component->route;
            
            std::map<i32, gb::ces_entity_weak_ptr> sorted_ai_cars;
            for (auto weak_car_ptr_it : m_ai_cars)
            {
                const auto weak_car_ptr = weak_car_ptr_it.second;
                if (!weak_car_ptr.expired())
                {
                    const auto car = std::static_pointer_cast<gb::game_object_3d>(weak_car_ptr.lock());
                    const auto car_descriptor_component = car->get_component<ces_car_descriptor_component>();
                    i32 place = car_descriptor_component->place;
                    sorted_ai_cars[place] = weak_car_ptr;
                }
            }
           
            for (auto weak_car_ptr_it : m_ai_cars)
            {
                const auto weak_car_ptr = weak_car_ptr_it.second;
                if (!weak_car_ptr.expired())
                {
                    const auto car = std::static_pointer_cast<gb::game_object_3d>(weak_car_ptr.lock());
                    const auto car_ai_input_component = car->get_component<ces_car_ai_input_component>();
                    const auto car_model_component = car->get_component<ces_car_model_component>();
                    const auto car_descriptor_component = car->get_component<ces_car_descriptor_component>();
                    glm::vec3 car_position = car->position;
                    glm::vec3 car_rotation = car->rotation;
                    f32 speed_multiplier = car_ai_input_component->speed_multiplier;
                    
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
                    
                    i32 near_checkpoint_index = (nearest_next_checkpoint_index + 1) % route.size();
                    i32 mid_checkpoint_index = (nearest_next_checkpoint_index + 2) % route.size();
                    i32 far_checkpoint_index = (nearest_next_checkpoint_index + 3) % route.size();
                    
                    auto near_checkpoint_position = route.at(near_checkpoint_index);
                    auto mid_checkpoint_position = route.at(mid_checkpoint_index);
                    auto far_checkpoint_position = route.at(far_checkpoint_index);
                    
                    auto offset = glm::cross(glm::normalize(glm::vec3(near_checkpoint_position.x - mid_checkpoint_position.x,
                                                                      0.f,
                                                                      near_checkpoint_position.y - mid_checkpoint_position.y)),
                                             glm::vec3(0.f,
                                                       1.f,
                                                       0.f)) * 3.f;
                    
                    ces_car_ai_input_component::e_follow_side follow_side = car_ai_input_component->follow_side;
                    if (follow_side == ces_car_ai_input_component::e_right)
                    {
                        offset *= -1.f;
                    }
                    else if (follow_side == ces_car_ai_input_component::e_undefined)
                    {
                        offset.x = 0.f;
                        offset.z = 0.f;
                    }
                    
                    near_checkpoint_position.x += offset.x;
                    near_checkpoint_position.y += offset.z;
                    
                    mid_checkpoint_position.x += offset.x;
                    mid_checkpoint_position.y += offset.z;
                    
                    auto next_position = near_checkpoint_position;
                    f32 additional_steer = 0.f;
                    auto corner_angle = glm::get_angle_abc(glm::vec2(car_position.x, car_position.z), next_position, mid_checkpoint_position);
                    if((corner_angle > 0.f && corner_angle < 90.f) ||
                       (corner_angle < 0.f && corner_angle > -90.f))
                    {
                        next_position = glm::mix(next_position, mid_checkpoint_position, fabsf(corner_angle) / (90.f * 1.5f));
                        speed_multiplier = glm::mix(.5f, speed_multiplier, fabsf(corner_angle) / 90.f);
                        additional_steer = glm::radians(90.f - fabsf(corner_angle));
                        additional_steer *= corner_angle > 0.f ? .33f : -.33f;
                    }
                    
                    i32 place = car_descriptor_component->place;
                    const auto in_front_car_it = sorted_ai_cars.find(place - 1);
                    if (in_front_car_it != sorted_ai_cars.end())
                    {
                        const auto in_front_car = std::static_pointer_cast<gb::game_object_3d>(in_front_car_it->second.lock());
                        glm::vec3 in_front_car_position = in_front_car->position;
                        f32 distance = glm::distance(in_front_car_position, car_position);
                        if (distance < 3.f)
                        {
                            speed_multiplier *= .1f;
                        }
                    }
                    
                    f32 steer_angle = atan2(next_position.x - car_position.x, next_position.y - car_position.z);
                    steer_angle -= glm::wrap_radians(car_rotation.y) - additional_steer;
                   
                    if (steer_angle < 0.f)
                    {
                        steer_angle += M_PI * 2.f;
                    }
                    
                    if (steer_angle > M_PI)
                    {
                        steer_angle -= M_PI * 2.f;
                    }
                    car_ai_input_component->updated = true;
                    car_ai_input_component->brake = (1.f - speed_multiplier) * 200.f;
                    f32 current_steer_angle = car_ai_input_component->steer_angle;
                    car_ai_input_component->steer_angle = glm::mix(current_steer_angle, steer_angle, .5f);
                    car_ai_input_component->throttle = car_model_component->get_max_force() * speed_multiplier;
                }
            }
        }
    }
    
    void ces_ai_system::on_feed_end(f32 dt)
    {
        
    }
}
