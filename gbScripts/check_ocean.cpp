#include "ocean_wave_model.h"
#include "ocean_mesh_builder.h"
#include "buoyancy_simulation.h"
#include "ocean_demo_props.h"
#include <stdexcept>
#include <limits>
#include <cassert>
#include <cmath>
#include <iostream>
int main()
{
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
