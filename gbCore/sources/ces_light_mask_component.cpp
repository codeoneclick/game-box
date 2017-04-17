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
#include "mesh.h"

namespace gb
{
    
    const ui32 ces_light_mask_component::k_max_num_vertices = 65535 / 4; // 16k vertices
    const ui32 ces_light_mask_component::k_max_num_indices = 65535 / 2;  // 32k indices
    const f32 ces_light_mask_component::k_bounds_trashhold = 1.75f;
    
    
    ces_light_mask_component::ces_light_mask_component() :
    m_radius(1.f),
    m_center(glm::vec2(0.f)),
    m_bounds(glm::vec4(0.f))
    {
        
#if !defined(__NO_RENDER__)
        
        vbo_shared_ptr vbo = std::make_shared<gb::vbo>(k_max_num_vertices, GL_DYNAMIC_DRAW);
        
#else
        
        vbo_shared_ptr vbo = std::make_shared<gb::vbo>(k_max_num_vertices, 0);
        
#endif
        vbo::vertex_attribute *vertices = vbo->lock();
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
        
        m_shadow_casters_vertices.resize(4, glm::vec2(0.f));
        m_shadow_casters_edges.resize(4, glm::vec4(0.f));
        
        m_mesh = std::make_shared<gb::mesh>(vbo, ibo);
    }
    
    ces_light_mask_component::~ces_light_mask_component()
    {
        
    }
    
    void ces_light_mask_component::update_bounds()
    {
        m_shadow_casters_vertices[0] = glm::vec2(m_center.x - m_radius * k_bounds_trashhold, m_center.y - m_radius * k_bounds_trashhold);
        m_shadow_casters_vertices[1] = glm::vec2(m_center.x + m_radius * k_bounds_trashhold, m_center.y - m_radius * k_bounds_trashhold);
        m_shadow_casters_vertices[2] = glm::vec2(m_center.x + m_radius * k_bounds_trashhold, m_center.y + m_radius * k_bounds_trashhold);
        m_shadow_casters_vertices[3] = glm::vec2(m_center.x - m_radius * k_bounds_trashhold, m_center.y + m_radius * k_bounds_trashhold);
        
        for(i32 i = 0; i < 4; ++i)
        {
            i32 next_index = (i + 1) % 4;
            m_shadow_casters_edges[i].x = m_shadow_casters_vertices[i].x;
            m_shadow_casters_edges[i].y = m_shadow_casters_vertices[i].y;
            m_shadow_casters_edges[i].z = m_shadow_casters_vertices[next_index].x;
            m_shadow_casters_edges[i].w = m_shadow_casters_vertices[next_index].y;
        }
        m_bounds.x = m_center.x - m_radius;
        m_bounds.y = m_center.y - m_radius;
        m_bounds.z = m_center.x + m_radius;
        m_bounds.w = m_center.y + m_radius;
    }
    
    void ces_light_mask_component::add_shadowcasters_geometry(const glm::mat4& shadow_caster_mat_m,
                                                              const std::vector<glm::vec2>& convex_hull_oriented_vertices)
    {
        bool is_shadow_geometry_inside = false;
        i32 convex_hull_oriented_vertices_count = static_cast<i32>(convex_hull_oriented_vertices.size());
        
        i32 shadow_casters_edges_count = static_cast<i32>(m_shadow_casters_edges.size());
        i32 shadow_casters_vertices_count = static_cast<i32>(m_shadow_casters_vertices.size());
        
        m_shadow_casters_edges.resize(shadow_casters_edges_count + convex_hull_oriented_vertices_count);
        m_shadow_casters_vertices.resize(shadow_casters_vertices_count + convex_hull_oriented_vertices_count);
        
        for(i32 i = 0, shadow_casters_edge_index = shadow_casters_edges_count,
            shadow_casters_vertex_index = shadow_casters_vertices_count;
            i < convex_hull_oriented_vertices_count; ++i)
        {
            i32 next_index = (i + 1) % convex_hull_oriented_vertices_count;
            
            glm::vec2 current_vertex = glm::transform(convex_hull_oriented_vertices[i], shadow_caster_mat_m);
            glm::vec2 next_vertex = glm::transform(convex_hull_oriented_vertices[next_index], shadow_caster_mat_m);
            
            m_shadow_casters_edges[shadow_casters_edge_index].x = current_vertex.x;
            m_shadow_casters_edges[shadow_casters_edge_index].y = current_vertex.y;
            m_shadow_casters_edges[shadow_casters_edge_index].z = next_vertex.x;
            m_shadow_casters_edges[shadow_casters_edge_index++].w = next_vertex.y;
            
            m_shadow_casters_vertices[shadow_casters_vertex_index].x = current_vertex.x;
            m_shadow_casters_vertices[shadow_casters_vertex_index++].y = current_vertex.y;
            
            if(glm::intersect(m_bounds, current_vertex))
            {
                is_shadow_geometry_inside = true;
            }
        }
        if(!is_shadow_geometry_inside)
        {
            m_shadow_casters_edges.resize(shadow_casters_edges_count);
            m_shadow_casters_vertices.resize(shadow_casters_vertices_count);
        }
    }
    
