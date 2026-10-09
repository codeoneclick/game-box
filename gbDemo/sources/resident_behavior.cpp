#include "resident_behavior.h"
#include <algorithm>
#include <cmath>
namespace game {
namespace {uint32_t random(resident_state& s){auto& x=s.random_state;x^=x<<13;x^=x>>17;x^=x<<5;return x;}
void stop(resident_state& s){s.walking=false;s.route.clear();s.route_index=0;s.idle_remaining=2.f+(random(s)%3001)/1000.f;}}
resident_state create_resident_state(const resident_navigation& n,uint32_t seed){
    resident_state s;s.random_state=seed?seed:1;float best=1e9f;
    // Foreground dry shore, close to the default camera and water props.
    for(size_t i=0;i<n.walkable.size();++i)if(n.walkable[i]){auto p=n.center(static_cast<int>(i));float distance=(p.x+62)*(p.x+62)+(p.z-74)*(p.z-74);
        if(distance<best){best=distance;s.position=p;s.available=true;}}
    if(s.available)s.reachable=n.reachable_points(s.position);return s;
}
void step_resident(const resident_navigation& n,float dt,resident_state& s){
    if(!s.available||!std::isfinite(dt)||dt<=0)return;dt=std::min(dt,.1f);
    if(!n.is_safe(s.position)){s.available=false;s.walking=false;return;}
    if(!s.walking){
        s.idle_remaining-=dt;if(s.idle_remaining>0)return;
        for(int attempt=0;attempt<16&&!s.reachable.empty();++attempt){auto p=s.reachable[random(s)%s.reachable.size()];
            float dx=p.x-s.position.x,dz=p.z-s.position.z;float d=dx*dx+dz*dz;
            // Frequent local strolls keep the resident visible and avoid whole-island trips.
            if(d<9||d>900)continue;auto path=n.find_path(s.position,p);
            if(path.size()>1){s.route=std::move(path);s.route_index=1;s.walking=true;break;}
        }if(!s.walking){stop(s);return;}
    }
    float budget=dt*1.2f,turn_budget=dt*3.14159265f;
    while(budget>0&&s.route_index<s.route.size()){
        auto p=s.route[s.route_index];float dx=p.x-s.position.x,dz=p.z-s.position.z,d=std::sqrt(dx*dx+dz*dz);
        if(d<.00001f){++s.route_index;continue;}
        float target=std::atan2(dx,dz),delta=std::atan2(std::sin(target-s.yaw),std::cos(target-s.yaw));
        const float turn=std::max(-turn_budget,std::min(turn_budget,delta));
        s.yaw+=turn;turn_budget-=std::abs(turn);
        float travel=std::min(d,budget);resident_point next={s.position.x+dx*travel/d,s.position.z+dz*travel/d};
        if(!n.segment_is_safe(s.position,next)){stop(s);return;}s.position=next;budget-=travel;
        if(travel>=d-.00001f)++s.route_index;
    }
    if(s.route_index>=s.route.size())stop(s);
}
}
