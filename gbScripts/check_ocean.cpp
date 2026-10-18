#include "ocean_wave_model.h"
#include "ocean_mesh_builder.h"
#include "buoyancy_simulation.h"
#include "ocean_demo_props.h"
#include <stdexcept>
#include <limits>
#include <cassert>
#include <cmath>
#include <iostream>
#include <set>
#include <map>
int main()
{
    game::terrain_config reef_config;
    const auto reefs=game::build_ocean_reefs(reef_config);
    const auto repeated_reefs=game::build_ocean_reefs(reef_config);
    assert(reefs.vertices.size()>1000&&reefs.vertices.size()<65535);
    assert(reefs.indices.size()==reefs.vertices.size()&&reefs.indices==repeated_reefs.indices);
    bool reef_underwater=false,reef_exposed=false;
    for(size_t i=0;i<reefs.vertices.size();++i){
        const auto& v=reefs.vertices[i];
        assert(v.position==repeated_reefs.vertices[i].position);
        assert(v.stone_texture);
        for(float p:v.position)assert(std::isfinite(p));
        assert(std::max(std::abs(v.position[0]),std::abs(v.position[2]))<reef_config.world_size*.5f);
        reef_underwater|=v.position[1]<reef_config.water_y-1.f;
        reef_exposed|=v.position[1]>reef_config.water_y+1.f;
        float normal_length=0;for(float n:v.normal)normal_length+=n*n;
        assert(std::abs(normal_length-1.f)<.001f);
        assert(reefs.indices[i]<reefs.vertices.size());
    }
    assert(reef_underwater&&reef_exposed);
    // The two-sided water mask needs closed solids, not one-sided rock caps.
    using reef_edge=std::pair<std::array<float,3>,std::array<float,3>>;
    std::map<reef_edge,int> reef_edges;
    for(size_t i=0;i<reefs.indices.size();i+=3)for(int side=0;side<3;++side){
        auto a=reefs.vertices[reefs.indices[i+side]].position;
        auto b=reefs.vertices[reefs.indices[i+(side+1)%3]].position;
        if(b<a)std::swap(a,b);
        ++reef_edges[{a,b}];
    }
    for(const auto& edge:reef_edges)assert(edge.second==2);
    auto lowered_reef_config=reef_config;lowered_reef_config.water_y-=.5f;
    const auto lowered_reefs=game::build_ocean_reefs(lowered_reef_config);
    assert(lowered_reefs.vertices.size()==reefs.vertices.size());
    for(size_t i=0;i<reefs.vertices.size();++i){
        assert(lowered_reefs.vertices[i].position[0]==reefs.vertices[i].position[0]);
        assert(lowered_reefs.vertices[i].position[2]==reefs.vertices[i].position[2]);
        assert(std::abs(lowered_reefs.vertices[i].position[1]-reefs.vertices[i].position[1]+.5f)<.0001f);
    }
    auto legacy_reef_config=reef_config;
    legacy_reef_config.world_size=512;legacy_reef_config.island_size=256;legacy_reef_config.water_size=768;
    const auto legacy_reefs=game::build_ocean_reefs(legacy_reef_config);
    assert(!legacy_reefs.vertices.empty());
    for(const auto& v:legacy_reefs.vertices)
        assert(std::max(std::abs(v.position[0]),std::abs(v.position[2]))<legacy_reef_config.world_size*.5f);
    ++reef_config.seed;
    const auto different_reefs=game::build_ocean_reefs(reef_config);
    assert(different_reefs.vertices.front().position!=reefs.vertices.front().position);
    // Different subdivisions leave wave-height cracks even when both grids
    // cover the same square. Every inner horizon vertex must share a surface edge vertex.
    const auto coast_grid=game::ocean_mesh_builder::build(2048,256);
    const auto coast_horizon=game::ocean_mesh_builder::build_horizon(2048,200000,256);
    std::set<std::pair<float,float>> surface_edge,horizon_edge;
    for(const auto& patch:coast_grid)for(const auto& v:patch.vertices)
        if(std::max(std::abs(v.position[0]),std::abs(v.position[2]))==1024.f)
            surface_edge.emplace(v.position[0],v.position[2]);
    for(const auto& v:coast_horizon.vertices)
        if(std::max(std::abs(v.position[0]),std::abs(v.position[2]))==1024.f)
            horizon_edge.emplace(v.position[0],v.position[2]);
    assert(surface_edge==horizon_edge&&surface_edge.size()==1024);
    const std::array<float,16> identity={{1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1}};
    const auto visibility_mesh=game::ocean_mesh_builder::build(8,128).front();
    game::ocean_mesh_quadtree visibility_tree(visibility_mesh);
    auto wide=identity;wide[0]=wide[10]=.1f;
    const auto all=visibility_tree.select(wide,-.5f,.5f,false,0);
    assert(all.water==visibility_mesh.indices&&all.foam==all.water);
    assert(all.tested_nodes==1);
    game::ocean_visibility cached;
    assert(visibility_tree.select(wide,-.5f,.5f,false,0,cached));
    assert(cached.water==all.water&&cached.foam==all.foam);
    const auto* cached_data=cached.water.data();
    assert(!visibility_tree.select(wide,-.5f,.5f,false,0,cached));
    auto slight=wide;slight[12]=.001f;
    assert(!visibility_tree.select(slight,-.5f,.5f,false,0,cached));
    assert(cached.water.data()==cached_data&&cached.tested_nodes==1);
    assert(visibility_tree.select(identity,-.5f,.5f,false,.2f,cached));
    const auto cache_reference=visibility_tree.select(identity,-.5f,.5f,false,.2f);
    assert(cached.water==cache_reference.water&&cached.foam==cache_reference.foam);
    assert(!visibility_tree.select(identity,-.5f,.5f,false,.2f,cached));
    const auto clipped=visibility_tree.select(identity,-.5f,.5f,false,.2f);
    assert(!clipped.water.empty()&&clipped.water.size()<all.water.size());
    const auto ordered_subset=[](const std::vector<uint16_t>& source,const std::vector<uint16_t>& selected){
        size_t at=0;for(size_t i=0;i<source.size()&&at<selected.size();i+=3)
            if(source[i]==selected[at]&&source[i+1]==selected[at+1]&&source[i+2]==selected[at+2])at+=3;
        assert(at==selected.size());
    };
    ordered_subset(visibility_mesh.indices,clipped.water);ordered_subset(clipped.water,clipped.foam);
    assert(clipped.foam.size()<clipped.water.size());
    auto beyond_far=wide;beyond_far[14]=4;
    assert(visibility_tree.select(beyond_far,-.5f,.5f,false,0).water.empty());
    const auto horizon_visible=visibility_tree.select(beyond_far,-.5f,.5f,true,1);
    assert(horizon_visible.water==visibility_mesh.indices&&horizon_visible.foam.empty());
    // The water vertex shader also clamps ordinary patches beyond extent 720.
    auto distant_mesh=visibility_mesh;
    for(auto& vertex:distant_mesh.vertices)vertex.position[0]+=800;
    game::ocean_mesh_quadtree distant_tree(distant_mesh);
    auto distant_vp=beyond_far;distant_vp[0]=.001f;
    const auto distant_visible=distant_tree.select(distant_vp,-.5f,.5f,false,1000);
    assert(distant_visible.water==distant_mesh.indices&&distant_visible.foam.empty());
    assert(visibility_tree.select(beyond_far,-.5f,.5f,false,1000).water.empty());
    auto outside=wide;outside[12]=4;
    assert(visibility_tree.select(outside,-.5f,.5f,false,0).water.empty());
    outside[12]=-4;assert(visibility_tree.select(outside,-.5f,.5f,false,0).water.empty());
    outside=wide;outside[13]=4;assert(visibility_tree.select(outside,-.5f,.5f,false,0).water.empty());
    outside[13]=-4;assert(visibility_tree.select(outside,-.5f,.5f,false,0).water.empty());
    outside=wide;outside[14]=-4;assert(visibility_tree.select(outside,-.5f,.5f,true,0).water.empty());
    game::ocean_visibility initially_outside;
    assert(visibility_tree.select(outside,-.5f,.5f,false,1,initially_outside));
    assert(initially_outside.water.empty()&&initially_outside.foam.empty());
    assert(!visibility_tree.select(outside,-.5f,.5f,false,1,initially_outside));
    // A conservative displacement envelope keeps triangles crossing a clip plane.
    assert(visibility_tree.select(wide,1.f,2.f,false,0).water==visibility_mesh.indices);
    assert(visibility_tree.select(wide,2.f,3.f,false,0).water.empty());
    auto invalid_matrix=identity;invalid_matrix[0]=std::numeric_limits<float>::quiet_NaN();
    assert(visibility_tree.select(invalid_matrix,-.5f,.5f,false,1).water==visibility_mesh.indices);
    assert(visibility_tree.select(identity,1.f,-1.f,false,1).water==visibility_mesh.indices);
    assert(visibility_tree.select(identity,0.f,std::numeric_limits<float>::infinity(),true,1).foam.empty());
    assert(visibility_tree.select(invalid_matrix,-.5f,.5f,false,1,cached));
    assert(visibility_tree.select(invalid_matrix,-.5f,.5f,false,1,cached));
    assert(visibility_tree.select(outside,-.5f,.5f,false,1,cached));
    assert(cached.water.empty()&&cached.foam.empty());
    assert(!visibility_tree.select(outside,-.5f,.5f,false,1,cached));
    assert(visibility_tree.select(wide,-.5f,.5f,true,1,cached));
    assert(cached.water==all.water&&cached.foam.empty());
    assert(!visibility_tree.select(wide,-.5f,.5f,true,1,cached));
    // Looking along Z collapses distant zero-writing geometry over the support.
    auto overlap=wide;overlap[10]=0;
    auto occluder_mesh=visibility_mesh;
    for(auto& vertex:occluder_mesh.vertices)vertex.position[0]*=.1f;
    game::ocean_mesh_quadtree occluder_tree(occluder_mesh);
    assert(occluder_tree.select(overlap,-.5f,.5f,false,1).foam==occluder_mesh.indices);
    auto crossing_w=wide;crossing_w[11]=1;crossing_w[15]=0;
    const auto crossing=visibility_tree.select(crossing_w,-.5f,.5f,false,1);
    assert(crossing.foam==crossing.water);
    game::ocean_mesh_quadtree empty_tree(game::terrain_mesh_data{});
    assert(empty_tree.select(identity,0,0,false,1).water.empty());
    // Compare hierarchical rejection against a direct per-triangle homogeneous
    // clip-plane check over several oblique projections and translations.
    for(int step=0;step<20;++step){
        auto vp=identity;vp[0]=.23f;vp[8]=.11f;vp[1]=.07f;vp[9]=.19f;vp[10]=.18f;
        vp[12]=step*.13f-1.2f;vp[13]=step*-.09f+.8f;vp[14]=step*.08f-.7f;
        const auto selected=visibility_tree.select(vp,-.7f,.8f,false,0);
        ordered_subset(visibility_mesh.indices,selected.water);
        size_t selected_at=0;
        for(size_t i=0;i<visibility_mesh.indices.size();i+=3){
            bool rejected=false;
            for(int axis=0;axis<3;++axis)for(int sign:{-1,1}){
                double greatest=-std::numeric_limits<double>::infinity();
                for(size_t j=0;j<3;++j)for(float y:{-.7f,.8f}){
                    const auto& p=visibility_mesh.vertices[visibility_mesh.indices[i+j]].position;
                    const double coordinate=double(vp[axis])*p[0]+double(vp[4+axis])*y+double(vp[8+axis])*p[2]+vp[12+axis];
                    greatest=std::max(greatest,1+sign*coordinate);
                }
                if(greatest<.001)rejected=true;
            }
            const bool present=selected_at<selected.water.size()&&selected.water[selected_at]==visibility_mesh.indices[i]&&selected.water[selected_at+1]==visibility_mesh.indices[i+1]&&selected.water[selected_at+2]==visibility_mesh.indices[i+2];
            if(present)selected_at+=3;
            if(!rejected)assert(present);
        }
    }
    game::ocean_settings settings;
    // Local excitation must change height even when directional travel is stopped.
    auto excited=settings;for(auto& wave:excited.waves)wave.amplitude=0;
    excited.waves[0]={1,0,24,.4f,.5f,0};
    float lowest=10.f,highest=-10.f;
    for(int i=0;i<=600;++i){
        const float h=game::sample_ocean(excited,5,8,i*.01).position.y;
        lowest=std::min(lowest,h);highest=std::max(highest,h);
    }
    assert(highest-lowest>.12f);
    float amplitude_limit=0;for(const auto& wave:settings.waves)amplitude_limit+=wave.amplitude*1.09f;
    for(float t:{0.f,4.5f,8.5f,31.f,300.f})for(int i=0;i<40;++i){
        auto h=game::sample_ocean(settings,i*9.f-160.f,i*3.7f-80.f,t);
        auto next=game::sample_ocean(settings,h.position.x,h.position.z,t+.001);
        assert(std::abs(h.position.y-settings.mean_level)<=amplitude_limit+.0001f);
        assert(h.position.y+settings.foam_height_offset<.5f);
        assert(std::abs(h.position.y-next.position.y)<.02f);
        auto repeat=game::sample_ocean(settings,h.position.x,h.position.z,t);
        assert(h.position.y==repeat.position.y);
    }
    auto fixed_xz=game::evaluate_ocean(settings,13.f,-7.f,2.4);
    assert(fixed_xz.position.x==13.f&&fixed_xz.position.z==-7.f);
    auto steep=settings;for(auto& wave:steep.waves)wave.amplitude=0;
    steep.waves[0]={1,0,24,3,.99f,0};
    auto steep_forward=game::evaluate_ocean(steep,4.008f,0,0);
    auto steep_inverse=game::sample_ocean(steep,steep_forward.position.x,steep_forward.position.z,0);
    if(std::abs(steep_forward.position.y-steep_inverse.position.y)>.001f){std::cerr<<"Steep wave inverse failed\n";return 2;}
    auto flat=settings;
    for(auto& wave:flat.waves) wave.amplitude=0;
    auto f=game::sample_ocean(flat,12.f,3.f,9.0);
    assert(std::abs(f.position.y-flat.mean_level)<1e-5f && f.normal.y>.9999f);
    for(int i=0;i<100;++i)
    {
        auto e=game::evaluate_ocean(settings,i-50.f,i*.37f,i*.3);
        auto s=game::sample_ocean(settings,e.position.x,e.position.z,i*.3);
        assert(std::abs(e.position.y-s.position.y)<.001f);
        assert(std::abs(s.normal.x*s.normal.x+s.normal.y*s.normal.y+s.normal.z*s.normal.z-1)<.0001f);
    }
    auto a=game::sample_ocean(settings,10,15,0), b=game::sample_ocean(settings,10,15,1);
    assert(std::abs(a.position.y-b.position.y)>.001f);
    const auto rejects_wave=[](game::ocean_settings invalid){bool rejected=false;try{game::sample_ocean(invalid,0,0,0);}catch(const std::invalid_argument&){rejected=true;}assert(rejected);};
    auto invalid=settings;invalid.waves[0].wavelength=0;rejects_wave(invalid);
    invalid=settings;invalid.waves[0].steepness=2;rejects_wave(invalid);
    invalid=settings;invalid.waves[0].dx=std::numeric_limits<float>::quiet_NaN();rejects_wave(invalid);
    invalid=settings;invalid.foam_width=0;rejects_wave(invalid);
    invalid=settings;invalid.foam_inner_distance=invalid.foam_width;rejects_wave(invalid);
    invalid=settings;invalid.foam_noise_scale=std::numeric_limits<float>::quiet_NaN();rejects_wave(invalid);
    invalid=settings;invalid.depth_fade_distance=0;rejects_wave(invalid);
    invalid=settings;invalid.reflection_strength=1.1f;rejects_wave(invalid);
    invalid=settings;invalid.foam_color[0]=-1;rejects_wave(invalid);
    invalid=settings;invalid.sun_direction={0,0,0};rejects_wave(invalid);
    invalid=settings;invalid.sun_direction.y=std::numeric_limits<float>::quiet_NaN();rejects_wave(invalid);
    const auto patches=game::ocean_mesh_builder::build(768,384);
    assert(patches.size()==9);
    // Interior vertices must break the regular lattice, with shared patch seams intact.
    assert(std::abs(patches[0].vertices[130].position[0]+382.f)>.05f);
    for(const auto& mesh:patches) {
        assert(mesh.vertices.size()<=65535);
        for(auto index:mesh.indices) assert(index<mesh.vertices.size());
        for(size_t i=0;i<mesh.indices.size();i+=3) {
            auto a=mesh.vertices[mesh.indices[i]].position,b=mesh.vertices[mesh.indices[i+1]].position,c=mesh.vertices[mesh.indices[i+2]].position;
            assert((b[2]-a[2])*(c[0]-a[0])-(b[0]-a[0])*(c[2]-a[2])>0);
        }
    }
    for(size_t row=0;row<3;++row)for(size_t col=0;col<2;++col)for(int z=0;z<=128;++z)
        assert(patches[row*3+col].vertices[z*129+128].position==patches[row*3+col+1].vertices[z*129].position);
    for(size_t row=0;row<2;++row)for(size_t col=0;col<3;++col)for(int x=0;x<=128;++x)
        assert(patches[row*3+col].vertices[128*129+x].position==patches[(row+1)*3+col].vertices[x].position);
    const auto horizon=game::ocean_mesh_builder::build_horizon(768,4096,384);
    const auto extended_horizon=game::ocean_mesh_builder::build_horizon(768,200000,384);
    game::ocean_mesh_quadtree horizon_tree(extended_horizon);
    auto narrow_horizon=identity;narrow_horizon[0]=.01f;narrow_horizon[9]=.01f;narrow_horizon[10]=0;narrow_horizon[13]=-6;
    const auto horizon_slice=horizon_tree.select(narrow_horizon,-.5f,.5f,true,0);
    assert(!horizon_slice.water.empty());
    assert(horizon_slice.water.size()<extended_horizon.indices.size()/20);
    ordered_subset(extended_horizon.indices,horizon_slice.water);
    assert(horizon.vertices.size()<65535);for(auto index:horizon.indices)assert(index<horizon.vertices.size());
    // A displaced wave mesh must not span the entire skirt in one triangle.
    // The first radial row should resolve waves at the central grid spacing.
    float nearest_edge=1.e9f;
    for(size_t i=0;i<horizon.indices.size();i+=3){
        for(size_t edge=0;edge<3;++edge){
            const auto& a=horizon.vertices[horizon.indices[i+edge]].position;
            const auto& b=horizon.vertices[horizon.indices[i+(edge+1)%3]].position;
            const float ra=std::max(std::abs(a[0]),std::abs(a[2]));
            const float rb=std::max(std::abs(b[0]),std::abs(b[2]));
            if(std::abs(ra-384.f)<.001f && rb>ra+.001f)nearest_edge=std::min(nearest_edge,rb-ra);
        }
    }
    assert(nearest_edge<=2.01f);
    for(size_t i=0;i<horizon.indices.size();i+=3){
        const auto& a=horizon.vertices[horizon.indices[i]].position;
        const auto& b=horizon.vertices[horizon.indices[i+1]].position;
        const auto& c=horizon.vertices[horizon.indices[i+2]].position;
        assert((b[2]-a[2])*(c[0]-a[0])-(b[0]-a[0])*(c[2]-a[2])>0);
    }
    for(int segments:{1,64,1024}){
        const auto narrow=game::ocean_mesh_builder::build_horizon(768,800,segments);
        assert(narrow.vertices.size()<=65535);
        for(auto index:narrow.indices)assert(index<narrow.vertices.size());
        for(const auto& vertex:narrow.vertices){
            const float extent=std::max(std::abs(vertex.position[0]),std::abs(vertex.position[2]));
            assert(extent>=384.f-.001f&&extent<=400.f+.001f);
        }
    }
    std::cout<<"Ocean CPU smoke checks passed\n";
    const std::vector<game::buoyancy_probe> probes={{{-1,-.5f,-2},1},{{1,-.5f,-2},1},{{-1,-.5f,2},1},{{1,-.5f,2},1}};
    game::buoyancy_settings buoyancy;
    game::buoyancy_state forced_reference;
    for(int i=0;i<3600;++i)game::step_buoyancy(settings,buoyancy,probes,i/120.,1.f/120,forced_reference);
    for(int rate:{30,60,144}){
        game::buoyancy_state forced;
        for(int i=0;i<30*rate;++i)game::step_buoyancy(settings,buoyancy,probes,i/static_cast<double>(rate),1.f/rate,forced);
        assert(std::abs(forced.position.y-forced_reference.position.y)<.03f);
        assert(std::abs(forced.pitch-forced_reference.pitch)<.03f&&std::abs(forced.roll-forced_reference.roll)<.03f);
    }
    for(float dt:{1.f/30,1.f/60,1.f/144}) {
        game::buoyancy_state state;state.position.y=2;
        for(int i=0;i<static_cast<int>(30/dt);++i)game::step_buoyancy(flat,buoyancy,probes,i*dt,dt,state);
        assert(std::abs(state.position.y-flat.mean_level)<.02f);
        assert(std::abs(state.pitch)<.01f&&std::abs(state.roll)<.01f);
    }
    game::buoyancy_state moving;
    for(int i=0;i<1800;++i)game::step_buoyancy(settings,buoyancy,probes,i/60.,1.f/60,moving);
    assert(std::isfinite(moving.pitch)&&std::abs(moving.pitch)+std::abs(moving.roll)>.001f);
    game::step_buoyancy(settings,buoyancy,probes,30,.25f,moving);
    assert(std::isfinite(moving.position.y)&&std::abs(moving.position.y)<2);
    assert(moving.wetness>=0&&moving.wetness<=1);
    auto unchanged=moving;game::step_buoyancy(settings,buoyancy,probes,30,0,moving);
    assert(moving.position.y==unchanged.position.y&&moving.pitch==unchanged.pitch);
    moving.position.y=100;moving.wetness=1;game::step_buoyancy(flat,buoyancy,probes,30,.1f,moving);assert(moving.wetness<1);
    auto invalid_buoyancy=buoyancy;invalid_buoyancy.mass=0;bool rejected=false;
    try{game::step_buoyancy(flat,invalid_buoyancy,probes,0,.1f,moving);}catch(const std::invalid_argument&){rejected=true;}assert(rejected);
    const auto props=game::build_ocean_demo_props(settings);int floats=0;
    for(const auto& p:props){if(p.floating){++floats;assert(p.probes.size()==4);}assert(!p.mesh.indices.empty());
        for(const auto& v:p.mesh.vertices)for(float n:v.normal)assert(std::isfinite(n));}
    assert(floats==3);
    std::cout<<"Buoyancy integration checks passed\n";
}
