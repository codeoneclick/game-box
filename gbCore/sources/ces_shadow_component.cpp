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
    
    void ces_shadow_component::update_shadow_geometry(const glm::vec2& light_caster_position, const glm::mat4& shadow_caster_mat_m,
                                                      const std::vector<glm::vec2>& convex_hull_oriented_vertices)
    {
        std::vector<ui16> back_facing_vertices;
        
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
            }
        }
        
        vbo_shared_ptr vbo = std::make_shared<gb::vbo>(back_facing_vertices.size() * 2, GL_STATIC_DRAW);
        vbo::vertex_attribute *vertices = vbo->lock();
        
        i32 index = 0;
        for(ui16 i = 0; i < back_facing_vertices.size(); ++i)
        {
            vertices[index++].m_position = glm::transform(convex_hull_oriented_vertices[back_facing_vertices[i]], shadow_caster_mat_m);
            glm::vec2 light_direction = glm::transform(convex_hull_oriented_vertices[back_facing_vertices[i]], shadow_caster_mat_m) - light_caster_position;
            light_direction = glm::normalize(light_direction);
            vertices[index++].m_position = glm::transform(convex_hull_oriented_vertices[back_facing_vertices[i]], shadow_caster_mat_m) + light_direction * 2048.f;
        }

        vbo->unlock();
        
        i32 num_indices = static_cast<i32>(back_facing_vertices.size() - 1) * 6;
        ibo_shared_ptr ibo = std::make_shared<gb::ibo>(num_indices, GL_STATIC_DRAW);
        ui16* indices = ibo->lock();

        index = 0;
        for(ui16 i = 0; i < back_facing_vertices.size() - 1; ++i)
        {
            indices[i * 6 + 0] = index;
            index += 2;
            indices[i * 6 + 1] = index;
            index -= 1;
            indices[i * 6 + 2] = index;
            index += 1;
            
            indices[i * 6 + 3] = index;
            index += 1;
            indices[i * 6 + 4] = index;
            index -= 2;
            indices[i * 6 + 5] = index;
            index += 1;
        }
        
        ibo->unlock();
        
        m_mesh = std::make_shared<gb::mesh>(vbo, ibo);
    }
    
    mesh_shared_ptr ces_shadow_component::get_shadow_mesh() const
    {
        return m_mesh;
    }
};