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
    mesh_transfering_data::mesh_transfering_data(vbo::vertex_data* vertices, ui16* indices,
                                                 ui32 num_vertices, ui32 num_indices,
                                                 const glm::vec3& min_bound, const glm::vec3& max_bound) :
    m_vertices(vertices),
    m_indices(indices),
    m_num_vertices(num_vertices),
    m_num_indices(num_indices),
    m_min_bound(min_bound),
    m_max_bound(max_bound)
    {
        m_type = e_resource_transfering_data_type_mesh;
    }
    
    mesh_transfering_data::~mesh_transfering_data(void)
    {
        delete[] m_vertices;
        delete[] m_indices;
    }
    
    const vbo::vertex_data* mesh_transfering_data::get_vertices(void) const
    {
        return m_vertices;
    }
    
    const ui16* mesh_transfering_data::get_indices(void) const
    {
        return m_indices;
    }
    
    ui32 mesh_transfering_data::get_num_vertices(void) const
    {
        return m_num_vertices;
    }
    
    ui32 mesh_transfering_data::get_num_indices(void) const
    {
        return m_num_indices;
    }
    
    const glm::vec3 mesh_transfering_data::get_min_bound(void) const
    {
        return m_min_bound;
    }
    
    const glm::vec3 mesh_transfering_data::get_max_bound(void) const
    {
        return m_max_bound;
    }
    
    skeleton_transfering_data::skeleton_transfering_data(ui32 num_bones) :
    m_num_bones(num_bones)
    {
        m_type = e_resource_transfering_data_type_skeleton;
    }
    
    skeleton_transfering_data::~skeleton_transfering_data(void)
    {
        m_bones_data.clear();
    }
    
    void skeleton_transfering_data::add_bone(i32 id, i32 parent_id)
    {
        m_bones_data.push_back(std::make_shared<bone_data>(id, parent_id));
    }
    
    ui32 skeleton_transfering_data::get_num_bones(void) const
    {
        return m_num_bones;
    }
    
    const std::vector<std::shared_ptr<bone_data>> skeleton_transfering_data::get_bones_data(void) const
    {
        return m_bones_data;
    }
    
    mesh::mesh(const std::string& guid, GLenum mode) :
    gb::resource(e_resource_type_mesh, guid),
    m_vbo(nullptr),
    m_ibo(nullptr),
    m_mesh_data(nullptr),
    m_skeleton_data(nullptr),
    m_mode(mode)
    {
        
    }
    
    std::shared_ptr<mesh> mesh::construct(const std::string& guid,
                                          const std::shared_ptr<vbo>& vbo,
                                          const std::shared_ptr<ibo>& ibo,
                                          const glm::vec3& min_bound,
                                          const glm::vec3& max_bound,
                                          GLenum mode)
    {
        assert(vbo != nullptr);
        assert(ibo != nullptr);
        
        std::shared_ptr<mesh> mesh = mesh::construct(guid, vbo, ibo, mode);
        mesh->m_min_bound = min_bound;
        mesh->m_max_bound = max_bound;
        return mesh;
    }
    
    std::shared_ptr<mesh> mesh::construct(const std::string &guid,
                                          const std::shared_ptr<vbo>& vbo,
                                          const std::shared_ptr<ibo>& ibo,
                                          GLenum mode)
    {
        assert(vbo != nullptr);
        assert(ibo != nullptr);
        
        std::shared_ptr<mesh> mesh = std::make_shared<gb::mesh>(guid, mode);
        mesh->m_vbo = vbo;
        mesh->m_ibo = ibo;
        
        vbo::vertex_attribute *vertices = vbo->lock();
        ui16* indices = ibo->lock();
        ui32 num_indices = ibo->get_used_size();
        
        glm::vec3 min_bound = glm::vec3(INT16_MAX);
        glm::vec3 max_bound = glm::vec3(INT16_MIN);
        
        for(ui32 i = 0; i < num_indices; ++i)
        {
            glm::vec3 point = vertices[indices[i]].m_position;
            min_bound = glm::min(point, min_bound);
            max_bound = glm::max(point, max_bound);
        }
        mesh->m_min_bound = std::move(min_bound);
        mesh->m_max_bound = std::move(max_bound);
        
        mesh->m_status |= e_resource_status_loaded;
        mesh->m_status |= e_resource_status_commited;
        return mesh;
    }
    
    mesh::~mesh(void)
    {
        
    }
    
    void mesh::on_transfering_data_serialized(const std::shared_ptr<resource_transfering_data> &data)
    {
        assert(data != nullptr);
        switch(data->get_type())
        {
            case e_resource_transfering_data_type_mesh:
            {
                m_mesh_data = std::static_pointer_cast<mesh_transfering_data>(data);
                m_min_bound = m_mesh_data->get_min_bound();
                m_max_bound = m_mesh_data->get_max_bound();
                
                m_status |= e_resource_status_loaded;
            }
                break;
                
            case e_resource_transfering_data_type_skeleton:
            {
                m_skeleton_data = std::static_pointer_cast<skeleton_transfering_data>(data);
            }
                break;
                
            case e_resource_transfering_data_type_sequence:
            {
                m_bindpose_data = std::static_pointer_cast<sequence_transfering_data>(data);
            }
                break;
                
            default:
            {
                assert(false);
            }
                break;
        }
    }
    
    void mesh::on_transfering_data_commited(const std::shared_ptr<resource_transfering_data> &data)
    {
        assert(data != nullptr);
        switch(data->get_type())
        {
            case e_resource_transfering_data_type_vb:
            {
                m_vbo = std::static_pointer_cast<vbo>(data);
            }
                break;
                
            case e_resource_transfering_data_type_ib:
            {
                m_ibo = std::static_pointer_cast<ibo>(data);
            }
                break;
                
            default:
            {
                assert(false);
            }
                break;
        }
        if(m_vbo != nullptr && m_ibo != nullptr)
        {
            m_mesh_data = nullptr;
            m_status |= e_resource_status_commited;
        }
    }
    
    std::shared_ptr<vbo> mesh::get_vbo(void) const
    {
        return resource::is_commited() ? m_vbo : nullptr;
    }
    
    std::shared_ptr<ibo> mesh::get_ibo(void) const
    {
        return resource::is_commited() ? m_ibo : nullptr;
    }
    
    const vbo::vertex_data* mesh::get_raw_vertices(void) const
    {
        return resource::is_loaded() ? m_mesh_data->get_vertices() : nullptr;
    }
    
    const ui16* mesh::get_raw_indices(void) const
    {
        return resource::is_loaded() ? m_mesh_data->get_indices() : nullptr;
    }
    
    ui32 mesh::get_num_raw_vertices(void) const
    {
        return resource::is_loaded() ? m_mesh_data->get_num_vertices() : 0;
    }
    
    ui32 mesh::get_num_raw_indices(void) const
    {
        return resource::is_loaded() ? m_mesh_data->get_num_indices() : 0;
    }
    
    const glm::vec3 mesh::get_min_bound() const
    {
        if(!resource::is_loaded())
        {
            std::cout<<"warning! mesh doesn't loaded - bounds is wrong."<<std::endl;
            return glm::vec3(.0f);
        }
        return m_min_bound;
    }
    
    const glm::vec3 mesh::get_max_bound() const
    {
        if(!resource::is_loaded())
        {
            std::cout<<"warning! mesh doesn't loaded - bounds is wrong."<<std::endl;
            return glm::vec3(.0f);
        }
        return m_max_bound;
    }
    
    const std::tuple<glm::vec3, glm::vec3> mesh::get_bounds() const
    {
        if(!resource::is_loaded())
        {
            std::cout<<"warning! mesh doesn't loaded - bounds is wrong."<<std::endl;
            return std::make_tuple(glm::vec3(0.f), glm::vec3(0.f));
        }
        return std::make_tuple(m_min_bound, m_max_bound);
    }
    
    const glm::vec3 mesh::get_min_bound(const glm::mat4& mat) const
    {
        if(!resource::is_loaded())
        {
            std::cout<<"warning! mesh doesn't loaded - bounds is wrong."<<std::endl;
            return glm::vec3(.0f);
        }
        return std::get<0>(mesh::get_bounds(mat));
    }
    
    const glm::vec3 mesh::get_max_bound(const glm::mat4& mat) const
    {
        if(!resource::is_loaded())
        {
            std::cout<<"warning! mesh doesn't loaded - bounds is wrong."<<std::endl;
            return glm::vec3(.0f);
        }
        return std::get<1>(mesh::get_bounds(mat));
    }
    
    const std::tuple<glm::vec3, glm::vec3> mesh::get_bounds(const glm::mat4& mat) const
    {
        if(!resource::is_loaded())
        {
            std::cout<<"warning! mesh doesn't loaded - bounds is wrong."<<std::endl;
            return std::make_tuple(glm::vec3(0.f), glm::vec3(0.f));
        }
        
        std::cout<<"warning! low performance function - do not call it often."<<std::endl;
        vbo::vertex_attribute *vertices = m_vbo->lock();
        ui16* indices = m_ibo->lock();
        ui32 num_indices = m_ibo->get_used_size();
        
        glm::vec3 min_bound = glm::vec3(INT16_MAX);
        glm::vec3 max_bound = glm::vec3(INT16_MIN);
        
        for(ui32 i = 0; i < num_indices; ++i)
        {
            glm::vec3 point = vertices[indices[i]].m_position;
            glm::vec4 tranformed_vertex = mat * glm::vec4(point, 1.0f);
            
            min_bound = glm::min(glm::vec3(tranformed_vertex.x, tranformed_vertex.y, tranformed_vertex.z), min_bound);
            max_bound = glm::max(glm::vec3(tranformed_vertex.x, tranformed_vertex.y, tranformed_vertex.z), max_bound);
            
        }
        return std::make_tuple(min_bound, max_bound);
    }
    
    const std::shared_ptr<skeleton_transfering_data> mesh::get_skeleton_data(void) const
    {
        return resource::is_loaded() ? m_skeleton_data : nullptr;
    }
    
    const std::shared_ptr<sequence_transfering_data> mesh::get_bindpose_data(void) const
    {
        return resource::is_loaded() ? m_bindpose_data : nullptr;
    }
    
    void mesh::bind(const std::string& attributes_guid, const std::array<i32, e_shader_attribute_max>& attributes)
    {
        if(resource::is_loaded() && resource::is_commited())
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
    }
    
    void mesh::draw() const
    {
        if(resource::is_loaded() && resource::is_commited())
        {
            gl_draw_elements(m_mode, m_ibo->get_used_size(), GL_UNSIGNED_SHORT, NULL);
        }
    }
    
    void mesh::draw(ui32 indices) const
    {
        if(resource::is_loaded() && resource::is_commited())
        {
            gl_draw_elements(m_mode, indices, GL_UNSIGNED_SHORT, NULL);
        }
    }
    
    void mesh::unbind(const std::string& attributes_guid, const std::array<i32, e_shader_attribute_max>& attributes)
    {
        if(resource::is_loaded() && resource::is_commited())
        {
            vao::unbind();
        }
    }
}