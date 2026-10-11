#include "ocean_mesh_builder.h"
#include <algorithm>
#include <cmath>
#include <stdexcept>
namespace game {
    std::vector<terrain_mesh_data> ocean_mesh_builder::build(float size,int cells)
    {
        if(!std::isfinite(size)||size<=0||cells<1||cells>1024) throw std::invalid_argument("Invalid ocean grid");
        std::vector<terrain_mesh_data> patches;
        const size_t patches_per_axis = static_cast<size_t>((cells+127)/128);
        patches.reserve(patches_per_axis*patches_per_axis);
        for(int oz=0;oz<cells;oz+=128) for(int ox=0;ox<cells;ox+=128) {
            const int nx=std::min(128,cells-ox),nz=std::min(128,cells-oz);
            terrain_mesh_data mesh;
            mesh.vertices.reserve(static_cast<size_t>(nx+1)*(nz+1));
            mesh.indices.reserve(static_cast<size_t>(nx)*nz*6);
            for(int z=0;z<=nz;++z) for(int x=0;x<=nx;++x){
                const int gx=ox+x,gz=oz+z;float px=gx*size/cells-size*.5f,pz=gz*size/cells-size*.5f;
                // World grid indices give identical jitter on both sides of patch seams.
                if(gx>0&&gz>0&&gx<cells&&gz<cells){
                    uint32_t hash=static_cast<uint32_t>(gx)*73856093u^static_cast<uint32_t>(gz)*19349663u;
                    hash^=hash>>13;hash*=1274126177u;
                    px+=(float(hash&65535)/65535.f-.5f)*size/cells*.5f;
                    pz+=(float((hash>>16)&65535)/65535.f-.5f)*size/cells*.5f;
                }
                mesh.vertices.push_back({{{px,0.f,pz}},{{0,1,0}},{{255,255,255,255}}});
            }
            for(int z=0;z<nz;++z) for(int x=0;x<nx;++x) {
                const uint16_t a=static_cast<uint16_t>(z*(nx+1)+x),b=a+1,c=a+static_cast<uint16_t>(nx+1),d=c+1;
                uint32_t diagonal=static_cast<uint32_t>(ox+x)*83492791u^static_cast<uint32_t>(oz+z)*2654435761u;
                diagonal^=diagonal>>16;diagonal*=0x7feb352du;diagonal^=diagonal>>15;diagonal*=0x846ca68bu;diagonal^=diagonal>>16;
                if(diagonal&1u)for(auto index:{a,c,d,a,d,b})mesh.indices.push_back(index);
                else for(auto index:{a,c,b,b,c,d}) mesh.indices.push_back(index);
            }
            patches.push_back(std::move(mesh));
        }
        return patches;
    }
    terrain_mesh_data ocean_mesh_builder::build_horizon(float inner_size,float outer_size,int segments)
    {
        if(!std::isfinite(inner_size)||!std::isfinite(outer_size)||inner_size<=0||outer_size<=inner_size||segments<1||segments>1024)
            throw std::invalid_argument("Invalid ocean horizon");
        terrain_mesh_data mesh;
        mesh.vertices.reserve(static_cast<size_t>(segments+1)*8);
        mesh.indices.reserve(static_cast<size_t>(segments)*24);
        for(int side=0;side<4;++side){
            const uint16_t base=static_cast<uint16_t>(mesh.vertices.size());
            for(int i=0;i<=segments;++i)for(float size:{inner_size,outer_size}){
                float x=-size*.5f+i*size/segments,z=-size*.5f;
                for(int rotation=0;rotation<side;++rotation){float old=x;x=-z;z=old;}
                mesh.vertices.push_back({{{x,0,z}},{{0,1,0}},{{255,255,255,255}}});
            }
            for(int i=0;i<segments;++i){uint16_t a=base+i*2,b=a+1,c=a+2,d=a+3;for(auto index:{a,c,b,b,c,d})mesh.indices.push_back(index);}
        }
        return mesh;
    }
}
