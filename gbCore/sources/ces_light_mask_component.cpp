//
//  ces_light_mask_component.cpp
//  gbCore
//
//  Created by sergey.sergeev on 1/18/16.
//  Copyright © 2016 sergey.sergeev. All rights reserved.
//


#include "ces_light_mask_component.h"
#include "glm_extensions.h"
#include "std_extensions.h"
#include "mesh_2d.h"

namespace gb
{
    const ui32 ces_light_mask_component::k_max_num_vertices = 65535 / 4; // 16k vertices
    const ui32 ces_light_mask_component::k_max_num_indices = 65535 / 2;  // 32k indices
    const f32 ces_light_mask_component::k_bounds_trashhold = 1.33f;
    
    ces_light_mask_component::ces_light_mask_component() :
    m_radius(1.f),
    m_center(glm::vec2(0.f)),
    m_light_mask_bounds(glm::vec4(0.f)),
    m_used_intersections(0)
    {
        vbo_shared_ptr vbo = nullptr;
        std::shared_ptr<vbo::vertex_declaration_PTC> vertex_declaration = std::make_shared<vbo::vertex_declaration_PTC>(k_max_num_vertices);
        
#if !defined(__NO_RENDER__)
        
        vbo = std::make_shared<gb::vbo>(vertex_declaration, GL_DYNAMIC_DRAW);
        
#else
        
        vbo = std::make_shared<gb::vbo>(vertex_declaration, 0);
        
#endif
        vbo::vertex_attribute_PTC *vertices = vbo->lock<vbo::vertex_attribute_PTC>();
        memset(vertices, 0x0, k_max_num_vertices * sizeof(vbo::vertex_attribute));
        vbo->unlock();
        
        
#if !defined(__NO_RENDER__)
        
        ibo_shared_ptr ibo = std::make_shared<gb::ibo>(k_max_num_indices, GL_DYNAMIC_DRAW);
        
#else
        
        ibo_shared_ptr ibo = std::make_shared<gb::ibo>(k_max_num_indices, 0);
        
#endif
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
        
        m_shadow_casters_edges.resize(4, glm::vec4(0.f));
        m_mesh = std::make_shared<gb::mesh_2d>(vbo, ibo);
    }
    
    ces_light_mask_component::~ces_light_mask_component()
    {
        
    }
    
    void ces_light_mask_component::update_bounds()
    {
        m_light_mask_bounds.x = m_center.x - m_radius * k_bounds_trashhold;
        m_light_mask_bounds.y = m_center.y - m_radius * k_bounds_trashhold;
        m_light_mask_bounds.z = m_center.x + m_radius * k_bounds_trashhold;
        m_light_mask_bounds.w = m_center.y + m_radius * k_bounds_trashhold;
        
        m_light_mask_bounds_geometry[0].x = m_light_mask_bounds.x;
        m_light_mask_bounds_geometry[0].y = m_light_mask_bounds.y;
        m_light_mask_bounds_geometry[1].x = m_light_mask_bounds.z;
        m_light_mask_bounds_geometry[1].y = m_light_mask_bounds.y;
        m_light_mask_bounds_geometry[2].x = m_light_mask_bounds.z;
        m_light_mask_bounds_geometry[2].y = m_light_mask_bounds.w;
        m_light_mask_bounds_geometry[3].x = m_light_mask_bounds.x;
        m_light_mask_bounds_geometry[3].y = m_light_mask_bounds.w;
        
        for(i32 i = 0; i < 4; ++i)
        {
            f32 angle = atan2f(m_light_mask_bounds_geometry[i].y - m_center.y, m_light_mask_bounds_geometry[i].x - m_center.x) - .0001f;
            for(i32 j = 0; j < 3; ++j, angle += .0001f)
            {
                m_unique_sorted_raytrace_angles.emplace(angle);
            }
            i32 next_index = (i + 1) % 4;
            m_shadow_casters_edges[i].x = m_light_mask_bounds_geometry[i].x;
            m_shadow_casters_edges[i].y = m_light_mask_bounds_geometry[i].y;
            m_shadow_casters_edges[i].z = m_light_mask_bounds_geometry[next_index].x;
            m_shadow_casters_edges[i].w = m_light_mask_bounds_geometry[next_index].y;
        }
    }
    
