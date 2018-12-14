//
//  ces_shadow_component.cpp
//  gbCore
//
//  Created by sergey.sergeev on 1/11/16.
//  Copyright © 2016 sergey.sergeev. All rights reserved.
//

#include "ces_shadow_component.h"
#include "glm_extensions.h"
#include "mesh_2d.h"

namespace gb
{
    ui32 ces_shadow_component::g_id = 1;
    
    ces_shadow_component::ces_shadow_component() :
    m_mesh(nullptr),
    m_id(g_id++)
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
        
        glm::vec3 vertex_position = glm::vec3(0.f);
        glm::vec2 convex_point = glm::vec2(0.f);
        i32 current_index = starting_index;
        for(ui16 i = 0; i < back_facing_vertices.size(); ++i)
        {
            convex_point = glm::transform(convex_hull_oriented_vertices[current_index], shadow_caster_mat_m);
            vertex_position = glm::vec3(convex_point.x , convex_point.y, 0.f);
            m_vertices[index++].m_position = vertex_position;
            
            glm::vec2 light_direction = glm::transform(convex_hull_oriented_vertices[current_index], shadow_caster_mat_m) - light_caster_position;
            light_direction = glm::normalize(light_direction);
            
            convex_point = glm::transform(convex_hull_oriented_vertices[current_index], shadow_caster_mat_m) + light_direction * 1024.f;
            vertex_position = glm::vec3(convex_point.x , convex_point.y, 0.f);
            m_vertices[index++].m_position = vertex_position;
            
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
        
        vbo_shared_ptr vbo = nullptr;
        std::shared_ptr<vbo::vertex_declaration_PTC> vertex_declaration = std::make_shared<vbo::vertex_declaration_PTC>(m_vertices.size());
        
#if USED_GRAPHICS_API != NO_GRAPHICS_API
        
        vbo = std::make_shared<gb::vbo>(vertex_declaration, GL_STATIC_DRAW);
        
#else
        
        vbo = std::make_shared<gb::vbo>(vertex_declaration, 0);
        
#endif
        
        vbo::vertex_attribute_PTC *vertices = vbo->lock<vbo::vertex_attribute_PTC>();
        std::memcpy(vertices, &m_vertices[0], sizeof(vbo::vertex_attribute_PTC) * m_vertices.size());
        vbo->unlock();
        
#if USED_GRAPHICS_API != NO_GRAPHICS_API
        
        ibo_shared_ptr ibo = std::make_shared<gb::ibo>(m_indices.size(), GL_STATIC_DRAW);
        
#else
        ibo_shared_ptr ibo = std::make_shared<gb::ibo>(m_indices.size(), 0);
        
#endif
        
        ui16* indices = ibo->lock();
        std::memcpy(indices, &m_indices[0], sizeof(ui16) * m_indices.size());
        ibo->unlock();
        
        m_mesh = std::make_shared<gb::mesh_2d>(vbo, ibo);
    }
    
    ui32 ces_shadow_component::get_id() const
    {
        return m_id;
    }
    
    mesh_2d_shared_ptr ces_shadow_component::get_shadow_mesh() const
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
