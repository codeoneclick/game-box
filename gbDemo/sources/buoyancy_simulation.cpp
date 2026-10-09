#include "buoyancy_simulation.h"
#include <algorithm>
#include <cmath>
#include <stdexcept>
namespace game {
    void step_buoyancy(const ocean_settings& ocean,const buoyancy_settings& settings,const std::vector<buoyancy_probe>& probes,double time,float dt,buoyancy_state& state)
    {
        ocean.validate();
        if(!std::isfinite(time)||!std::isfinite(dt)||dt<0||!std::isfinite(settings.mass)||settings.mass<=0||
           !std::isfinite(settings.inertia_x)||settings.inertia_x<=0||!std::isfinite(settings.inertia_z)||settings.inertia_z<=0||
           !std::isfinite(settings.draft)||settings.draft<=0||probes.empty()||!std::isfinite(settings.damping)||settings.damping<0||
           !std::isfinite(settings.angular_damping)||settings.angular_damping<0)
            throw std::invalid_argument("Invalid buoyancy settings");
        float volume=0;
        for(const auto& p:probes){
            if(!std::isfinite(p.local.x)||!std::isfinite(p.local.y)||!std::isfinite(p.local.z)||!std::isfinite(p.volume)||p.volume<=0)
                throw std::invalid_argument("Invalid buoyancy probe");
            volume+=p.volume;
        }
        if(!std::isfinite(volume))throw std::invalid_argument("Invalid buoyancy volume");
        dt=std::min(dt,.25f);if(dt==0)return;
        const int steps=static_cast<int>(std::ceil(dt*120.f));const float h=dt/steps;
        for(int i=0;i<steps;++i){
            float force=-settings.mass*9.81f,torque_x=0,torque_z=0,wet=0;
            const float cp=std::cos(state.pitch),sp=std::sin(state.pitch),cr=std::cos(state.roll),sr=std::sin(state.roll),cy=std::cos(state.yaw),sy=std::sin(state.yaw);
            for(const auto& probe:probes){
                const auto p=probe.local;
                // Engine matrices are Rx * Rz * Ry: apply local yaw, roll, pitch.
                const float yaw_x=p.x*cy+p.z*sy,yaw_z=-p.x*sy+p.z*cy;
                const float rx=yaw_x*cr-p.y*sr,roll_y=yaw_x*sr+p.y*cr;
                const float ry=roll_y*cp-yaw_z*sp,rz=roll_y*sp+yaw_z*cp;
                const auto wave=sample_ocean(ocean,state.position.x+rx,state.position.z+rz,time+(i+1)*h);
                const float depth=std::max(wave.position.y-state.position.y-ry,0.f);
                const float lift=settings.mass*9.81f*probe.volume/volume*std::min(depth/settings.draft,3.f);
                force+=lift;torque_x-=rz*lift;torque_z+=rx*lift;wet=std::max(wet,std::min(depth/settings.draft,1.f));
            }
            state.vertical_velocity+=(force/settings.mass-settings.damping*state.vertical_velocity)*h;
            state.pitch_velocity+=(torque_x/settings.inertia_x-settings.angular_damping*state.pitch_velocity)*h;
            state.roll_velocity+=(torque_z/settings.inertia_z-settings.angular_damping*state.roll_velocity)*h;
            state.position.y+=state.vertical_velocity*h;state.pitch+=state.pitch_velocity*h;state.roll+=state.roll_velocity*h;
            state.wetness+=(wet-state.wetness)*(1.f-std::exp(-h*(wet>state.wetness?4.f:.25f)));
        }
    }
}
