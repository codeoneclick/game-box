//
//  ces_interaction_system.h
//  gbDemo
//
//  Created by serhii serhiiv on 7/24/17.
//  Copyright © 2017 sergey.sergeev. All rights reserved.
//

#include "ces_interaction_system.h"
#include "ces_character_controllers_component.h"
#include "ces_scene_state_automat_component.h"
#include "ces_character_statistic_component.h"
#include "ces_character_pathfinder_component.h"
#include "ces_character_selector_component.h"
#include "ces_character_animation_component.h"
#include "ces_level_controllers_component.h"
#include "ces_level_path_grid_component.h"
#include "ces_bound_touch_3d_component.h"
#include "game_object_2d.h"
#include "game_object_3d.h"
#include "ai_actions_processor.h"
#include "camera_2d.h"
#include "camera_3d.h"
#include "pathfinder.h"
#include "path_map.h"
#include "ai_move_action.h"
#include "footprint_controller.h"
#include "glm_extensions.h"
#include "ces_box2d_body_component.h"
#include "ces_car_input_component.h"
#include "ces_car_model_component.h"
#include "glm_extensions.h"

namespace game
{
    ces_interaction_system::ces_interaction_system()
    {
        ces_base_system::add_required_component_guid(m_level_components_mask, ces_level_controllers_component::class_guid());
        //ces_base_system::add_required_component_guid(m_level_components_mask, ces_level_path_grid_component::class_guid());
        ces_base_system::add_required_components_mask(m_level_components_mask);
        
        ces_base_system::add_required_component_guid(m_character_components_mask, ces_character_controllers_component::class_guid());
        ces_base_system::add_required_component_guid(m_character_components_mask, ces_character_statistic_component::class_guid());
        ces_base_system::add_required_component_guid(m_character_components_mask, ces_car_input_component::class_guid());
        ces_base_system::add_required_components_mask(m_character_components_mask);
    }
    
    ces_interaction_system::~ces_interaction_system()
    {
    }
    
    void ces_interaction_system::on_feed_start(f32 dt)
    {
    }
    
