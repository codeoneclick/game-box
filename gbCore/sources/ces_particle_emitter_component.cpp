//
//  ces_particle_emitter_component.cpp
//  gbCore
//
//  Created by serhii.s on 3/21/19.
//  Copyright © 2019 sergey.sergeev. All rights reserved.
//

#include "ces_particle_emitter_component.h"
#include "mesh_3d.h"
#include "vbo.h"
#include "ibo.h"
#include "std_extensions.h"

namespace gb
{
    ces_particle_emitter_component::ces_particle_emitter_component() :
    m_settings(nullptr),
    m_emitt_timestamp(0)
    {
    }
    
    ces_particle_emitter_component::~ces_particle_emitter_component()
    {
        
    }
    
    void ces_particle_emitter_component::emitt_particle(ui32 index, const glm::vec3& position)
    {
        m_particles[index]->m_spawn_position = position;
        m_particles[index]->m_delta_position = glm::vec3(0.f);
        m_particles[index]->m_velocity = glm::vec3(0.f);
        
        f32 random_value = std::get_random_f(0.f, 1.f);
        f32 source_size = glm::mix(m_settings->m_source_size.x,
                                   m_settings->m_source_size.y, random_value);
        f32 destination_size = glm::mix(m_settings->m_destination_size.x,
                                        m_settings->m_destination_size.y, random_value);
        
        m_particles[index]->m_source_size = glm::vec2(source_size);
        m_particles[index]->m_destination_size = glm::vec2(destination_size);
        m_particles[index]->m_color = m_settings->m_source_color;
        m_particles[index]->m_timestamp = std::get_tick_count();
        
        f32 horizontal_velocity = glm::mix(m_settings->m_min_horizontal_velocity,
                                           m_settings->m_max_horizontal_velocity, std::get_random_f(0.f, 1.f));
        f32 horizontal_angle = std::get_random_f(0.f, 1.f) * M_PI * 2.f;
        
        m_particles[index]->m_velocity.x += horizontal_velocity * cosf(horizontal_angle);
        m_particles[index]->m_velocity.z += horizontal_velocity * sinf(horizontal_angle);
        
        m_particles[index]->m_velocity.y += glm::mix(m_settings->m_min_vertical_velocity,
                                                     m_settings->m_max_vertical_velocity, std::get_random_f(0.f, 1.f));
        m_particles[index]->m_velocity *= m_settings->m_velocity_sensitivity;
    }
    
    mesh_3d_shared_ptr ces_particle_emitter_component::construct_particles_mesh()
    {
        m_particles.clear();
        m_particles.resize(m_settings->m_num_particles);
        
        std::shared_ptr<vbo::vertex_declaration_PTNTC> vertex_declaration = std::make_shared<vbo::vertex_declaration_PTNTC>(m_settings->m_num_particles * 4);
        vbo_shared_ptr vbo = std::make_shared<gb::vbo>(vertex_declaration, gl::constant::dynamic_draw);
        
        ibo_shared_ptr ibo = std::make_shared<gb::ibo>(m_settings->m_num_particles * 6, gl::constant::static_draw);
        
        vbo::vertex_attribute_PTNTC *vertices = vbo->lock<vbo::vertex_attribute_PTNTC>();
        ui16* indices = ibo->lock();
        
        for(ui32 i = 0; i < m_settings->m_num_particles; ++i)
        {
            m_particles[i] = std::make_shared<ces_particle_emitter_component::particle>();
            m_particles[i]->m_current_size = glm::vec2(0.f);
            m_particles[i]->m_color = glm::u8vec4(0);
            m_particles[i]->m_timestamp = 0.f;
            
            vertices[i * 4 + 0].m_normal = glm::packSnorm4x8(glm::vec4(0.f, 1.f, 0.f, 0.f));
            vertices[i * 4 + 1].m_normal = glm::packSnorm4x8(glm::vec4(0.f, 1.f, 0.f, 0.f));
            vertices[i * 4 + 2].m_normal = glm::packSnorm4x8(glm::vec4(0.f, 1.f, 0.f, 0.f));
            vertices[i * 4 + 3].m_normal = glm::packSnorm4x8(glm::vec4(0.f, 1.f, 0.f, 0.f));
            
            vertices[i * 4 + 0].m_texcoord = glm::packUnorm2x16(glm::vec2(0.f,  0.f));
            vertices[i * 4 + 1].m_texcoord = glm::packUnorm2x16(glm::vec2(1.f,  0.f));
            vertices[i * 4 + 2].m_texcoord = glm::packUnorm2x16(glm::vec2(1.f,  1.f));
            vertices[i * 4 + 3].m_texcoord = glm::packUnorm2x16(glm::vec2(0.f,  1.f));
        }
        vbo->unlock();
        
        for(ui32 i = 0; i < m_settings->m_num_particles; ++i)
        {
            indices[i * 6 + 0] = static_cast<ui16>(i * 4 + 0);
            indices[i * 6 + 1] = static_cast<ui16>(i * 4 + 1);
            indices[i * 6 + 2] = static_cast<ui16>(i * 4 + 2);
            
            indices[i * 6 + 3] = static_cast<ui16>(i * 4 + 0);
            indices[i * 6 + 4] = static_cast<ui16>(i * 4 + 2);
            indices[i * 6 + 5] = static_cast<ui16>(i * 4 + 3);
        }
        ibo->unlock();
        
        mesh_3d_shared_ptr mesh = gb::mesh_3d::construct("particle.emitter", vbo, ibo);
        return mesh;
    }
    
    std::vector<std::shared_ptr<ces_particle_emitter_component::particle>> ces_particle_emitter_component::get_particles() const
    {
        return m_particles;
    }
    
    void ces_particle_emitter_component::set_emitt_timestamp(f32 timestamp)
    {
        m_emitt_timestamp = timestamp;
    }
    
    f32 ces_particle_emitter_component::get_emitt_timestamp() const
    {
        return m_emitt_timestamp;
    }
    
    void ces_particle_emitter_component::set_settings(const std::shared_ptr<ces_particle_emitter_component::emitter_settings>& settings)
    {
        m_settings = settings;
    }

    std::shared_ptr<ces_particle_emitter_component::emitter_settings> ces_particle_emitter_component::get_settings() const
    {
        return m_settings;
    }
    
    void ces_particle_emitter_component::set_enabled(bool value)
    {
        m_is_enabled = value;
    }
    
    bool ces_particle_emitter_component::get_enabled() const
    {
        return m_is_enabled;
    }
}

