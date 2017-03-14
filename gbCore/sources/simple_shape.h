//
//  simple_shape.h
//  gbCore
//
//  Created by serhii serhiiv on 3/14/17.
//  Copyright © 2017 sergey.sergeev. All rights reserved.
//

#pragma once

#include "resource.h"
#include "vbo.h"
#include "ibo.h"
#include "vao.h"
#include "shader.h"

namespace gb
{
    class simple_shape
    {
    public:
        
        struct edge
        {
            edge()
            {
                
            };
            
            edge(const glm::vec2& point_01, const glm::vec2& point_02)
            {
                m_points[0] = point_01;
                m_points[1] = point_02;
            };
            
            ~edge()
            {
                
            }
            
            std::array<glm::vec2, 2> m_points;
        };
        
        struct triangle
        {
            triangle()
            {
                
            };
            
            triangle(const glm::vec2& point_01, const glm::vec2& point_02, const glm::vec2& point_03)
            {
                m_points[0] = point_01;
                m_points[1] = point_02;
                m_points[2] = point_03;
                
                m_edges[0] = edge(point_01, point_02);
                m_edges[1] = edge(point_02, point_03);
                m_edges[2] = edge(point_03, point_01);
                
                m_edges_directions[0] = point_02 - point_01;
                m_edges_directions[1] = point_03 - point_02;
                m_edges_directions[2] = point_01 - point_03;
            };
            
            ~triangle()
            {
                
            };
            
            std::array<edge, 3> m_edges;
            std::array<glm::vec2, 3> m_edges_directions;
            std::array<glm::vec2, 3> m_points;
        };
        
    private:
        
    protected:
        
        std::vector<triangle> m_triangles;
        
        static std::pair<f32, f32> project_triangle(const glm::vec2 axis, const triangle& triangle);
        
    public:
        
        simple_shape(const vbo_shared_ptr& vbo, const ibo_shared_ptr& ibo, const glm::mat4& mat = glm::mat4(1.f));
        ~simple_shape();
        
        static bool intersection(const vbo_shared_ptr& vbo_01, const ibo_shared_ptr& ibo_01, const glm::mat4& mat_01,
                                 const vbo_shared_ptr& vbo_02, const ibo_shared_ptr& ibo_02, const glm::mat4& mat_02);
    };
};

