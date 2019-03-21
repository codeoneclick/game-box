//
//  ces_particle_emitter_system.cpp
//  gbCore
//
//  Created by serhii.s on 3/21/19.
//  Copyright © 2019 sergey.sergeev. All rights reserved.
//

#include "ces_particle_emitter_system.h"
#include "ces_geometry_3d_component.h"
#include "ces_particle_emitter_component.h"
#include "mesh_3d.h"
#include "vbo.h"
#include "ibo.h"
#include "camera_3d.h"
#include "std_extensions.h"

namespace gb
{
    ces_particle_emitter_system::ces_particle_emitter_system()
    {
        ces_base_system::add_required_component_guid(m_components_mask, ces_geometry_3d_component::class_guid());
        ces_base_system::add_required_component_guid(m_components_mask, ces_particle_emitter_component::class_guid());
        ces_base_system::add_required_components_mask(m_components_mask);
    }
    
    void ces_particle_emitter_system::on_feed_start(f32 dt)
    {
        
    }
    
    void ces_particle_emitter_system::on_feed(const ces_entity_shared_ptr& root, f32 dt)
    {
        const auto camera_3d = ces_base_system::get_current_camera_3d();
        glm::mat4 mat_v = camera_3d->get_mat_v();
        
        const auto camera_right_worldspace = glm::vec3(mat_v[0][0], mat_v[1][0], mat_v[2][0]);
        const auto camera_up_worldspace =  glm::vec3(mat_v[0][1], mat_v[1][1], mat_v[2][1]);
        
        ces_base_system::enumerate_entities_with_components(m_components_mask, [=](const ces_entity_shared_ptr& entity) {
            
            const auto geometry_component = entity->get_component<ces_geometry_component>();
            const auto particle_emitter_component = entity->get_component<ces_particle_emitter_component>();
            
            const auto mesh = std::static_pointer_cast<mesh_3d>(geometry_component->get_mesh());
            const auto settings = particle_emitter_component->get_settings();
            const auto particles = particle_emitter_component->get_particles();
          
            ui64 current_time = std::get_tick_count();
            
            vbo::vertex_attribute_PTNTC *vertices = mesh->get_vbo()->lock<vbo::vertex_attribute_PTNTC>();
            
            for(ui32 i = 0; i < settings->m_num_particles; ++i)
            {
                ui64 particle_age = current_time - particles[i]->m_timestamp;
                
                if(particle_age > settings->m_duration)
                {
                    if((current_time - particle_emitter_component->get_emitt_timestamp()) > std::get_random_f(settings->m_min_emitt_interval,
                                                                                   settings->m_max_emitt_interval))
                    {
                        particle_emitter_component->set_emitt_timestamp(current_time);
                        particle_emitter_component->emitt_particle(i);
                        particle_age = 0;
                    }
                    else
                    {
                        particles[i]->m_size = glm::vec2(0.f);
                        particles[i]->m_color = glm::u8vec4(0);
                    }
                }
                
                f32 particle_clamp_age = glm::clamp(static_cast<f32>(particle_age) / static_cast<f32>(settings->m_duration), 0.f, 1.f);
                f32 start_velocity = glm::length(particles[i]->m_velocity);
                f32 end_velocity = settings->m_end_velocity * start_velocity;
                f32 velocity_integral = start_velocity * particle_clamp_age + (end_velocity - start_velocity) * particle_clamp_age * particle_clamp_age / 2.f;
                particles[i]->m_position += glm::normalize(particles[i]->m_velocity) * velocity_integral * static_cast<f32>(settings->m_duration);
                particles[i]->m_position += settings->m_gravity * static_cast<f32>(particle_age) * particle_clamp_age;
                
                f32 random_value = std::get_random_f(0.f, 1.f);
                f32 start_size = glm::mix(settings->m_source_size.x,
                                          settings->m_source_size.y, random_value);
                f32 end_size = glm::mix(settings->m_destination_size.x,
                                        settings->m_destination_size.y, random_value);
                particles[i]->m_size = glm::vec2(glm::mix(start_size, end_size, particle_clamp_age));
                
                particles[i]->m_color = glm::mix(settings->m_source_color,
                                                 settings->m_destination_color,
                                                 particle_clamp_age);
                
                glm::vec3 vertex_position_worldspace = particles[i]->m_position +
                camera_right_worldspace * -.5f * particles[i]->m_size.x +
                camera_up_worldspace * -.5f * particles[i]->m_size.y;
                vertices[i * 4 + 0].m_position = vertex_position_worldspace;
                
                vertex_position_worldspace = particles[i]->m_position +
                camera_right_worldspace * .5f * particles[i]->m_size.x +
                camera_up_worldspace * -.5f * particles[i]->m_size.y;
                vertices[i * 4 + 1].m_position = vertex_position_worldspace;
                
                vertex_position_worldspace = particles[i]->m_position +
                camera_right_worldspace * .5f * particles[i]->m_size.x +
                camera_up_worldspace * .5f * particles[i]->m_size.y;
                vertices[i * 4 + 2].m_position = vertex_position_worldspace;
                
                vertex_position_worldspace = particles[i]->m_position +
                camera_right_worldspace * -.5f * particles[i]->m_size.x +
                camera_up_worldspace * .5f * particles[i]->m_size.y;
                vertices[i * 4 + 3].m_position = vertex_position_worldspace;
                
                vertices[i * 4 + 0].m_color = particles[i]->m_color;
                vertices[i * 4 + 1].m_color = particles[i]->m_color;
                vertices[i * 4 + 2].m_color = particles[i]->m_color;
                vertices[i * 4 + 3].m_color = particles[i]->m_color;
            }
            mesh->get_vbo()->unlock();
        });
    }
    
    void ces_particle_emitter_system::on_feed_end(f32 dt)
    {
        
    }
}
