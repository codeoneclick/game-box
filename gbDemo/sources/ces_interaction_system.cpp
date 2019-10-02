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
#include "ces_car_drift_state_component.h"
#include "ces_car_camera_follow_component.h"
#include "ces_garage_database_component.h"
#include "ces_car_impact_component.h"
#include "ces_keyboard_listener_component.h"

#if defined(__OSX__)

#define __MANUAL_INPUT__ 1

#endif

namespace game
{
    ces_interaction_system::ces_interaction_system()
    {
        ces_base_system::add_required_component_guid(m_level_components_mask, ces_level_descriptor_component::class_guid());
        ces_base_system::add_required_components_mask(m_level_components_mask);
        
        ces_base_system::add_required_component_guid(m_car_components_mask, ces_car_statistic_component::class_guid());
        ces_base_system::add_required_components_mask(m_car_components_mask);
        
        ces_base_system::add_required_component_guid(m_camera_follow_car_components_mask, ces_car_camera_follow_component::class_guid());
        ces_base_system::add_required_components_mask(m_camera_follow_car_components_mask);
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
        
        ces_base_system::enumerate_entities_with_components(m_camera_follow_car_components_mask, [=](const gb::ces_entity_shared_ptr& entity) {
            m_camera_follow_car = entity;
        });
        
        ces_base_system::enumerate_entities_with_components(m_level_components_mask, [this](const gb::ces_entity_shared_ptr& entity) {
            m_level = entity;
            ces_interaction_system::add_touch_recognition(entity, gb::e_input_state::e_input_state_pressed);
            ces_interaction_system::add_touch_recognition(entity, gb::e_input_state::e_input_state_released);
            ces_interaction_system::add_touch_recognition(entity, gb::e_input_state::e_input_state_dragged);
        });
        
        bool is_keyboard_control = false;
        
        if (!m_main_car.expired())
        {
            const auto car = std::static_pointer_cast<gb::game_object_3d>(m_main_car.lock());
            const auto car_input_component = car->get_component<ces_car_input_component>();
            const auto car_descriptor_component = car->get_component<ces_car_descriptor_component>();
            const auto car_drift_state_component = car->get_component<ces_car_drift_state_component>();
            const auto car_impact_component= m_main_car.lock()->get_component<ces_car_impact_component>();
            
            const auto garage_database_component = root->get_component<ces_garage_database_component>();
            const auto selected_car = garage_database_component->get_selected_car(1);
            
            if (car_input_component)
            {
                const auto car_model_component = car->get_component<ces_car_model_component>();
                car_input_component->steer_angle = 0.f;
                car_input_component->brake = 0.f;
                car_input_component->throttle = 0.f;
                
                f32 steer_angle = 0.f;
                f32 handling = glm::mix(.33f, 1.f, selected_car->get_car_handling_upgrade());
                
                glm::vec3 car_position = car->position;
                glm::vec3 car_rotation = car->rotation;
                
                const auto& bound_touch_component = m_level.lock()->get_component<gb::ces_bound_touch_component>();
                const auto max_bound = bound_touch_component->as_3d()->get_max_bound();
                const auto min_bound = bound_touch_component->as_3d()->get_min_bound();
                
                const auto camera = ces_base_system::get_current_camera_3d();
                assert(camera != nullptr);
                const auto viewport = camera->get_viewport();
                f32 screen_width = viewport.z;
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
                    if (m_is_interacted
                        
#if defined(__IOS__)
                        && (m_interaction_point.x < screen_width * .2f ||
                            m_interaction_point.x > screen_width - (screen_width * .2f))
                        
#endif
                        
                        )
                    {
                        steer_angle = atan2(intersected_point.x - car_position.x, intersected_point.z - car_position.z);
                        steer_angle -= glm::wrap_radians(car_rotation.y);
                        
                        if (steer_angle < 0.f)
                        {
                            steer_angle += M_PI * 2.f;
                        }
                        
                        if (steer_angle > M_PI)
                        {
                            steer_angle -= M_PI * 2.f;
                        }
                        
                        f32 force = car_model_component->get_max_force() + (selected_car->get_car_speed_upgrade() * 100.f);
                        if (car_impact_component->is_speed_up_impact_exist())
                        {
                            force = glm::mix(force, force * car_impact_component->get_speed_up_max_impact(), .33f);
                        }
                        car_input_component->throttle = force;
                        car_input_component->steer_angle = steer_angle * handling;
                        car_input_component->brake = 0.f;
                    }
                    else
                    {
                        f32 force = car_model_component->get_max_force() + (selected_car->get_car_speed_upgrade() * 100.f);
                        if (car_impact_component->is_speed_up_impact_exist())
                        {
                            force = glm::mix(force, force * car_impact_component->get_speed_up_max_impact(), .33f);
                        }
                        
#if defined(__OSX__)
                        
                        force = 0.f;
#endif
                        
                        car_input_component->throttle = force;
                        car_input_component->steer_angle = 0.f;
                        car_input_component->brake = 0.f;
                    }
                }
                
                
                if (m_move_state == e_move_state::e_forward)
                {
                    f32 force = car_model_component->get_max_force() + (selected_car->get_car_speed_upgrade() * 100.f);
                    if (car_impact_component->is_speed_up_impact_exist())
                    {
                        force = glm::mix(force, force * car_impact_component->get_speed_up_max_impact(), .33f);
                    }
                    car_input_component->throttle = force;
                    car_input_component->brake = 0.f;
                }
                else if (m_move_state == e_move_state::e_backward)
                {
                    
                }
                
                if (m_steer_state == e_steer_state::e_right)
                {
                    steer_angle -= M_PI * 2.f;
                }
                else if (m_steer_state == e_steer_state::e_left)
                {
                    steer_angle += M_PI * 2.f;
                }
                
                car_input_component->steer_angle = steer_angle * handling;
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
        
#if defined(__OSX__)
        
        const auto& keyboard_listener_component = entity->get_component<gb::ces_keyboard_listener_component>();
        if (keyboard_listener_component && keyboard_listener_component->get_callbacks().size() == 0)
        {
            keyboard_listener_component->add_callback(std::bind(&ces_interaction_system::on_key, this,
                                                                std::placeholders::_1,
                                                                std::placeholders::_2,
                                                                std::placeholders::_3));
        }
        
#endif
        
    }
    
#define k_key_w 0x0D
#define k_key_s 0x01
#define k_key_a 0x00
#define k_key_d 0x02
    
    void ces_interaction_system::on_key(const gb::ces_entity_shared_ptr& entity,
                                        gb::e_input_state input_state,
                                        i32 key)
    {
        if (input_state == gb::e_input_state::e_input_state_pressed)
        {
            if (key == k_key_s)
            {
                m_move_state = e_move_state::e_backward;
            }
            
            if (key == k_key_w)
            {
                m_move_state = e_move_state::e_forward;
            }
            
            if (key == k_key_a)
            {
                m_steer_state = e_steer_state::e_left;
            }
            
            if (key == k_key_d)
            {
                m_steer_state = e_steer_state::e_right;
            }
        }
        else if (input_state == gb::e_input_state::e_input_state_released)
        {
            if (key == k_key_s)
            {
                m_move_state = e_move_state::e_none;
            }
            
            if (key == k_key_w)
            {
                m_move_state = e_move_state::e_none;
            }
            
            if (key == k_key_a)
            {
                m_steer_state = e_steer_state::e_none;
            }
            
            if (key == k_key_d)
            {
                m_steer_state = e_steer_state::e_none;
            }
        }
    }
    
    void ces_interaction_system::on_touched(const gb::ces_entity_shared_ptr& entity, const glm::vec2& touch_point,
                                            gb::e_input_source input_source, gb::e_input_state input_state)
    {
        if (!m_camera_follow_car.expired() && input_state == gb::e_input_state_dragged)
        {
            const auto car = std::static_pointer_cast<gb::game_object_3d>(m_camera_follow_car.lock());
            const auto car_camera_follow_component = car->get_component<ces_car_camera_follow_component>();
            if (car_camera_follow_component && car_camera_follow_component->preview_mode == ces_car_camera_follow_component::e_preview_mode::e_2)
            {
                const auto delta = touch_point - m_interaction_point;
                const auto camera_3d = get_current_camera_3d();
                auto rotation = camera_3d->get_rotation();
                rotation -= .5f * delta.x;
                camera_3d->set_rotation(rotation);
            }
        }
        
        if (input_state == gb::e_input_state_pressed)
        {
            m_first_interaction_point = touch_point;
        }
        else if (input_state == gb::e_input_state_released)
        {
            if (!m_main_car.expired())
            {
                const auto camera = ces_base_system::get_current_camera_3d();
                const auto viewport = camera->get_viewport();
                f32 screen_width = viewport.z;
                f32 screen_height = viewport.w;
                const auto delta = m_first_interaction_point - touch_point;
                if (abs(delta.y) > screen_height * .2f &&
                    touch_point.x > screen_width * .2f &&
                    touch_point.x < screen_width - (screen_width * .2f) &&
                    m_first_interaction_point.x > screen_width * .2f &&
                    m_first_interaction_point.x < screen_width - (screen_width * .2f))
                {
                    const auto car_impact_component = m_main_car.lock()->get_component<ces_car_impact_component>();
                    if (car_impact_component)
                    {
                        if (delta.y > 0.f)
                        {
                            car_impact_component->expect_to_speed_up_impact();
                        }
                        else
                        {
                            car_impact_component->expect_to_slow_motion_impact();
                        }
                    }
                }
            }
        }
        
        if (input_state == gb::e_input_state_dragged ||
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

