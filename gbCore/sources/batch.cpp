//
//  batch.cpp
//  gbCore
//
//  Created by serhii serhiiv on 10/6/16.
//  Copyright © 2016 sergey.sergeev. All rights reserved.
//

#if !defined(__NO_RENDER__)

#include "batch.h"
#include "material.h"
#include "vbo.h"
#include "ibo.h"
#include "mesh_2d.h"
#include "glm_extensions.h"

namespace gb
{
    batch_cache::batch_cache(ui32 vbo_version, ui32 ibo_version, ui32 matrix_version, ui32 vbo_offset, ui32 ibo_offset) :
    m_vbo_version(vbo_version),
    m_ibo_version(ibo_version),
    m_matrix_version(matrix_version),
    m_vbo_offset(vbo_offset),
    m_ibo_offset(ibo_offset)
    {
        
    }
    
    batch_cache::~batch_cache()
    {
        
    }
    
    ui32 batch_cache::get_vbo_version() const
    {
        return m_vbo_version;
    }
    
    ui32 batch_cache::get_ibo_version() const
    {
        return m_ibo_version;
    }
    
    ui32 batch_cache::get_matrix_version() const
    {
        return m_matrix_version;
    }
    
    ui32 batch_cache::get_vbo_offset() const
    {
        return m_vbo_offset;
    }
    
    ui32 batch_cache::get_ibo_offset() const
    {
        return m_ibo_offset;
    }
    
    void batch_cache::update(ui32 vbo_version, ui32 ibo_version, ui32 matrix_version, ui32 vbo_offset, ui32 ibo_offset)
    {
        m_vbo_version = vbo_version;
        m_ibo_version = ibo_version;
        m_matrix_version = matrix_version;
        
        m_vbo_offset = vbo_offset;
        m_ibo_offset = ibo_offset;
    }
    
    const ui32 batch::k_max_num_vertices = 65535 / 4; // 16k vertices
    const ui32 batch::k_max_num_indices = 65535 / 2;  // 32k indices
    
    batch::batch(const material_shared_ptr& material) :
    m_material(material),
    m_num_vertices_in_batch(0),
    m_num_indices_in_batch(0),
    m_is_vbo_changed(true),
    m_is_ibo_changed(true)
    {
        m_guid = m_material->get_guid();
        
        vertex_declaration_shared_ptr vertex_declaration = std::make_shared<vertex_declaration_PTC>(k_max_num_vertices);
        auto vbo = std::make_shared<gb::vbo>(vertex_declaration, GL_DYNAMIC_DRAW);
        vbo->unlock();
        
        ibo_shared_ptr ibo = std::make_shared<gb::ibo>(k_max_num_indices, GL_DYNAMIC_DRAW);
        ui16* indices = ibo->lock();
        memset(indices, 0x0, k_max_num_indices * sizeof(ui16));
        ibo->unlock();
        
        m_batch = std::make_shared<gb::mesh_2d>(vbo, ibo);
    }
    
    batch::~batch()
    {
        
    }
    
    void batch::add(const mesh_2d_shared_ptr& mesh, const glm::mat4& matrix, ui32 matrix_version)
    {
        ui32 mesh_id = mesh->get_id();
        auto iterator = m_cache.find(mesh_id);
        
        ui32 vbo_version = mesh->get_vbo()->get_version();
        ui32 ibo_version = mesh->get_ibo()->get_version();
        
        vertex_attribute* batch_vertices = m_batch->get_vbo()->lock();
        vertex_attribute* vertices = mesh->get_vbo()->lock();
        
        ui16* batch_indices = m_batch->get_ibo()->lock();
        ui16* indices = mesh->get_ibo()->lock();
        
        if(iterator != m_cache.end())
        {
            if(iterator->second->get_vbo_version() != vbo_version ||
               iterator->second->get_matrix_version() != matrix_version ||
               iterator->second->get_vbo_offset() != m_num_vertices_in_batch)
            {
                m_is_vbo_changed = true;
                
                for(ui32 i = 0; i < mesh->get_vbo()->used_size; ++i)
                {
                    batch_vertices[m_num_vertices_in_batch + i] = vertices[i];
                    batch_vertices[m_num_vertices_in_batch + i].position = glm::transform(vertices[i].position, matrix);
                }
            }
            
            if(iterator->second->get_ibo_version() != ibo_version ||
               iterator->second->get_ibo_offset() != m_num_indices_in_batch)
            {
                m_is_ibo_changed = true;
                
                for(ui32 i = 0; i < mesh->get_ibo()->get_used_size(); ++i)
                {
                    batch_indices[m_num_indices_in_batch + i] = indices[i] + m_num_vertices_in_batch;
                }
            }
            
            iterator->second->update(vbo_version, ibo_version, matrix_version, m_num_vertices_in_batch, m_num_indices_in_batch);
        }
        else
        {
            m_cache[mesh_id] = std::make_shared<batch_cache>(vbo_version, ibo_version, matrix_version, m_num_vertices_in_batch, m_num_indices_in_batch);
            m_is_vbo_changed = true;
            m_is_ibo_changed = true;
            
            for(ui32 i = 0; i < mesh->get_vbo()->used_size; ++i)
            {
                batch_vertices[m_num_vertices_in_batch + i] = vertices[i];
                batch_vertices[m_num_vertices_in_batch + i].position = glm::transform(vertices[i].position, matrix);
            }
            for(ui32 i = 0; i < mesh->get_ibo()->get_used_size(); ++i)
            {
                batch_indices[m_num_indices_in_batch + i] = indices[i] + m_num_vertices_in_batch;
            }
        }
        
        m_num_vertices_in_batch += mesh->get_vbo()->used_size;
        m_num_indices_in_batch += mesh->get_ibo()->get_used_size();
    }
    
    void batch::reset()
    {
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
        if(m_num_indices_in_batch != 0)
        {
            if(m_is_vbo_changed || m_batch->get_vbo()->used_size != m_num_vertices_in_batch)
            {
                m_batch->get_vbo()->unlock(false, m_num_vertices_in_batch);
            }
            
            if(m_is_ibo_changed || m_batch->get_ibo()->get_used_size() != m_num_indices_in_batch)
            {
                m_batch->get_ibo()->unlock(false, m_num_indices_in_batch);
            }
            
            m_material->bind();
            m_material->get_shader()->set_mat4(glm::mat4(1.f), e_shader_uniform_mat_m);
            
            m_batch->bind(m_material->get_shader()->get_guid(), m_material->get_shader()->get_attributes());
            m_batch->draw(m_num_indices_in_batch);
            m_batch->unbind(m_material->get_shader()->get_guid(), m_material->get_shader()->get_attributes());
            
            m_material->unbind();
            
            m_is_vbo_changed = false;
            m_is_ibo_changed = false;
        }
    }
}

#endif
