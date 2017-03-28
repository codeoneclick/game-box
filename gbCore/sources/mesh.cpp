//
//  mesh.cpp
//  gbCore
//
//  Created by sergey.sergeev on 8/13/15.
//  Copyright (c) 2015 sergey.sergeev. All rights reserved.
//

#include "mesh.h"
#include "resource_status.h"

namespace gb
{
#if !defined(__NO_RENDER__)

    mesh::mesh(const vbo_shared_ptr& vbo, const ibo_shared_ptr& ibo, GLenum mode) :

#else

	mesh::mesh(const vbo_shared_ptr& vbo, const ibo_shared_ptr& ibo, ui32 mode) :

#endif
    m_vbo(vbo),
    m_ibo(ibo),
    m_mode(mode)
    {
        
    }
    
    mesh::~mesh()
    {
        
    }
    
    vbo_shared_ptr mesh::get_vbo() const
    {
        return m_vbo;
    }
    
    ibo_shared_ptr mesh::get_ibo() const
    {
        return m_ibo;
    }
    
    ui32 mesh::get_id() const
    {
        return m_vbo->get_id();
    }
    
    void mesh::bind(const std::string& attributes_guid, const std::array<i32, e_shader_attribute_max>& attributes)
    {
        assert(attributes_guid.length() != 0);
        std::shared_ptr<vao> vao_state = m_vao_states[attributes_guid];
        if(!vao_state)
        {
            vao_state = std::make_shared<vao>();
            vao::bind(vao_state);
            m_vbo->bind(attributes);
            m_ibo->bind();
            m_vao_states[attributes_guid] = vao_state;
        }
        else
        {
            vao::bind(vao_state);
        }
    }
    
    void mesh::draw() const
    {
#if !defined(__NO_RENDER__)

        gl_draw_elements(m_mode, m_ibo->get_used_size(), GL_UNSIGNED_SHORT, NULL);
        
#endif
    }
    
    void mesh::draw(ui32 indices) const
    {
#if !defined(__NO_RENDER__)
        
        gl_draw_elements(m_mode, indices, GL_UNSIGNED_SHORT, NULL);
        
#endif
    }
    
    void mesh::unbind(const std::string& attributes_guid, const std::array<i32, e_shader_attribute_max>& attributes)
    {
        vao::unbind();
    }
    
