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
#include "thread_operation.h"

namespace gb
{
    const ui32 ces_light_mask_component::k_max_num_vertices = 65535 / 4; // 16k vertices
    const ui32 ces_light_mask_component::k_max_num_indices = 65535 / 2;  // 32k indices
    const f32 ces_light_mask_component::k_bounds_trashhold = 1.33f;
    
    ces_light_mask_component::ces_light_mask_component() :
    m_radius(1.f),
    m_center(glm::vec2(0.f)),
    m_bounds(glm::vec4(0.f))
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
        m_mesh = std::make_shared<gb::mesh_2d>(vbo, ibo);
    }
    
    ces_light_mask_component::~ces_light_mask_component()
    {
        
    }
    
    void ces_light_mask_component::update_bounds()
    {
        m_bounds.x = m_center.x - m_radius * k_bounds_trashhold;
        m_bounds.y = m_center.y - m_radius * k_bounds_trashhold;
        m_bounds.z = m_center.x + m_radius * k_bounds_trashhold;
        m_bounds.w = m_center.y + m_radius * k_bounds_trashhold;
        
        m_bounds_geometry[0].x = m_bounds.x;
        m_bounds_geometry[0].y = m_bounds.y;
        m_bounds_geometry[1].x = m_bounds.z;
        m_bounds_geometry[1].y = m_bounds.y;
        m_bounds_geometry[2].x = m_bounds.z;
        m_bounds_geometry[2].y = m_bounds.w;
        m_bounds_geometry[3].x = m_bounds.x;
        m_bounds_geometry[3].y = m_bounds.w;
    }
    
    bool ces_light_mask_component::is_shadowcaster_affect(const std::vector<glm::vec2>& convex_hull_oriented_vertices) const
    {
        bool is_shadowcaster_geometry_inside = false;
        for(const auto& vertex : convex_hull_oriented_vertices)
        {
            if(glm::intersect(m_bounds, vertex))
            {
                is_shadowcaster_geometry_inside = true;
                break;
            }
        }
        return is_shadowcaster_geometry_inside;
    }
    
    void ces_light_mask_component::push_shadowcasters_geometry(const std::vector<std::vector<glm::vec2>>& shadowcasters_geometry)
    {
        std::lock_guard<std::mutex> guard(m_shadow_geometry_apply_mutex);
        m_shadow_geometry_to_apply_queue.push(std::make_tuple(m_center, m_bounds_geometry, shadowcasters_geometry));
    }
    
    void ces_light_mask_component::apply_shadowcasters_geometry()
    {
        glm::vec2 center;
        std::array<glm::vec2, 4> bounds;
        std::vector<std::vector<glm::vec2>> shadowcasters_geometry;
        while(!m_shadow_geometry_to_apply_queue.empty())
        {
            std::lock_guard<std::mutex> guard(m_shadow_geometry_apply_mutex);
            center = std::get<0>(m_shadow_geometry_to_apply_queue.front());
            bounds = std::get<1>(m_shadow_geometry_to_apply_queue.front());
            shadowcasters_geometry = std::get<2>(m_shadow_geometry_to_apply_queue.front());
            m_shadow_geometry_to_apply_queue.pop();
        }
        
        if(shadowcasters_geometry.size() != 0)
        {
            std::set<f32> sorted_raytrace_angles;
            std::vector<glm::vec4> shadowcasters_edges;
            
            i32 shadowcasters_geometry_vertices_count = 0;
            for(const auto& shadowcaster_geometry : shadowcasters_geometry)
            {
                shadowcasters_geometry_vertices_count += shadowcaster_geometry.size();
            }
            shadowcasters_edges.resize(4 + shadowcasters_geometry_vertices_count);
            
            for(i32 i = 0; i < 4; ++i)
            {
                f32 angle = atan2f(bounds[i].y - center.y, bounds[i].x - center.x) - .0001f;
                for(i32 j = 0; j < 3; ++j, angle += .0001f)
                {
                    sorted_raytrace_angles.insert(angle);
                }
                i32 next_index = (i + 1) % 4;
                shadowcasters_edges[i].x = bounds[i].x;
                shadowcasters_edges[i].y = bounds[i].y;
                shadowcasters_edges[i].z = bounds[next_index].x;
                shadowcasters_edges[i].w = bounds[next_index].y;
            }
            
            i32 shadow_casters_edge_index = 4;
            for(const auto& shadowcaster_geometry : shadowcasters_geometry)
            {
                i32 shadowcaster_geometry_vertices_count = static_cast<i32>(shadowcaster_geometry.size());
                glm::vec2 current_vertex = shadowcaster_geometry[0];
                
                for(i32 i = 0; i < shadowcaster_geometry_vertices_count; ++i)
                {
                    i32 next_index = (i + 1) % shadowcaster_geometry_vertices_count;
                    glm::vec2 next_vertex = shadowcaster_geometry[next_index];
                    
                    shadowcasters_edges[shadow_casters_edge_index].x = current_vertex.x;
                    shadowcasters_edges[shadow_casters_edge_index].y = current_vertex.y;
                    shadowcasters_edges[shadow_casters_edge_index].z = next_vertex.x;
                    shadowcasters_edges[shadow_casters_edge_index++].w = next_vertex.y;
                    
                    f32 angle = atan2f(current_vertex.y - center.y, current_vertex.x - center.x) - .0001f;
                    for(i32 j = 0; j < 3; ++j, angle += .0001f)
                    {
                        sorted_raytrace_angles.insert(angle);
                    }
                    current_vertex.x = next_vertex.x;
                    current_vertex.y = next_vertex.y;
                }
            }
            
            i32 shadow_casters_edges_count = static_cast<i32>(shadowcasters_edges.size());
            
            glm::vec2 direction;
            glm::vec4 ray;
            ray.x = center.x;
            ray.y = center.y;
            
            f32 closest_distance;
            glm::vec2 closest_intersection;
            
            f32 distance;
            glm::vec2 intersection;
            i32 intersections_count = 0;
            
            vbo::vertex_attribute_PTC* vertices = m_mesh->get_vbo()->lock<vbo::vertex_attribute_PTC>();
            i32 vertices_count = 0;
            vertices[vertices_count].m_position.x = center.x;
            vertices[vertices_count++].m_position.y = center.y;
            
            std::for_each(sorted_raytrace_angles.begin(), sorted_raytrace_angles.end(), [&](f32 angle) {
                
                direction.x = cosf(angle);
                direction.y = sinf(angle);
                
                ray.z = center.x + direction.x;
                ray.w = center.y + direction.y;
                
                closest_distance = std::numeric_limits<f32>::max();
                closest_intersection.x = std::numeric_limits<f32>::min();
                closest_intersection.y = std::numeric_limits<f32>::min();
                
                for(i32 j = 0; j < shadow_casters_edges_count; ++j)
                {
                    const glm::vec4& shadowcaster_edge = shadowcasters_edges[j];
                    distance = std::numeric_limits<f32>::max();
                    bool is_intersected = glm::intersect(ray, shadowcaster_edge, &intersection, &distance);
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
                    vertices[vertices_count].m_position.x = closest_intersection.x;
                    vertices[vertices_count++].m_position.y = closest_intersection.y;
                    intersections_count++;
                }
            });
            
            ui16* indices = m_mesh->get_ibo()->lock();
            
            vertices_count = static_cast<i32>(intersections_count) + 1;
            i32 indices_count = 0;
            
            for(i32 i = 0; i < intersections_count; ++i)
            {
                i32 next_vertex_index = std::max((i + 2) % vertices_count, 1);
                indices[indices_count++] = 0;
                indices[indices_count++] = i + 1;
                indices[indices_count++] = next_vertex_index;
            }
            
            if(indices_count % 3 == 0)
            {
                gb::thread_operation_shared_ptr operation = std::make_shared<gb::thread_operation>(gb::thread_operation::e_thread_operation_queue_main);
                operation->set_execution_callback([this, vertices_count, indices_count]() {
                    m_mesh->get_vbo()->unlock(vertices_count);
                    m_mesh->get_ibo()->unlock(indices_count);
                });
                operation->add_to_execution_queue();
            }
        }
    }
    
    mesh_2d_shared_ptr ces_light_mask_component::get_mesh() const
    {
        return m_mesh;
    }
    
    void ces_light_mask_component::push_inside_outside_request(const std::vector<ces_entity_weak_ptr>& entities_source,
                                                               const inside_outside_result_callback_t& callback)
    {
        std::lock_guard<std::mutex> guard(m_inside_outside_request_mutex);
        m_inside_outside_requests_queue.push(std::make_pair(entities_source, callback));
    }
    
    std::pair<std::vector<ces_entity_weak_ptr>, ces_light_mask_component::inside_outside_result_callback_t> ces_light_mask_component::pop_inside_outside_request()
    {
        std::pair<std::vector<ces_entity_weak_ptr>, ces_light_mask_component::inside_outside_result_callback_t> request;
        while(!m_inside_outside_requests_queue.empty())
        {
            std::lock_guard<std::mutex> guard(m_inside_outside_request_mutex);
            request = m_inside_outside_requests_queue.front();
            m_inside_outside_requests_queue.pop();
        }
        return request;
    }
    
    bool ces_light_mask_component::is_inside_outside_requests_exist() const
    {
        return !m_inside_outside_requests_queue.empty();
    }
};
