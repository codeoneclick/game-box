#include "resident_behavior.h"
#include <algorithm>
#include <cmath>
#include <stdexcept>
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
std::vector<resident_state> create_resident_population(const resident_navigation& n,size_t count,uint32_t seed){
    std::vector<resident_state> population;if(!count)return population;
    auto first=create_resident_state(n,seed);
    if(!first.available)throw std::runtime_error("Terrace has no safe resident spawn");
    population.reserve(count);
    std::vector<std::shared_ptr<const std::vector<resident_point>>> regions(n.walkable.size());
    auto share_region=[&](resident_state& s){
        const int cell=n.cell_at(s.position);
        if(regions[cell]) {s.shared_reachable=regions[cell];return;}
        if(s.reachable.empty())s.reachable=n.reachable_points(s.position);
        auto points=std::make_shared<const std::vector<resident_point>>(std::move(s.reachable));
        for(const auto& p:*points)regions[n.cell_at(p)]=points;
        s.shared_reachable=std::move(points);
    };
    share_region(first);
    population.push_back(first);
    std::vector<int> candidates;
    for(size_t i=0;i<n.walkable.size();++i)if(n.walkable[i])candidates.push_back(static_cast<int>(i));
    auto sequence=first;
    for(size_t i=candidates.size();i>1;--i)std::swap(candidates[i-1],candidates[random(sequence)%i]);
    for(auto cell:candidates){
        if(population.size()==count)break;
        const auto p=n.center(cell);bool spaced=true;
        for(const auto& other:population){float dx=p.x-other.position.x,dz=p.z-other.position.z;
            if(dx*dx+dz*dz<2.4f*2.4f){spaced=false;break;}}
        if(!spaced||!n.is_safe(p))continue;
        resident_state s;s.position=p;s.available=true;s.random_state=random(sequence);
        if(!s.random_state)s.random_state=1;
        s.idle_remaining=2.f+(random(s)%3001)/1000.f;
        share_region(s);population.push_back(std::move(s));
    }
    if(population.size()!=count)throw std::runtime_error("Terrace cannot fit requested resident population");
    return population;
}
void step_resident(const resident_navigation& n,float dt,resident_state& s){
    if(!s.available||!std::isfinite(dt)||dt<=0)return;dt=std::min(dt,.1f);
    if(!n.is_safe(s.position)){s.available=false;s.walking=false;return;}
    if(!s.walking){
        s.idle_remaining-=dt;if(s.idle_remaining>0)return;
        const auto& reachable=s.shared_reachable?*s.shared_reachable:s.reachable;
        for(int attempt=0;attempt<16&&!reachable.empty();++attempt){auto p=reachable[random(s)%reachable.size()];
            float dx=p.x-s.position.x,dz=p.z-s.position.z;float d=dx*dx+dz*dz;
            // Frequent local strolls keep the resident visible and avoid whole-island trips.
            if(d<9||d>900)continue;auto path=n.find_path(s.position,p);
            if(path.size()>1){s.route=std::move(path);s.route_index=1;s.walking=true;break;}
        }if(!s.walking){stop(s);return;}
    }
    const auto previous=s.position;
    float budget=dt*1.2f;
    while(budget>0&&s.route_index<s.route.size()){
        auto p=s.route[s.route_index];float dx=p.x-s.position.x,dz=p.z-s.position.z,d=std::sqrt(dx*dx+dz*dz);
        if(d<.00001f){++s.route_index;continue;}
        float travel=std::min(d,budget);resident_point next={s.position.x+dx*travel/d,s.position.z+dz*travel/d};
        if(!n.segment_is_safe(s.position,next)){stop(s);break;}s.position=next;budget-=travel;
        if(travel>=d-.00001f)++s.route_index;
    }
    const float moved_x=s.position.x-previous.x,moved_z=s.position.z-previous.z;
    if(moved_x*moved_x+moved_z*moved_z>1.e-12f)s.yaw=std::atan2(moved_x,moved_z);
    if(s.walking&&s.route_index>=s.route.size())stop(s);
}
}