    void ces_light_mask_component::update_mesh()
    {
        i32 shadow_casters_vertices_count = static_cast<i32>(m_shadow_casters_vertices.size());
        std::vector<f32> angles;
        angles.resize(shadow_casters_vertices_count * 3);
        
        for(i32 i = 0, angle_index = 0; i < shadow_casters_vertices_count; ++i)
        {
            const glm::vec2& point = m_shadow_casters_vertices[i];
            f32 angle = atan2f(point.y - m_center.y, point.x - m_center.x);
            
            angles[angle_index++] = angle - .0001f;
            angles[angle_index++] = angle;
            angles[angle_index++] = angle + .0001f;
        }
        
        glm::vec2 direction;
        glm::vec4 ray;
        
        f32 closest_distance;
        glm::vec2 closest_intersection;
        
        f32 distance;
        glm::vec2 intersection;
        
        i32 shadow_casters_edges_count = static_cast<i32>(m_shadow_casters_edges.size());
        
        std::list<glm::vec3> intersections;
        for(auto angle : angles)
        {
            direction.x = cosf(angle);
            direction.y = sinf(angle);
            
            if(std::is_f32_equal(direction.x, 0.f) || std::is_f32_equal(direction.y, 0.f))
            {
                continue;
            }
            
            ray.x = m_center.x;
            ray.y = m_center.y;
            ray.z = m_center.x + direction.x;
            ray.w = m_center.y + direction.y;
            
            closest_distance = std::numeric_limits<f32>::max();
            closest_intersection.x = std::numeric_limits<f32>::min();
            closest_intersection.y = std::numeric_limits<f32>::min();
            
            for(i32 j = 0; j < shadow_casters_edges_count; ++j)
            {
                distance = std::numeric_limits<f32>::max();
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
            
            if(closest_intersection.x == std::numeric_limits<f32>::min() &&
               closest_intersection.y == std::numeric_limits<f32>::min())
            {
                continue;
            }
            intersections.push_back(glm::vec3(closest_intersection.x, closest_intersection.y, angle));
        }
        
        intersections.sort([](const glm::vec3& intersection_01, const glm::vec3& intersection_02) -> bool {
            return intersection_01.z < intersection_02.z;
        });
        
        vbo::vertex_attribute* vertices = m_mesh->get_vbo()->lock();
        ui16* indices = m_mesh->get_ibo()->lock();
        
        i32 vertices_offset = 0;
        i32 indices_offset = 0;
        
        vertices[vertices_offset].m_position.x = m_center.x;
        vertices[vertices_offset++].m_position.y = m_center.y;
        
        for(const auto& intersection : intersections)
        {
            vertices[vertices_offset].m_position.x = intersection.x;
            vertices[vertices_offset++].m_position.y = intersection.y;
        }
        
        for(i32 i = 1; i < intersections.size() + 1; ++i)
        {
            i32 next_vertex_index = std::max((i + 1) % vertices_offset, 1);
            indices[indices_offset++] = 0;
            indices[indices_offset++] = i;
            indices[indices_offset++] = next_vertex_index;
        }
        
        if(indices_offset % 3 == 0)
        {
            m_mesh->get_vbo()->unlock(vertices_offset);
            m_mesh->get_ibo()->unlock(indices_offset);
        }
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
