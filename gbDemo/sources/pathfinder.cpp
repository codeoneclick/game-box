//
//  pathfinder.cpp
//  gbDemo
//
//  Created by serhii serhiiv on 6/14/17.
//  Copyright © 2017 sergey.sergeev. All rights reserved.
//

#include "pathfinder.h"
#include "path_map.h"

namespace game
{
    std::queue<glm::vec2> pathfinder::find_path(const glm::vec2& start_position,
                                                const glm::vec2& end_position,
                                                const pathfinder_shared_ptr& pathfinder,
                                                const path_map_shared_ptr& path_map)
    {
        glm::ivec2 end_position_index;
        end_position_index.x = std::max(0, std::min(static_cast<i32>(end_position.x / path_map->get_cell_size().x), path_map->get_size().x - 1));
        end_position_index.y = std::max(0, std::min(static_cast<i32>(end_position.y / path_map->get_cell_size().y), path_map->get_size().y - 1));
        
        glm::ivec2 start_position_index;
        start_position_index.x = std::max(0, std::min(static_cast<i32>(start_position.x / path_map->get_cell_size().x), path_map->get_size().x - 1));
        start_position_index.y = std::max(0, std::min(static_cast<i32>(start_position.y / path_map->get_cell_size().y), path_map->get_size().y - 1));
        
        std::vector<std::shared_ptr<astar_node>> path;
        pathfinder->set_start(path_map->get_path_node(start_position_index.x, start_position_index.y));
        pathfinder->set_goal(path_map->get_path_node(end_position_index.x, end_position_index.y));
        
        std::queue<glm::vec2> move_path;
        bool is_found = pathfinder->find_path(path);
        if(is_found && path.size() > 1)
        {
            path.resize(path.size() - 1);
            std::reverse(path.begin(), path.end());
            
            for(const auto& point : path)
            {
                glm::vec2 goal_position;
                goal_position.x = static_cast<f32>(point->get_x()) * static_cast<f32>(path_map->get_cell_size().x) + static_cast<f32>(path_map->get_cell_size().x) * .5f;
                goal_position.y = static_cast<f32>(point->get_y()) * static_cast<f32>(path_map->get_cell_size().y) + static_cast<f32>(path_map->get_cell_size().y) * .5f;
                move_path.push(goal_position);
            }
            move_path.pop();
            move_path.push(end_position);
        }
        return move_path;
    }
}
