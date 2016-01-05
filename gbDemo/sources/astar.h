//
//  astar.h
//  gbDemo
//
//  Created by sergey.sergeev on 10/7/15.
//  Copyright (c) 2015 sergey.sergeev. All rights reserved.
//

#ifndef astar_h
#define astar_h

#include "main_headers.h"
#include "declarations.h"
#include "path_node.h"

namespace koth
{
    class astar_node : public path_node
    {
    private:
        
    protected:
        
        f32 m_f, m_g, m_h;
        ui32 m_x, m_y;
        bool m_open, m_closed;
        
    public:
        
        astar_node() :
        m_f(0.f), m_g(0.f), m_h(0.f),
        m_closed(false), m_open(false)
        {
            
        }
        
        virtual ~astar_node()
        {
            
        }
        
        void set_position(i32 x, i32 y)
        {
            m_x = x;
            m_y = y;
        }
        
        void set_f(f32 f)
        {
            m_f = f;
        }
        
        void set_g(f32 g)
        {
            m_g = g;
        }
        
        void set_h(f32 h)
        {
            m_h = h;
        }
        
        void set_open(bool value)
        {
            m_open = value;
        }
        
        void set_closed(bool value)
        {
            m_closed = value;
        }
        
        inline ui32 get_x() const
        {
            return m_x;
        }
        
        inline ui32 get_y() const
        {
            return m_y;
        }
        
        inline f32 get_f() const
        {
            return m_f;
        }
        
        inline f32 get_g() const
        {
            return m_g;
        }
        
        inline f32 get_h() const
        {
            return m_h;
        }
        
        inline bool is_open() const
        {
            return m_open;
        }
        
        inline bool is_closed() const
        {
            return m_closed;
        }
        
        virtual f32 distance_to(const std::shared_ptr<astar_node>& node) const = 0;
        
        void release()
        {
            m_open = m_closed = false;
            m_f = m_g = m_h = 0.f;
            m_parent.reset();
        }
    };
    
    struct compare_nodes
    {
        bool operator() (const std::shared_ptr<astar_node>& node_01, const std::shared_ptr<astar_node>& node_02) const
        {
            return node_01->get_f() < node_02->get_f();
        }
    };
    
    class astar
    {
    private:
        
        void release_nodes();
        void push_open(const std::shared_ptr<astar_node>& node);
        void pop_open(const std::shared_ptr<astar_node>& node);
        
        std::vector<std::shared_ptr<astar_node>> m_open, m_closed;
        
        inline f32 distance_between(const std::shared_ptr<astar_node>& node_01,
                                    const std::shared_ptr<astar_node>& node_02) const
        {
            return node_01->distance_to(node_02);
        }
        
        std::vector<std::shared_ptr<astar_node>> construct_path(const std::shared_ptr<astar_node>& node)
        {
            std::vector<std::shared_ptr<astar_node>> path;
            std::shared_ptr<astar_node> parent = std::static_pointer_cast<astar_node>(node->get_parent());
            path.push_back(node);
            while(parent != nullptr)
            {
                path.push_back(parent);
                parent = std::static_pointer_cast<astar_node>(parent->get_parent());
            }
            return path;
        }
        
    public:
        
        astar();
        ~astar();
        
        std::vector<std::shared_ptr<astar_node>> get_path(const std::shared_ptr<astar_node>& start,
                                                          const std::shared_ptr<astar_node>& goal);
        void clear();
    };
};

#endif
