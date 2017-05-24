//
//  mesh_3d.h
//  gbCore
//
//  Created by serhii serhiiv on 5/12/17.
//  Copyright © 2017 sergey.sergeev. All rights reserved.
//

#pragma once

#include "resource.h"
#include "mesh_2d.h"
#include "resource_status.h"

namespace gb
{
    extern const ui32 k_max_bones;
    
    struct mesh_3d_bone_data
    {
        i32	m_id;
        f32 m_weigth;
    };
    
    struct mesh_3d_vertex_data
    {
        glm::vec3 m_position;
        glm::vec2 m_texcoord;
        glm::vec3 m_normal;
        glm::vec3 m_tangent;
        std::vector<mesh_3d_bone_data> m_bones;
    };
    
    class mesh_3d_transfering_data final : public resource_transfering_data
    {
    private:
        
    protected:
        
        mesh_3d_vertex_data* m_vertices;
        ui16* m_indices;
        
        ui32 m_num_vertices;
        ui32 m_num_indices;
        
        glm::vec3 m_min_bound;
        glm::vec3 m_max_bound;
        
    public:
        
        mesh_3d_transfering_data(mesh_3d_vertex_data* vertices, ui16* indices,
                                 ui32 num_vertices, ui32 num_indices,
                                 const glm::vec3& min_bound, const glm::vec3& max_bound);
        
        ~mesh_3d_transfering_data();
        
        const mesh_3d_vertex_data* get_vertices() const;
        const ui16* get_indices() const;
        
        ui32 get_num_vertices() const;
        ui32 get_num_indices() const;
        
        const glm::vec3 get_min_bound() const;
        const glm::vec3 get_max_bound() const;
    };
    
    class skeleton_3d_transfering_data final : public resource_transfering_data
    {
    private:
        
    protected:
        
        ui32 m_num_bones;
        std::vector<bone_3d_data_shared_ptr> m_bones_raw_data;
        
    public:
        
        skeleton_3d_transfering_data(ui32 num_bones);
        ~skeleton_3d_transfering_data();
        
        void add_bone(i32 id, i32 parent_id);
        ui32 get_num_bones() const;
        const std::vector<bone_3d_data_shared_ptr>& get_bones_raw_data() const;
    };
    
    template<typename T>
    class mesh_3d : public mesh_2d<T>
    {
    private:
        
    protected:
        
        mesh_3d_transfering_data_shared_ptr m_mesh_data;
        skeleton_3d_transfering_data_shared_ptr m_skeleton_data;
        sequence_3d_transfering_data_shared_ptr m_bindpose_data;
        
        glm::vec3 m_max_bound;
        glm::vec3 m_min_bound;
        
        void on_transfering_data_serialized(const std::shared_ptr<resource_transfering_data>& data)
        {
            assert(data != nullptr);
            switch(data->get_type())
            {
                case e_resource_transfering_data_type_mesh_3d:
                {
                    m_mesh_data = std::static_pointer_cast<mesh_3d_transfering_data>(data);
                    m_min_bound = m_mesh_data->get_min_bound();
                    m_max_bound = m_mesh_data->get_max_bound();
                    
                    this->m_status |= e_resource_status_loaded;
                }
                    break;
                    
                case e_resource_transfering_data_type_skeleton_3d:
                {
                    m_skeleton_data = std::static_pointer_cast<skeleton_3d_transfering_data>(data);
                }
                    break;
                    
                case e_resource_transfering_data_type_sequence:
                {
                    m_bindpose_data = std::static_pointer_cast<sequence_3d_transfering_data>(data);
                }
                    break;
                    
                default:
                {
                    assert(false);
                }
                    break;
            }
        };
        
        void on_transfering_data_commited(const std::shared_ptr<resource_transfering_data>& data)
        {
            assert(data != nullptr);
            switch(data->get_type())
            {
                case e_resource_transfering_data_type_vbo:
                {
                    this->m_vbo = std::static_pointer_cast<vbo<T>>(data);
                }
                    break;
                    
                case e_resource_transfering_data_type_ibo:
                {
                    this->m_ibo = std::static_pointer_cast<ibo>(data);
                }
                    break;
                    
                default:
                {
                    assert(false);
                }
                    break;
            }
            if(this->m_vbo != nullptr &&
               this->m_ibo != nullptr)
            {
                m_mesh_data = nullptr;
                this->m_status |= e_resource_status_commited;
            }
        };
        
    public:
        
        mesh_3d(const std::string& guid)  : mesh_2d<T>(e_resource_type_mesh_3d, guid),
        m_mesh_data(nullptr),
        m_skeleton_data(nullptr)
        {
            this->m_vbo = nullptr;
            this->m_ibo = nullptr,
            this->m_mode = GL_TRIANGLES;
        };
        
