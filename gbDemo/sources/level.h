//
//  level.h
//  gbDemo
//
//  Created by sergey.sergeev on 10/6/15.
//  Copyright (c) 2015 sergey.sergeev. All rights reserved.
//

#ifndef level_h
#define level_h

#include "main_headers.h"
#include "declarations.h"
#include "pathfinder.h"
#include "astar.h"
#include "koth_declarations.h"

namespace koth
{
    enum e_level_box_state
    {
        e_level_box_state_none = 0,
        e_level_box_state_fall_down,
        e_level_box_state_drop_down
    };
    
    class level_node : public astar_node
    {
    private:
        
        bool m_passable;
        
    protected:
        
    public:
        
        level_node()
        {
            
        }
        
        ~level_node()
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
    
    class level
    {
    private:
        
    protected:
        
        glm::ivec2 m_size;
        
        glm::vec2 m_boxes_size;
        f32 m_boxes_offset;
        
        gb::scene_fabricator_shared_ptr m_fabricator;
        gb::scene_graph_shared_ptr m_graph;
        
        std::vector<gb::model3d_static_shared_ptr> m_boxes;
        std::vector<i32> m_boxes_states;
        std::vector<f32> m_boxes_under_strain;
        
        std::vector<level_node_shared_ptr> m_navigation_map;
        
        void update_navigation_map();
        
    public:
        level(const gb::scene_fabricator_shared_ptr& fabricator,
              const gb::scene_graph_shared_ptr& graph);
        ~level();
        
        void construct(const std::string& filename);
        
        void update(f32 deltatime);
        
        void set_box_state(i32 x, i32 z);
        void set_box_under_strain(f32 x, f32 z);
        
        void cleanup();
        
        level_node_shared_ptr get_path_node(i32 x, i32 y);
        
        glm::ivec2 get_size() const;
    };
};


#endif
