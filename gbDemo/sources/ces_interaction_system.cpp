//
//  ces_interaction_system.h
//  gbDemo
//
//  Created by serhii serhiiv on 7/24/17.
//  Copyright © 2017 sergey.sergeev. All rights reserved.
//

#include "ces_interaction_system.h"
#include "ces_scene_state_automat_component.h"
#include "ces_car_statistic_component.h"
#include "ces_bound_touch_3d_component.h"
#include "ces_level_descriptor_component.h"
#include "game_object_2d.h"
#include "game_object_3d.h"
#include "camera_2d.h"
#include "camera_3d.h"
#include "glm_extensions.h"
#include "ces_box2d_body_component.h"
#include "ces_car_input_component.h"
#include "ces_car_model_component.h"
#include "ces_car_descriptor_component.h"
#include "glm_extensions.h"
#include "ces_level_route_component.h"

namespace game
{
    ces_interaction_system::ces_interaction_system()
    {
        ces_base_system::add_required_component_guid(m_level_components_mask, ces_level_descriptor_component::class_guid());
        ces_base_system::add_required_components_mask(m_level_components_mask);
        
        ces_base_system::add_required_component_guid(m_car_components_mask, ces_car_statistic_component::class_guid());
        ces_base_system::add_required_components_mask(m_car_components_mask);
    }
    
    void ces_interaction_system::on_feed_start(f32 dt)
    {
    }
    
    void ces_interaction_system::on_feed(const gb::ces_entity_shared_ptr& root, f32 dt)
    {
        ces_base_system::enumerate_entities_with_components(m_car_components_mask, [this](const gb::ces_entity_shared_ptr& entity) {
            std::string key = entity->tag;
            auto character_statistic_component = entity->get_component<ces_car_statistic_component>();
            if(character_statistic_component->mode == ces_car_statistic_component::e_mode::e_mode_ai)
            {
                m_ai_cars[key] = entity;
            }
            else if(character_statistic_component->mode == ces_car_statistic_component::e_mode::e_mode_player)
            {
                m_main_car = entity;
            }
            m_all_cars[key] = entity;
        });
        
        ces_base_system::enumerate_entities_with_components(m_level_components_mask, [this](const gb::ces_entity_shared_ptr& entity) {
            m_level = entity;
            ces_interaction_system::add_touch_recognition(entity, gb::e_input_state::e_input_state_pressed);
            ces_interaction_system::add_touch_recognition(entity, gb::e_input_state::e_input_state_released);
            ces_interaction_system::add_touch_recognition(entity, gb::e_input_state::e_input_state_dragged);
        });
        
        if (!m_main_car.expired())
        {
            const auto car = std::static_pointer_cast<gb::game_object_3d>(m_main_car.lock());
            const auto car_input_component = car->get_component<ces_car_input_component>();
            const auto car_descriptor_component = car->get_component<ces_car_descriptor_component>();
            if (car_input_component)
            {
                const auto car_model_component = car->get_component<ces_car_model_component>();
                
                car_input_component->updated = true;
                car_input_component->steer_angle = 0.f;
                car_input_component->brake = 0.f;
                
                
#if defined(__MANUAL_INPUT__)
                
                car_input_component->updated = false;
                car_input_component->throttle = 0.f;
                
#endif
                
                const auto& bound_touch_component = m_level.lock()->get_component<gb::ces_bound_touch_component>();
                const auto max_bound = bound_touch_component->as_3d()->get_max_bound();
                const auto min_bound = bound_touch_component->as_3d()->get_min_bound();
                
                glm::vec3 car_position = car->position;
                glm::vec3 car_rotation = car->rotation;
                
                const auto camera = ces_base_system::get_current_camera_3d();
                assert(camera != nullptr);
                glm::ray ray;
                glm::unproject(glm::ivec2(m_interaction_point.x, m_interaction_point.y), camera->get_mat_v(), camera->get_mat_p(), camera->get_viewport(), &ray);
                glm::vec3 intersected_point = glm::vec3(0.f);
                if (glm::plane_intersection(glm::vec3(min_bound.x, 0.f, max_bound.z),
                                            glm::vec3(max_bound.x, 0.f, max_bound.z),
                                            glm::vec3(max_bound.x, 0.f, min_bound.z),
                                            glm::vec3(min_bound.x, 0.f, min_bound.z),
                                            ray,
                                            &intersected_point))
                {
                    f32 motion_blur_effect_power = car_descriptor_component->motion_blur_effect_power;
                    
                    if (m_is_interacted)
                    {
                        f32 steer_angle = atan2(intersected_point.x - car_position.x, intersected_point.z - car_position.z);
                        steer_angle -= glm::wrap_radians(car_rotation.y);
                        
                        if (steer_angle < 0.f)
                        {
                            steer_angle += M_PI * 2.f;
                        }
                        
                        if (steer_angle > M_PI)
                        {
                            steer_angle -= M_PI * 2.f;
                        }
                        
                        car_input_component->throttle = car_model_component->get_max_force() * (1.f - glm::clamp(motion_blur_effect_power, 0.f, .25f));
                        car_input_component->steer_angle = steer_angle;
                        
                        
#if defined(__MANUAL_INPUT__)
                        
                        car_input_component->updated = true;
                        car_input_component->brake = 0.f;
                        f32 distance = glm::distance(glm::vec2(car_position.x, car_position.z), glm::vec2(intersected_point.x, intersected_point.z));
                        car_input_component->throttle = car_model_component->get_max_force() * (distance / 10.f);
                        
#endif
                        
                    }
                    else
                    {
                        
#if defined(__MANUAL_INPUT__)
                        
                        car_input_component->brake = 200.f;
                        
#else
                        car_input_component->throttle = car_model_component->get_max_force() * (1.f - glm::clamp(motion_blur_effect_power, 0.f, .25f));
                        car_input_component->steer_angle = 0.f;
                        car_input_component->updated = true;
                        car_input_component->brake = 0.f;
#endif
                        
                    }
                }
            }
        }
    }
    
