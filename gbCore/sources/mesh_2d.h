//
//  mesh.h
//  gbCore
//
//  Created by sergey.sergeev on 8/13/15.
//  Copyright (c) 2015 sergey.sergeev. All rights reserved.
//

#pragma once

#include "resource.h"
#include "vbo.h"
#include "ibo.h"
#include "vao.h"
#include "shader.h"
#include "glm_extensions.h"

namespace gb
{
    template<class T>
    class mesh_2d : public resource
    {
    private:
        
    protected:
        
        std::shared_ptr<gb::vbo<T>> m_vbo;
        ibo_shared_ptr m_ibo;
        
        std::unordered_map<std::string, std::shared_ptr<vao>> m_vao_states;
        
        ui32 m_mode;
        
        virtual void on_transfering_data_serialized(const std::shared_ptr<resource_transfering_data>& data)
        {
            assert(false);
        };
        
        virtual void on_transfering_data_commited(const std::shared_ptr<resource_transfering_data>& data)
        {
            assert(false);
        };
        
    public:
        
        template<class T_COPY>
        mesh_2d(const mesh_2d<T_COPY>& copy) :
        m_vbo(std::static_pointer_cast<T_COPY>(copy->m_vbo)),
        m_ibo(copy->m_ibo),
        m_vao_states(copy->m_vao_states),
        m_mode(copy->m_mode)
        {
            
        }
        
        mesh_2d(e_resource_type type, const std::string& guid): resource(type, guid),
        m_vbo(nullptr),
        m_ibo(nullptr),
        m_mode(GL_TRIANGLES)
        {
            
        }
        
#if !defined(__NO_RENDER__)
        
        mesh_2d(const std::shared_ptr<gb::vbo<T>>& vbo, const ibo_shared_ptr& ibo, GLenum mode = GL_TRIANGLES) : resource(e_resource_type_mesh_2d, ""),
        
#else
        
        mesh_2d(const std::shared_ptr<gb::vbo<T>>& vbo, const ibo_shared_ptr& ibo, ui32 mode = GL_TRIANGLES) : resource(e_resource_type_mesh_2d, ""),
        
#endif
        m_vbo(vbo),
        m_ibo(ibo),
        m_mode(mode)
        {
            
        };
        
        ~mesh_2d()
        {
            
        };
        
        ui32 get_id() const
        {
            return m_vbo->get_id();
        };
        
        std::shared_ptr<gb::vbo<T>> get_vbo() const
        {
            return m_vbo;
        };
        
        ibo_shared_ptr get_ibo() const
        {
            return m_ibo;
        };
        
        virtual void bind(const std::string& attributes_guid, const std::array<i32, e_shader_attribute_max>& attributes)
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
        };
        
        virtual void draw() const
        {
#if !defined(__NO_RENDER__)
            
            gl_draw_elements(m_mode, m_ibo->get_used_size(), GL_UNSIGNED_SHORT, NULL);
            
#endif
        };
        
        virtual void draw(ui32 indices) const
        {
#if !defined(__NO_RENDER__)
            
            gl_draw_elements(m_mode, indices, GL_UNSIGNED_SHORT, NULL);
            
#endif
        };
        
        virtual void unbind(const std::string& attributes_guid, const std::array<i32, e_shader_attribute_max>& attributes)
        {
            vao::unbind();
        };
        
