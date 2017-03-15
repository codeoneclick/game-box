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
        
        if(ibo->get_used_size() % 3 == 0)
        {
            for(i32 i = 0; i < ibo->get_used_size(); i += 3)
            {
                m_triangles.push_back(glm::triangle(glm::vec2(glm::transform(vertices[indices[i]].m_position, mat).x,
                                                              glm::transform(vertices[indices[i]].m_position, mat).y),
                                                    glm::vec2(glm::transform(vertices[indices[i + 1]].m_position, mat).x,
                                                              glm::transform(vertices[indices[i + 1]].m_position, mat).y),
                                                    glm::vec2(glm::transform(vertices[indices[i + 2]].m_position, mat).x,
                                                              glm::transform(vertices[indices[i + 2]].m_position, mat).y)));
            }
        }
        else
        {
            std::cout<<"wrong geomerty"<<std::endl;
        }
    }
    
    simple_shape::~simple_shape()
    {
        
    }
    
    bool simple_shape::intersection(const vbo_shared_ptr& vbo_01, const ibo_shared_ptr& ibo_01, const glm::mat4& mat_01,
                                    const vbo_shared_ptr& vbo_02, const ibo_shared_ptr& ibo_02, const glm::mat4& mat_02)
    {
        simple_shape shape_01(vbo_01, ibo_01, mat_01);
        simple_shape shape_02(vbo_02, ibo_02, mat_02);
        
        for(const auto& triangle_01 : shape_01.m_triangles)
        {
            for(const auto& triangle_02 : shape_02.m_triangles)
            {
                for(const auto& segment_01 : triangle_01.segments)
                {
                    for(const auto& segment_02 : triangle_02.segments)
                    {
                        if(glm::segments_intersect(segment_01, segment_02))
                        {
                            return true;
                        }
                    }
                }
                
                for(const auto& point_01 : triangle_01.points)
                {
                    if(glm::point_in_triangle(point_01, triangle_02))
                    {
                        return true;
                    }
                }
                
                for(const auto& point_02 : triangle_02.points)
                {
                    if(glm::point_in_triangle(point_02, triangle_01))
                    {
                        return true;
                    }
                }
            }
        }
        return false;
    }
}
