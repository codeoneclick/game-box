//
//  astar.cpp
//  gbDemo
//
//  Created by sergey.sergeev on 10/7/15.
//  Copyright (c) 2015 sergey.sergeev. All rights reserved.
//

#include "astar.h"

namespace koth
{
    astar::astar()
    {
        
    }
    
    astar::~astar()
    {
        
    }
    
    std::vector<std::shared_ptr<astar_node>> astar::get_path(const std::shared_ptr<astar_node> &start, const std::shared_ptr<astar_node> &goal)
    {
        std::shared_ptr<astar_node> current_node, child_node = nullptr;
        f32 f, g, h;
        
        std::make_heap(m_open.begin(), m_open.end(), compare_nodes());
        astar::push_open(start);
        
        while(!m_open.empty())
        {
            std::sort(m_open.begin(), m_open.end(), compare_nodes());
            
            current_node = m_open.front();
            astar::pop_open(current_node);
            
            current_node->set_closed(true);
            m_closed.push_back(current_node);
            
            if(current_node == goal)
            {
                return construct_path(current_node);
            }
            
            for(const auto& children : current_node->get_children())
            {
                child_node = std::static_pointer_cast<astar_node>(children.first);
                g = current_node->get_g() + children.second;
                if( (child_node->is_open() || child_node->is_closed()) && child_node->get_g() < g)
                {
                    continue;
                }
                
                h = distance_between(child_node, goal);
                f = g + h;
                child_node->set_f(f);
                child_node->set_g(g);
                child_node->set_h(h);
                child_node->set_parent(current_node);
                
                if(child_node->is_closed())
                {
                    child_node->set_closed(false);
                }
                
                if(!child_node->is_open())
                {
                     astar::push_open(child_node);
                }
            }
        }
        return std::vector<std::shared_ptr<astar_node>>();
    }
    
    void astar::push_open(const std::shared_ptr<astar_node>& node)
    {
        m_open.push_back(node);
        std::push_heap(m_open.begin(), m_open.end(), compare_nodes());
        node->set_open(true);
    }
    
    void astar::pop_open(const std::shared_ptr<astar_node>& node)
    {
        std::pop_heap(m_open.begin(), m_open.end(), compare_nodes());
        m_open.pop_back();
        node->set_open(false);
    }
    
    void astar::release_nodes()
    {
        for(const auto& node : m_open)
        {
            node->release();
        }
        for(const auto& node : m_closed)
        {
            node->release();
        }
    }
    
    void astar::clear()
    {
        astar::release_nodes();
        m_open.clear();
        m_closed.clear();
    }
}
