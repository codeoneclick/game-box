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
    const f32 ces_light_mask_component::k_bounds_trashhold = 2.f;
    
#define k_angle_trashhold .0001f
    
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
        
        auto shadow_caster_metadata = std::make_shared<gb::ces_light_mask_component::shadow_caster_metadata>();
        shadow_caster_metadata->m_shadow_casters_vertices.resize(4, glm::vec2(0.f));
        shadow_caster_metadata->m_shadow_casters_edges.resize(4, glm::vec4(0.f));
        m_shadow_casters_metadata.insert(std::make_pair(0, shadow_caster_metadata));
        
        m_mesh = std::make_shared<gb::mesh_2d>(vbo, ibo);
    }
    
    ces_light_mask_component::~ces_light_mask_component()
    {
        
    }
    
    void ces_light_mask_component::update_bounds()
    {
        auto shadow_caster_metadata = m_shadow_casters_metadata[0];
        shadow_caster_metadata->m_shadow_casters_vertices[0] = glm::vec2(m_center.x - m_radius * k_bounds_trashhold, m_center.y - m_radius * k_bounds_trashhold);
        shadow_caster_metadata->m_shadow_casters_vertices[1] = glm::vec2(m_center.x + m_radius * k_bounds_trashhold, m_center.y - m_radius * k_bounds_trashhold);
        shadow_caster_metadata->m_shadow_casters_vertices[2] = glm::vec2(m_center.x + m_radius * k_bounds_trashhold, m_center.y + m_radius * k_bounds_trashhold);
        shadow_caster_metadata->m_shadow_casters_vertices[3] = glm::vec2(m_center.x - m_radius * k_bounds_trashhold, m_center.y + m_radius * k_bounds_trashhold);
        
        for(i32 i = 0; i < 4; ++i)
        {
            i32 next_index = (i + 1) % 4;
            shadow_caster_metadata->m_shadow_casters_edges[i].x = shadow_caster_metadata->m_shadow_casters_vertices[i].x;
            shadow_caster_metadata->m_shadow_casters_edges[i].y = shadow_caster_metadata->m_shadow_casters_vertices[i].y;
            shadow_caster_metadata->m_shadow_casters_edges[i].z = shadow_caster_metadata->m_shadow_casters_vertices[next_index].x;
            shadow_caster_metadata->m_shadow_casters_edges[i].w = shadow_caster_metadata->m_shadow_casters_vertices[next_index].y;
        }
        m_bounds.x = m_center.x - m_radius * k_bounds_trashhold;
        m_bounds.y = m_center.y - m_radius * k_bounds_trashhold;
        m_bounds.z = m_center.x + m_radius * k_bounds_trashhold;
        m_bounds.w = m_center.y + m_radius * k_bounds_trashhold;
    }
    
    void ces_light_mask_component::add_shadowcasters_geometry(i32 id,
                                                              const ces_entity_shared_ptr& shadow_caster,
                                                              ui32 absolute_transform_matrix_version,
                                                              const glm::mat4& shadow_caster_mat_m,
                                                              const std::vector<glm::vec2>& convex_hull_oriented_vertices)
    {
        //bool is_shadow_geometry_inside = false;
        bool is_shadow_geometry_need_to_update = false;
       
        i32 convex_hull_oriented_vertices_count = static_cast<i32>(convex_hull_oriented_vertices.size());
        
        std::shared_ptr<gb::ces_light_mask_component::shadow_caster_metadata> shadow_caster_metadata = nullptr;
        auto shadow_caster_metadata_it = m_shadow_casters_metadata.find(id);
        if(shadow_caster_metadata_it != m_shadow_casters_metadata.end())
        {
            if(absolute_transform_matrix_version != shadow_caster_metadata_it->second->m_absolute_transform_matrix_version)
            {
                is_shadow_geometry_need_to_update = true;
            }
            shadow_caster_metadata = shadow_caster_metadata_it->second;
        }
        else
        {
            shadow_caster_metadata = std::make_shared<gb::ces_light_mask_component::shadow_caster_metadata>();
            shadow_caster_metadata->m_shadow_casters_vertices.resize(convex_hull_oriented_vertices_count, glm::vec2(0.f));
            shadow_caster_metadata->m_shadow_casters_edges.resize(convex_hull_oriented_vertices_count, glm::vec4(0.f));
            m_shadow_casters_metadata.insert(std::make_pair(id, shadow_caster_metadata));
            is_shadow_geometry_need_to_update = true;
        }
        
        //i32 shadow_casters_edges_count = static_cast<i32>(m_shadow_casters_edges.size());
        //i32 shadow_casters_vertices_count = static_cast<i32>(m_shadow_casters_vertices.size());
        
        //m_shadow_casters_edges.resize(shadow_casters_edges_count + convex_hull_oriented_vertices_count);
        //m_shadow_casters_vertices.resize(shadow_casters_vertices_count + convex_hull_oriented_vertices_count);
        
        if(is_shadow_geometry_need_to_update)
        {
            for(i32 i = 0; i < convex_hull_oriented_vertices_count; ++i)
            {
                i32 next_index = (i + 1) % convex_hull_oriented_vertices_count;
                
                glm::vec2 current_vertex = glm::transform(convex_hull_oriented_vertices[i], shadow_caster_mat_m);
                glm::vec2 next_vertex = glm::transform(convex_hull_oriented_vertices[next_index], shadow_caster_mat_m);
                
                shadow_caster_metadata->m_shadow_casters_edges[i].x = current_vertex.x;
                shadow_caster_metadata->m_shadow_casters_edges[i].y = current_vertex.y;
                shadow_caster_metadata->m_shadow_casters_edges[i].z = next_vertex.x;
                shadow_caster_metadata->m_shadow_casters_edges[i].w = next_vertex.y;
                
                shadow_caster_metadata->m_shadow_casters_vertices[i].x = current_vertex.x;
                shadow_caster_metadata->m_shadow_casters_vertices[i].y = current_vertex.y;
                
                shadow_caster_metadata->m_absolute_transform_matrix_version = absolute_transform_matrix_version;
                
                /*if(glm::intersect(m_bounds, current_vertex))
                {
                    is_shadow_geometry_inside = true;
                }*/
            }
        }
        
        /*if(!is_shadow_geometry_inside)
        {
            m_shadow_casters_edges.resize(shadow_casters_edges_count);
            m_shadow_casters_vertices.resize(shadow_casters_vertices_count);
        }*/
    }
    
    void ces_light_mask_component::update_mesh()
    {
        glm::vec2 direction;
        glm::vec4 ray;
        
        f32 closest_distance;
        glm::vec2 closest_intersection;
        
        f32 distance;
        glm::vec2 intersection;
        
        std::list<glm::vec3> intersections;
        //std::vector<f32> angles;
        for(const auto& shadow_caster_metadata : m_shadow_casters_metadata)
        {
            for(size_t i = 0; i < shadow_caster_metadata.second->m_shadow_casters_vertices.size(); ++i)
            {
                const glm::vec2& shadow_caster_vertex = shadow_caster_metadata.second->m_shadow_casters_vertices[i];
                f32 angle = atan2f(shadow_caster_vertex.y - m_center.y, shadow_caster_vertex.x - m_center.x) - k_angle_trashhold;
                for(i32 j = 0; j < 3; ++j, angle += k_angle_trashhold)
                {
                    direction.x = cosf(angle);
                    direction.y = sinf(angle);
                    
                    if(std::is_f32_equal(direction.x, 0.f) && std::is_f32_equal(direction.y, 0.f))
                    {
                        angle += k_angle_trashhold;
                        continue;
                    }
                    
                    ray.x = m_center.x;
                    ray.y = m_center.y;
                    ray.z = m_center.x + direction.x;
                    ray.w = m_center.y + direction.y;
                    
                    closest_distance = std::numeric_limits<f32>::max();
                    closest_intersection.x = std::numeric_limits<f32>::min();
                    closest_intersection.y = std::numeric_limits<f32>::min();
                    
                    for(const auto& shadow_caster_metadata : m_shadow_casters_metadata)
                    {
                        for(size_t k = 0; k < shadow_caster_metadata.second->m_shadow_casters_edges.size(); ++k)
                        {
                            const glm::vec4& shadow_caster_edge = shadow_caster_metadata.second->m_shadow_casters_edges[k];
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
                    }
                    
                    if(closest_intersection.x == std::numeric_limits<f32>::min() &&
                       closest_intersection.y == std::numeric_limits<f32>::min())
                    {
                        continue;
                    }
                    intersections.push_back(glm::vec3(closest_intersection.x, closest_intersection.y, angle));
                }
                //angles.push_back(angle - .0001f);
                //angles.push_back(angle);
                //angles.push_back(angle + .0001f);
            }
        }
        
        //i32 shadow_casters_edges_count = static_cast<i32>(m_shadow_casters_edges.size());
        
        
        /*for(auto angle : angles)
        {
            direction.x = cosf(angle);
            direction.y = sinf(angle);
            
            if(std::is_f32_equal(direction.x, 0.f) && std::is_f32_equal(direction.y, 0.f))
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
            
            for(const auto& shadow_caster_metadata : m_shadow_casters_metadata)
            {
                for(const auto& shadow_caster_edge : shadow_caster_metadata.second->m_shadow_casters_edges)
                {
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
            }
            
            if(closest_intersection.x == std::numeric_limits<f32>::min() &&
               closest_intersection.y == std::numeric_limits<f32>::min())
            {
                continue;
            }
            intersections.push_back(glm::vec3(closest_intersection.x, closest_intersection.y, angle));
        }*/
        
        intersections.sort([](const glm::vec3& intersection_01, const glm::vec3& intersection_02) -> bool {
            return intersection_01.z < intersection_02.z;
        });
        
        vbo::vertex_attribute_PTC* vertices = m_mesh->get_vbo()->lock<vbo::vertex_attribute_PTC>();
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
    
    mesh_2d_shared_ptr ces_light_mask_component::get_mesh() const
    {
        return m_mesh;
    }
    
    void ces_light_mask_component::cleanup()
    {
        //m_shadow_casters_vertices.resize(4);
        //m_shadow_casters_edges.resize(4);
    }
};
