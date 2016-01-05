//
//  pathfinder.h
//  gbDemo
//
//  Created by sergey.sergeev on 10/7/15.
//  Copyright (c) 2015 sergey.sergeev. All rights reserved.
//

#ifndef pathfinder_h
#define pathfinder_h

#include "main_headers.h"
#include "declarations.h"
#include "astar.h"

namespace koth
{
    class pathfinder
    {
    private:
        
        std::shared_ptr<astar_node> m_start;
        std::shared_ptr<astar_node> m_goal;
        
    public:
        
        explicit pathfinder() :
        m_start(nullptr), m_goal(nullptr)
        {
            
        }

        void set_start(const std::shared_ptr<astar_node>& start)
        {
            m_start = start;
        }

        void set_goal(const std::shared_ptr<astar_node>& goal)
        {
            m_goal = goal;
        }
        
        std::shared_ptr<astar_node> get_start() const
        {
            return m_start;
        }
        
        std::shared_ptr<astar_node> get_goal() const
        {
            return m_goal;
        }
        
        bool find_path(std::vector<std::shared_ptr<astar_node>>& solution)
        {
            std::shared_ptr<astar> algorithm = std::make_shared<astar>();

            std::vector<std::shared_ptr<astar_node>> path = algorithm->get_path(m_start, m_goal);
            algorithm->clear();
            
            bool is_found = path.size() != 0;
            if(!is_found)
            {
                return false;
            }
            for(const auto& iterator : path)
            {
                solution.push_back(iterator);
            }
            return true;
        }
    };
};

#endif
