//
//  ces_ai_system.cpp
//  gbDemo
//
//  Created by serhii serhiiv on 3/9/17.
//  Copyright © 2017 sergey.sergeev. All rights reserved.
//

#include "ces_ai_system.h"
#include "ces_ai_component.h"
#include "ces_transformation_2d_component.h"
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
#include "ces_character_controller_component.h"
#include "ces_character_statistic_component.h"
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

#include "character.h"

#define k_camera_trashhold 64.f;

namespace game
{
    ces_ai_system::ces_ai_system() :
    m_path_map(nullptr),
    m_pathfinder(std::make_shared<pathfinder>())
    {
        
    }
    
    ces_ai_system::~ces_ai_system()
    {
        
    }
    
    void ces_ai_system::on_feed_start(f32 deltatime)
    {
        m_camera_2d_bounds = ces_base_system::get_current_camera_2d()->bound;
        m_camera_2d_bounds.x -= k_camera_trashhold;
        m_camera_2d_bounds.y -= k_camera_trashhold;
        m_camera_2d_bounds.z += k_camera_trashhold;
        m_camera_2d_bounds.w += k_camera_trashhold;
    }
    
    void ces_ai_system::on_feed(const gb::ces_entity_shared_ptr& entity, f32 deltatime)
    {
        ces_ai_system::update_recursively(entity, deltatime);
    }
    
    void ces_ai_system::on_feed_end(f32 deltatime)
    {
        
    }
    