    void ces_light_mask_component::add_shadowcasters_geometry(const std::vector<glm::vec2>& convex_hull_oriented_vertices)
    {
        bool is_shadow_geometry_inside = false;
        i32 convex_hull_oriented_vertices_count = static_cast<i32>(convex_hull_oriented_vertices.size());
        
        i32 shadow_casters_edges_count = static_cast<i32>(m_shadow_casters_edges.size());
        
        m_shadow_casters_edges.resize(shadow_casters_edges_count + convex_hull_oriented_vertices_count);
        
        glm::vec2 current_vertex = convex_hull_oriented_vertices[0];
        
        std::vector<glm::vec2> shadow_casters_vertices;
        shadow_casters_vertices.resize(convex_hull_oriented_vertices_count);
        
        for(i32 i = 0, shadow_casters_edge_index = shadow_casters_edges_count;
            i < convex_hull_oriented_vertices_count; ++i)
        {
            i32 next_index = (i + 1) % convex_hull_oriented_vertices_count;
            glm::vec2 next_vertex = convex_hull_oriented_vertices[next_index];
            
            m_shadow_casters_edges[shadow_casters_edge_index].x = current_vertex.x;
            m_shadow_casters_edges[shadow_casters_edge_index].y = current_vertex.y;
            m_shadow_casters_edges[shadow_casters_edge_index].z = next_vertex.x;
            m_shadow_casters_edges[shadow_casters_edge_index++].w = next_vertex.y;
            
            shadow_casters_vertices[i].x = current_vertex.x - m_center.x;
            shadow_casters_vertices[i].y = current_vertex.y - m_center.y;
            
            if(glm::intersect(m_light_mask_bounds, current_vertex))
            {
                is_shadow_geometry_inside = true;
            }
            current_vertex.x = next_vertex.x;
            current_vertex.y = next_vertex.y;
        }
        if(!is_shadow_geometry_inside)
        {
            m_shadow_casters_edges.resize(shadow_casters_edges_count);
        }
        else
        {
            for(i32 i = 0; i < convex_hull_oriented_vertices_count; ++i)
            {
                f32 angle = atan2f(shadow_casters_vertices[i].y, shadow_casters_vertices[i].x) - .0001f;
                for(i32 j = 0; j < 3; ++j, angle += .0001f)
                {
                    m_unique_sorted_raytrace_angles.insert(angle);
                }
            }
        }
    }
    
    void ces_light_mask_component::update_mesh()
    {
        i32 shadow_casters_edges_count = static_cast<i32>(m_shadow_casters_edges.size());
        
        glm::vec2 direction;
        glm::vec4 ray;
        ray.x = m_center.x;
        ray.y = m_center.y;
        
        f32 closest_distance;
        glm::vec2 closest_intersection;
        
        f32 distance;
        glm::vec2 intersection;
        
        std::for_each(m_unique_sorted_raytrace_angles.begin(), m_unique_sorted_raytrace_angles.end(), [&](f32 angle) {
           
            direction.x = cosf(angle);
            direction.y = sinf(angle);
            
            ray.z = m_center.x + direction.x;
            ray.w = m_center.y + direction.y;
            
            closest_distance = std::numeric_limits<f32>::max();
            closest_intersection.x = std::numeric_limits<f32>::min();
            closest_intersection.y = std::numeric_limits<f32>::min();
            
            for(i32 j = 0; j < shadow_casters_edges_count; ++j)
            {
                const glm::vec4& shadow_caster_edge = m_shadow_casters_edges[j];
                distance = std::numeric_limits<f32>::max();
                bool is_intersected = glm::intersect(ray, shadow_caster_edge, &intersection, &distance);
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
            if(closest_distance != std::numeric_limits<f32>::max())
            {
                m_intersections[m_used_intersections].x = closest_intersection.x;
                m_intersections[m_used_intersections].y = closest_intersection.y;
                m_used_intersections++;
                assert(m_used_intersections <= k_max_intersections_count);
            }
        });
        
        vbo::vertex_attribute_PTC* vertices = m_mesh->get_vbo()->lock<vbo::vertex_attribute_PTC>();
        ui16* indices = m_mesh->get_ibo()->lock();
        
        i32 vertices_count = static_cast<i32>(m_used_intersections) + 1;
        i32 vertices_offset = 0;
        i32 indices_offset = 0;
        
        vertices[vertices_offset].m_position.x = m_center.x;
        vertices[vertices_offset++].m_position.y = m_center.y;
        
        for(i32 i = 0; i < m_used_intersections; ++i)
        {
            vertices[vertices_offset].m_position.x = m_intersections[i].x;
            vertices[vertices_offset++].m_position.y = m_intersections[i].y;
            
            i32 next_vertex_index = std::max((i + 2) % vertices_count, 1);
            indices[indices_offset++] = 0;
            indices[indices_offset++] = i + 1;
            indices[indices_offset++] = next_vertex_index;
        }
        
        if(indices_offset % 3 == 0)
        {
            m_mesh->get_vbo()->unlock(vertices_offset);
            m_mesh->get_ibo()->unlock(indices_offset);
        }
    }
    
    mesh_2d_shared_ptr ces_light_mask_component::get_mesh() const
    {
        return m_mesh;
    }
    
    void ces_light_mask_component::cleanup()
    {
        m_unique_sorted_raytrace_angles.clear();
        m_shadow_casters_edges.resize(4);
        m_used_intersections = 0;
    }
};
