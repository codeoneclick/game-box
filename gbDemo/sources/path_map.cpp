//
//  path_map.cpp
//  gbDemo
//
//  Created by sergey.sergeev on 10/6/15.
//  Copyright (c) 2015 sergey.sergeev. All rights reserved.
//

#include "path_map.h"
#include "std_extensions.h"

namespace game
{
    path_map::path_map(const glm::ivec2& size) :
    m_size(size)
    {
        m_navigation_map.resize(m_size.x * m_size.y, nullptr);
    }
    
    path_map::~path_map()
    {
        
    }
    
    void path_map::set_path_passable(ui32 x, ui32 y, bool value)
    {
        m_navigation_map[x + y * m_size.x]->set_passable(value);
        m_navigation_map[x + y * m_size.x]->set_position(x, y);
    }
    
    std::shared_ptr<path_map_node> path_map::get_path_node(ui32 x, ui32 y)
    {
        std::shared_ptr<path_map_node> path_node = nullptr;
        if(x > -1 && x < m_size.x && y > -1 && y < m_size.y)
        {
            path_node = m_navigation_map[x + y * m_size.x];
        }
        return path_node;
    }
    
    void path_map::update()
    {
        std::shared_ptr<path_map_node> current_child = nullptr;
        for(i32 x = 0; x < m_size.x; ++x)
        {
            for(i32 y = 0; y < m_size.y; ++y)
            {
                m_navigation_map[x + y * m_size.x]->remove_children();
                
                for(i32 i = -1; i < 2; ++i)
                {
                    i32 new_x = m_navigation_map[x + y * m_size.x]->get_x() + i;
                    for(i32 j = -1; j < 2; ++j)
                    {
                        i32 new_y = m_navigation_map[x + y * m_size.x]->get_y() + j;
                        if(new_x > -1 && new_x < m_size.x && new_y > -1 && new_y < m_size.y)
                        {
                            current_child = m_navigation_map[new_x + new_y * m_size.x];
                            if(current_child->get_passable() && (new_x != x || new_y != y))
                            {
                                m_navigation_map[x + y * m_size.x]->add_child(current_child, m_navigation_map[x + y * m_size.x]->distance_to_local(current_child));
                            }
                        }
                    }
                }
            }
        }
    }
}
