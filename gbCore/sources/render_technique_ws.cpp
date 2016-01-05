//
//  render_technique_ws.cpp
//  gbCore
//
//  Created by sergey.sergeev on 8/17/15.
//  Copyright (c) 2015 sergey.sergeev. All rights reserved.
//

#include "render_technique_ws.h"
#include "texture.h"
#include "camera.h"
#include "frustum.h"
#include "mesh.h"
#include "animation_mixer.h"
#include "scene_graph.h"
#include "ces_entity.h"
#include "ces_render_component.h"
#include "ces_geometry_component.h"
#include "ces_transformation_component.h"
#include "ces_animation_component.h"
#include "ces_debug_render_component.h"
#include "ces_particle_emitter_component.h"
#include "ces_skybox_component.h"
#include "ces_batch_component.h"
#include "batch.h"
#include "scene_graph_parameters.h"

namespace gb
{
    render_technique_ws::render_technique_ws(ui32 width, ui32 height, const std::string& name, ui32 index,
                                             bool is_depth_compare_mode_enabled, i32 num_passes) :
    gb::render_technique_base(width, height, name, index),
    m_num_passes(std::max(num_passes, 1))
    {
        ui32 color_attachment_id;
        gl_create_textures(1, &color_attachment_id);
        gl_bind_texture(GL_TEXTURE_2D, color_attachment_id);
        gl_texture_parameter_i(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        gl_texture_parameter_i(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        gl_texture_parameter_i(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        gl_texture_parameter_i(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        gl_texture_image2d(GL_TEXTURE_2D, 0, GL_RGBA, m_frame_width, m_frame_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
        
        ui32 depth_attachment_id;
        gl_create_textures(1, &depth_attachment_id);
        gl_bind_texture(GL_TEXTURE_2D, depth_attachment_id);
        gl_texture_parameter_i(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        gl_texture_parameter_i(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        gl_texture_parameter_i(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        gl_texture_parameter_i(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        
        if(is_depth_compare_mode_enabled)
        {
#if defined(__IOS__)
            
            gl_texture_parameter_i(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE_EXT, GL_COMPARE_REF_TO_TEXTURE_EXT);
            gl_texture_parameter_i(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC_EXT, GL_LEQUAL);
            
#else
            
            gl_texture_parameter_i(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
            gl_texture_parameter_i(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
            
#endif
        }
        
#if defined(__OSX__)
        gl_texture_image2d(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, m_frame_width, m_frame_height, 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, NULL);
#else
#if defined(__OPENGL_30__)
        gl_texture_image2d(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, m_frame_width, m_frame_height, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_INT, NULL);
#else
        gl_texture_image2d(GL_TEXTURE_2D, 0, GL_DEPTH_STENCIL_OES, m_frame_width, m_frame_height, 0, GL_DEPTH_STENCIL_OES, GL_UNSIGNED_INT_24_8_OES, NULL);
#endif
#endif
        
        gl_create_frame_buffers(1, &m_frame_buffer);
        gl_bind_frame_buffer(GL_FRAMEBUFFER, m_frame_buffer);
        gl_attach_frame_buffer_texture2d(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, color_attachment_id, 0);
        
#if defined(__OSX__)
        
        gl_attach_frame_buffer_texture2d(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, depth_attachment_id, 0);
        
#else
        
        gl_attach_frame_buffer_texture2d(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depth_attachment_id, 0);
        gl_attach_frame_buffer_texture2d(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_TEXTURE_2D, depth_attachment_id, 0);
        
#endif
        
        GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
        assert(status == GL_FRAMEBUFFER_COMPLETE);
        
        std::string color_attachment_guid = m_name;
        color_attachment_guid.append(".color");
        m_color_attachment_texture = texture::construct(color_attachment_guid,
                                                        color_attachment_id,
                                                        m_frame_width,
                                                        m_frame_height);
        
        m_color_attachment_texture->set_wrap_mode(GL_CLAMP_TO_EDGE);
        
        std::string depth_attachment_guid = m_name;
        depth_attachment_guid.append(".depth");
        m_depth_attachment_texture = texture::construct(depth_attachment_guid,
                                                        depth_attachment_id,
                                                        m_frame_width,
                                                        m_frame_height);
        m_depth_attachment_texture->set_wrap_mode(GL_CLAMP_TO_EDGE);
        
        m_entities.resize(m_num_passes);
    }
    
    render_technique_ws::~render_technique_ws()
    {
        
    }
    
    texture_shared_ptr render_technique_ws::get_color_attachment_texture() const
    {
        assert(m_color_attachment_texture);
        return m_color_attachment_texture;
    }
    
    texture_shared_ptr render_technique_ws::get_depth_attachment_texture() const
    {
        assert(m_depth_attachment_texture);
        return m_depth_attachment_texture;
    }
    
    void render_technique_ws::add_entity(const ces_entity_shared_ptr& entity, i32 technique_pass, const material_shared_ptr& material)
    {
        ces_render_component* render_component = unsafe_get_render_component(entity);
        assert(render_component);
        
        i32 z_order = material->get_z_order() + 1;
        if(z_order > m_entities[technique_pass].size())
        {
            m_entities[technique_pass].resize(z_order);
        }
        m_entities[technique_pass][z_order - 1].push(entity);
    }
    
    bool render_technique_ws::is_need_to_draw() const
    {
        for(i32 technique_pass = 0; technique_pass < m_num_passes; ++technique_pass)
        {
            for(i32 i = 0; i < m_entities[technique_pass].size(); ++i)
            {
                if(!m_entities[technique_pass][i].empty())
                {
                    return true;
                }
            }
        }
        return false;
    }
    
    void render_technique_ws::bind()
    {
        gl_bind_frame_buffer(GL_FRAMEBUFFER, m_frame_buffer);
        gl_viewport(0, 0, m_frame_width, m_frame_height);
        
        gl_enable(GL_DEPTH_TEST);
        material::get_cached_parameters()->m_is_depth_test = true;
        gl_depth_mask(GL_TRUE);
        material::get_cached_parameters()->m_is_depth_mask = true;
        gl_enable(GL_STENCIL_TEST);
        material::get_cached_parameters()->m_is_stencil_test = true;

        glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
        
        gl_clear_color(m_clear_color.r, m_clear_color.g, m_clear_color.b, m_clear_color.a);
        gl_clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    }
    
    void render_technique_ws::unbind()
    {
        gl_bind_frame_buffer(GL_FRAMEBUFFER, NULL);
    }
    
    void render_technique_ws::draw()
    {
        for(i32 technique_pass = 0; technique_pass < m_num_passes; ++technique_pass)
        {
            for(i32 i = 0; i < m_entities[technique_pass].size(); ++i)
            {
                while (!m_entities[technique_pass][i].empty())
                {
                    ces_entity_shared_ptr entity = m_entities[technique_pass][i].front();
                    
                    ces_render_component* render_component = unsafe_get_render_component(entity);
                    assert(render_component);
                    
                    ces_transformation_component* transformation_component = unsafe_get_transformation_component(entity);
                    assert(transformation_component);
                    
                    ces_animation_component* animation_component = unsafe_get_animation_component(entity);
                    
                    ces_debug_render_component* debug_render_component = unsafe_get_debug_render_component(entity);
                    
                    ces_batch_component* batch_component = unsafe_get_batch_component(entity);
                    
                    ces_geometry_component* geometry_component = unsafe_get_geometry_component(entity);
                    
                    ces_particle_emitter_component* particle_emitter_component = unsafe_get_particle_emitter_component(entity);
                    
                    ces_skybox_component* skybox_component = unsafe_get_skybox_component(entity);
                    
                    material_shared_ptr material = render_component->get_material(m_name, technique_pass);
                    mesh_shared_ptr mesh = nullptr;
                    
                    bool is_need_to_draw = true;
                    bool is_batched = false;
                    
                    if(batch_component)
                    {
                        batch_shared_ptr batch = batch_component->get_batch();
                        if(batch)
                        {
                            is_need_to_draw = batch_component->get_render_state(material->get_guid()) == batch::e_batch_render_state_waiting;
                            is_batched = true;
                            mesh = batch->get_mesh();
                            batch_component->set_render_state(material->get_guid(), batch::e_batch_render_state_done);
                        }
                        else
                        {
                            assert(false);
                        }
                    }
                    
                    if(!mesh && geometry_component)
                    {
                         mesh = geometry_component->get_mesh();
                    }
                    else if(!mesh && particle_emitter_component)
                    {
                        mesh = particle_emitter_component->get_mesh();
                    }
                    else if(!mesh)
                    {
                        assert(false);
                    }
                    
                    if(is_need_to_draw)
                    {
                        render_component->on_bind(m_name, technique_pass, material);
                        
                        material->get_shader()->set_mat4(is_batched ? glm::mat4(1.f) : transformation_component->get_matrix_m(),
                                                         e_shader_uniform_mat_m);
                        
                        if(animation_component)
                        {
                            material->get_shader()->set_mat4_array(animation_component->get_animation_mixer()->get_transformations(),
                                                                   animation_component->get_animation_mixer()->get_transformation_size(), e_shader_uniform_mat_bones);
                        }
                        
                        scene_graph_parameters_shared_ptr scene_graph_parameters = render_component->get_scene_graph_parameters();
                        
                        if(skybox_component && material->is_reflecting())
                        {
                            glm::vec3 skybox_position = scene_graph_parameters->get_eye_position();
                            skybox_position.y = skybox_position.y * -1.f + 1.f;
                            transformation_component->set_position(skybox_position);
                            material->get_shader()->set_mat4(transformation_component->get_matrix_m(), e_shader_uniform_mat_m);
                        }
                        else if(skybox_component)
                        {
                            glm::vec3 skybox_position = scene_graph_parameters->get_eye_position();
                            transformation_component->set_position(skybox_position);
                            material->get_shader()->set_mat4(transformation_component->get_matrix_m(), e_shader_uniform_mat_m);
                        }
                        
                        render_component->on_draw(m_name, technique_pass, mesh, material);
                    }
                    
                    if(debug_render_component && material->is_debugging())
                    {
                        debug_render_component->on_bind();
                        material->get_shader()->set_mat4(transformation_component->get_matrix_m(), e_shader_uniform_mat_m);
                        
                        debug_render_component->on_draw();
                        debug_render_component->on_unbind();
                    }
                    m_entities[technique_pass][i].pop();
                }
            }
        }
    }
    
    i32 render_technique_ws::get_num_passes() const
    {
        return m_num_passes;
    }
}