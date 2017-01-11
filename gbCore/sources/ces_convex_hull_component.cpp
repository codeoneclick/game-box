//
//  ces_convex_hull_component.cpp
//  gbCore
//
//  Created by sergey.sergeev on 1/13/16.
//  Copyright © 2016 sergey.sergeev. All rights reserved.
//

#include "ces_convex_hull_component.h"
#include "glm_extensions.h"

#define k_min_vertices_in_convex 3

namespace gb
{
    ces_convex_hull_component::ces_convex_hull_component() :
    m_center(0.f)
    {
        center.getter([=]() {
            return m_center;
        });
        
        oriented_vertices.getter([=]() -> const std::vector<glm::vec2>& {
            return m_oriented_vertices;
        });
    }
    
    ces_convex_hull_component::~ces_convex_hull_component()
    {
        m_oriented_vertices.clear();
    }
    
    void ces_convex_hull_component::create(const vbo::vertex_attribute* vertices, i32 vertices_count)
    {
        assert(vertices_count >= k_min_vertices_in_convex);
        
        if(vertices_count < k_min_vertices_in_convex)
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
        glm::vec2 convex_point_01 = glm::vec2(0.f);
        glm::vec2 convex_point_02 = glm::vec2(0.f);
        glm::vec2 convex_point_03 = glm::vec2(0.f);
        
        glm::vec2 min_bound = glm::vec2(INT16_MAX);
        glm::vec2 max_bound = glm::vec2(INT16_MIN);
        
        do
        {
            end_point_index = (start_point_index + 1) % vertices_count;
            
            for(i32 i = 0; i < vertices_count; ++i)
            {
                convex_point_01 = glm::vec2(vertices[start_point_index].m_position.x,
                                            vertices[start_point_index].m_position.y);
                
                convex_point_02 = glm::vec2(vertices[i].m_position.x,
                                            vertices[i].m_position.y);
                
                convex_point_03 = glm::vec2(vertices[end_point_index].m_position.x,
                                            vertices[end_point_index].m_position.y);
                
                if (glm::orientation(convex_point_01,
                                     convex_point_02,
                                     convex_point_03) == glm::e_orientation_counterclockwise)
                {
                    end_point_index = i;
                }
            }
            m_oriented_vertices.push_back(glm::vec2(vertices[end_point_index].m_position.x, vertices[end_point_index].m_position.y));
            
            min_bound = glm::min(m_oriented_vertices.back(), min_bound);
            max_bound = glm::max(m_oriented_vertices.back(), max_bound);
            
            next[start_point_index] = end_point_index;
            
            start_point_index = end_point_index;
        }
        while (start_point_index != leftmost_point_index);
        
        m_center = (max_bound - min_bound) / 2.f;
    }
};
