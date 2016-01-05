//
//  batch.cpp
//  gbCore
//
//  Created by sergey.sergeev on 10/28/15.
//  Copyright © 2015 sergey.sergeev. All rights reserved.
//

#include "batch.h"
#include "mesh.h"
#include "vbo.h"
#include "ibo.h"
#include "glm_extensions.h"
#include "thread_operation.h"

namespace gb
{
    const ui32 batch::k_max_num_vertices = 65535 / 4; // 16k vertices
    const ui32 batch::k_max_num_indices = 65535 / 2;  // 32k indices
    
    batch::batch() :
    m_mesh(nullptr),
    m_num_vertices(0),
    m_num_indices(0),
    m_data_state(e_batch_data_state_waiting)
    {
        vbo_shared_ptr vbo = std::make_shared<gb::vbo>(k_max_num_vertices, GL_DYNAMIC_DRAW);
        vbo::vertex_attribute* vertices = vbo->lock();
        memset(vertices, 0x0, k_max_num_vertices * sizeof(vbo::vertex_attribute));
        vbo->unlock();
        
        ibo_shared_ptr ibo = std::make_shared<gb::ibo>(k_max_num_indices, GL_DYNAMIC_DRAW);
        ui16* indices = ibo->lock();
        memset(indices, 0x0, k_max_num_indices * sizeof(ui16));
        ibo->unlock();
        
        m_mesh = mesh::construct("batch", vbo, ibo, glm::vec3(INT16_MIN), glm::vec3(INT16_MAX));
    }
    
    batch::~batch()
    {
        
    }
    
    mesh_shared_ptr batch::get_mesh() const
    {
        return m_mesh;
    }
    
    batch::e_batch_data_state batch::get_data_state() const
    {
        return m_data_state;
    }
    
    batch::e_batch_render_state batch::get_render_state(const std::string& guid) const
    {
        const auto& iterator = m_render_states.find(guid);
        if(iterator != m_render_states.end())
        {
            return iterator->second;
        }
        return e_batch_render_state_not_exist;
    }
    
    void batch::set_render_state(const std::string& guid, e_batch_render_state state)
    {
        m_render_states[guid] = state;
    }
    
    void batch::add_data(const std::string& guid, const mesh_shared_ptr& mesh, const glm::mat4& mat_m)
    {
        const auto& iterator = m_data.find(guid);
        if(iterator == m_data.end())
        {
            m_data[guid] = std::make_pair(mesh, mat_m);
        }
    }
    
    void batch::cleanup()
    {
        m_num_vertices = 0;
        m_num_indices = 0;
        
        m_data.clear();
        
        m_data_state = e_batch_data_state_waiting;
    }
    
    void batch::generate()
    {
        m_data_state = e_batch_data_state_generating;
        
        thread_operation_shared_ptr completion_operation = std::make_shared<thread_operation>(thread_operation::e_thread_operation_queue_main);
        completion_operation->set_execution_callback([this]() {
            m_mesh->get_vbo()->unlock(m_num_vertices);
            m_mesh->get_ibo()->unlock(m_num_indices);
            
            m_data_state = e_batch_data_state_generated;
        });
        
        thread_operation_shared_ptr generate_operation = std::make_shared<thread_operation>(thread_operation::e_thread_operation_queue_background);
        generate_operation->set_execution_callback([this](void) {
            for(const auto& data_iterator : m_data)
            {
                mesh_shared_ptr mesh = data_iterator.second.first;
                glm::mat4 m_mat = data_iterator.second.second;
                
                ui16* main_indices = m_mesh->get_ibo()->lock();
                ui16* sub_indices = mesh->get_ibo()->lock();
                
                for(ui32 i = 0; i < mesh->get_ibo()->get_used_size(); ++i)
                {
                    main_indices[m_num_indices + i] = sub_indices[i] + m_num_vertices;
                }
                
                vbo::vertex_attribute* main_vertices = m_mesh->get_vbo()->lock();
                vbo::vertex_attribute* sub_vertices = mesh->get_vbo()->lock();
                
                for(ui32 i = 0; i < mesh->get_vbo()->get_used_size(); ++i)
                {
                    glm::vec3 position = sub_vertices[i].m_position;
                    glm::vec4 normal = glm::unpackSnorm4x8(sub_vertices[i].m_normal);
                    glm::vec4 tangent = glm::unpackSnorm4x8(sub_vertices[i].m_tangent);
                    
                    main_vertices[m_num_vertices + i] = sub_vertices[i];
                    main_vertices[m_num_vertices + i].m_position = glm::transform(position, m_mat);
                    main_vertices[m_num_vertices + i].m_normal = glm::packSnorm4x8(glm::transform(normal, m_mat));
                    main_vertices[m_num_vertices + i].m_tangent = glm::packSnorm4x8(glm::transform(tangent, m_mat));
                }
                
                m_num_vertices += mesh->get_vbo()->get_used_size();
                m_num_indices += mesh->get_ibo()->get_used_size();
            }
        });
        
        completion_operation->add_dependency(generate_operation);
        completion_operation->add_to_execution_queue();
    }
}