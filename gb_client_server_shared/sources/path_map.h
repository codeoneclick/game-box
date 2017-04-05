//
//  path_map.h
//  gbDemo
//
//  Created by sergey.sergeev on 10/6/15.
//  Copyright (c) 2015 sergey.sergeev. All rights reserved.
//

#pragma once

#include "main_headers.h"
#include "declarations.h"
#include "pathfinder.h"
#include "astar.h"

namespace game
{

    class path_map_node : public astar_node
    {
    private:
        
        bool m_passable;
        
    protected:
        
    public:
        
        path_map_node()
        {
            
        }
        
        ~path_map_node()
        {
            
        }
        
        void set_passable(const bool passable)
        {
            m_passable = passable;
        }
        
        bool get_passable() const
        {
            return m_passable;
        }
        
        f32 distance_to_local(const std::shared_ptr<astar_node>& node) const
        {
            if(node->get_x() != m_x && node->get_y() != m_y)
            {
                return 1.41421356237f;
            }
            else
            {
                return 1.f;
            }
        }
        
        f32 distance_to(const std::shared_ptr<astar_node>& node) const
        {
            i32 x = m_x - node->get_x(), y = m_y - node->get_y();
            return sqrtf(static_cast<f32>(x * x + y * y));
        }
    };
    
    class path_map
    {
    private:
        
    protected:
        
        glm::ivec2 m_size;
        glm::ivec2 m_cell_size;
        std::vector<std::shared_ptr<path_map_node>> m_navigation_map;
        
    public:
        
        path_map(const glm::ivec2& size, const glm::ivec2& cell_size);
        ~path_map();
        
        void set_path_passable(i32 x, i32 y, bool value);
        std::shared_ptr<path_map_node> get_path_node(i32 x, i32 y) const;
        void update();
        
        glm::ivec2 get_size() const;
        glm::ivec2 get_cell_size() const;
    };
};
