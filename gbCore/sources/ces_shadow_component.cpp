//
//  ces_shadow_component.cpp
//  gbCore
//
//  Created by sergey.sergeev on 1/11/16.
//  Copyright © 2016 sergey.sergeev. All rights reserved.
//

#include "ces_shadow_component.h"
#include "glm_extensions.h"
#include "mesh.h"

namespace gb
{
    ces_shadow_component::ces_shadow_component() :
    m_mesh(nullptr)
    {
        m_type = e_ces_component_type_shadow;
    }
    
    ces_shadow_component::~ces_shadow_component()
    {
        
    }
    
    std::vector<glm::vec2> ces_shadow_component::convex_hull(const vbo::vertex_attribute* vertices, i32 vertices_count)
    {
        assert(vertices_count >= 3);
        
        if(vertices_count < 3)
        {
            return std::vector<glm::vec2>();
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
        
        std::vector<glm::vec2> oriented_vertices;
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
            oriented_vertices.push_back(vertices[end_point_index].m_position);
            next[start_point_index] = end_point_index;
            
            start_point_index = end_point_index;
        }
        while (start_point_index != leftmost_point_index);
        
        return oriented_vertices;
    }
    
    void ces_shadow_component::serialize_shadow_caster_geometry(const mesh_shared_ptr& mesh)
    {
        
    }

    void ces_shadow_component::update_shadow_geometry(const glm::vec2& light_caster_position, const glm::mat4& shadow_caster_mat_m,
                                                      const vbo::vertex_attribute* shadow_caster_vertices, i32 shadow_caster_vertices_count,
                                                      const ui16* shadow_caster_indices, i32 shadow_caster_indices_count)
    {
        std::vector<glm::vec2> convex_hull_vertices = ces_shadow_component::convex_hull(shadow_caster_vertices, shadow_caster_vertices_count);
        
        std::vector<ui16> back_facing_vertices;
        
        for(i32 i = 0; i < convex_hull_vertices.size(); ++i)
        {
            glm::vec2 point_01 = glm::transform(convex_hull_vertices[i], shadow_caster_mat_m);
            
            i32 next_point_index = (i + 1) % convex_hull_vertices.size();
            
            glm::vec2 point_02 = glm::transform(convex_hull_vertices[next_point_index], shadow_caster_mat_m);
            
            glm::vec2 edge_normal = glm::vec2((point_01.y - point_02.y) * -1.f,
                                              point_01.x - point_02.x);
            
            glm::vec2 edge_middle = (point_01 + point_02) * .5f;
            glm::vec2 light_direction = light_caster_position - edge_middle;
            
            if (glm::dot(light_direction, edge_normal) > 0.f)
            {
                if(std::find(back_facing_vertices.begin(), back_facing_vertices.end(), i) == back_facing_vertices.end())
                {
                    back_facing_vertices.push_back(i);
                }
                if(std::find(back_facing_vertices.begin(), back_facing_vertices.end(), next_point_index) == back_facing_vertices.end())
                {
                    back_facing_vertices.push_back(next_point_index);
                }
            }
        }
        
        vbo_shared_ptr vbo = std::make_shared<gb::vbo>(back_facing_vertices.size() * 2, GL_STATIC_DRAW);
        vbo::vertex_attribute *vertices = vbo->lock();
        
        i32 index = 0;
        for(ui16 i = 0; i < back_facing_vertices.size(); ++i)
        {
            vertices[index++].m_position = convex_hull_vertices[back_facing_vertices[i]];
            glm::vec2 light_direction = light_caster_position - convex_hull_vertices[back_facing_vertices[i]];
            light_direction = glm::normalize(light_direction);
            vertices[index++].m_position = convex_hull_vertices[back_facing_vertices[i]] + light_direction * 256.f;
        }

        vbo->unlock();
        
        ibo_shared_ptr ibo = std::make_shared<gb::ibo>(back_facing_vertices.size() * 2, GL_STATIC_DRAW);
        ui16* indices = ibo->lock();

        for(ui16 i = 0; i < back_facing_vertices.size() * 2; ++i)
        {
            indices[i] = i;
        }
        
        ibo->unlock();
        
        m_mesh = std::make_shared<gb::mesh>(vbo, ibo, GL_LINES);
    }
    
    void ces_shadow_component::generate_mesh()
    {
        
    }
    
    void ces_shadow_component::cleanup()
    {
        
    }
    
    mesh_shared_ptr ces_shadow_component::get_mesh() const
    {
        return m_mesh;
    }
};