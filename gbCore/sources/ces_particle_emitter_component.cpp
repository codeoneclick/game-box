//
//  ces_particle_emitter_component.cpp
//  gbCore
//
//  Created by sergey.sergeev on 9/1/15.
//  Copyright (c) 2015 sergey.sergeev. All rights reserved.
//

#include "ces_particle_emitter_component.h"
#include "mesh.h"
#include "vbo.h"
#include "ibo.h"
#include "std_extensions.h"
#include "scene_graph_parameters.h"

namespace gb
{
    ces_particle_emitter_component::ces_particle_emitter_component() :
    m_settings(nullptr),
    m_mesh(nullptr),
    m_last_emitt_timestamp(0)
    {
        m_type = e_ces_component_type_particle_emitter;
    }
    
    ces_particle_emitter_component::~ces_particle_emitter_component()
    {
        
    }
    
    void ces_particle_emitter_component::emitt_particle(ui32 index)
    {
        m_particles[index].m_position = glm::vec3(0.f);
        m_particles[index].m_velocity = glm::vec3(0.f);
        
        m_particles[index].m_size = glm::vec2(m_settings->get_source_size_x(),
                                              m_settings->get_source_size_y());
        
        m_particles[index].m_color = glm::u8vec4(m_settings->get_source_color_r(),
                                                 m_settings->get_source_color_g(),
                                                 m_settings->get_source_color_b(),
                                                 m_settings->get_source_color_a());
        
        m_particles[index].m_timestamp = std::get_tick_count();
        
        f32 horizontal_velocity = glm::mix(m_settings->get_min_horizontal_velocity(),
                                           m_settings->get_max_horizontal_velocity(), std::get_random_f(0.f, 1.f));
        f32 horizontal_angle = std::get_random_f(0.f, 1.f) * M_PI * 2.f;
        
        m_particles[index].m_velocity.x += horizontal_velocity * cosf(horizontal_angle);
        m_particles[index].m_velocity.z += horizontal_velocity * sinf(horizontal_angle);
        
        m_particles[index].m_velocity.y += glm::mix(m_settings->get_min_vertical_velocity(),
                                                    m_settings->get_max_vertical_velocity(), std::get_random_f(0.f, 1.f));
        m_particles[index].m_velocity *= m_settings->get_velocity_sensitivity();
    }
    
    void ces_particle_emitter_component::create_particles_mesh()
    {
        m_particles.clear();
        m_particles.resize(m_settings->get_num_particles());
        
        vbo_shared_ptr vbo = std::make_shared<gb::vbo>(m_settings->get_num_particles() * 4, GL_DYNAMIC_DRAW);
        vbo::vertex_attribute* vertices = vbo->lock();
        for(ui32 i = 0; i < m_settings->get_num_particles(); ++i)
        {
            m_particles[i].m_size = glm::vec2(0.f);
            m_particles[i].m_color = glm::u8vec4(0);
            
            vertices[i * 4 + 0].m_texcoord = glm::packUnorm2x16(glm::vec2(0.f,  0.f));
            vertices[i * 4 + 1].m_texcoord = glm::packUnorm2x16(glm::vec2(1.f,  0.f));
            vertices[i * 4 + 2].m_texcoord = glm::packUnorm2x16(glm::vec2(1.f,  1.f));
            vertices[i * 4 + 3].m_texcoord = glm::packUnorm2x16(glm::vec2(0.f,  1.f));
        }
        vbo->unlock();
        
        ibo_shared_ptr ibo = std::make_shared<gb::ibo>(m_settings->get_num_particles() * 6, GL_STATIC_DRAW);
        ui16* indices = ibo->lock();
        for(ui32 i = 0; i < m_settings->get_num_particles(); ++i)
        {
            indices[i * 6 + 0] = static_cast<ui16>(i * 4 + 0);
            indices[i * 6 + 1] = static_cast<ui16>(i * 4 + 1);
            indices[i * 6 + 2] = static_cast<ui16>(i * 4 + 2);
            
            indices[i * 6 + 3] = static_cast<ui16>(i * 4 + 0);
            indices[i * 6 + 4] = static_cast<ui16>(i * 4 + 2);
            indices[i * 6 + 5] = static_cast<ui16>(i * 4 + 3);
        }
        ibo->unlock();
        
        m_mesh = mesh::construct("particle.emitter", vbo, ibo,
                                 glm::vec3(0.f), glm::vec3(0.f));
    }
    