    bool mesh::intersect(const vbo_shared_ptr& vbo_01, const ibo_shared_ptr& ibo_01, const glm::mat4& mat_m_01,
                         const vbo_shared_ptr& vbo_02, const ibo_shared_ptr& ibo_02, const glm::mat4& mat_m_02,
                         std::vector<glm::triangle>* out_triangles_01, std::vector<glm::triangle>* out_triangles_02)
    {
        std::vector<glm::triangle> triangles_01;
        std::vector<glm::triangle> triangles_02;
        
        if(ibo_01->get_used_size() % 3 == 0 && ibo_02->get_used_size() % 3 == 0)
        {
            vbo::vertex_attribute *vertices_01 = vbo_01->lock();
            ui16 *indices_01 = ibo_01->lock();
            triangles_01.resize(ibo_01->get_used_size() / 3);
            
            for(i32 i = 0, j = 0; i < ibo_01->get_used_size(); i += 3, ++j)
            {
                glm::transform_to_vec2_out(vertices_01[indices_01[i]].m_position, mat_m_01, &triangles_01[j].points[0]);
                glm::transform_to_vec2_out(vertices_01[indices_01[i + 1]].m_position, mat_m_01, &triangles_01[j].points[1]);
                glm::transform_to_vec2_out(vertices_01[indices_01[i + 2]].m_position, mat_m_01, &triangles_01[j].points[2]);
                
                triangles_01[j].segments[0].point_01 = triangles_01[j].points[0];
                triangles_01[j].segments[0].point_02 = triangles_01[j].points[1];
                
                triangles_01[j].segments[1].point_01 = triangles_01[j].points[1];
                triangles_01[j].segments[1].point_02 = triangles_01[j].points[2];
                
                triangles_01[j].segments[2].point_01 = triangles_01[j].points[2];
                triangles_01[j].segments[2].point_02 = triangles_01[j].points[0];
                
                /*triangles_01[j] = glm::triangle(glm::vec2(glm::transform(vertices_01[indices_01[i]].m_position, mat_m_01).x,
                                                          glm::transform(vertices_01[indices_01[i]].m_position, mat_m_01).y),
                                                glm::vec2(glm::transform(vertices_01[indices_01[i + 1]].m_position, mat_m_01).x,
                                                          glm::transform(vertices_01[indices_01[i + 1]].m_position, mat_m_01).y),
                                                glm::vec2(glm::transform(vertices_01[indices_01[i + 2]].m_position, mat_m_01).x,
                                                          glm::transform(vertices_01[indices_01[i + 2]].m_position, mat_m_01).y));*/
            }
            
            vbo::vertex_attribute *vertices_02 = vbo_02->lock();
            ui16 *indices_02 = ibo_02->lock();
            triangles_02.resize(ibo_02->get_used_size() / 3);
            
            for(i32 i = 0, j = 0; i < ibo_02->get_used_size(); i += 3, ++j)
            {
                glm::transform_to_vec2_out(vertices_02[indices_02[i]].m_position, mat_m_02, &triangles_02[j].points[0]);
                glm::transform_to_vec2_out(vertices_02[indices_02[i + 1]].m_position, mat_m_02, &triangles_02[j].points[1]);
                glm::transform_to_vec2_out(vertices_02[indices_02[i + 2]].m_position, mat_m_02, &triangles_02[j].points[2]);
                
                triangles_02[j].segments[0].point_01 = triangles_02[j].points[0];
                triangles_02[j].segments[0].point_02 = triangles_02[j].points[1];
                
                triangles_02[j].segments[1].point_01 = triangles_02[j].points[1];
                triangles_02[j].segments[1].point_02 = triangles_02[j].points[2];
                
                triangles_02[j].segments[2].point_01 = triangles_02[j].points[2];
                triangles_02[j].segments[2].point_02 = triangles_02[j].points[0];
                
                /*triangles_02[j] = glm::triangle(glm::vec2(glm::transform(vertices_02[indices_02[i]].m_position, mat_m_02).x,
                                                          glm::transform(vertices_02[indices_02[i]].m_position, mat_m_02).y),
                                                glm::vec2(glm::transform(vertices_02[indices_02[i + 1]].m_position, mat_m_02).x,
                                                          glm::transform(vertices_02[indices_02[i + 1]].m_position, mat_m_02).y),
                                                glm::vec2(glm::transform(vertices_02[indices_02[i + 2]].m_position, mat_m_02).x,
                                                          glm::transform(vertices_02[indices_02[i + 2]].m_position, mat_m_02).y));*/
            }
        }
        else
        {
            std::cout<<"wrong geomerty"<<std::endl;
            return false;
        }
        
        for(const auto& triangle_01 : triangles_01)
        {
            for(const auto& triangle_02 : triangles_02)
            {
                for(const auto& segment_01 : triangle_01.segments)
                {
                    for(const auto& segment_02 : triangle_02.segments)
                    {
                        if(glm::segments_intersect(segment_01, segment_02))
                        {
                            if(out_triangles_01)
                            {
                                *out_triangles_01 = std::move(triangles_01);
                            }
                            if(out_triangles_02)
                            {
                                *out_triangles_02 = std::move(triangles_02);
                            }
                            return true;
                        }
                    }
                }
                
                for(const auto& point_01 : triangle_01.points)
                {
                    if(glm::point_in_triangle(point_01, triangle_02))
                    {
                        if(out_triangles_01)
                        {
                            *out_triangles_01 = std::move(triangles_01);
                        }
                        if(out_triangles_02)
                        {
                            *out_triangles_02 = std::move(triangles_02);
                        }
                        return true;
                    }
                }
                
                for(const auto& point_02 : triangle_02.points)
                {
                    if(glm::point_in_triangle(point_02, triangle_01))
                    {
                        if(out_triangles_01)
                        {
                            *out_triangles_01 = std::move(triangles_01);
                        }
                        if(out_triangles_02)
                        {
                            *out_triangles_02 = std::move(triangles_02);
                        }
                        return true;
                    }
                }
            }
        }
        return false;
    }
}