    void ces_ai_system::update_recursively(const gb::ces_entity_shared_ptr& entity, f32 deltatime)
    {
        std::shared_ptr<ces_ai_component> ai_component = entity->get_component<ces_ai_component>();
        auto character_statistic_component = entity->get_component<ces_character_statistic_component>();
        
        if(ai_component && character_statistic_component && !character_statistic_component->is_dead)
        {
            glm::ivec4 map_bounds = glm::ivec4(0,
                                               0,
                                               m_path_map->get_size().x * m_path_map->get_cell_size().x,
                                               m_path_map->get_size().y * m_path_map->get_cell_size().y);
            ai_actions_processor_shared_ptr actions_processor = ai_component->actions_processor;
            actions_processor->update(deltatime);
            
            if(!actions_processor->is_actions_exist() || actions_processor->top_action()->instance_guid() != ai_attack_action::class_guid())
            {
                for(auto character_weak : m_all_characters)
                {
                    if(!character_weak.second.expired())
                    {
                        auto character = character_weak.second.lock();
                        auto character_statistic_component = character->get_component<ces_character_statistic_component>();
                        if(character != entity && m_main_character.lock() != entity && !character_statistic_component->is_dead)
                        {
                            auto executor_transformation_component = entity->get_component<gb::ces_transformation_2d_component>();
                            auto target_transformation_component = character->get_component<gb::ces_transformation_2d_component>();
                            
                            glm::vec2 executor_position = executor_transformation_component->get_position();
                            glm::vec2 target_position = target_transformation_component->get_position();
                            f32 distance_to_target = glm::distance(executor_position, target_position);
                            
                            if(distance_to_target <= 64.f)
                            {
                                gb::ces_entity_shared_ptr light_source_entity = entity->get_child(character::parts::k_light_source_part, true);
                                gb::mesh_2d_shared_ptr light_source_mesh = light_source_entity->get_component<gb::ces_light_mask_component>()->get_mesh();
                                
                                gb::ces_entity_shared_ptr bounds_entity = character->get_child(character::parts::k_bounds_part, true);
                                gb::mesh_2d_shared_ptr bounds_mesh = bounds_entity->get_component<gb::ces_geometry_component>()->get_mesh();
                                
                                if(light_source_mesh && bounds_mesh)
                                {
                                    if(gb::mesh_2d::intersect(bounds_mesh->get_vbo(), bounds_mesh->get_ibo(), target_transformation_component->get_matrix_m(), true,
                                                              light_source_mesh->get_vbo(), light_source_mesh->get_ibo(), glm::mat4(1.f), false))
                                    {
                                        actions_processor->interrupt_all_actions();
                                        ai_attack_action_shared_ptr attack_action = std::make_shared<ai_attack_action>();
                                        attack_action->set_parameters(std::static_pointer_cast<gb::game_object_2d>(entity), std::static_pointer_cast<gb::game_object_2d>(character),
                                                                      64.f, map_bounds);
                                        actions_processor->add_action(attack_action);
                                    }
                                }
                            }
                        }
                    }
                }
            }
            
            if(!actions_processor->is_actions_exist() || (actions_processor->top_action()->instance_guid() != ai_chase_action::class_guid() &&
                                                          actions_processor->top_action()->instance_guid() != ai_attack_action::class_guid()))
            {
                for(auto character_weak : m_all_characters)
                {
                    if(!character_weak.second.expired())
                    {
                        auto character = character_weak.second.lock();
                        auto character_statistic_component = character->get_component<ces_character_statistic_component>();
                        if(character != entity && m_main_character.lock() != entity && !character_statistic_component->is_dead)
                        {
                            auto executor_transformation_component = entity->get_component<gb::ces_transformation_2d_component>();
                            auto target_transformation_component = character->get_component<gb::ces_transformation_2d_component>();
                            
                            glm::vec2 executor_position = executor_transformation_component->get_position();
                            glm::vec2 target_position = target_transformation_component->get_position();
                            f32 distance_to_target = glm::distance(executor_position, target_position);
                            
                            if(distance_to_target <= 256.f)
                            {
                                gb::ces_entity_shared_ptr light_source_entity = entity->get_child(character::parts::k_light_source_part, true);
                                gb::mesh_2d_shared_ptr light_source_mesh = light_source_entity->get_component<gb::ces_light_mask_component>()->get_mesh();
                                
                                gb::ces_entity_shared_ptr bounds_entity = character->get_child(character::parts::k_bounds_part, true);
                                gb::mesh_2d_shared_ptr bounds_mesh = bounds_entity->get_component<gb::ces_geometry_component>()->get_mesh();
                                
                                if(light_source_mesh && bounds_mesh)
                                {
                                    if(gb::mesh_2d::intersect(bounds_mesh->get_vbo(), bounds_mesh->get_ibo(), target_transformation_component->get_matrix_m(), true,
                                                              light_source_mesh->get_vbo(), light_source_mesh->get_ibo(), glm::mat4(1.f), false))
                                    {
                                        actions_processor->interrupt_all_actions();
                                        ai_chase_action_shared_ptr chase_action = std::make_shared<ai_chase_action>();
                                        chase_action->set_parameters(std::static_pointer_cast<gb::game_object_2d>(entity),
                                                                     std::static_pointer_cast<gb::game_object_2d>(character),
                                                                     256.f,
                                                                     m_path_map,
                                                                     m_pathfinder);
                                        actions_processor->add_action(chase_action);
                                    }
                                }
                            }
                        }
                    }
                }
            }
            
            
            if(!actions_processor->is_actions_exist())
            {
                glm::ivec2 goal_position_index;
                goal_position_index.x = std::get_random_i(0, m_path_map->get_size().x - 1);
                goal_position_index.y = std::get_random_i(0, m_path_map->get_size().y - 1);
                
                auto transformation_component = entity->get_component<gb::ces_transformation_2d_component>();
                glm::vec2 current_position = transformation_component->get_position();
                glm::ivec2 current_position_index;
                current_position_index.x = std::max(0, std::min(static_cast<i32>(current_position.x / m_path_map->get_cell_size().x), m_path_map->get_size().x - 1));
                current_position_index.y = std::max(0, std::min(static_cast<i32>(current_position.y / m_path_map->get_cell_size().y), m_path_map->get_size().y - 1));
                
                std::vector<std::shared_ptr<astar_node>> path;
                m_pathfinder->set_start(m_path_map->get_path_node(current_position_index.x, current_position_index.y));
                m_pathfinder->set_goal(m_path_map->get_path_node(goal_position_index.x, goal_position_index.y));
                
                bool is_found = m_pathfinder->find_path(path);
                if(is_found && path.size() > 1)
                {
                    path.resize(path.size() - 1);
                    std::reverse(path.begin(), path.end());
                    
                    for(const auto& point : path)
                    {
                        ai_move_action_shared_ptr move_action = std::make_shared<ai_move_action>();
                        glm::vec2 goal_position;
                        goal_position.x = static_cast<f32>(point->get_x()) * static_cast<f32>(m_path_map->get_cell_size().x) + static_cast<f32>(m_path_map->get_cell_size().x) * .5f;
                        goal_position.y = static_cast<f32>(point->get_y()) * static_cast<f32>(m_path_map->get_cell_size().y) + static_cast<f32>(m_path_map->get_cell_size().y) * .5f;
                        move_action->set_parameters(std::static_pointer_cast<gb::game_object_2d>(entity), goal_position);
                        actions_processor->add_action(move_action);
                    }
                }
            }
        }
        
        auto character_controller_component = entity->get_component<ces_character_controller_component>();
        if(character_controller_component)
        {
            std::string character_key = entity->tag;
            if(character_controller_component->mode == ces_character_controller_component::e_mode::ai)
            {
                m_ai_characters[character_key] = entity;
            }
            else if(character_controller_component->mode == ces_character_controller_component::e_mode::main)
            {
                m_main_character = entity;
            }
            m_all_characters[character_key] = entity;
        }
        
        std::list<gb::ces_entity_shared_ptr> children = entity->children;
        for(const auto& child : children)
        {
            ces_ai_system::update_recursively(child, deltatime);
        }
    }
    
    void ces_ai_system::set_path_map(const path_map_shared_ptr& path_map)
    {
        m_path_map = path_map;
    }
}