        template<class T1, class T2>
        static bool intersect(const std::shared_ptr<gb::vbo<T1>>& vbo_01, const ibo_shared_ptr& ibo_01, const glm::mat4& mat_m_01, bool use_mat_m_01,
                              const std::shared_ptr<gb::vbo<T2>>& vbo_02, const ibo_shared_ptr& ibo_02, const glm::mat4& mat_m_02, bool use_mat_m_02,
                              std::vector<glm::triangle>* out_triangles_01 = nullptr, std::vector<glm::triangle>* out_triangles_02 = nullptr)
        {
            std::vector<glm::triangle> triangles_01;
            std::vector<glm::triangle> triangles_02;
            
            if(ibo_01->get_used_size() % 3 == 0 && ibo_02->get_used_size() % 3 == 0)
            {
                T1 *vertices_01 = vbo_01->lock();
                ui16 *indices_01 = ibo_01->lock();
                triangles_01.resize(ibo_01->get_used_size() / 3);
                
                for(i32 i = 0, j = 0; i < ibo_01->get_used_size(); i += 3, ++j)
                {
                    if(use_mat_m_01)
                    {
                        glm::transform_to_vec2_out(vertices_01[indices_01[i]].position, mat_m_01, &triangles_01[j].points[0]);
                        glm::transform_to_vec2_out(vertices_01[indices_01[i + 1]].position, mat_m_01, &triangles_01[j].points[1]);
                        glm::transform_to_vec2_out(vertices_01[indices_01[i + 2]].position, mat_m_01, &triangles_01[j].points[2]);
                    }
                    else
                    {
                        glm::vec3 position = vertices_01[indices_01[i]].position;
                        triangles_01[j].points[0].x = position.x;
                        triangles_01[j].points[0].y = position.y;
                        
                        position = vertices_01[indices_01[i + 1]].position;
                        triangles_01[j].points[1].x = position.x;
                        triangles_01[j].points[1].y = position.y;
                        
                        position = vertices_01[indices_01[i + 2]].position;
                        triangles_01[j].points[2].x = position.x;
                        triangles_01[j].points[2].y = position.y;
                    }
                    
                    triangles_01[j].segments[0].point_01 = triangles_01[j].points[0];
                    triangles_01[j].segments[0].point_02 = triangles_01[j].points[1];
                    
                    triangles_01[j].segments[1].point_01 = triangles_01[j].points[1];
                    triangles_01[j].segments[1].point_02 = triangles_01[j].points[2];
                    
                    triangles_01[j].segments[2].point_01 = triangles_01[j].points[2];
                    triangles_01[j].segments[2].point_02 = triangles_01[j].points[0];
                }
                
                T2 *vertices_02 = vbo_02->lock();
                ui16 *indices_02 = ibo_02->lock();
                triangles_02.resize(ibo_02->get_used_size() / 3);
                
                for(i32 i = 0, j = 0; i < ibo_02->get_used_size(); i += 3, ++j)
                {
                    if(use_mat_m_02)
                    {
                        glm::transform_to_vec2_out(vertices_02[indices_02[i]].position, mat_m_02, &triangles_02[j].points[0]);
                        glm::transform_to_vec2_out(vertices_02[indices_02[i + 1]].position, mat_m_02, &triangles_02[j].points[1]);
                        glm::transform_to_vec2_out(vertices_02[indices_02[i + 2]].position, mat_m_02, &triangles_02[j].points[2]);
                    }
                    else
                    {
                        glm::vec3 position = vertices_02[indices_02[i]].position;
                        triangles_02[j].points[0].x = position.x;
                        triangles_02[j].points[0].y = position.y;
                        
                        position = vertices_02[indices_02[i + 1]].position;
                        triangles_02[j].points[1].x = position.x;
                        triangles_02[j].points[1].y = position.y;
                        
                        position = vertices_02[indices_02[i + 2]].position;
                        triangles_02[j].points[2].x = position.x;
                        triangles_02[j].points[2].y = position.y;
                    }
                    
                    triangles_02[j].segments[0].point_01 = triangles_02[j].points[0];
                    triangles_02[j].segments[0].point_02 = triangles_02[j].points[1];
                    
                    triangles_02[j].segments[1].point_01 = triangles_02[j].points[1];
                    triangles_02[j].segments[1].point_02 = triangles_02[j].points[2];
                    
                    triangles_02[j].segments[2].point_01 = triangles_02[j].points[2];
                    triangles_02[j].segments[2].point_02 = triangles_02[j].points[0];
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
        };
    };
};
