//
//  ces_convex_hull_component.cpp
//  gbCore
//
//  Created by sergey.sergeev on 1/13/16.
//  Copyright © 2016 sergey.sergeev. All rights reserved.
//

#include "ces_convex_hull_component.h"
#include "glm_extensions.h"

namespace gb
{
    ces_convex_hull_component::ces_convex_hull_component()
    {
        m_type = e_ces_component_type_convex_hull;
    }
    
    ces_convex_hull_component::~ces_convex_hull_component()
    {
        m_oriented_vertices.clear();
    }
    
    void ces_convex_hull_component::create_convex_hull(const vbo::vertex_attribute* vertices, i32 vertices_count)
    {
        assert(vertices_count >= 3);
        
        if(vertices_count < 3)
        {
            return;
        }
        
        i32 next[vertices_count];
        i32 leftmost_point_index = 0;
        
        for(i32 i = 0; i < vertices_count; ++i)
        {
            next[i] = -1;
            
            if (vertices[i].m_position.x < vertices[leftmost_point_index].m_position.x)
            {
                leftmost_point_index = i;
            }
        }
        
        i32 start_point_index = leftmost_point_index, end_point_index;
        
        do
        {
            end_point_index = (start_point_index + 1) % vertices_count;
            
            for(i32 i = 0; i < vertices_count; ++i)
            {
                if (glm::orientation(vertices[start_point_index].m_position,
                                     vertices[i].m_position,
                                     vertices[end_point_index].m_position) == glm::e_orientation_counterclockwise)
                {
                    end_point_index = i;
                }
            }
            m_oriented_vertices.push_back(vertices[end_point_index].m_position);
            next[start_point_index] = end_point_index;
            
            start_point_index = end_point_index;
        }
        while (start_point_index != leftmost_point_index);
    }
    
    const std::vector<glm::vec2>& ces_convex_hull_component::get_oriented_vertices() const
    {
        return m_oriented_vertices;
    }
};