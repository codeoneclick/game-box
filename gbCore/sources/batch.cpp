//
//  batch.cpp
//  gbCore
//
//  Created by serhii serhiiv on 10/6/16.
//  Copyright © 2016 sergey.sergeev. All rights reserved.
//

#include "batch.h"
#include "material.h"
#include "vbo.h"
#include "ibo.h"
#include "mesh.h"
#include "glm_extensions.h"

namespace gb
{
    const ui32 batch::k_max_num_vertices = 65535 / 4; // 16k vertices
    const ui32 batch::k_max_num_indices = 65535 / 2;  // 32k indices
    
    batch::batch(const material_shared_ptr& material) :
    m_material(material),
    m_num_vertices_in_batch(0),
    m_num_indices_in_batch(0)
    {
        m_guid = m_material->get_guid();
        
        vbo_shared_ptr vbo = std::make_shared<gb::vbo>(k_max_num_vertices, GL_DYNAMIC_DRAW);
        vbo::vertex_attribute *vertices = vbo->lock();
        memset(vertices, 0x0, k_max_num_vertices * sizeof(vbo::vertex_attribute));
        vbo->unlock();
        
        ibo_shared_ptr ibo = std::make_shared<gb::ibo>(k_max_num_indices, GL_DYNAMIC_DRAW);
        ui16* indices = ibo->lock();
        memset(indices, 0x0, k_max_num_indices * sizeof(ui16));
        ibo->unlock();
        
        m_main_mesh = std::make_shared<gb::mesh>(vbo, ibo);
    }
    
    batch::~batch()
    {
        
    }
    
    void batch::add(const mesh_shared_ptr& mesh, const glm::mat4& matrix)
    {
        m_meshes.push_back(mesh);
        m_matrices.push_back(matrix);
        
        m_num_vertices_in_batch += mesh->get_vbo()->get_used_size();
        m_num_indices_in_batch += mesh->get_ibo()->get_used_size();
    }
    
    void batch::reset()
    {
        m_meshes.clear();
        m_meshes.resize(0);
        
        m_matrices.clear();
        m_matrices.resize(0);
        
        m_num_vertices_in_batch = 0;
        m_num_indices_in_batch = 0;
    }
    
    std::string batch::get_guid() const
    {
        return m_guid;
    }
    
    ui32 batch::get_num_vertices() const
    {
        return m_num_vertices_in_batch;
    }
    
    ui32 batch::get_num_indices() const
    {
        return m_num_indices_in_batch;
    }
    
    void batch::draw()
    {
        ui32 num_locked_vertices = 0;
        ui32 num_locked_indices = 0;
        
        for(ui32 i = 0; i < m_meshes.size(); ++i)
        {
            mesh_shared_ptr mesh = m_meshes.at(i);
            glm::mat4& matrix = m_matrices.at(i);
            
            ui16* main_indices = m_main_mesh->get_ibo()->lock();
            ui16* indices = mesh->get_ibo()->lock();
            
            for(ui32 j = 0; j < mesh->get_ibo()->get_used_size(); ++j)
            {
                main_indices[num_locked_indices + j] = indices[j] + num_locked_vertices;
            }
            
            vbo::vertex_attribute* main_vertices = m_main_mesh->get_vbo()->lock();
            vbo::vertex_attribute* vertices = mesh->get_vbo()->lock();
            
            for(ui32 j = 0; j < mesh->get_vbo()->get_used_size(); ++j)
            {
                main_vertices[num_locked_vertices + j] = vertices[j];
                main_vertices[num_locked_vertices + j].m_position = glm::transform(vertices[j].m_position, matrix);
            }
            
            num_locked_vertices += mesh->get_vbo()->get_used_size();
            num_locked_indices += mesh->get_ibo()->get_used_size();
        }
        
        m_main_mesh->get_vbo()->unlock(num_locked_vertices);
        m_main_mesh->get_ibo()->unlock(num_locked_indices);
        
        m_material->bind();
        
        m_main_mesh->bind(m_material->get_shader()->get_guid(), m_material->get_shader()->get_attributes());
        m_main_mesh->draw(num_locked_indices);
        m_main_mesh->unbind(m_material->get_shader()->get_guid(), m_material->get_shader()->get_attributes());
        
        m_material->unbind();
    }
}