    void ces_interaction_system::on_feed_end(f32 dt)
    {
        
    }
    
    void ces_interaction_system::add_touch_recognition(const gb::ces_entity_shared_ptr& entity,
                                                       gb::e_input_state input_state)
    {
        const auto& bound_touch_component = entity->get_component<gb::ces_bound_touch_component>();
        if(!bound_touch_component->is_respond_to(input_state, gb::e_input_source::e_input_source_mouse_left))
        {
            bound_touch_component->enable(input_state, gb::e_input_source_mouse_left, true);
            bound_touch_component->add_callback(input_state, std::bind(&ces_interaction_system::on_touched, this,
                                                                       std::placeholders::_1,
                                                                       std::placeholders::_2,
                                                                       std::placeholders::_3,
                                                                       std::placeholders::_4));
        }
    }
    
    void ces_interaction_system::on_touched(const gb::ces_entity_shared_ptr& entity, const glm::vec2& touch_point,
                                            gb::e_input_source input_source, gb::e_input_state input_state)
    {
        if(input_state == gb::e_input_state_dragged ||
           input_state == gb::e_input_state_pressed)
        {
            m_interaction_point = touch_point;
            m_is_interacted = true;
        }
        
        else if(input_state == gb::e_input_state_released)
        {
            m_is_interacted = false;
            
#if defined(__MANUAL_INPUT__)
            
            if (!m_main_car.expired())
            {
                const auto car = std::static_pointer_cast<gb::game_object_3d>(m_main_car.lock());
                const auto car_input_component = car->get_component<ces_car_input_component>();
                if (car_input_component)
                {
                    car_input_component->throttle = 0.f;
                    car_input_component->steer_angle = 0.f;
                    car_input_component->brake = 200.f;
                }
            }
            
#endif
        }
    }
}

