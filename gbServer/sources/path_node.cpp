//
//  path_node.cpp
//  gbDemo
//
//  Created by sergey.sergeev on 10/7/15.
//  Copyright (c) 2015 sergey.sergeev. All rights reserved.
//

#include "path_node.h"


namespace koth
{
    path_node::path_node()
    {
        
    }
    
    path_node::~path_node()
    {
        
    }
    
    std::shared_ptr<path_node> path_node::get_parent() const
    {
        return m_parent.lock();
    }
    
    std::vector<std::pair<std::shared_ptr<path_node>, f32>>& path_node::get_children()
    {
        return m_children;
    }
    
    void path_node::add_child(const std::shared_ptr<path_node>& child, f32 distance)
    {
        m_children.push_back(std::make_pair(child,distance));
    }
    
    void path_node::remove_children()
    {
        m_children.clear();
    }
    
    void path_node::set_parent(const std::shared_ptr<path_node>& parent)
    {
        m_parent = parent;
    }
}
