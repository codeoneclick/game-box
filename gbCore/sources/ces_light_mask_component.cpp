//
//  ces_light_mask_component.cpp
//  gbCore
//
//  Created by sergey.sergeev on 1/18/16.
//  Copyright © 2016 sergey.sergeev. All rights reserved.
//

#if !defined(__NO_RENDER__)

#include "ces_light_mask_component.h"
#include "glm_extensions.h"
#include "mesh.h"

namespace gb
{
    
    const ui32 ces_light_mask_component::k_max_num_vertices = 65535 / 4; // 16k vertices
    const ui32 ces_light_mask_component::k_max_num_indices = 65535 / 2;  // 32k indices
    
    ces_light_mask_component::ces_light_mask_component() :
    m_radius(1.f),
    m_center(glm::vec2(0.f)),
    m_bounds(glm::vec4(0.f))
    {
        vbo_shared_ptr vbo = std::make_shared<gb::vbo>(k_max_num_vertices, GL_DYNAMIC_DRAW);
        vbo::vertex_attribute *vertices = vbo->lock();
        memset(vertices, 0x0, k_max_num_vertices * sizeof(vbo::vertex_attribute));
        vbo->unlock();
        
        ibo_shared_ptr ibo = std::make_shared<gb::ibo>(k_max_num_indices, GL_DYNAMIC_DRAW);
        ui16* indices = ibo->lock();
        memset(indices, 0x0, k_max_num_indices * sizeof(ui16));
        ibo->unlock();
        
        radius.getter([=]() {
            return m_radius;
        });
        
        radius.setter([=](f32 radius) {
            m_radius = radius;
            ces_light_mask_component::update_bounds();
        });
        
        center.getter([=]() {
            return m_center;
        });
        
        center.setter([=](const glm::vec2& center) {
            m_center = center;
            ces_light_mask_component::update_bounds();
        });
        
        m_shadow_casters_vertices.resize(4, glm::vec2(0.f));
        m_shadow_casters_edges.resize(4, std::make_pair(glm::vec2(0.f), glm::vec2(0.f)));
        
        m_mesh = std::make_shared<gb::mesh>(vbo, ibo);
    }
    
    ces_light_mask_component::~ces_light_mask_component()
    {
        
    }
    
    void ces_light_mask_component::update_bounds()
    {
        m_shadow_casters_vertices[0] = glm::vec2(m_center.x - m_radius, m_center.y - m_radius);
        m_shadow_casters_vertices[1] = glm::vec2(m_center.x + m_radius, m_center.y - m_radius);
        m_shadow_casters_vertices[2] = glm::vec2(m_center.x + m_radius, m_center.y + m_radius);
        m_shadow_casters_vertices[3] = glm::vec2(m_center.x - m_radius, m_center.y + m_radius);
        
        for(i32 i = 0; i < 4; ++i)
        {
            i32 next_index = (i + 1) % 4;
            m_shadow_casters_edges[i] = std::make_pair(m_shadow_casters_vertices[i],
                                                       m_shadow_casters_vertices[next_index]);
        }
        m_bounds = glm::vec4(m_center.x - m_radius,
                             m_center.y - m_radius,
                             m_center.x + m_radius,
                             m_center.y + m_radius);
    }
    
    void ces_light_mask_component::add_shadowcasters_geometry(const glm::mat4& shadow_caster_mat_m,
                                                              const std::vector<glm::vec2>& convex_hull_oriented_vertices)
    {
        bool is_shadow_geometry_inside = false;
        for(i32 i = 0; i < convex_hull_oriented_vertices.size(); ++i)
        {
            i32 next_index = (i + 1) % convex_hull_oriented_vertices.size();
            glm::vec2 current_vertex = glm::transform(convex_hull_oriented_vertices[i], shadow_caster_mat_m);
            glm::vec2 next_vertex = glm::transform(convex_hull_oriented_vertices[next_index], shadow_caster_mat_m);
            m_shadow_casters_edges.push_back(std::make_pair(current_vertex, next_vertex));
            m_shadow_casters_vertices.push_back(current_vertex);
            
            if(glm::intersect(m_bounds, current_vertex))
            {
                is_shadow_geometry_inside = true;
            }
        }
        if(!is_shadow_geometry_inside)
        {
            m_shadow_casters_edges.resize(m_shadow_casters_edges.size() -
                                          convex_hull_oriented_vertices.size());
            m_shadow_casters_vertices.resize(m_shadow_casters_vertices.size() -
                                             convex_hull_oriented_vertices.size());
        }
    }
    
    void ces_light_mask_component::update_mesh()
    {
        std::list<f32> angles;
        for(i32 i = 0; i < m_shadow_casters_vertices.size(); ++i)
        {
            glm::vec2 point = m_shadow_casters_vertices[i];
            f32 angle = atan2f(point.y - m_center.y, point.x - m_center.x);
            
            angles.push_back(angle - .0001f);
            angles.push_back(angle);
            angles.push_back(angle + .0001f);
        }
        
        std::list<std::pair<glm::vec2, f32>> intersections;
        for(auto angle : angles)
        {
            glm::vec2 direction = glm::vec2(cosf(angle), sinf(angle));
            std::pair<glm::vec2, glm::vec2> ray = std::make_pair(m_center, m_center + direction);
            
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
            intersections.push_back(std::make_pair(closest_intersection, angle));
        }
        
        intersections.sort([](const std::pair<glm::vec2, f32>& a, const std::pair<glm::vec2, f32>& b) -> bool {
            return a.second < b.second;
        });
        
        vbo::vertex_attribute* vertices = m_mesh->get_vbo()->lock();
        ui16* indices = m_mesh->get_ibo()->lock();
        
        i32 vertices_offset = 0;
        i32 indices_offset = 0;
        
        glm::vec3 vertex_position = glm::vec3(m_center.x, m_center.y, 0.f);
        vertices[vertices_offset++].m_position = vertex_position;
        
        for(const auto& intersection : intersections)
        {
            vertex_position = glm::vec3(intersection.first.x , intersection.first.y, 0.f);
            vertices[vertices_offset++].m_position = vertex_position;
        }
        
        for(i32 i = 1; i < intersections.size() + 1; ++i)
        {
            i32 next_vertex_index = std::max((i + 1) % vertices_offset, 1);
            indices[indices_offset++] = 0;
            indices[indices_offset++] = i;
            indices[indices_offset++] = next_vertex_index;
        }
        
        m_mesh->get_vbo()->unlock(vertices_offset);
        m_mesh->get_ibo()->unlock(indices_offset);
    }
    
    mesh_shared_ptr ces_light_mask_component::get_mesh() const
    {
        return m_mesh;
    }
    
    void ces_light_mask_component::cleanup()
    {
        m_shadow_casters_vertices.resize(4);
        m_shadow_casters_edges.resize(4);
    }
};

#endif
