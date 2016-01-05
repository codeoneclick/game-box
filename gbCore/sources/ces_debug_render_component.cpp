//
//  ces_debug_render_component.cpp
//  gbCore
//
//  Created by sergey.sergeev on 8/31/15.
//  Copyright (c) 2015 sergey.sergeev. All rights reserved.
//

#include "ces_debug_render_component.h"
#include "material.h"
#include "shader.h"
#include "camera.h"
#include "mesh.h"
#include "built_in_shaders.h"
#include "scene_graph_parameters.h"

namespace gb
{
    std::once_flag g_debug_render_component_material_created;
    
    ces_debug_render_component::ces_debug_render_component() :
    m_mesh(nullptr),
    m_material(nullptr)
    {
        m_type = e_ces_component_type_debug_render;
    }
    
    ces_debug_render_component::~ces_debug_render_component(void)
    {
        
    }
    
    void ces_debug_render_component::bind_main_shader_uniforms(const material_shared_ptr& material)
    {
        scene_graph_parameters_shared_ptr scene_graph_parameters = ces_base_component::get_scene_graph_parameters();
        material->get_shader()->set_mat4(scene_graph_parameters->get_eye_matrix_p(), e_shader_uniform_mat_p);
        material->get_shader()->set_mat4(!material->is_reflecting() ?
                                         scene_graph_parameters->get_eye_matrix_v() : scene_graph_parameters->get_eye_matrix_i_v(), e_shader_uniform_mat_v);
        material->get_shader()->set_mat4(scene_graph_parameters->get_eye_matrix_n(), e_shader_uniform_mat_n);
        
        material->get_shader()->set_vec3(scene_graph_parameters->get_eye_position(), e_shader_uniform_vec_camera_position);
        material->get_shader()->set_f32(scene_graph_parameters->get_eye_near(), e_shader_uniform_f32_camera_near);
        material->get_shader()->set_f32(scene_graph_parameters->get_eye_far(), e_shader_uniform_f32_camera_far);
        material->get_shader()->set_vec4(material->get_clipping_plane(), e_shader_uniform_vec_clip);
    }
    
    void ces_debug_render_component::on_bind()
    {
        if(m_mesh)
        {
            assert(m_material);
            m_material->bind();
            
            ces_debug_render_component::bind_main_shader_uniforms(m_material);
        }
    }
    
    void ces_debug_render_component::on_draw()
    {
        if(m_mesh)
        {
            assert(m_material);
            assert(m_material->get_shader()->is_commited());
            assert(m_mesh->is_commited());
            
            m_mesh->bind(m_material->get_shader()->get_guid(), m_material->get_shader()->get_attributes());
            m_mesh->draw();
            m_mesh->unbind(m_material->get_shader()->get_guid(), m_material->get_shader()->get_attributes());
        }
    }
    
    void ces_debug_render_component::on_unbind()
    {
        if(m_mesh)
        {
            assert(m_material);
            m_material->unbind();
        }
    }
    
    void ces_debug_render_component::set_mesh(const mesh_shared_ptr& mesh)
    {
        m_mesh = mesh;
    }
    
    material_shared_ptr ces_debug_render_component::get_material() const
    {
        return m_material;
    }
}