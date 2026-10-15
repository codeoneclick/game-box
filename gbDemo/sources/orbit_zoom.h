#pragma once
#include <algorithm>
#include <cmath>

namespace game {
    struct orbit_zoom {
        float radius=245.f,target=245.f,velocity=0.f;
        float minimum=15.f,maximum=700.f;
        void reset(float value,float lower,float upper){
            minimum=lower;maximum=upper;
            radius=target=std::max(minimum,std::min(maximum,value));velocity=0.f;
        }
        void scroll(float delta){
            if(!std::isfinite(delta)||delta==0.f)return;
            const double exponent=std::max(-80.0,std::min(80.0,-double(delta)*.12));
            target=static_cast<float>(std::max(double(minimum),std::min(double(maximum),target*std::exp(exponent))));
        }
        bool advance(float dt){
            if(!std::isfinite(dt)||dt<=0.f)return false;
            const float previous=radius;
            // Exact critically damped spring solution: stable at any frame rate.
            constexpr double frequency=14.0;
            const double offset=double(radius)-target;
            const double impulse=(velocity+frequency*offset)*dt;
            const double decay=std::exp(-frequency*dt);
            radius=static_cast<float>(target+(offset+impulse)*decay);
            velocity=static_cast<float>((velocity-frequency*impulse)*decay);
            if(radius<minimum||radius>maximum){radius=std::max(minimum,std::min(maximum,radius));velocity=0.f;}
            if(std::abs(radius-target)<.0001f&&std::abs(velocity)<.001f){radius=target;velocity=0.f;}
            return radius!=previous;
        }
    };
}
