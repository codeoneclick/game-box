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

namespace game {
namespace {
float point_distance(resident_point a,resident_point b){float x=a.x-b.x,z=a.z-b.z;return std::sqrt(x*x+z*z);}
bool same_link(const resident_stair_link& a,const resident_stair_link& b){
    return a.id==b.id&&a.lower==b.lower&&a.upper==b.upper&&point_distance(a.bottom,b.bottom)<.001f&&
        point_distance(a.top,b.top)<.001f&&std::abs(a.bottom_y-b.bottom_y)<.001f&&std::abs(a.top_y-b.top_y)<.001f;
}
}
std::vector<resident_stair_link> build_resident_stair_links(const std::vector<EditorStairs>& stairs,const terrain_config& config,const std::vector<resident_navigation>& nav){
    std::vector<resident_stair_link> result;
    for(const auto& flight:stairs){
        if(flight.lower_terrace<1||static_cast<size_t>(flight.lower_terrace)>=nav.size()||!std::isfinite(flight.yaw)||
           !std::isfinite(flight.position[0])||!std::isfinite(flight.position[1])||!std::isfinite(flight.position[2]))continue;
        resident_stair_link link;link.id=flight.id;link.lower=flight.lower_terrace-1;link.upper=flight.lower_terrace;
        link.bottom_y=nav[link.lower].shore_y;link.top_y=nav[link.upper].shore_y;
        const float rise=flight.lower_terrace==1?config.layer_height-config.shore_height:config.layer_height;
        if(!std::isfinite(rise)||rise<=0||std::abs(flight.position[1]-link.bottom_y)>.05f||std::abs(link.top_y-link.bottom_y-rise)>.05f)continue;
        const float dx=std::sin(flight.yaw),dz=std::cos(flight.yaw);
        // Search only along the flight centerline, outward from each nominal landing.
        // This retains the flight corridor and cannot bind a remote island cell.
        auto landing=[&](size_t terrace,float direction,resident_point& p){
            for(float extra=0;extra<=2.001f;extra+=.125f){
                const float distance=direction*(stairs_run*.5f+.5f+extra);
                p={flight.position[0]+dx*distance,flight.position[2]+dz*distance};
                if(nav[terrace].is_safe(p))return true;
            }return false;
        };
        if(landing(link.lower,-1,link.bottom)&&landing(link.upper,1,link.top)){
            resident_point center={flight.position[0],flight.position[2]};
            link.bottom_deck=point_distance(center,link.bottom)-stairs_run*.5f;
            link.top_deck=point_distance(center,link.top)-stairs_run*.5f;
            result.push_back(link);
        }
    }return result;
}
bool begin_resident_stairs(const std::vector<resident_navigation>& nav,const std::vector<resident_stair_link>& links,size_t index,size_t terrace,resident_state& s){
    if(!s.available||s.stair_pending||index>=links.size()||terrace>=nav.size())return false;
    const auto& link=links[index];if(terrace!=link.lower&&terrace!=link.upper)return false;
    if(link.lower>=nav.size()||link.upper>=nav.size())return false;
    const bool up=terrace==link.lower;const auto source=up?link.bottom:link.top;
    auto path=nav[terrace].find_path(s.position,source);if(path.empty())return false;
    s.stair=link;s.stair_up=up;s.stair_pending=true;s.stair_transit=false;s.stair_returning=false;s.stair_progress=0;
    s.route=std::move(path);s.route_index=s.route.size()>1?1:0;s.walking=true;s.elevation=nav[terrace].shore_y;return true;
}
void rebind_resident_stairs(const std::vector<resident_navigation>& nav,const std::vector<resident_stair_link>& links,size_t terrace,resident_state& s){
    if(!s.stair_pending)return;
    const bool retained=std::any_of(links.begin(),links.end(),[&](const resident_stair_link& link){return same_link(link,s.stair);});
    if(retained)return;
    if(s.stair_transit){
        s.stair_returning=true;
        // If terrain edits removed the source, complete the retained flight to its
        // safe destination instead. If neither landing survives, hide until refresh.
        const auto source=s.stair_up?s.stair.bottom:s.stair.top;
        const size_t destination=s.stair_up?s.stair.upper:s.stair.lower;
        const auto target=s.stair_up?s.stair.top:s.stair.bottom;
        if(terrace>=nav.size()||!nav[terrace].is_safe(source)){
            if(destination<nav.size()&&nav[destination].is_safe(target))s.stair_returning=false;
            else{s.available=false;s.walking=false;s.stair_pending=false;s.stair_transit=false;}
        }
    }else{s.stair_pending=false;stop(s);}
}
void step_resident_with_stairs(const std::vector<resident_navigation>& nav,const std::vector<resident_stair_link>& links,float dt,size_t& terrace,resident_state& s){
    if(terrace>=nav.size()||!s.available||!std::isfinite(dt)||dt<=0)return;
    const auto frame_previous=s.position;
    dt=std::min(dt,.1f);
    if(!s.stair_pending){
        s.elevation=nav[terrace].shore_y;
        // Stairs are occasional stroll destinations, using the same deterministic RNG.
        if(!links.empty()&&!s.walking&&s.idle_remaining<=dt&&(random(s)%3)==0){
            const size_t start=random(s)%links.size();
            for(size_t i=0;i<links.size();++i)if(begin_resident_stairs(nav,links,(start+i)%links.size(),terrace,s))break;
        }
        if(!s.stair_pending){step_resident(nav[terrace],dt,s);return;}
    }
    if(!s.stair_transit){
        const auto source=s.stair_up?s.stair.bottom:s.stair.top;
        if(point_distance(s.position,source)>.001f){
            step_resident(nav[terrace],dt,s);
            if(!s.walking&&point_distance(s.position,source)>.001f){s.stair_pending=false;return;}
            if(point_distance(s.position,source)>.001f)return;
            s.stair_transit=true;s.stair_progress=0;s.walking=true;return;
        }
        s.stair_transit=true;s.stair_progress=0;
    }
    const auto source=s.stair_up?s.stair.bottom:s.stair.top;
    const auto target=s.stair_up?s.stair.top:s.stair.bottom;
    const float start_y=s.stair_up?s.stair.bottom_y:s.stair.top_y;
    const float end_y=s.stair_up?s.stair.top_y:s.stair.bottom_y;
    const float length=std::sqrt(std::pow(point_distance(source,target),2)+(end_y-start_y)*(end_y-start_y));
    s.stair_progress=std::max(0.f,std::min(1.f,s.stair_progress+(s.stair_returning?-1.f:1.f)*dt*1.2f/std::max(length,.001f)));
    const auto previous=frame_previous;const float t=s.stair_progress;
    s.position={source.x+(target.x-source.x)*t,source.z+(target.z-source.z)*t};
    // Match the stone tread tops, including their .018m cap. Sample in
    // canonical uphill coordinates so descent and a cancelled flight's retreat
    // stand on exactly the same surface. Outside the flight, decks are flat.
    const float horizontal=point_distance(source,target);
    const float uphill=(s.stair_up?t:1.f-t)*horizontal-s.stair.bottom_deck;
    s.elevation=uphill<0?s.stair.bottom_y:s.stair.top_y;
    if(uphill>=0&&uphill<=stairs_run){
        const int tread=std::min(stairs_step_count-1,static_cast<int>(std::floor(uphill/(stairs_run/float(stairs_step_count)))));
        s.elevation=s.stair.bottom_y+(s.stair.top_y-s.stair.bottom_y)*(tread+1)/float(stairs_step_count)+.018f;
    }
    s.walking=true;
    if(point_distance(previous,s.position)>1.e-6f)s.yaw=std::atan2(s.position.x-previous.x,s.position.z-previous.z);
    if((s.stair_returning&&t<=0)||(!s.stair_returning&&t>=1)){
        if(!s.stair_returning)terrace=s.stair_up?s.stair.upper:s.stair.lower;
        s.stair_pending=false;s.stair_transit=false;s.stair_returning=false;stop(s);
        s.elevation=nav[terrace].shore_y;s.available=nav[terrace].is_safe(s.position);
        s.shared_reachable.reset();s.reachable=nav[terrace].reachable_points(s.position);
    }
}
}