    void ces_particle_emitter_component::update(f32 deltatime)
    {
        if(m_mesh)
        {
            vbo::vertex_attribute* vertices = m_mesh->get_vbo()->lock();
            ui64 current_time = std::get_tick_count();
            
            for(ui32 i = 0; i < m_settings->get_num_particles(); ++i)
            {
                ui64 particle_age = current_time - m_particles[i].m_timestamp;
                
                if(particle_age > m_settings->get_duration())
                {
                    if((current_time - m_last_emitt_timestamp) > std::get_random_f(m_settings->get_min_emitt_interval(),
                                                                                   m_settings->get_max_emitt_interval()))
                    {
                        m_last_emitt_timestamp = current_time;
                        ces_particle_emitter_component::emitt_particle(i);
                        particle_age = 0;
                    }
                    else
                    {
                        m_particles[i].m_size = glm::vec2(0.f);
                        m_particles[i].m_color = glm::u8vec4(0);
                    }
                }
                
                f32 particle_clamp_age = glm::clamp(static_cast<f32>(particle_age) / static_cast<f32>(m_settings->get_duration()), 0.f, 1.f);
                f32 start_velocity = glm::length(m_particles[i].m_velocity);
                f32 end_velocity = m_settings->get_end_velocity() * start_velocity;
                f32 velocity_integral = start_velocity * particle_clamp_age + (end_velocity - start_velocity) * particle_clamp_age * particle_clamp_age / 2.f;
                m_particles[i].m_position += glm::normalize(m_particles[i].m_velocity) * velocity_integral * static_cast<f32>(m_settings->get_duration());
                m_particles[i].m_position += glm::vec3(m_settings->get_gravity_x(),
                                                       m_settings->get_gravity_y(),
                                                       m_settings->get_gravity_z()) * static_cast<f32>(particle_age) * particle_clamp_age;
                
                f32 random_value = std::get_random_f(0.f, 1.f);
                f32 start_size = glm::mix(m_settings->get_source_size_x(),
                                          m_settings->get_source_size_y(), random_value);
                f32 end_size = glm::mix(m_settings->get_destination_size_x(),
                                        m_settings->get_destination_size_y(), random_value);
                m_particles[i].m_size = glm::vec2(glm::mix(start_size, end_size, particle_clamp_age));
                
                m_particles[i].m_color = glm::mix(glm::u8vec4(m_settings->get_source_color_r(),
                                                              m_settings->get_source_color_g(),
                                                              m_settings->get_source_color_b(),
                                                              m_settings->get_source_color_a()),
                                                  
                                                  glm::u8vec4(m_settings->get_destination_color_r(),
                                                              m_settings->get_destination_color_g(),
                                                              m_settings->get_destination_color_b(),
                                                              m_settings->get_destination_color_a()), particle_clamp_age);
                
                scene_graph_parameters_shared_ptr scene_graph_parameters = ces_base_component::get_scene_graph_parameters();
                glm::mat4 mat_s = scene_graph_parameters->get_matrix_s(m_particles[i].m_position);
                
                glm::vec4 position = glm::vec4(-m_particles[i].m_size.x, -m_particles[i].m_size.y, 0.f, 1.f);
                position = mat_s * position;
                vertices[i * 4 + 0].m_position = glm::vec3(position.x, position.y, position.z);
                
                position = glm::vec4(m_particles[i].m_size.x, -m_particles[i].m_size.y, 0.f, 1.f);
                position = mat_s * position;
                vertices[i * 4 + 1].m_position = glm::vec3(position.x, position.y, position.z);
                
                position = glm::vec4(m_particles[i].m_size.x, m_particles[i].m_size.y, 0.f, 1.f);
                position = mat_s * position;
                vertices[i * 4 + 2].m_position = glm::vec3(position.x, position.y, position.z);
                
                position = glm::vec4(-m_particles[i].m_size.x, m_particles[i].m_size.y, 0.f, 1.f);
                position = mat_s * position;
                vertices[i * 4 + 3].m_position = glm::vec3(position.x, position.y, position.z);
                
                vertices[i * 4 + 0].m_color = m_particles[i].m_color;
                vertices[i * 4 + 1].m_color = m_particles[i].m_color;
                vertices[i * 4 + 2].m_color = m_particles[i].m_color;
                vertices[i * 4 + 3].m_color = m_particles[i].m_color;
            }
            m_mesh->get_vbo()->unlock();
        }
    }
    
    void ces_particle_emitter_component::set_settings(const std::shared_ptr<particle_emitter_configuration>& settings)
    {
        m_settings = settings;
        ces_particle_emitter_component::create_particles_mesh();
    }
    
    mesh_shared_ptr ces_particle_emitter_component::get_mesh() const
    {
        return m_mesh;
    }
}