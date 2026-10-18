#include "ocean_demo_props.h"
#include "terrain_generator.h"
#include <algorithm>
#include <cmath>
namespace game {
    namespace {
        using point=std::array<float,3>;
        using tint=std::array<uint8_t,4>;
        void triangle(terrain_mesh_data& m,point a,point b,point c,tint color) {
            const point ab={{b[0]-a[0],b[1]-a[1],b[2]-a[2]}},ac={{c[0]-a[0],c[1]-a[1],c[2]-a[2]}};
            point n={{ab[1]*ac[2]-ab[2]*ac[1],ab[2]*ac[0]-ab[0]*ac[2],ab[0]*ac[1]-ab[1]*ac[0]}};
            const float l=std::sqrt(n[0]*n[0]+n[1]*n[1]+n[2]*n[2]);for(auto& value:n)value/=l;
            for(auto p:{a,b,c}){m.indices.push_back(static_cast<uint16_t>(m.vertices.size()));m.vertices.push_back({p,n,color});}
        }
        void quad(terrain_mesh_data& m,point a,point b,point c,point d,tint color){triangle(m,a,b,c,color);triangle(m,b,d,c,color);}
        void box(terrain_mesh_data& m,float x,float y,float z,float sx,float sy,float sz,tint color) {
            const point a={{x-sx,y-sy,z-sz}},b={{x+sx,y-sy,z-sz}},c={{x-sx,y-sy,z+sz}},d={{x+sx,y-sy,z+sz}};
            const point e={{x-sx,y+sy,z-sz}},f={{x+sx,y+sy,z-sz}},g={{x-sx,y+sy,z+sz}},h={{x+sx,y+sy,z+sz}};
            quad(m,e,g,f,h,color);quad(m,a,b,c,d,color);quad(m,a,e,b,f,color);quad(m,c,d,g,h,color);quad(m,a,c,e,g,color);quad(m,b,f,d,h,color);
        }
        void probes(ocean_demo_prop& p,float x,float z,float draft) {
            p.buoyancy.draft=draft;
            p.buoyancy.inertia_x=p.buoyancy.mass*(z*z*4+1)/12;
            p.buoyancy.inertia_z=p.buoyancy.mass*(x*x*4+1)/12;
            for(float px:{-x,x})for(float pz:{-z,z})p.probes.push_back({{px,-draft,pz},1});
        }
    }
    std::vector<ocean_demo_prop> build_ocean_demo_props(const ocean_settings& ocean) {
        std::vector<ocean_demo_prop> result;
        // Visible from the default camera (-X,+Z), outside the dry coastline.
        ocean_demo_prop crate;crate.position={-104,ocean.mean_level,75};crate.yaw=.3f;crate.floating=true;
        box(crate.mesh,0,.2f,0,1.8f,.9f,1.8f,{{134,75,32,255}});
        for(float x:{-1.45f,1.45f})box(crate.mesh,x,.23f,0,.12f,1.f,1.9f,{{77,43,24,255}});
        probes(crate,1.5f,1.5f,.7f);result.push_back(crate);
        ocean_demo_prop raft;raft.position={-120,ocean.mean_level,100};raft.yaw=-.4f;raft.floating=true;
        for(int i=0;i<5;++i)box(raft.mesh,-2.4f+i*1.2f,0,0,.52f,.4f,3.4f,{{static_cast<uint8_t>(112+i*6),75,37,255}});
        for(float z:{-2.f,2.f})box(raft.mesh,0,.48f,z,3.f,.12f,.17f,{{65,44,26,255}});
        probes(raft,2.2f,2.5f,.4f);result.push_back(raft);
        ocean_demo_prop boat;boat.position={-76,ocean.mean_level,120};boat.yaw=.55f;boat.floating=true;
        const std::array<std::array<float,2>,8> ring={{{{-2.3f,-3.5f}},{{-2.3f,3.5f}},{{-1.f,5.f}},{{1.f,5.f}},{{2.3f,3.5f}},{{2.3f,-3.5f}},{{1.f,-5.f}},{{-1.f,-5.f}}}};
        for(size_t i=0;i<ring.size();++i){
            auto a=ring[i],b=ring[(i+1)%ring.size()];
            quad(boat.mesh,{{a[0]*.6f,-.75f,a[1]*.85f}},{{b[0]*.6f,-.75f,b[1]*.85f}},{{a[0],.75f,a[1]}},{{b[0],.75f,b[1]}},{{137,79,31,255}});
            quad(boat.mesh,{{a[0],.75f,a[1]}},{{b[0],.75f,b[1]}},{{a[0]*.83f,.75f,a[1]*.92f}},{{b[0]*.83f,.75f,b[1]*.92f}},{{180,115,49,255}});
            quad(boat.mesh,{{a[0]*.83f,.75f,a[1]*.92f}},{{b[0]*.83f,.75f,b[1]*.92f}},{{a[0]*.6f,-.45f,a[1]*.85f}},{{b[0]*.6f,-.45f,b[1]*.85f}},{{108,62,27,255}});
        }
        box(boat.mesh,0,-.5f,0,1.3f,.08f,3.6f,{{114,68,28,255}});
        for(float z:{-2.f,1.6f})box(boat.mesh,0,.32f,z,1.8f,.12f,.35f,{{173,112,50,255}});
        probes(boat,1.3f,3.f,.75f);result.push_back(boat);
        for(int i=0;i<3;++i){
            ocean_demo_prop rock;rock.position={-45.f-i*12.f,-.5f,105.f+i*8.f};
            const float r=3.f+i,h=2.8f;
            for(int side=0;side<7;++side){
                const float angle=side*6.283185307f/7,angle2=(side+1)*6.283185307f/7;
                point a={{std::cos(angle)*r,-1.f,std::sin(angle)*r}},b={{std::cos(angle2)*r,-1.f,std::sin(angle2)*r}};
                triangle(rock.mesh,a,{{.7f,h,-.4f}},b,{{146,149,131,255}});triangle(rock.mesh,a,b,{{0,-3.f,0}},{{102,113,112,255}});
            }
            result.push_back(rock);
        }
        ocean_demo_prop post;post.position={-92.f,ocean.mean_level,111.f};
        box(post.mesh,0,.6f,0,.3f,1.5f,.3f,{{104,66,34,255}});result.push_back(post);
        return result;
    }
    terrain_mesh_data build_ocean_reefs(const terrain_config& config) {
        config.validate();
        terrain_mesh_data mesh;
        const auto random=[&](uint32_t key){
            uint32_t h=config.seed^key*0x9e3779b9u;
            h^=h>>16;h*=0x7feb352du;h^=h>>15;h*=0x846ca68bu;h^=h>>16;
            return float(h&65535u)/65535.f;
        };
        const float scale=std::max(.35f,std::min(1.f,config.island_size/(1792.f*.67f)));
        for(int group=0;group<12;++group){
            const uint32_t key=1000+group*100;
            float angle=(group+.2f+.6f*random(key))*6.283185307f/12.f;
            // Leave the existing floating props' approach to the beach open.
            if(angle>2.f&&angle<2.5f)angle+=.45f;
            const float dx=std::cos(angle),dz=std::sin(angle);
            const float coast=terrain_generator::coast_radius(config,angle);
            const float edge=(config.world_size*.5f-28.f*scale)/std::max(std::abs(dx),std::abs(dz));
            const float radius=std::min(coast+(45.f+80.f*random(key+1))*scale,edge);
            if(radius<coast+30.f*scale)continue;
            const float center_x=dx*radius,center_z=dz*radius;
            const int count=4+int(random(key+2)*2.99f);
            for(int rock=0;rock<count;++rock){
                const uint32_t rk=key+3+rock*11;
                const float offset=rock*2.39996323f+random(rk);
                const float spread=rock==0?0.f:(6.f+10.f*random(rk+1))*scale;
                const float x=center_x+std::cos(offset)*spread,z=center_z+std::sin(offset)*spread;
                const float r=(rock==0?9.f+4.f*random(rk+2):3.5f+5.f*random(rk+2))*scale;
                const float height=(rock%4==3?.25f+random(rk+3)*.55f:2.f+random(rk+3)*4.5f)*scale;
                std::array<point,8> bottom,waterline,crown;
                for(int side=0;side<8;++side){
                    const float a=side*6.283185307f/8.f+.1f*random(rk+4);
                    const float rough=.8f+.25f*random(rk+side+13);
                    const float ux=std::cos(a)*r*rough,uz=std::sin(a)*r*rough;
                    bottom[side]={{x+ux*.75f,config.water_y-4.5f*scale,z+uz*.75f}};
                    waterline[side]={{x+ux,config.water_y-.65f*scale,z+uz}};
                    crown[side]={{x+ux*.65f+r*.13f,config.water_y+height*.55f,z+uz*.65f-r*.1f}};
                }
                const point tip={{x+r*.19f,config.water_y+height,z-r*.16f}};
                const point foot={{x,config.water_y-6.f*scale,z}};
                for(int side=0;side<8;++side){
                    const int next=(side+1)%8;
                    const uint8_t gray=uint8_t(125.f+35.f*random(rk+side+31));
                    const tint dry={{gray,uint8_t(gray+6),uint8_t(gray-3),255}};
                    const tint wet={{uint8_t(gray*.70f),uint8_t(gray*.76f),uint8_t(gray*.73f),255}};
                    quad(mesh,bottom[side],waterline[side],bottom[next],waterline[next],wet);
                    quad(mesh,waterline[side],crown[side],waterline[next],crown[next],dry);
                    triangle(mesh,crown[side],tip,crown[next],dry);
                    triangle(mesh,bottom[side],bottom[next],foot,wet);
                }
            }
        }
        for(auto& vertex:mesh.vertices){
            vertex.stone_texture=true;
            // Match the existing pixel stone projection used by stairs.
            vertex.texcoord={{std::abs(vertex.normal[0])>std::abs(vertex.normal[2])?vertex.position[2]:vertex.position[0],vertex.position[1]}};
        }
        return mesh;
    }
}
