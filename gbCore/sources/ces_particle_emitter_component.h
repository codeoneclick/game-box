//
//  ces_particle_emitter_component.hpp
//  gbCore
//
//  Created by serhii.s on 3/21/19.
//  Copyright © 2019 sergey.sergeev. All rights reserved.
//

#pragma once

#include "ces_base_component.h"

namespace gb
{
    class ces_particle_emitter_component : public ces_base_component
    {
    public:
        
        struct particle
        {
            glm::vec3 m_spawn_position;
            glm::vec3 m_delta_position;
            glm::vec3 m_velocity;
            glm::vec2 m_source_size;
            glm::vec2 m_destination_size;
            glm::vec2 m_current_size;
            glm::u8vec4 m_color;
            f32 m_age;
            f32 m_duration;
        };
        
        struct emitter_settings
        {
            ui32 m_num_particles;
            
            f32 m_duration;
            f32 m_live_time;
            
            glm::vec2 m_source_size;
            glm::u8vec4 m_source_color;
            
            glm::vec2 m_destination_size;
            glm::u8vec4 m_destination_color;
            
            f32 m_min_horizontal_velocity;
            f32 m_max_horizontal_velocity;
            
            f32 m_min_vertical_velocity;
            f32 m_max_vertical_velocity;
            
            f32 m_velocity_sensitivity;
            
            f32 m_min_emitt_interval;
            f32 m_max_emitt_interval;
            
            f32 m_end_velocity;
            
            glm::vec3 m_gravity;
            
            i32 m_num_particles_per_emitt = 4;
        };
    
    private:
        
    protected:
        
        std::shared_ptr<emitter_settings> m_settings;
        std::vector<std::shared_ptr<particle>> m_particles;
        f32 m_emitt_delay = 0.f;
        i32 m_num_particles_per_emitt = 0;
        glm::vec3 m_last_emitted_particle_position;
        bool m_should_use_last_emitted_particle_position = false;
        bool m_is_enabled = false;
        
    public:
        
        STTI_CLASS_GUID(ces_particle_emitter_component, ces_base_component::g_guids_container)
        ces_particle_emitter_component();
        ~ces_particle_emitter_component();
        
        void set_settings(const std::shared_ptr<emitter_settings>& settings);
        std::shared_ptr<emitter_settings> get_settings() const;
        
        mesh_3d_shared_ptr construct_particles_mesh();
        
        std::vector<std::shared_ptr<particle>> get_particles() const;
        void emitt_particle(ui32 index, const glm::vec3& position);
        
        void update_emitt_delay(f32 value);
        void dec_emitt_delay(f32 dt);
        bool is_emitt_delay_exist() const;
        
        void set_num_particles_per_emitt(i32 value);
        i32 get_num_particles_per_emitt() const;
        
        void set_enabled(bool value);
        bool get_enabled() const;
        
        glm::vec3 get_last_emitted_particle_position() const;
        bool should_use_last_emitted_particle_position() const;
    };
};

