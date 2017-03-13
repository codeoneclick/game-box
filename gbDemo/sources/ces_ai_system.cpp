//
//  ces_ai_system.cpp
//  gbDemo
//
//  Created by serhii serhiiv on 3/9/17.
//  Copyright © 2017 sergey.sergeev. All rights reserved.
//

#include "ces_ai_system.h"
#include "ces_ai_component.h"
#include "ces_transformation_component.h"
#include "std_extensions.h"
#include "pathfinder.h"
#include "path_map.h"
#include "ai_move_action.h"
#include "ai_actions_processor.h"
#include "game_object.h"

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
        
        if(ai_component)
        {
            ai_actions_processor_shared_ptr actions_processor = ai_component->actions_processor;
            actions_processor->update(deltatime);
            
            if(!actions_processor->is_actions_exist())
            {
                glm::ivec2 goal_position_index;
                goal_position_index.x = std::get_random_i(0, m_path_map->get_size().x - 1);
                goal_position_index.y = std::get_random_i(0, m_path_map->get_size().y - 1);
                
                auto transformation_component = entity->get_component<gb::ces_transformation_component>();
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
                        move_action->set_parameters(std::static_pointer_cast<gb::game_object>(entity), goal_position);
                        actions_processor->add_action(move_action);
                    }
                }
            }
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