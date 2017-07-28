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
#include "ces_character_parts_component.h"
#include "ces_character_state_automat_component.h"
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
#include "game_object_2d.h"

#include "ces_light_mask_component.h"
#include "ces_transformation_2d_component.h"
#include "ces_geometry_component.h"

#if !defined(__NO_RENDER__)

#include "information_bubble_controller.h"
#include "bloodprint_controller.h"
#include "footprint_controller.h"
#include "information_bubble.h"
#include "bloodprint.h"
#include "footprint.h"

#endif

namespace game
{
    ces_ai_system::ces_ai_system()
    {
        ces_base_system::add_required_component_guid(m_level_components_mask, ces_level_controllers_component::class_guid());
        ces_base_system::add_required_component_guid(m_level_components_mask, ces_level_path_grid_component::class_guid());
        ces_base_system::add_required_components_mask(m_level_components_mask);
        
        ces_base_system::add_required_component_guid(m_character_components_mask, ces_character_controllers_component::class_guid());
        ces_base_system::add_required_components_mask(m_character_components_mask);
    }
    
    ces_ai_system::~ces_ai_system()
    {
    }
    
    void ces_ai_system::on_feed_start(f32 dt)
    {
    }
    
    void ces_ai_system::on_feed(const gb::ces_entity_shared_ptr& root, f32 dt)
    {
        ces_base_system::enumerate_entities_with_components(m_character_components_mask, [this](const gb::ces_entity_shared_ptr& entity) {
            auto character_controllers_component = entity->get_component<ces_character_controllers_component>();
            std::string character_key = entity->tag;
            if(character_controllers_component->mode == ces_character_controllers_component::e_mode::e_mode_ai)
            {
                m_ai_characters[character_key] = entity;
            }
            else if(character_controllers_component->mode == ces_character_controllers_component::e_mode::e_mode_manual)
            {
                m_main_character = entity;
            }
            m_all_characters[character_key] = entity;
        });
        
        ces_base_system::enumerate_entities_with_components(m_level_components_mask, [this](const gb::ces_entity_shared_ptr& entity) {
            m_level = entity;
        });
        
        for(const auto& weak_entity : m_ai_characters)
        {
            if(!weak_entity.second.expired())
            {
                auto current_character = weak_entity.second.lock();
                auto current_character_statistic_component = current_character->get_component<ces_character_statistic_component>();
                if(!current_character_statistic_component->is_dead)
                {
                    auto character_state_automat_component = current_character->get_component<ces_character_state_automat_component>();
                    auto actions_processor = character_state_automat_component->get_actions_processor();
                    
                    if(!actions_processor->is_actions_exist() ||
                       actions_processor->top_action()->instance_guid() != ai_attack_action::class_guid())
                    {
                        for(auto opponent_character_weak : m_all_characters)
                        {
                            if(!opponent_character_weak.second.expired())
                            {
                                auto opponent_character = opponent_character_weak.second.lock();
                                auto opponent_character_statistic_component = opponent_character->get_component<ces_character_statistic_component>();
                                if(opponent_character != current_character &&
                                   !opponent_character_statistic_component->is_dead)
                                {
                                    auto current_character_transformation_component = current_character->get_component<gb::ces_transformation_2d_component>();
                                    auto opponent_character_transformation_component = opponent_character->get_component<gb::ces_transformation_2d_component>();
                                    
                                    glm::vec2 current_character_position = current_character_transformation_component->get_position();
                                    glm::vec2 opponent_character_position = opponent_character_transformation_component->get_position();
                                    f32 distance = glm::distance(current_character_position, opponent_character_position);
                                    
                                    if(distance <= current_character_statistic_component->current_attack_distance)
                                    {
                                        auto light_source_entity = current_character->get_child(ces_character_parts_component::parts::k_light_source_part, true);
                                        auto light_source_mesh = light_source_entity->get_component<gb::ces_light_mask_component>()->get_mesh();
                                        
                                        auto bounds_entity = opponent_character->get_child(ces_character_parts_component::parts::k_bounds_part, true);
                                        auto bounds_mesh = bounds_entity->get_component<gb::ces_geometry_component>()->get_mesh();
                                        
                                        if(light_source_mesh && bounds_mesh)
                                        {
                                            if(gb::mesh_2d::intersect(bounds_mesh->get_vbo(), bounds_mesh->get_ibo(), opponent_character_transformation_component->get_matrix_m(), true,
                                                                      light_source_mesh->get_vbo(), light_source_mesh->get_ibo(), glm::mat4(1.f), false))
                                            {
                                                actions_processor->interrupt_all_actions();
                                                auto attack_action = std::make_shared<ai_attack_action>(current_character);
                                                attack_action->set_parameters(std::static_pointer_cast<gb::game_object_2d>(opponent_character));
                                                attack_action->set_start_callback([](const ai_action_shared_ptr& action) {
                                                    auto character = action->get_owner();
                                                    const auto& character_state_automat_component = character->get_component<ces_character_state_automat_component>();
                                                    character_state_automat_component->set_state(game::ces_character_state_automat_component::e_state_attack);
                                                });
                                                attack_action->set_end_callback([](const ai_action_shared_ptr& action) {
                                                    auto character = action->get_owner();
                                                    const auto& character_state_automat_component = character->get_component<ces_character_state_automat_component>();
                                                    character_state_automat_component->set_state(game::ces_character_state_automat_component::e_state_idle);
                                                });
                                                
                                                actions_processor->add_action(attack_action);
                                                break;
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                    
                    if(!actions_processor->is_actions_exist() ||
                       (actions_processor->top_action()->instance_guid() != ai_chase_action::class_guid() &&
                        actions_processor->top_action()->instance_guid() != ai_attack_action::class_guid()))
                    {
                        for(auto opponent_character_weak : m_all_characters)
                        {
                            if(!opponent_character_weak.second.expired() && !m_level.expired())
                            {
                                auto opponent_character = opponent_character_weak.second.lock();
                                auto opponent_character_statistic_component = opponent_character->get_component<ces_character_statistic_component>();
                                if(current_character != opponent_character &&
                                   !opponent_character_statistic_component->is_dead)
                                {
                                    auto current_character_transformation_component = current_character->get_component<gb::ces_transformation_2d_component>();
                                    auto opponent_character_transformation_component = opponent_character->get_component<gb::ces_transformation_2d_component>();
                                    
                                    glm::vec2 current_character_position = current_character_transformation_component->get_position();
                                    glm::vec2 opponent_character_position = opponent_character_transformation_component->get_position();
                                    f32 distance = glm::distance(current_character_position, opponent_character_position);
                                    f32 chase_start_distance = current_character_statistic_component->current_chase_start_distance;
                                    
                                    if(distance <= chase_start_distance)
                                    {
                                        auto light_source_entity = current_character->get_child(ces_character_parts_component::parts::k_light_source_part, true);
                                        auto light_source_mesh = light_source_entity->get_component<gb::ces_light_mask_component>()->get_mesh();
                                        
                                        auto bounds_entity = opponent_character->get_child(ces_character_parts_component::parts::k_bounds_part, true);
                                        auto bounds_mesh = bounds_entity->get_component<gb::ces_geometry_component>()->get_mesh();
                                        
                                        if(light_source_mesh && bounds_mesh)
                                        {
                                            if(gb::mesh_2d::intersect(bounds_mesh->get_vbo(), bounds_mesh->get_ibo(), opponent_character_transformation_component->get_matrix_m(), true,
                                                                      light_source_mesh->get_vbo(), light_source_mesh->get_ibo(), glm::mat4(1.f), false))
                                            {
                                                auto character_pathfinder_component = current_character->get_component<ces_character_pathfinder_component>();
                                                auto pathfinder = character_pathfinder_component->get_pathfinder();
                                                
                                                auto level = m_level.lock();
                                                auto level_path_grid_component = level->get_component<ces_level_path_grid_component>();
                                                auto path_grid = level_path_grid_component->get_path_grid();
                                                
                                                actions_processor->interrupt_all_actions();
                                                ai_chase_action_shared_ptr chase_action = std::make_shared<ai_chase_action>(current_character);
                                                chase_action->set_parameters(std::static_pointer_cast<gb::game_object_2d>(opponent_character),
                                                                             path_grid);
                                                chase_action->set_start_callback([](const ai_action_shared_ptr& action) {
                                                    auto character = action->get_owner();
                                                    const auto& character_state_automat_component = character->get_component<ces_character_state_automat_component>();
                                                    character_state_automat_component->set_state(game::ces_character_state_automat_component::e_state_chase);
                                                });
                                                chase_action->set_end_callback([](const ai_action_shared_ptr& action) {
                                                    auto character = action->get_owner();
                                                    const auto& character_state_automat_component = character->get_component<ces_character_state_automat_component>();
                                                    character_state_automat_component->set_state(game::ces_character_state_automat_component::e_state_idle);
                                                });
                                                actions_processor->add_action(chase_action);
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                    
                    if(!actions_processor->is_actions_exist() && !m_level.expired())
                    {
                        auto level = m_level.lock();
                        auto level_path_grid_component = level->get_component<ces_level_path_grid_component>();
                        auto level_size = level_path_grid_component->get_level_size();
                        auto path_grid = level_path_grid_component->get_path_grid();
                        
                        auto character_pathfinder_component = current_character->get_component<ces_character_pathfinder_component>();
                        auto pathfinder = character_pathfinder_component->get_pathfinder();
                        
                        glm::vec2 goal_position;
                        goal_position.x = std::get_random_f(0.f, level_size.x - 1.f);
                        goal_position.y = std::get_random_f(0.f, level_size.y - 1.f);
                        
                        auto current_character_transformation_component = current_character->get_component<gb::ces_transformation_2d_component>();
                        glm::vec2 current_position = current_character_transformation_component->get_position();
                        
                        std::queue<glm::vec2> path = game::pathfinder::find_path(current_position, goal_position, pathfinder, path_grid);
                        ai_move_action_shared_ptr move_action = std::make_shared<ai_move_action>(current_character);
                        move_action->set_parameters(std::move(path));
                        move_action->set_start_callback([](const ai_action_shared_ptr& action) {
                            auto character = action->get_owner();
                            const auto& character_state_automat_component = character->get_component<ces_character_state_automat_component>();
                            character_state_automat_component->set_state(game::ces_character_state_automat_component::e_state_move);
                        });
                        move_action->set_end_callback([](const ai_action_shared_ptr& action) {
                            auto character = action->get_owner();
                            const auto& character_state_automat_component = character->get_component<ces_character_state_automat_component>();
                            character_state_automat_component->set_state(game::ces_character_state_automat_component::e_state_idle);
                        });
                        actions_processor->add_action(move_action);
                    }
                }
            }
        }
    }
    
    void ces_ai_system::on_feed_end(f32 dt)
    {
        
    }
}
