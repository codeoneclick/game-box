//
//  ces_particle_emitter_component.h
//  gbCore
//
//  Created by sergey.sergeev on 9/1/15.
//  Copyright (c) 2015 sergey.sergeev. All rights reserved.
//

#ifndef ces_particle_emitter_component_h
#define ces_particle_emitter_component_h

#include "ces_base_component.h"
#include "configuration_accessor.h"

namespace gb
{
    struct particle
    {
        glm::vec3 m_position;
        glm::vec3 m_velocity;
        glm::vec2 m_size;
        glm::u8vec4 m_color;
        ui64 m_timestamp;
    };
    
    class ces_particle_emitter_component : public ces_base_component
    {
    private:
        
    protected:
        
        std::shared_ptr<particle_emitter_configuration> m_settings;
        std::vector<particle> m_particles;
        mesh_shared_ptr m_mesh;
        f32 m_last_emitt_timestamp;
        
        void emitt_particle(ui32 index);
        void create_particles_mesh();
        
    public:
        
        ces_particle_emitter_component();
        ~ces_particle_emitter_component();
        
        mesh_shared_ptr get_mesh() const;
        
        void set_settings(const std::shared_ptr<particle_emitter_configuration>& settings);
        void update(f32 deltatime);
    };
};


#endif