    void ces_interaction_system::on_feed(const gb::ces_entity_shared_ptr& root, f32 dt)
    {
        ces_base_system::enumerate_entities_with_components(m_character_components_mask, [this](const gb::ces_entity_shared_ptr& entity) {
            std::string character_key = entity->tag;
            auto character_statistic_component = entity->get_component<ces_character_statistic_component>();
            if(character_statistic_component->mode == ces_character_statistic_component::e_mode::e_mode_mob)
            {
                m_ai_characters[character_key] = entity;
            }
            else if(character_statistic_component->mode == ces_character_statistic_component::e_mode::e_mode_npc)
            {
                m_npc_characters[character_key] = entity;
            }
            else if(character_statistic_component->mode == ces_character_statistic_component::e_mode::e_mode_player)
            {
                m_main_character = entity;
            }
            m_all_characters[character_key] = entity;
            //ces_interaction_system::add_touch_recognition(entity, gb::e_input_state::e_input_state_pressed);
            //ces_interaction_system::add_touch_recognition(entity, gb::e_input_state::e_input_state_released);
        });
        
        ces_base_system::enumerate_entities_with_components(m_level_components_mask, [this](const gb::ces_entity_shared_ptr& entity) {
            m_level = entity;
            ces_interaction_system::add_touch_recognition(entity, gb::e_input_state::e_input_state_pressed);
            ces_interaction_system::add_touch_recognition(entity, gb::e_input_state::e_input_state_released);
            ces_interaction_system::add_touch_recognition(entity, gb::e_input_state::e_input_state_dragged);
        });
        
        if (!m_main_character.expired())
        {
            const auto car = std::static_pointer_cast<gb::game_object_3d>(m_main_character.lock());
            const auto car_input_component = car->get_component<ces_car_input_component>();
            const auto car_model_component = car->get_component<ces_car_model_component>();
            
            car_input_component->updated = true;
            car_input_component->steer_angle = 0.f;
            car_input_component->brake = 0.f;
            
            
#if defined(__OSX__)
            
            car_input_component->updated = false;
            car_input_component->throttle = 0.f;
            
#endif
            
            const auto& bound_touch_component = m_level.lock()->get_component<gb::ces_bound_touch_component>();
            const auto max_bound = bound_touch_component->as_3d()->get_max_bound();
            const auto min_bound = bound_touch_component->as_3d()->get_min_bound();
            
            const auto camera = ces_base_system::get_current_camera_3d();
            assert(camera != nullptr);
            glm::ray ray;
            glm::unproject(glm::ivec2(m_touch_point.x, m_touch_point.y), camera->get_mat_v(), camera->get_mat_p(), camera->get_viewport(), &ray);
            glm::vec3 intersected_point = glm::vec3(0.f);
            if (glm::plane_intersection(glm::vec3(min_bound.x, 0.f, max_bound.z),
                                        glm::vec3(max_bound.x, 0.f, max_bound.z),
                                        glm::vec3(max_bound.x, 0.f, min_bound.z),
                                        glm::vec3(min_bound.x, 0.f, min_bound.z),
                                        ray,
                                        &intersected_point))
            {
                if (m_should_move)
                {
                    glm::vec3 car_position = car->position;
                    glm::vec3 car_rotation = car->rotation;
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
                    
                    f32 distance = glm::distance(glm::vec2(car_position.x, car_position.z), glm::vec2(intersected_point.x, intersected_point.z));
                    car_input_component->throttle = car_model_component->get_max_force();
                    car_input_component->steer_angle = steer_angle;
                    
#if defined(__OSX__)
                    
                    car_input_component->updated = true;
                    car_input_component->brake = 0.f;
                    car_input_component->throttle = car_model_component->get_max_force() * (distance / 10.f);
                    if (m_previous_distance > distance && fabsf(steer_angle) < M_PI_4)
                    {
                        f32 current_brake = car_input_component->brake;
                        current_brake +=  car_model_component->get_max_force() * .5f;
                        car_input_component->brake = current_brake;
                    }
                    
#endif
                    
                    m_previous_distance = distance;
                }
                else
                {
                    
#if defined(__OSX__)
                    
                    car_input_component->brake = 200.f;
                    
#else
                    
                    car_input_component->throttle = car_model_component->get_max_force() * .75f;
                    
#endif
                    
                }
            }
        }
        
        /*if(!m_main_character.expired() && !m_level.expired())
         {
         auto character = m_main_character.lock();
         auto level = m_level.lock();
         auto camera_2d = ces_base_system::get_current_camera_2d();
         glm::vec2 character_position = std::static_pointer_cast<gb::game_object_2d>(character)->position;
         glm::ivec2 level_size = level->get_component<ces_level_path_grid_component>()->get_level_size();
         
         glm::vec2 camera_position = camera_2d->get_position();
         camera_position = glm::mix(camera_position, character_position * -1.f, .5f);
         glm::ivec2 viewport_size = camera_2d->viewport_size;
         glm::vec2 camera_pivot = camera_2d->pivot;
         camera_position = glm::clamp(camera_position,
         glm::vec2(-level_size.x + static_cast<f32>(viewport_size.x) * camera_pivot.x,
         -level_size.y + static_cast<f32>(viewport_size.y) * camera_pivot.y),
         glm::vec2(static_cast<f32>(viewport_size.x) * -camera_pivot.x,
         static_cast<f32>(viewport_size.y) * -camera_pivot.y));
         camera_2d->set_position(camera_position);
         }*/
        
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
        if(/*entity == m_level.lock() &&*/ input_state == gb::e_input_state_dragged ||
           input_state == gb::e_input_state_pressed)
        {
            /*if(!m_main_character.expired() && !m_level.expired())
             {
             auto camera_2d = ces_base_system::get_current_camera_2d();
             glm::ivec2 viewport_size = camera_2d->viewport_size;
             glm::vec2 camera_pivot = camera_2d->pivot;
             glm::vec2 position = camera_2d->get_position();
             glm::vec2 offset = position + glm::vec2(viewport_size.x * camera_pivot.x,
             viewport_size.y * camera_pivot.y);
             glm::vec2 end_position = touch_point - offset;
             
             auto character = m_main_character.lock();
             const auto& character_statistic_component = character->get_component<ces_character_statistic_component>();
             if(!character_statistic_component->is_dead)
             {
             auto character_selector_component = character->get_component<ces_character_selector_component>();
             character_selector_component->remove_all_selections();
             auto level = m_level.lock();
             const auto& level_path_grid_component = level->get_component<ces_level_path_grid_component>();
             auto path_grid = level_path_grid_component->get_path_grid();
             const auto& character_pathfinder_component = character->get_component<ces_character_pathfinder_component>();
             auto pathfinder = character_pathfinder_component->get_pathfinder();
             const auto& character_state_automat_component = character->get_component<ces_character_state_automat_component>();
             const auto& actions_processor = character_state_automat_component->get_actions_processor();
             actions_processor->interrupt_all_actions();
             glm::vec2 start_position = std::static_pointer_cast<gb::game_object_2d>(character)->position;
             std::queue<glm::vec2> path = game::pathfinder::find_path(start_position, end_position,
             pathfinder, path_grid);
             ai_move_action_shared_ptr move_action = std::make_shared<ai_move_action>(character);
             move_action->set_parameters(std::move(path));
             move_action->set_start_callback([](const ai_action_shared_ptr& action) {
             auto character = action->get_owner();
             const auto& character_state_automat_component = character->get_component<ces_character_state_automat_component>();
             character_state_automat_component->set_state(game::ces_character_state_automat_component::e_state_move);
             });
             move_action->set_in_progress_callback([](const ai_action_shared_ptr& action) {
             });
             move_action->set_end_callback([](const ai_action_shared_ptr& action) {
             auto character = action->get_owner();
             const auto& character_state_automat_component = character->get_component<ces_character_state_automat_component>();
             character_state_automat_component->set_state(game::ces_character_state_automat_component::e_state_idle);
             });
             actions_processor->add_action(move_action);
             }
             }*/
            m_touch_point = touch_point;
            m_should_move = true;
        }
        
        else if(input_state == gb::e_input_state_released)
        {
            m_should_move = false;
            
#if defined(__OSX__)
            
            if (!m_main_character.expired())
            {
                const auto car = std::static_pointer_cast<gb::game_object_3d>(m_main_character.lock());
                const auto car_input_component = car->get_component<ces_car_input_component>();
                
                car_input_component->throttle = 0.f;
                car_input_component->steer_angle = 0.f;
                car_input_component->brake = 200.f;
            }
            
#endif
            
            /*for(auto it : m_npc_characters)
             {
             if(it.second.lock() == entity)
             {
             auto character_selector_component = m_main_character.lock()->get_component<ces_character_selector_component>();
             character_selector_component->remove_all_selections();
             character_selector_component->add_selection(entity);
             }
             }
             
             for(auto it : m_ai_characters)
             {
             if(it.second.lock() == entity)
             {
             auto character_selector_component = m_main_character.lock()->get_component<ces_character_selector_component>();
             character_selector_component->remove_all_selections();
             character_selector_component->add_selection(entity);
             }
             }*/
        }
    }
}

