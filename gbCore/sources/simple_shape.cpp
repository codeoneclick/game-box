//
//  simple_shape.cpp
//  gbCore
//
//  Created by serhii serhiiv on 3/14/17.
//  Copyright © 2017 sergey.sergeev. All rights reserved.
//

#include "simple_shape.h"
#include "vbo.h"
#include "ibo.h"
#include "glm_extensions.h"

namespace gb
{
    simple_shape::simple_shape(const vbo_shared_ptr& vbo, const ibo_shared_ptr& ibo, const glm::mat4& mat)
    {
        vbo::vertex_attribute *vertices = vbo->lock();
        ui16 *indices = ibo->lock();
        
        for(i32 i = 0; i < ibo->get_used_size() - 3; i += 3)
        {
            m_triangles.push_back(triangle(glm::vec2(glm::transform(vertices[indices[i]].m_position, mat).x,
                                                     glm::transform(vertices[indices[i]].m_position, mat).y),
                                           glm::vec2(glm::transform(vertices[indices[i + 1]].m_position, mat).x,
                                                     glm::transform(vertices[indices[i + 1]].m_position, mat).y),
                                           glm::vec2(glm::transform(vertices[indices[i + 2]].m_position, mat).x,
                                                     glm::transform(vertices[indices[i + 2]].m_position, mat).y)));
        }
    }
    
    simple_shape::~simple_shape()
    {
        
    }
    
    std::pair<f32, f32> simple_shape::project_triangle(const glm::vec2 axis, const triangle& triangle)
    {
        std::pair<f32, f32> min_max;
        f32 dot_product = glm::dot(axis, triangle.m_points[0]);
        min_max.first = dot_product;
        min_max.second = dot_product;
        for (i32 i = 0; i < triangle.m_points.size(); ++i)
        {
            dot_product = glm::dot(axis, triangle.m_points[i]);
            if (dot_product < min_max.first)
            {
                min_max.first = dot_product;
            }
            else
            {
                if (dot_product > min_max.second)
                {
                    min_max.second = dot_product;
                }
            }
        }
        return min_max;
    }
    
    bool simple_shape::intersection(const vbo_shared_ptr& vbo_01, const ibo_shared_ptr& ibo_01, const glm::mat4& mat_01,
                                    const vbo_shared_ptr& vbo_02, const ibo_shared_ptr& ibo_02, const glm::mat4& mat_02)
    {
        simple_shape shape_01(vbo_01, ibo_01, mat_01);
        simple_shape shape_02(vbo_02, ibo_02, mat_02);
        
        for(i32 i = 0; i < shape_01.m_triangles.size(); ++i)
        {
            for(i32 j = 0; j < shape_02.m_triangles.size(); ++i)
            {
                size_t edge_count_01 = shape_01.m_triangles[i].m_edges_directions.size();
                size_t edge_count_02 = shape_02.m_triangles[j].m_edges_directions.size();
                
                glm::vec2 translation_axis;
                glm::vec2 edge;
                
                for (i32 k = 0; k < edge_count_01 + edge_count_02; ++k)
                {
                    if (k < edge_count_01)
                    {
                        edge = shape_01.m_triangles[i].m_edges_directions[k];
                    }
                    else
                    {
                        edge = shape_02.m_triangles[j].m_edges_directions[k - edge_count_01];
                    }
                    
                    glm::vec2 axis = glm::vec2(-edge.y, edge.x);
                    axis = glm::normalize(axis);
                    
                    std::pair<f32, f32> min_max_01;
                    std::pair<f32, f32> min_max_02;
                    
                    min_max_01 = project_triangle(axis, shape_01.m_triangles[i]);
                    min_max_02 = project_triangle(axis, shape_02.m_triangles[j]);
                    
                    f32 result = 0.f;
                    
                    if (min_max_01.first < min_max_02.first)
                    {
                        result = min_max_02.first - min_max_01.second;
                    }
                    else
                    {
                        result = min_max_01.first - min_max_02.second;
                    }
                    
                    if(result <= 0)
                    {
                        return true;
                    }
                }
            }
        }
        return false;
    }
}
