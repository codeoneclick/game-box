#pragma once
#include "ocean_wave_model.h"
#include <vector>
namespace game {
    struct buoyancy_probe {ocean_vec3 local;float volume=1;};
    struct buoyancy_settings {float mass=60,inertia_x=100,inertia_z=40,draft=.5f,damping=3.5f,angular_damping=3.5f;};
    struct buoyancy_state {ocean_vec3 position;float yaw=0,pitch=0,roll=0,vertical_velocity=0,pitch_velocity=0,roll_velocity=0,wetness=0;};
    void step_buoyancy(const ocean_settings&,const buoyancy_settings&,const std::vector<buoyancy_probe>&,double time,float dt,buoyancy_state&);
}
