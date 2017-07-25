//
//  ces_interaction_system.h
//  gbDemo
//
//  Created by serhii serhiiv on 7/24/17.
//  Copyright © 2017 sergey.sergeev. All rights reserved.
//

#include "ces_interaction_system.h"
#include "ces_character_controllers_component.h"
#include "ces_character_state_automat_component.h"
#include "ces_character_statistic_component.h"
#include "ces_character_pathfinder_component.h"
#include "ces_character_selector_component.h"
#include "ces_character_animation_component.h"
#include "ces_level_controllers_component.h"
#include "ces_level_path_grid_component.h"
#include "ces_bound_touch_component.h"
#include "game_object_2d.h"
#include "ai_actions_processor.h"
#include "camera_2d.h"
#include "pathfinder.h"
#include "path_map.h"
#include "ai_move_action.h"
#include "footprint_controller.h"

namespace game
{
    ces_interaction_system::ces_interaction_system()
    {
        ces_base_system::add_required_component_guid(m_level_components_mask, ces_level_controllers_component::class_guid());
        ces_base_system::add_required_component_guid(m_level_components_mask, ces_level_path_grid_component::class_guid());
        ces_base_system::add_required_components_mask(m_level_components_mask);
        
        ces_base_system::add_required_component_guid(m_character_components_mask, ces_character_controllers_component::class_guid());
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
            auto character_component = entity->get_component<ces_character_controllers_component>();
            if(character_component->mode == ces_character_controllers_component::e_mode::e_mode_ai)
            {
                m_ai_characters[character_key] = entity;
            }
            else if(character_component->mode == ces_character_controllers_component::e_mode::e_mode_manual)
            {
                m_main_character = entity;
            }
            m_all_characters[character_key] = entity;
            ces_interaction_system::add_touch_recognition(entity, gb::e_input_state::e_input_state_pressed);
            ces_interaction_system::add_touch_recognition(entity, gb::e_input_state::e_input_state_released);
        });
        
        ces_base_system::enumerate_entities_with_components(m_level_components_mask, [this](const gb::ces_entity_shared_ptr& entity) {
            m_level = entity;
            ces_interaction_system::add_touch_recognition(entity, gb::e_input_state::e_input_state_pressed);
            ces_interaction_system::add_touch_recognition(entity, gb::e_input_state::e_input_state_released);
        });
        
        if(!m_main_character.expired() && !m_level.expired())
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
        if(entity == m_level.lock() && input_state == gb::e_input_state_released)
        {
            if(!m_main_character.expired() && !m_level.expired())
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
                    while(!path.empty())
                    {
                        ai_move_action_shared_ptr move_action = std::make_shared<ai_move_action>(character);
                        move_action->set_parameters(std::static_pointer_cast<gb::game_object_2d>(character),
                                                    path.front());
                        move_action->set_start_callback([](const ai_action_shared_ptr& action) {
                            auto character = action->get_owner();
                            auto character_animation_component = character->get_component<ces_character_animation_component>();
                            character_animation_component->play_animation(ces_character_animation_component::animations::k_walk_animation, true);
                        });
                        move_action->set_in_progress_callback([](const ai_action_shared_ptr& action) {
                            auto character = action->get_owner();
                            auto character_controllers_component = character->get_component<ces_character_controllers_component>();
                            footprint_controller_shared_ptr footprint_controller = character_controllers_component->footprint_controller;
                            if(footprint_controller->is_ready_to_push_footprint())
                            {
                                footprint_controller->push_footprint(glm::u8vec4(255, 255, 255, 255),
                                                                     std::static_pointer_cast<gb::game_object_2d>(character)->position,
                                                                     std::static_pointer_cast<gb::game_object_2d>(character)->rotation);
                            }
                        });
                        move_action->set_end_callback([](const ai_action_shared_ptr& action) {
                            auto character = action->get_owner();
                            auto character_animation_component = character->get_component<ces_character_animation_component>();
                            character_animation_component->play_animation(ces_character_animation_component::animations::k_idle_animation, true);
                        });
                        actions_processor->add_action(move_action);
                        path.pop();
                    }
                }
            }
        }
    }
}

