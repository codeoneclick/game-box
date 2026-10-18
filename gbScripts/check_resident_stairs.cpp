#include "resident_behavior.h"
#include <cassert>
#include <cmath>
#include <iostream>
using namespace game;
resident_navigation rectangle(float z0,float z1,float y,float yaw=0){
    terrain_mesh_data m;
    for(auto p:{resident_point{-8,z0},resident_point{-8,z1},resident_point{8,z1},resident_point{8,z0}})
        m.vertices.push_back({{{p.x*std::cos(yaw)+p.z*std::sin(yaw),y,-p.x*std::sin(yaw)+p.z*std::cos(yaw)}},{{0,1,0}},{{255,255,255,255}}});
    m.indices={0,1,2,0,2,3};return build_resident_navigation(m,y,.5f);
}
void assert_tread_clearance(const resident_state& s){
    if(!s.stair_transit)return;
    const auto& link=s.stair;
    const float dx=link.top.x-link.bottom.x,dz=link.top.z-link.bottom.z;
    const float length=std::sqrt(dx*dx+dz*dz);
    const float along=((s.position.x-link.bottom.x)*dx+(s.position.z-link.bottom.z)*dz)/length-link.bottom_deck;
    float expected=link.bottom_y;
    if(along>=0&&along<=stairs_run){
        const int tread=std::min(stairs_step_count-1,static_cast<int>(std::floor(along/(stairs_run/float(stairs_step_count)))));
        expected=link.bottom_y+(link.top_y-link.bottom_y)*(tread+1)/float(stairs_step_count)+.018f;
    }else if(along>stairs_run)expected=link.top_y;
    assert(std::abs(s.elevation-expected)<.0001f);
}
int main(){
    std::vector<resident_navigation> nav={rectangle(-12,0,2),rectangle(0,12,4)};
    terrain_config config;config.shore_height=0;EditorStairs stairs;stairs.id=7;stairs.position={{0,2,0}};
    auto links=build_resident_stair_links({stairs},config,nav);assert(links.size()==1);
    // Probe every tread, both flat decks, and the upper cliff at local z=1.65.
    // Direction and retreat must not change the support height at a given XZ.
    for(bool up:{false,true})for(bool retreat:{false,true})
    for(float local_z:{-2.1f,-1.75f,-1.25f,-.75f,-.25f,.25f,.75f,1.25f,1.65f,1.75f,2.1f}){
        resident_state probe;probe.available=true;probe.stair=links[0];probe.stair_pending=true;probe.stair_transit=true;
        probe.stair_up=up;probe.stair_returning=retreat;
        const auto& link=probe.stair;
        const float length=link.bottom_deck+stairs_run+link.top_deck;
        const float fraction=(local_z+stairs_run*.5f+link.bottom_deck)/length;
        probe.stair_progress=up?fraction:1-fraction;
        probe.position={0,local_z};size_t probe_terrace=up?0:1;
        step_resident_with_stairs(nav,links,.000001f,probe_terrace,probe);
        assert_tread_clearance(probe);
        if(local_z==1.65f)assert(std::abs(probe.elevation-4.018f)<.0001f);
    }
    resident_state s;s.position={0,-5};s.available=true;size_t terrace=0;
    assert(!begin_resident_stairs(nav,{},0,terrace,s));
    assert(begin_resident_stairs(nav,links,0,terrace,s));
    bool climbed=false;
    for(int i=0;i<300;++i){auto previous=s.position;step_resident_with_stairs(nav,links,.1f,terrace,s);
        float dx=s.position.x-previous.x,dz=s.position.z-previous.z;
        assert(std::sqrt(dx*dx+dz*dz)<.121f);
        if(dx*dx+dz*dz>1.e-10f)assert(std::abs(std::atan2(dx,dz)-s.yaw)<.001f);
        if(s.stair_transit){climbed=true;assert_tread_clearance(s);assert(s.elevation>=2&&s.elevation<=4.019f);assert(terrace==0);}
        if(terrace==1)break;
    }
    assert(climbed&&terrace==1&&nav[1].is_safe(s.position)&&s.elevation==4);
    assert(begin_resident_stairs(nav,links,0,terrace,s));
    for(int i=0;i<300&&terrace==1;++i){step_resident_with_stairs(nav,links,.1f,terrace,s);assert_tread_clearance(s);}
    assert(terrace==0&&nav[0].is_safe(s.position)&&s.elevation==2);
    assert(begin_resident_stairs(nav,links,0,terrace,s));
    while(!s.stair_transit)step_resident_with_stairs(nav,links,.1f,terrace,s);
    for(int i=0;i<10;++i)step_resident_with_stairs(nav,links,.1f,terrace,s);
    auto previous=s.position;
    rebind_resident_stairs(nav,{},terrace,s);
    assert(s.position.x==previous.x&&s.position.z==previous.z);
    for(int i=0;i<300&&s.stair_pending;++i){step_resident_with_stairs(nav,{},.1f,terrace,s);assert_tread_clearance(s);}
    assert(!s.stair_pending&&terrace==0&&s.available&&nav[0].is_safe(s.position));
    auto absent=build_resident_stair_links({},config,nav);assert(absent.empty());
    auto blocked=nav;blocked[1].walkable.assign(blocked[1].walkable.size(),0);
    assert(build_resident_stair_links({stairs},config,blocked).empty());
    // The shore-to-first-terrace rise is layer_height - shore_height.
    auto shore_config=config;shore_config.shore_height=.45f;
    auto shore_nav=std::vector<resident_navigation>{rectangle(-12,0,.45f),rectangle(0,12,2)};
    auto shore_stairs=stairs;shore_stairs.position[1]=.45f;
    auto shore_links=build_resident_stair_links({shore_stairs},shore_config,shore_nav);
    assert(shore_links.size()==1);
    resident_state shore;shore.available=true;shore.position={0,-5};size_t shore_terrace=0;
    assert(begin_resident_stairs(shore_nav,shore_links,0,shore_terrace,shore));
    for(int i=0;i<300&&shore_terrace==0;++i){step_resident_with_stairs(shore_nav,shore_links,.1f,shore_terrace,shore);assert_tread_clearance(shore);}
    assert(shore_terrace==1&&shore.elevation==2&&shore_nav[1].is_safe(shore.position));
    // Pending approaches cancel without moving; an interrupted flight can finish
    // to the other landing if a terrain update removed its source terrace.
    assert(begin_resident_stairs(nav,links,0,terrace,s));
    previous=s.position;rebind_resident_stairs(nav,{},terrace,s);
    assert(!s.stair_pending&&s.position.x==previous.x&&s.position.z==previous.z);
    assert(begin_resident_stairs(nav,links,0,terrace,s));
    while(!s.stair_transit)step_resident_with_stairs(nav,links,.1f,terrace,s);
    auto removed_source=nav;removed_source[0].walkable.assign(nav[0].walkable.size(),0);
    rebind_resident_stairs(removed_source,{},terrace,s);
    for(int i=0;i<300&&s.stair_pending;++i)step_resident_with_stairs(removed_source,{},.1f,terrace,s);
    assert(terrace==1&&s.available&&nav[1].is_safe(s.position));
    // A rotated flight preserves world-space direction and terrain assignments.
    const float yaw=1.570796327f;stairs.yaw=yaw;
    auto rotated=std::vector<resident_navigation>{rectangle(-12,0,2,yaw),rectangle(0,12,4,yaw)};
    auto rotated_links=build_resident_stair_links({stairs},config,rotated);assert(rotated_links.size()==1);
    resident_state r;r.available=true;r.position={-5,0};terrace=0;
    assert(begin_resident_stairs(rotated,rotated_links,0,terrace,r));
    for(int i=0;i<300&&terrace==0;++i){step_resident_with_stairs(rotated,rotated_links,.1f,terrace,r);assert_tread_clearance(r);}
    assert(terrace==1&&rotated[1].is_safe(r.position)&&std::abs(r.yaw-yaw)<.001f);
    // Authored terraces above the eight initially populated levels remain reachable.
    for(int lower:{8,15}){
        std::vector<resident_navigation> high_nav(16);
        const float bottom=(lower-1)*config.layer_height,top=lower*config.layer_height;
        high_nav[lower-1]=rectangle(-12,0,bottom);high_nav[lower]=rectangle(0,12,top);
        EditorStairs high;high.id=99;high.lower_terrace=lower;high.position={{0,bottom,-1.65f}};
        const auto high_links=build_resident_stair_links({high},config,high_nav);assert(high_links.size()==1);
        resident_state high_state;high_state.available=true;high_state.position={0,-5};size_t high_terrace=lower-1;
        assert(begin_resident_stairs(high_nav,high_links,0,high_terrace,high_state));
        for(int i=0;i<300&&high_terrace==size_t(lower-1);++i)step_resident_with_stairs(high_nav,high_links,.1f,high_terrace,high_state);
        assert(high_terrace==size_t(lower)&&high_state.elevation==top);
    }
    // Without installed links the former terrace-locked behavior is identical.
    auto original=create_resident_state(nav[0],987);auto wrapper=original;terrace=0;
    for(int i=0;i<1000;++i){step_resident(nav[0],.1f,original);step_resident_with_stairs(nav,{},.1f,terrace,wrapper);
        assert(terrace==0&&wrapper.position.x==original.position.x&&wrapper.position.z==original.position.z&&wrapper.random_state==original.random_state);}
    std::cout<<"Resident stairs CPU checks passed\n";
}
