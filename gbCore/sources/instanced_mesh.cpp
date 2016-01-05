//
//  instanced_mesh.cpp
//  gbCore
//
//  Created by sergey.sergeev on 10/1/15.
//  Copyright (c) 2015 sergey.sergeev. All rights reserved.
//

#include "instanced_mesh.h"
#include "resource_status.h"

namespace gb
{
    instanced_mesh::instanced_mesh(const std::string& guid, i32 num_instances, GLenum mode) :
    gb::mesh(guid, mode),
    m_num_instances(std::max(1, num_instances))
    {
        
    }
    
    std::shared_ptr<instanced_mesh> instanced_mesh::construct(const std::string& guid,
                                                              const vbo_shared_ptr& vbo,
                                                              const ibo_shared_ptr& ibo,
                                                              const glm::vec3& min_bound,
                                                              const glm::vec3& max_bound,
                                                              i32 num_instances,
                                                              GLenum mode)
    {
        assert(vbo != nullptr);
        assert(ibo != nullptr);
        
        instanced_mesh_shared_ptr mesh = instanced_mesh::construct(guid, vbo, ibo, num_instances, mode);
        mesh->m_min_bound = min_bound;
        mesh->m_max_bound = max_bound;
        return mesh;
    }
    
    std::shared_ptr<instanced_mesh> instanced_mesh::construct(const std::string& guid,
                                                              const vbo_shared_ptr& vbo,
                                                              const ibo_shared_ptr& ibo,
                                                              i32 num_instances,
                                                              GLenum mode)
    {
        assert(vbo != nullptr);
        assert(ibo != nullptr);
        
        instanced_mesh_shared_ptr mesh = std::make_shared<gb::instanced_mesh>(guid, num_instances, mode);
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
    
    instanced_mesh::~instanced_mesh()
    {
        
    }
    
    i32 instanced_mesh::get_num_instances() const
    {
        return m_num_instances;
    }
    
    void instanced_mesh::set_num_instances(i32 num_instances)
    {
        m_num_instances = num_instances;
    }
    
    void instanced_mesh::draw() const
    {
        if(resource::is_loaded() && resource::is_commited())
        {
            gl_draw_elements_instanced(m_mode, m_ibo->get_used_size(), GL_UNSIGNED_SHORT, NULL, m_num_instances);
        }
    }
    
    void instanced_mesh::draw(ui32 indices) const
    {
        if(resource::is_loaded() && resource::is_commited())
        {
            gl_draw_elements_instanced(m_mode, indices, GL_UNSIGNED_SHORT, NULL, m_num_instances);
        }
    }
}