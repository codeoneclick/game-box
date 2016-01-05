//
//  instanced_models3d_static.cpp
//  gbCore
//
//  Created by sergey.sergeev on 10/6/15.
//  Copyright (c) 2015 sergey.sergeev. All rights reserved.
//

#include "instanced_models3d_static.h"
#include "ces_geometry_component.h"
#include "ces_render_component.h"
#include "material.h"
#include "instanced_mesh.h"

namespace gb
{
    instanced_models3d_static::instanced_models3d_static(i32 num_instances) :
    m_num_instances(num_instances)
    {
        ces_geometry_component_shared_ptr geometry_component = std::make_shared<ces_geometry_component>();
        ces_entity::add_component(geometry_component);
        
        m_positions.resize(num_instances, glm::vec3(0.f));
        m_rotations.resize(num_instances, glm::vec3(0.f));
        m_scales.resize(num_instances, glm::vec3(1.f));
        
        m_matrices_t.resize(num_instances, glm::translate(glm::mat4(1.f), glm::vec3(0.f)));
        
        glm::mat4 matrix_r = glm::rotate(glm::mat4(1.f), 0.f, glm::vec3(1.f, 0.f, 0.f));
        matrix_r = glm::rotate(matrix_r, 0.f, glm::vec3(0.f, 0.f, 1.f));
        matrix_r = glm::rotate(matrix_r, 0.f, glm::vec3(0.f, 1.f, 0.f));
        m_matrices_r.resize(num_instances, matrix_r);
        
        m_matrices_s.resize(num_instances, glm::scale(glm::mat4(1.f), glm::vec3(1.f)));
        
        m_transform_parameters.resize(num_instances, m_matrices_t[0] * m_matrices_r[0] * m_matrices_s[0]);
    }
    
    instanced_models3d_static::~instanced_models3d_static()
    {
        
    }
    
    void instanced_models3d_static::add_material(const std::string& technique_name, i32 technique_pass, const material_shared_ptr& material)
    {
        renderable_game_object::add_material(technique_name, technique_pass, material);
        
        unsafe_get_render_component_from_this->set_custom_shader_uniform_array(&m_transform_parameters[0], static_cast<i32>(m_transform_parameters.size()),
                                                                               "u_transform_parameters");
    }

    void instanced_models3d_static::set_mesh(const mesh_shared_ptr& mesh)
    {
        unsafe_get_geometry_component_from_this->set_mesh(mesh);
    }
    
    void instanced_models3d_static::set_position(const glm::vec3& position, i32 instance_id)
    {
        if(instance_id < m_num_instances)
        {
            m_positions[instance_id] = position;
            m_matrices_t[instance_id] = glm::translate(glm::mat4(1.f), position);
            m_transform_parameters[instance_id] = m_matrices_t[instance_id] * m_matrices_r[instance_id] * m_matrices_s[instance_id];
            unsafe_get_render_component_from_this->set_custom_shader_uniform_array(&m_transform_parameters[0], static_cast<i32>(m_transform_parameters.size()),
                                                                                   "u_transform_parameters");
        }
        else
        {
            assert(false);
        }
    }
    
    glm::vec3 instanced_models3d_static::get_position(i32 instance_id) const
    {
        if(instance_id < m_num_instances)
        {
            return m_positions[instance_id];
        }
        else
        {
            assert(false);
            return glm::vec3(0.f);
        }
    }
    
    void instanced_models3d_static::set_rotation(const glm::vec3& rotation, i32 instance_id)
    {
        if(instance_id < m_num_instances)
        {
            m_rotations[instance_id] = rotation;
            glm::mat4 matrix_r = glm::rotate(glm::mat4(1.f), rotation.x, glm::vec3(1.f, 0.f, 0.f));
            matrix_r = glm::rotate(matrix_r, rotation.z, glm::vec3(0.f, 0.f, 1.f));
            matrix_r = glm::rotate(matrix_r, rotation.y, glm::vec3(0.f, 1.f, 0.f));
            
            m_matrices_r[instance_id] = matrix_r;
            m_transform_parameters[instance_id] = m_matrices_t[instance_id] * m_matrices_r[instance_id] * m_matrices_s[instance_id];
            unsafe_get_render_component_from_this->set_custom_shader_uniform_array(&m_transform_parameters[0], static_cast<i32>(m_transform_parameters.size()),
                                                                                   "u_transform_parameters");
        }
        else
        {
            assert(false);
        }
    }
    
    glm::vec3 instanced_models3d_static::get_rotation(i32 instance_id) const
    {
        if(instance_id < m_num_instances)
        {
            return m_rotations[instance_id];
        }
        else
        {
            assert(false);
            return glm::vec3(0.f);
        }
    }
    
    void instanced_models3d_static::set_scale(const glm::vec3& scale, i32 instance_id)
    {
        if(instance_id < m_num_instances)
        {
            m_scales[instance_id] = scale;
            m_matrices_s[instance_id] = glm::scale(glm::mat4(1.f), scale);
            m_transform_parameters[instance_id] = m_matrices_t[instance_id] * m_matrices_r[instance_id] * m_matrices_s[instance_id];
            unsafe_get_render_component_from_this->set_custom_shader_uniform_array(&m_transform_parameters[0], static_cast<i32>(m_transform_parameters.size()),
                                                                                   "u_transform_parameters");
        }
        else
        {
            assert(false);
        }
    }
    
    glm::vec3 instanced_models3d_static::get_scale(i32 instance_id) const
    {
        if(instance_id < m_num_instances)
        {
            return m_scales[instance_id];
        }
        else
        {
            assert(false);
            return glm::vec3(0.f);
        }
    }
}