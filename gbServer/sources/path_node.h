//
//  path_node.h
//  gbDemo
//
//  Created by sergey.sergeev on 10/7/15.
//  Copyright (c) 2015 sergey.sergeev. All rights reserved.
//

#ifndef path_node_h
#define path_node_h

#include "main_headers.h"
#include "declarations.h"

namespace koth
{
    class path_node
    {
    private:
        
    protected:
        
        std::weak_ptr<path_node> m_parent;
        std::vector<std::pair<std::shared_ptr<path_node>, f32>> m_children;
        
    public:
        
        path_node();
        virtual ~path_node();
        
        void set_parent(const std::shared_ptr<path_node>& parent);
        std::shared_ptr<path_node> get_parent() const;

        void add_child(const std::shared_ptr<path_node>& child, f32 distance);
        std::vector<std::pair<std::shared_ptr<path_node>, f32>>& get_children();
        
        void remove_children();
    };
};

#endif
