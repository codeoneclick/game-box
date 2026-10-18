#include "ocean_wave_model.h"
#include <cmath>
#include <algorithm>
#include <cstdint>
#include <stdexcept>
namespace game
{
    void ocean_settings::validate() const
    {
        if(!std::isfinite(interaction_size)||interaction_size<=0||
           !std::isfinite(interaction_resolution)||interaction_resolution<1||
           !std::isfinite(water_detail_radius)||water_detail_radius<=0||!std::isfinite(water_detail_radius*3.125f))
            throw std::invalid_argument("Invalid ocean field dimensions");
        if(!std::isfinite(mean_level)) throw std::invalid_argument("Nonfinite ocean level");
        if(!std::isfinite(sun_direction.x)||!std::isfinite(sun_direction.y)||!std::isfinite(sun_direction.z)||
            sun_direction.x*sun_direction.x+sun_direction.y*sun_direction.y+sun_direction.z*sun_direction.z<.0001f)
            throw std::invalid_argument("Invalid ocean sun direction");
        for(float v:{foam_mid_min_width,foam_mid_max_width,foam_warp_strength,foam_height_offset,foam_width,foam_inner_distance,foam_noise_scale,foam_noise_speed,foam_sharpness,foam_outer_strength,
            crest_foam_strength,facet_strength,absorption,refraction_strength,highlight_strength,depth_fade_distance,
            refraction_depth_distance,fresnel_power,reflection_strength,caustic_scale,caustic_speed,caustic_strength,caustic_depth_falloff})
            if(!std::isfinite(v)||v<0)throw std::invalid_argument("Invalid ocean visual setting");
        if(foam_mid_min_width<=0||foam_mid_max_width<foam_mid_min_width||foam_height_offset>.1f||foam_inner_distance<=0||foam_width<=foam_inner_distance||foam_width>8||foam_noise_scale<=0||foam_sharpness<=0||
            depth_fade_distance<=0||refraction_depth_distance<=0||fresnel_power<=0||caustic_scale<=0||reflection_strength>1||foam_outer_strength>1||crest_foam_strength>1)
            throw std::invalid_argument("Invalid ocean visual range");
        for(const auto& color:{deep_color,shallow_color,foam_color,foam_shadow_color})for(float v:color)
            if(!std::isfinite(v)||v<0||v>1)throw std::invalid_argument("Invalid ocean color");
        float slope=0;
        for(const auto& w:waves) {
            if(!std::isfinite(w.dx)||!std::isfinite(w.dz)||!std::isfinite(w.wavelength)||!std::isfinite(w.amplitude)||!std::isfinite(w.steepness)||!std::isfinite(w.speed)||
               w.dx*w.dx+w.dz*w.dz<.0001f||w.wavelength<=0||w.amplitude<0||w.steepness<0||w.steepness>1||w.speed<0)
                throw std::invalid_argument("Invalid Gerstner wave");
            slope+=6.283185307f/w.wavelength*w.amplitude*w.steepness;
        }
        if(slope>=.8f) throw std::invalid_argument("Ocean waves could fold");
    }
    namespace {
        float field_hash(float x,float z){uint32_t h=static_cast<uint32_t>(static_cast<int32_t>(x))*73856093u^static_cast<uint32_t>(static_cast<int32_t>(z))*19349663u;h^=h>>13;h*=1274126177u;return float(h&65535u)/65535.f;}
        float mix(float a,float b,float t){return a+(b-a)*t;}
        float noise(float x,float z){float ix=std::floor(x),iz=std::floor(z),fx=x-ix,fz=z-iz;fx=fx*fx*(3-2*fx);fz=fz*fz*(3-2*fz);return mix(mix(field_hash(ix,iz),field_hash(ix+1,iz),fx),mix(field_hash(ix,iz+1),field_hash(ix+1,iz+1),fx),fz);}
        float smooth(float a,float b,float v){float f=std::max(0.f,std::min(1.f,(v-a)/(b-a)));return f*f*(3-2*f);}
        float wave(float x,float z,const ocean_wave& w,float t,float sx,float sz){
            float l=std::sqrt(w.dx*w.dx+w.dz*w.dz),dx=w.dx/l,dz=w.dz/l;
            float local=noise(x*.037f+sx,z*.037f+sz);
            float period=4.5f+4.f*noise(sx,sz),clock=t+local*7.f+sx*.13f;
            float age=clock-period*std::floor(clock/period);
            float onset=1-std::exp(-age*4.f);
            float pulse=onset*onset*std::exp(-age*.9f)*(1-smooth(period-1,period,age));
            float energy=.18f+2.f*pulse+.7f*noise(x*.08f+t*.17f+sx,z*.08f-t*.12f+sz);
            float phase_noise=noise(x*.045f-t*.13f+sx,z*.045f+t*.11f+sz)-.5f;
            float k=6.283185307f/w.wavelength;
            float phase=k*(dx*x+dz*z-w.speed*t)+2.4f*phase_noise;
            float counter=k*((-dz*x+dx*z)*.85f+w.speed*t*.73f)+sx+phase_noise;
            float primary=std::sin(phase);
            return w.amplitude*energy*(.8f*(primary+.18f*(primary*primary-.5f))+.45f*std::sin(counter));
        }
        float height(const ocean_settings& s,float x,float z,float t){float dx=.031f*t,dz=-.019f*t;
            float wx=noise(x*.027f+dx,z*.027f+dz)-.5f,wz=noise(-z*.041f-dx+19,x*.041f-dz+7)-.5f;
            float qx=x+wx*8,qz=z+wz*8;
            float raw=wave(qx,qz,s.waves[0],t,0,5)+wave(qx+7,qz+19,s.waves[3],t,7,19)+wave(qx+wx*2.7f,qz+wz*2.7f,s.waves[1],t,17,31)+wave(x+wx*.8f,z+wz*.8f,s.waves[2],t,41,11);
            float limit=std::max(.001f,1.09f*(s.waves[0].amplitude+s.waves[1].amplitude+s.waves[2].amplitude+s.waves[3].amplitude));
            return s.mean_level+limit*std::tanh(raw/limit);
        }
    }
    ocean_surface_sample evaluate_ocean(const ocean_settings& s,float x,float z,double seconds) {
        s.validate();if(!std::isfinite(x)||!std::isfinite(z)||!std::isfinite(seconds))throw std::invalid_argument("Nonfinite ocean sample");
        float t=static_cast<float>(seconds),dx=(height(s,x+.05f,z,t)-height(s,x-.05f,z,t))/.1f,dz=(height(s,x,z+.05f,t)-height(s,x,z-.05f,t))/.1f;
        float l=std::sqrt(dx*dx+1+dz*dz);return {{x,height(s,x,z,t),z},{-dx/l,1/l,-dz/l}};
    }
    ocean_surface_sample sample_ocean(const ocean_settings& s,float x,float z,double seconds){return evaluate_ocean(s,x,z,seconds);}
}