        static std::shared_ptr<mesh_3d<T>> construct(const std::string& guid,
                                                     const std::shared_ptr<gb::vbo<T>>& vbo,
                                                     const ibo_shared_ptr& ibo,
                                                     const glm::vec3& max_bound,
                                                     const glm::vec3& min_bound)
        {
            assert(vbo != nullptr);
            assert(ibo != nullptr);
            
            std::shared_ptr<mesh_3d<T>> mesh = mesh_3d<T>::construct(guid, vbo, ibo);
            mesh->m_min_bound = min_bound;
            mesh->m_max_bound = max_bound;
            return mesh;
        };
        
        static std::shared_ptr<mesh_3d<T>> construct(const std::string& guid,
                                                     const std::shared_ptr<gb::vbo<T>>& vbo,
                                                     const ibo_shared_ptr& ibo)
        {
            assert(vbo != nullptr);
            assert(ibo != nullptr);
            
            std::shared_ptr<mesh_3d<T>> mesh = std::make_shared<mesh_3d<T>>(guid);
            mesh->m_vbo = vbo;
            mesh->m_ibo = ibo;
            
            T *vertices = vbo->lock();
            ui16* indices = ibo->lock();
            ui32 indices_count = ibo->get_used_size();
            
            glm::vec3 max_bound = glm::vec3(INT16_MIN);
            glm::vec3 min_bound = glm::vec3(INT16_MAX);
            
            for(ui32 i = 0; i < indices_count; ++i)
            {
                glm::vec3 point = vertices[indices[i]].m_position;
                max_bound = glm::max(point, max_bound);
                min_bound = glm::min(point, min_bound);
            }
            mesh->m_min_bound = std::move(min_bound);
            mesh->m_max_bound = std::move(max_bound);
            
            mesh->m_status |= e_resource_status_loaded;
            mesh->m_status |= e_resource_status_commited;
            return mesh;
        };
        
        ~mesh_3d()
        {
            
        };
        
        std::shared_ptr<gb::vbo<T>> get_vbo() const
        {
            return resource::is_commited() ? this->m_vbo : nullptr;
        };
        
        ibo_shared_ptr get_ibo() const
        {
            return resource::is_commited() ? this->m_ibo : nullptr;
        };
        
        const glm::vec3 get_min_bound() const
        {
             return resource::is_loaded() ? m_min_bound : std::move(glm::vec3(0.f));
        };
        
        const glm::vec3 get_max_bound() const
        {
            return resource::is_loaded() ? m_max_bound : std::move(glm::vec3(0.f));
        };
        
        const mesh_3d_vertex_data* get_raw_vertices() const
        {
            return resource::is_loaded() ? m_mesh_data->get_vertices() : nullptr;
        };
        
        const ui16* get_raw_indices() const
        {
            return resource::is_loaded() ? m_mesh_data->get_indices() : nullptr;
        };
        
        ui32 get_num_raw_vertices() const
        {
            return resource::is_loaded() ? m_mesh_data->get_num_vertices() : 0;
        };
        
        ui32 get_num_raw_indices() const
        {
            return resource::is_loaded() ? m_mesh_data->get_num_indices() : 0;
        };
        
        const skeleton_3d_transfering_data_shared_ptr get_skeleton_data() const
        {
             return resource::is_loaded() ? m_skeleton_data : nullptr;
        };
        
        const sequence_3d_transfering_data_shared_ptr get_bindpose_data() const
        {
            return resource::is_loaded() ? m_bindpose_data : nullptr;
        };
        
        void bind(const std::string& attributes_guid, const std::array<i32, e_shader_attribute_max>& attributes)
        {
            if(resource::is_loaded() && resource::is_commited())
            {
                assert(attributes_guid.length() != 0);
                std::shared_ptr<vao> vao_state = this->m_vao_states[attributes_guid];
                if(!vao_state)
                {
                    vao_state = std::make_shared<vao>();
                    vao::bind(vao_state);
                    this->m_vbo->bind(attributes);
                    this->m_ibo->bind();
                    this->m_vao_states[attributes_guid] = vao_state;
                }
                else
                {
                    vao::bind(vao_state);
                }
            }
        };
        
        void draw() const
        {
            if(resource::is_loaded() && resource::is_commited())
            {
#if !defined(__NO_RENDER__)
                
                gl_draw_elements(GL_TRIANGLES, this->m_ibo->get_used_size(), GL_UNSIGNED_SHORT, NULL);
                
#endif
            }
        };
        
        void draw(ui32 indices) const
        {
            if(resource::is_loaded() && resource::is_commited())
            {
#if !defined(__NO_RENDER__)
                
                gl_draw_elements(GL_TRIANGLES, indices, GL_UNSIGNED_SHORT, NULL);
                
#endif
            }
        };
        
        void unbind(const std::string& attributes_guid, const std::array<i32, e_shader_attribute_max>& attributes)
        {
            if(resource::is_loaded() && resource::is_commited())
            {
                vao::unbind();
            }
        };
    };
};

