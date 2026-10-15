#include "voxel_tree_builder.h"
#include "terrain_editor_state.h"
#include "outline_ids.h"
#include <algorithm>
#include <cmath>
#include <map>
#include <set>
#include <vector>
#include <stdexcept>

namespace game { namespace {
using Cell = std::array<int,3>;
using Point = std::array<float,3>;
using Color = std::array<uint8_t,4>;
const float voxel_size = .35f;
uint32_t mix(uint32_t v) { v=(v^(v>>16))*0x7feb352du; v=(v^(v>>15))*0x846ca68bu; return v^(v>>16); }
struct TreeVoxels {
    std::map<Cell,int> cells;
    void put(int x,int y,int z,int material) {
        if(y>=0) cells[Cell{{x,y,z}}]=material;
    }
    int at(Cell p) const { auto i=cells.find(p); return i==cells.end()?0:i->second; }
    void keep_rooted(Cell root){
        std::set<Cell> connected;std::vector<Cell> pending(1,root);connected.insert(root);
        for(size_t i=0;i<pending.size();++i)for(int axis=0;axis<3;++axis)for(int sign:{-1,1}){
            auto next=pending[i];next[axis]+=sign;
            if(at(next)&&connected.insert(next).second)pending.push_back(next);
        }
        for(auto i=cells.begin();i!=cells.end();)if(!connected.count(i->first))i=cells.erase(i);else ++i;
    }
    void box(int x0,int y0,int z0,int x1,int y1,int z1,int material) {
        for(int y=y0;y<=y1;++y)for(int z=z0;z<=z1;++z)for(int x=x0;x<=x1;++x)put(x,y,z,material);
    }
    // Rounded voxel lobes retain flat cuboidal steps without disconnected leaves.
    void lobe(float cx,float cy,float cz,float rx,float ry,float rz,int material) {
        for(int y=int(std::floor(cy-ry));y<=int(std::ceil(cy+ry));++y)
        for(int z=int(std::floor(cz-rz));z<=int(std::ceil(cz+rz));++z)
        for(int x=int(std::floor(cx-rx));x<=int(std::ceil(cx+rx));++x) {
            float a=(x+.5f-cx)/rx,b=(y+.5f-cy)/ry,c=(z+.5f-cz)/rz;
            if(a*a+b*b+c*c<=1.12f)put(x,y,z,material);
        }
    }
};
Color tint(TreeType species,int material) {
    if(material==1)return species==TreeType::Palm?Color{{91,53,20,255}}:Color{{53,26,8,255}};
    if(material==4)return Color{{192,190,167,255}};
    if(material==5)return Color{{20,19,15,255}};
    if(species==TreeType::Pine)return material==2?Color{{36,67,17,255}}:Color{{52,88,24,255}};
    if(species==TreeType::Birch)return material==2?Color{{80,125,20,255}}:Color{{103,146,29,255}};
    if(species==TreeType::Palm)return material==2?Color{{66,115,13,255}}:Color{{94,137,20,255}};
    if(species==TreeType::Thuja)return material==2?Color{{34,87,13,255}}:Color{{60,112,20,255}};
    return material==2?Color{{43,99,8,255}}:Color{{64,122,13,255}};
}
void populate(TreeVoxels& v,TreeType species,uint32_t seed) {
    const float skew=(int(mix(seed)&3u)-1)*.25f;
    if(species==TreeType::Pine) {
        // Five drooping bough decks, with notches between radial branch fans.
        // Coarse spatial variation changes whole voxel clumps, avoiding both
        // smooth cones and a pile of identical horizontal octagonal plates.
        const float angle=float(mix(seed+71u)&65535u)*6.283185307f/65536.f;
        const int decks[5]={5,9,13,17,20};
        const float radii[5]={7.2f,6.f,4.6f,3.1f,1.2f};
        for(int tier=0;tier<5;++tier) {
            const uint32_t branch_seed=mix(seed+uint32_t(tier)*131u);
            const float radius=radii[tier];
            const float cx=(int(branch_seed&3u)-1)*.25f,cz=(int((branch_seed>>2)&3u)-1)*.25f;
            const int base=decks[tier],extent=int(std::ceil(radius+1));
            for(int z=-extent;z<extent;++z)for(int x=-extent;x<extent;++x){
                const float dx=x+.5f-cx,dz=z+.5f-cz,distance=std::sqrt(dx*dx+dz*dz);
                const float fan=.5f+.5f*std::cos(std::atan2(dz,dx)*6.f+angle+tier*.83f);
                const uint32_t patch=mix(branch_seed^mix(uint32_t(int(std::floor(dx*.5f))))^
                                         (mix(uint32_t(int(std::floor(dz*.5f))))*31u));
                const float reach=radius*(.78f+.22f*fan)+(int(patch&3u)-1)*.16f;
                if(distance>reach||(distance>radius*.6f&&fan<.16f))continue;
                const int top=base+(distance<radius*.5f?1:0);
                const int material=(patch&7u)==0?3:2;
                v.box(x,std::max(4,top-1),z,x,top,z,material);
            }
            // Branch wood connects the inner boughs across the visible gaps.
            for(int arm=0;arm<6;++arm){
                const float a=(arm*6.283185307f-angle-tier*.83f)/6.f;
                for(float r=0;r<radius*.65f;r+=.5f)
                    v.put(int(std::floor(std::cos(a)*r)),base,int(std::floor(std::sin(a)*r)),1);
            }
        }
        // Keep bark visible in tier gaps and a substantial bare lower trunk.
        v.box(-1,0,-1,0,19,0,1);
        v.box(-1,20,-1,0,20,0,2);
        v.put(-1,21,-1,3);
        // Edge notches may isolate a tiny clump on some seeds. Keep only the
        // six-connected tree rooted at the trunk, never floating leaf specks.
        v.keep_rooted(Cell{{-1,0,-1}});
    } else if(species==TreeType::Oak) {
        v.box(-1,0,-1,1,14,1,1);
        for(int arm=0;arm<4;++arm) {
            float angle=arm*1.570796327f+.4f;
            for(int s=0;s<5;++s) { int x=int(std::round(std::cos(angle)*s)),z=int(std::round(std::sin(angle)*s));v.box(x-1,8+s/2,z-1,x,9+s/2,z,1); }
            v.lobe(std::cos(angle)*4+skew,13,std::sin(angle)*4,4.7f,3.5f,4.7f,2+arm%2);
        }
        v.lobe(skew,16,0,5.5f,4.5f,5.5f,2);
    } else if(species==TreeType::Birch) {
        v.box(-1,0,-1,0,16,0,4);
        for(int y=1;y<15;++y)if((mix(seed+uint32_t(y)*31)&3u)==0)
            v.put((y%2)-1,y,((y/2)%2)-1,5);
        v.lobe(skew,17,0,3.2f,3.3f,3.2f,3);
        v.lobe(-2,14,1,3.2f,3.f,3.2f,2);
        v.lobe(2,13,-1,3.5f,3.f,3.2f,3);
        v.lobe(0,11,2,3.f,2.f,3.f,2);
    } else if(species==TreeType::Palm) {
        for(int y=0;y<17;++y) {
            int x=int(std::floor(3.f*std::pow(y/16.f,1.6f)));
            v.box(x-1,y,-1,x,y,0,1);
        }
        v.box(2,16,-1,3,18,0,1);
        for(int arm=0;arm<7;++arm) {
            float angle=arm*6.283185307f/7+skew;
            int previous_y=18;
            for(int step=0;step<=30;++step) {
                float s=step/3.f,t=s/10.f;
                int x=int(std::round(3+std::cos(angle)*s)),z=int(std::round(std::sin(angle)*s));
                int y=int(std::round(18+3*std::sin(t*3.14159265f)-5*t*t));
                int width=s<2||s>8?0:1;
                // Broad block fronds droop at their outer tips; all sides are solid.
                v.box(x-width,std::min(y,previous_y),z-width,x+width,std::max(y,previous_y),z+width,2+arm%2);
                previous_y=y;
                if(step==30)v.box(x,y-2,z,x,y,z,2+arm%2);
            }
        }
    } else { // Thuja: narrow, continuously tapering body with overlapping sprays.
        v.box(-1,0,-1,0,18,0,1);
        v.lobe(0,9,0,3.2f,6.5f,3.2f,2);
        v.lobe(0,16,0,1.7f,4.5f,1.7f,2);
        for(int tier=0;tier<6;++tier)for(int arm=0;arm<4;++arm) {
            float angle=arm*1.570796327f+tier*.65f+skew;
            float r=3.1f-tier*.42f;
            v.lobe(std::cos(angle)*r,5+tier*2.5f,std::sin(angle)*r,1.6f-tier*.12f,2.f,1.6f-tier*.12f,3);
        }
        v.put(0,20,0,3);
    }
}
} // namespace

terrain_mesh_data build_voxel_tree(const EditorTree& tree,uint32_t seed) {
    TreeVoxels voxels; const uint32_t variation=mix(seed^mix(tree.id));populate(voxels,tree.type,variation);
    terrain_mesh_data mesh;
    const float cosine=std::cos(tree.yaw),sine=std::sin(tree.yaw),scale=std::max(.01f,tree.scale);
    const float phase=.05f+.9f*float(variation&0xffffffu)/16777216.f;
    const uint32_t silhouette=outline_tree_id(tree.id);
    // Greedy rectangles merge exposed coplanar faces of the same material.
    // Vertical faces stay sliced at every voxel Y boundary: nonlinear wind
    // depends only on height and the shared root, so adjacent edge samples
    // remain identical even where different materials split a rectangle.
    // This also removes wood/leaf and leaf/leaf internal interfaces.
    for(int axis=0;axis<3;++axis)for(int sign=-1;sign<=1;sign+=2) {
        int u=(axis+1)%3,w=(axis+2)%3;
        std::map<int,std::map<std::array<int,2>,int>> planes;
        for(const auto& entry:voxels.cells) {
            Cell neighbor=entry.first;neighbor[axis]+=sign;
            if(voxels.at(neighbor))continue;
            planes[entry.first[axis]+(sign>0?1:0)][std::array<int,2>{{entry.first[u],entry.first[w]}}]=entry.second;
        }
        for(auto& plane:planes) {
            auto& mask=plane.second;
            while(!mask.empty()) {
                auto first=mask.begin();int x=first->first[0],y=first->first[1],material=first->second,dx=1,dy=1;
                auto matches=[&](int a,int b){auto i=mask.find(std::array<int,2>{{a,b}});return i!=mask.end()&&i->second==material;};
                if(u!=1)while(matches(x+dx,y))++dx;
                bool extend=w!=1;
                while(extend) {for(int a=0;a<dx;++a)if(!matches(x+a,y+dy)){extend=false;break;}if(extend)++dy;}
                for(int b=0;b<dy;++b)for(int a=0;a<dx;++a)mask.erase(std::array<int,2>{{x+a,y+b}});
                if(mesh.vertices.size()+4>65535)throw std::length_error("Voxel tree exceeds 16-bit mesh limit");
                uint16_t base=static_cast<uint16_t>(mesh.vertices.size());
                Point normal={{0,0,0}};normal[axis]=float(sign);
                const Point world_normal={{cosine*normal[0]+sine*normal[2],normal[1],-sine*normal[0]+cosine*normal[2]}};
                for(int corner=0;corner<4;++corner) {
                    Point p={{0,0,0}};p[axis]=plane.first*voxel_size;p[u]=(x+((corner==1||corner==2)?dx:0))*voxel_size;p[w]=(y+(corner>=2?dy:0))*voxel_size;
                    const float weight=std::max(0.f,std::min(1.f,p[1]/7.f));
                    Point world={{tree.position[0]+scale*(cosine*p[0]+sine*p[2]),tree.position[1]+scale*p[1],tree.position[2]+scale*(-sine*p[0]+cosine*p[2])}};
                    terrain_vertex vertex(world,world_normal,tint(tree.type,material));vertex.tree_bend={{weight,phase}};vertex.texcoord={{tree.position[0],tree.position[2]}};vertex.outline_id=silhouette;mesh.vertices.push_back(vertex);
                }
                const int positive[6]={0,1,2,0,2,3},negative[6]={0,2,1,0,3,2};
                const int* indices=sign>0?positive:negative;
                for(int i=0;i<6;++i)mesh.indices.push_back(static_cast<uint16_t>(base+indices[i]));
            }
        }
    }
    return mesh;
}
} // namespace game
