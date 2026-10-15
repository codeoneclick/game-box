#include "terrain_generator.h"
#include "terrain_mesh_builder.h"
#include "resident_navigation.h"
#include "resident_behavior.h"
#include <cassert>
#include <cmath>
#include <iostream>
#include <limits>
int main(){
    auto quad=[](game::terrain_mesh_data& mesh,float x0,float z0,float x1,float z1){
        uint16_t base=static_cast<uint16_t>(mesh.vertices.size());
        for(auto p:{game::resident_point{x0,z0},game::resident_point{x0,z1},game::resident_point{x1,z1},game::resident_point{x1,z0}})
            mesh.vertices.push_back({{{p.x,.5f,p.z}},{{0,1,0}},{{255,255,255,255}}});
        for(auto i:{0,1,2,0,2,3})mesh.indices.push_back(base+static_cast<uint16_t>(i));
    };
    game::terrain_mesh_data ring;
    quad(ring,-10,-10,10,-5);quad(ring,-10,5,10,10);quad(ring,-10,-5,-5,5);quad(ring,5,-5,10,5);
    auto ring_nav=game::build_resident_navigation(ring,.5f);
    assert(!ring_nav.segment_is_safe({-7.5f,0},{7.5f,0}));
    auto ring_path=ring_nav.find_path({-7.5f,0},{7.5f,0});assert(!ring_path.empty());
    for(size_t i=1;i<ring_path.size();++i)assert(ring_nav.segment_is_safe(ring_path[i-1],ring_path[i]));
    game::terrain_mesh_data separate;quad(separate,-10,-10,-5,10);quad(separate,5,-10,10,10);
    assert(game::build_resident_navigation(separate,.5f).find_path({-7.5f,0},{7.5f,0}).empty());
    game::terrain_mesh_data narrow;quad(narrow,0,0,1,10);
    assert(!game::create_resident_state(game::build_resident_navigation(narrow,.5f),1).available);
    game::terrain_mesh_data wide;quad(wide,-10,-10,10,10);
    auto wide_nav=game::build_resident_navigation(wide,.5f);
    auto compare_support=[](const game::resident_navigation& cached){
        auto exact=cached;exact.solid_cells.clear();
        assert(std::count(cached.solid_cells.begin(),cached.solid_cells.end(),uint8_t(1))>0);
        for(int z=0;z<cached.height;++z)for(int x=0;x<cached.width;++x){
            for(auto offset:{.001f,.25f,.5f,.75f,.999f}){
                game::resident_point p={cached.origin_x+(x+offset)*cached.cell_size,cached.origin_z+(z+offset)*cached.cell_size};
                assert(cached.surface_supports(p)==exact.surface_supports(p));
                assert(cached.is_safe(p)==exact.is_safe(p));
            }
        }
    };
    compare_support(wide_nav);compare_support(ring_nav);
    // Buildings must override certified interior cells as well as triangles.
    auto occupied=wide_nav;
    game::resident_obstacle house;house.center={0,0};house.cosine=std::cos(.37f);house.sine=std::sin(.37f);house.half_width=2;house.half_depth=1;
    occupied.obstacles.push_back(house);
    assert(occupied.solid_cells[occupied.cell_at({0,0})]);
    assert(!occupied.surface_supports({0,0}));
    assert(!occupied.surface_supports({house.cosine*1.9f,-house.sine*1.9f}));
    assert(occupied.surface_supports({house.sine*1.1f,house.cosine*1.1f}));
    assert(!occupied.is_safe({0,0}));
    assert(!occupied.segment_is_safe({-7,0},{7,0}));
    compare_support(occupied);
    for(size_t i=0;i<occupied.walkable.size();++i)if(occupied.walkable[i]&&!occupied.is_safe(occupied.center(int(i))))occupied.walkable[i]=0;
    auto detour=occupied.find_path({-7,0},{7,0});assert(!detour.empty());
    for(size_t i=1;i<detour.size();++i)assert(occupied.segment_is_safe(detour[i-1],detour[i]));
    auto corner=game::create_resident_state(wide_nav,1);corner.position={0,0};corner.yaw=-1;corner.walking=true;
    corner.route={{0,0},{0,.03f},{.03f,.03f},{1,1}};corner.route_index=1;
    game::step_resident(wide_nav,.1f,corner);
    assert(std::abs(corner.yaw-std::atan2(corner.position.x,corner.position.z))<.00001f);
    game::terrain_config config;
    auto mesh=game::terrain_mesh_builder::build(game::terrain_generator::generate(config));
    auto nav=game::build_resident_navigation(mesh,config.shore_height);
    std::vector<game::resident_navigation> terraces;
    std::vector<std::vector<game::resident_state>> crowds;
    for(int level=0;level<config.layers_count;++level){
        terraces.push_back(game::build_resident_navigation(mesh,level?level*config.layer_height:config.shore_height));
        compare_support(terraces.back());
        crowds.push_back(game::create_resident_population(terraces.back(),25,1337+level));
        const auto& crowd=crowds.back();assert(crowd.size()==25);
        for(size_t i=0;i<crowd.size();++i){
            assert(crowd[i].available&&terraces.back().is_safe(crowd[i].position));
            for(size_t j=0;j<i;++j){float dx=crowd[i].position.x-crowd[j].position.x,dz=crowd[i].position.z-crowd[j].position.z;
                assert(dx*dx+dz*dz>=2.4f*2.4f);
            }
        }
        if(level+1<config.layers_count)assert(!terraces.back().is_safe({0,0}));
    }
    int crowd_walks=0;std::vector<std::vector<uint8_t>> moved(crowds.size(),std::vector<uint8_t>(25,0));
    for(int tick=0;tick<600;++tick)for(size_t level=0;level<crowds.size();++level)for(auto& visitor:crowds[level]){
        auto previous=visitor;game::step_resident(terraces[level],.1f,visitor);
        assert(visitor.available&&terraces[level].is_safe(visitor.position));
        assert(terraces[level].segment_is_safe(previous.position,visitor.position));
        if(visitor.position.x!=previous.position.x||visitor.position.z!=previous.position.z)moved[level][&visitor-crowds[level].data()]=1;
        if(!previous.walking&&visitor.walking)++crowd_walks;
    }
    assert(crowd_walks>=200);
    for(const auto& level:moved)for(auto walked:level)assert(walked);
    std::cout<<"Resident crowd smoke passed: count=200 terraces=8 walks="<<crowd_walks<<std::endl;
    auto state=game::create_resident_state(nav,1337);
    assert(state.available&&nav.is_safe(state.position));
    auto points=nav.reachable_points(state.position);assert(points.size()>100);
    assert(!nav.is_safe({0,0}));assert(!nav.is_safe({1000,1000}));
    auto route=nav.find_path(state.position,points.back());assert(!route.empty());
    for(auto p:route)assert(nav.is_safe(p));
    for(size_t i=1;i<route.size();++i)assert(nav.segment_is_safe(route[i-1],route[i]));
    auto empty=game::build_resident_navigation(game::terrain_mesh_data{},config.shore_height);
    assert(!game::create_resident_state(empty,1).available);
    for(auto seed:{42u,2026u}){
        auto variant=config;variant.seed=seed;variant.resolution=seed==42?256:512;
        auto variant_nav=game::build_resident_navigation(game::terrain_mesh_builder::build(game::terrain_generator::generate(variant)),variant.shore_height);
        auto visitor=game::create_resident_state(variant_nav,seed);assert(visitor.available&&variant_nav.is_safe(visitor.position));
        auto targets=variant_nav.reachable_points(visitor.position);assert(!targets.empty());
        auto path=variant_nav.find_path(visitor.position,targets.back());assert(!path.empty());
        for(size_t i=1;i<path.size();++i)assert(variant_nav.segment_is_safe(path[i-1],path[i]));
    }
    auto other=state;bool idle=false,walk=false;int stops=0;
    for(int i=0;i<180000;++i){auto before=state;game::step_resident(nav,1.f/60,state);game::step_resident(nav,1.f/60,other);
        assert(state.position.x==other.position.x&&state.position.z==other.position.z);
        assert(nav.is_safe(state.position)&&nav.segment_is_safe(before.position,state.position));
        const float dx=state.position.x-before.position.x,dz=state.position.z-before.position.z;
        if(dx*dx+dz*dz>1.e-12f)assert(std::abs(std::atan2(std::sin(state.yaw-std::atan2(dx,dz)),std::cos(state.yaw-std::atan2(dx,dz))))<.00001f);
        idle|=!state.walking;walk|=state.walking;if(before.walking&&!state.walking)++stops;
    }
    assert(idle&&walk&&stops>100);
    auto before=state;game::step_resident(nav,0,state);game::step_resident(nav,-1,state);
    game::step_resident(nav,std::numeric_limits<float>::quiet_NaN(),state);
    assert(state.position.x==before.position.x&&state.position.z==before.position.z&&state.idle_remaining==before.idle_remaining);
    game::step_resident(nav,2,state);
    float dx=state.position.x-before.position.x,dz=state.position.z-before.position.z;
    assert(std::sqrt(dx*dx+dz*dz)<=.1201f&&nav.is_safe(state.position));
    std::cout<<"Resident navigation/behavior smoke passed: reachable="<<points.size()<<" stops="<<stops<<std::endl;
}
