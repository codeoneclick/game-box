//
//  ces_light_mask_component.cpp
//  gbCore
//
//  Created by sergey.sergeev on 1/18/16.
//  Copyright © 2016 sergey.sergeev. All rights reserved.
//

#include "ces_light_mask_component.h"
#include "glm_extensions.h"
#include "mesh.h"

namespace gb
{
    ces_light_mask_component::ces_light_mask_component()
    {
        m_type = e_ces_component_type_light_mask;
    }
    
    ces_light_mask_component::~ces_light_mask_component()
    {
        
    }
    
    void ces_light_mask_component::update_mask_geometry(const glm::mat4& shadow_caster_mat_m,
                                                        const std::vector<glm::vec2>& convex_hull_oriented_vertices)
    {
        for(i32 i = 0; i < convex_hull_oriented_vertices.size(); ++i)
        {
            i32 next_vertex_index = (i + 1) % convex_hull_oriented_vertices.size();
            m_shadow_casters_edges.push_back(std::make_pair(glm::transform(convex_hull_oriented_vertices[i], shadow_caster_mat_m),
                                                            glm::transform(convex_hull_oriented_vertices[next_vertex_index], shadow_caster_mat_m)));
            
        }
    }
    
    void ces_light_mask_component::generate_mask_mesh(const glm::vec2& light_caster_position)
    {
        std::vector<f32> angles;
        for(i32 i = 0; i < m_shadow_casters_edges.size(); ++i)
        {
            glm::vec2 point = m_shadow_casters_edges[i].first;
            f32 angle = atan2f(point.y - light_caster_position.y, point.x - light_caster_position.x);
            angles.push_back(angle);
            
            point = m_shadow_casters_edges[i].second;
            angle = atan2f(point.y - light_caster_position.y, point.x - light_caster_position.x);
            
            angles.push_back(angle - .0001f);
            angles.push_back(angle);
            angles.push_back(angle + .0001f);
        }
        
        std::list<std::pair<glm::vec2, f32>> intersections;
        for(i32 i = 0; i < angles.size(); ++i)
        {
            glm::vec2 direction = glm::vec2(cosf(angles[i]), sinf(angles[i]));
            std::pair<glm::vec2, glm::vec2> ray = std::make_pair(light_caster_position, light_caster_position + direction);
            
            f32 closest_distance = INT16_MAX;
            glm::vec2 closest_intersection = glm::vec2(INT16_MIN);
            
            for(i32 j = 0; j < m_shadow_casters_edges.size(); ++j)
            {
                f32 distance = INT16_MAX;
                glm::vec2 intersection;
                bool is_intersected = glm::intersect(ray, m_shadow_casters_edges[j], &intersection, &distance);
                if(!is_intersected)
                {
                    continue;
                }
                if(distance < closest_distance)
                {
                    closest_distance = distance;
                    closest_intersection = intersection;
                }
            }
            
            if(closest_intersection == glm::vec2(INT16_MIN))
            {
                continue;
            }
            intersections.push_back(std::make_pair(closest_intersection, angles[i]));
        }
        
        intersections.sort([](const std::pair<glm::vec2, f32>& a, const std::pair<glm::vec2, f32>& b) -> bool{
            return a.second < b.second;
        });
        
        m_vertices.push_back(light_caster_position);
        
        for(const auto& intersection : intersections)
        {
            m_vertices.push_back(intersection.first);
        }
        
        for(i32 i = 1; i < m_vertices.size(); ++i)
        {
            m_indices.push_back(0);
            m_indices.push_back(i);
        }
    }
    
    mesh_shared_ptr ces_light_mask_component::get_mask_mesh() const
    {
        if(m_vertices.size() == 0 || m_indices.size() == 0)
        {
            return nullptr;
        }
        
        vbo_shared_ptr vbo = std::make_shared<gb::vbo>(m_vertices.size(), GL_STATIC_DRAW);
        vbo::vertex_attribute *vertices = vbo->lock();
        std::memcpy(vertices, &m_vertices[0], sizeof(vbo::vertex_attribute) * m_vertices.size());
        vbo->unlock();
        
        ibo_shared_ptr ibo = std::make_shared<gb::ibo>(m_indices.size(), GL_STATIC_DRAW);
        ui16* indices = ibo->lock();
        std::memcpy(indices, &m_indices[0], sizeof(ui16) * m_indices.size());
        ibo->unlock();
        
        mesh_shared_ptr mesh = std::make_shared<gb::mesh>(vbo, ibo, GL_LINES);

        return mesh;
    }
    
    void ces_light_mask_component::cleanup()
    {
        m_vertices.clear();
        m_indices.clear();
        
        m_shadow_casters_edges.clear();
    }
};