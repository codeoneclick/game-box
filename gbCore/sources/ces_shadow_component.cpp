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
    CTTI_GUID_CPP(ces_shadow_component)
    ces_shadow_component::ces_shadow_component() :
    m_mesh(nullptr)
    {

    }
    
    ces_shadow_component::~ces_shadow_component()
    {
        
    }
    
    void ces_shadow_component::update_shadow_geometry(const glm::vec2& light_caster_position, const glm::mat4& shadow_caster_mat_m,
                                                      const std::vector<glm::vec2>& convex_hull_oriented_vertices)
    {
        std::vector<bool> back_facing_flags;
        std::vector<ui16> back_facing_vertices;
        
        back_facing_flags.resize(convex_hull_oriented_vertices.size(), false);
        
        for(i32 i = 0; i < convex_hull_oriented_vertices.size(); ++i)
        {
            glm::vec2 point_01 = glm::transform(convex_hull_oriented_vertices[i], shadow_caster_mat_m);
            
            i32 next_point_index = (i + 1) % convex_hull_oriented_vertices.size();
            
            glm::vec2 point_02 = glm::transform(convex_hull_oriented_vertices[next_point_index], shadow_caster_mat_m);
            
            glm::vec2 edge_normal = glm::vec2((point_01.y - point_02.y) * -1.f,
                                              point_01.x - point_02.x);
            
            glm::vec2 edge_middle = (point_01 + point_02) * .5f;
            glm::vec2 light_direction = light_caster_position - edge_middle;
            
            if (glm::dot(light_direction, edge_normal) < 0.f)
            {
                if(std::find(back_facing_vertices.begin(), back_facing_vertices.end(), i) == back_facing_vertices.end())
                {
                    back_facing_vertices.push_back(i);
                }
                if(std::find(back_facing_vertices.begin(), back_facing_vertices.end(), next_point_index) == back_facing_vertices.end())
                {
                    back_facing_vertices.push_back(next_point_index);
                }
                back_facing_flags[i] = true;
            }
        }
        
        i32 starting_index = 0;
        for (i32 i = 0; i < convex_hull_oriented_vertices.size(); i++)
        {
            i32 current_edge = i;
            i32 next_edge = (i + 1) % convex_hull_oriented_vertices.size();
            
            if (!back_facing_flags[current_edge] && back_facing_flags[next_edge])
            {
                starting_index = next_edge;
            }
        }
        
        i32 index = 0;
        for(ui16 i = 0; i < std::max(static_cast<i32>(back_facing_vertices.size()) - 1, 0); ++i)
        {
            m_indices.push_back(index + m_vertices.size());
            index += 2;
            m_indices.push_back(index + m_vertices.size());
            index -= 1;
            m_indices.push_back(index + m_vertices.size());
            index += 1;
            
            m_indices.push_back(index + m_vertices.size());
            index += 1;
            m_indices.push_back(index + m_vertices.size());
            index -= 2;
            m_indices.push_back(index + m_vertices.size());
            index += 1;
        }
        
        index = static_cast<i32>(m_vertices.size());
        m_vertices.resize(m_vertices.size() + back_facing_vertices.size() * 2);
        
        i32 current_index = starting_index;
        for(ui16 i = 0; i < back_facing_vertices.size(); ++i)
        {
            m_vertices[index++].m_position = glm::transform(convex_hull_oriented_vertices[current_index], shadow_caster_mat_m);
            glm::vec2 light_direction = glm::transform(convex_hull_oriented_vertices[current_index], shadow_caster_mat_m) - light_caster_position;
            light_direction = glm::normalize(light_direction);
            m_vertices[index++].m_position = glm::transform(convex_hull_oriented_vertices[current_index], shadow_caster_mat_m) + light_direction * 1024.f;
            
            current_index = (current_index + 1) % convex_hull_oriented_vertices.size();
        }
    }
    
    void ces_shadow_component::generate_shadow_mesh()
    {
        if(m_vertices.size() == 0 || m_indices.size() == 0)
        {
            m_mesh = nullptr;
            return;
        }
        
        vbo_shared_ptr vbo = std::make_shared<gb::vbo>(m_vertices.size(), GL_STATIC_DRAW);
        vbo::vertex_attribute *vertices = vbo->lock();
        std::memcpy(vertices, &m_vertices[0], sizeof(vbo::vertex_attribute) * m_vertices.size());
        vbo->unlock();
        
        ibo_shared_ptr ibo = std::make_shared<gb::ibo>(m_indices.size(), GL_STATIC_DRAW);
        ui16* indices = ibo->lock();
        std::memcpy(indices, &m_indices[0], sizeof(ui16) * m_indices.size());
        ibo->unlock();
        
        m_mesh = std::make_shared<gb::mesh>(vbo, ibo);
    }
    
    mesh_shared_ptr ces_shadow_component::get_shadow_mesh() const
    {
        return m_mesh;
    }
    
    void ces_shadow_component::cleanup()
    {
        m_vertices.clear();
        m_vertices.resize(0);
        
        m_indices.clear();
        m_indices.resize(0);
    }
};