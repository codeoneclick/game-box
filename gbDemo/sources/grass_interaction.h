#pragma once
#include <array>
#include <vector>
#include <cmath>
#include <algorithm>

namespace game {
struct grass_impulse {
    float x,z,y,dx,dz,age;
};
// Retain a bounded, fixed-rate trail, rather than a copy per rendered frame.
class grass_interaction {
    std::vector<grass_impulse> m_trail;
    float m_clock=0;
public:
    const std::vector<grass_impulse>& trail() const {return m_trail;}
    void update(float dt,const std::vector<grass_impulse>& current) {
        dt=std::isfinite(dt)?std::max(0.f,std::min(dt,.1f)):0.f;
        for(auto& p:m_trail)p.age+=dt;
        m_trail.erase(std::remove_if(m_trail.begin(),m_trail.end(),[](const grass_impulse& p){return p.age>=1.5f;}),m_trail.end());
        m_clock+=dt;
        if(m_clock>=.125f){
            m_clock=std::fmod(m_clock,.125f);
            for(auto p:current){p.age=.001f;m_trail.push_back(p);}
        }
    }
    static float strength(float age) {
        if(age<=0.f)return 1.f;
        // Smooth recovery without a discontinuity when the trail expires.
        const float t=std::max(0.f,1.f-age/1.5f);
        return .18f*t*t*(3.f-2.f*t);
    }
};
}
