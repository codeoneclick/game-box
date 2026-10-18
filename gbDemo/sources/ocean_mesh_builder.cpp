#include "ocean_mesh_builder.h"
#include <algorithm>
#include <cmath>
#include <stdexcept>
#include <functional>
#include <limits>
namespace game {
    ocean_mesh_quadtree::ocean_mesh_quadtree(const terrain_mesh_data& mesh):m_indices(mesh.indices)
    {
        if(m_indices.size()%3)throw std::invalid_argument("Incomplete ocean triangle");
        std::vector<size_t> triangles;
        for(size_t i=0;i<m_indices.size();i+=3){
            bounds b;
            std::array<double,2> centroid={{0,0}};
            for(size_t j=0;j<3;++j){
                if(m_indices[i+j]>=mesh.vertices.size())throw std::invalid_argument("Invalid ocean index");
                const auto& p=mesh.vertices[m_indices[i+j]].position;
                if(!std::isfinite(p[0])||!std::isfinite(p[2]))throw std::invalid_argument("Nonfinite ocean vertex");
                centroid[0]+=double(p[0])/3;centroid[1]+=double(p[2])/3;
                if(j==0)b={p[0],p[2],p[0],p[2]};
                else {b.min_x=std::min(b.min_x,p[0]);b.max_x=std::max(b.max_x,p[0]);b.min_z=std::min(b.min_z,p[2]);b.max_z=std::max(b.max_z,p[2]);}
            }
            triangles.push_back(m_triangle_bounds.size());m_triangle_bounds.push_back(b);m_triangle_centroids.push_back(centroid);
        }
        if(!triangles.empty())build_node(triangles,0);
    }
    int ocean_mesh_quadtree::build_node(const std::vector<size_t>& triangles,int depth)
    {
        node n;n.triangles=triangles;n.box=m_triangle_bounds[triangles.front()];
        for(auto triangle:triangles){const auto& b=m_triangle_bounds[triangle];
            n.box.min_x=std::min(n.box.min_x,b.min_x);n.box.max_x=std::max(n.box.max_x,b.max_x);
            n.box.min_z=std::min(n.box.min_z,b.min_z);n.box.max_z=std::max(n.box.max_z,b.max_z);
        }
        const int index=static_cast<int>(m_nodes.size());m_nodes.push_back(std::move(n));
        if(triangles.size()<=512||depth>=8)return index;
        // Long horizon triangles can extend far beyond their centroids. Split
        // their centroids' range so a distant vertex cannot collapse all
        // triangles into one quadrant. Culling still uses full vertex bounds.
        auto low=m_triangle_centroids[triangles.front()],high=low;
        for(auto triangle:triangles)for(size_t axis=0;axis<2;++axis){
            low[axis]=std::min(low[axis],m_triangle_centroids[triangle][axis]);
            high[axis]=std::max(high[axis],m_triangle_centroids[triangle][axis]);
        }
        const double mid_x=(low[0]+high[0])*.5,mid_z=(low[1]+high[1])*.5;
        std::array<std::vector<size_t>,4> groups;
        for(auto triangle:triangles){const auto& centroid=m_triangle_centroids[triangle];
            const int quadrant=(centroid[0]>=mid_x?1:0)+(centroid[1]>=mid_z?2:0);
            groups[quadrant].push_back(triangle);
        }
        for(const auto& group:groups)if(group.size()==triangles.size())return index;
        for(size_t i=0;i<4;++i)if(!groups[i].empty()){
            const int child=build_node(groups[i],depth+1);m_nodes[index].children[i]=child;
        }
        return index;
    }
    ocean_visibility ocean_mesh_quadtree::select(const std::array<float,16>& matrix,float min_y,float max_y,bool horizon,float foam_extent) const
    {
        ocean_visibility result;
        select(matrix,min_y,max_y,horizon,foam_extent,result);
        return result;
    }
    bool ocean_mesh_quadtree::select(const std::array<float,16>& matrix,float min_y,float max_y,bool horizon,float foam_extent,ocean_visibility& result) const
    {
        result.tested_nodes=0;
        if(m_nodes.empty()){
            const bool changed=!result.initialized||!result.water.empty()||!result.foam.empty()||!result.water_nodes.empty()||!result.foam_nodes.empty();
            result.water.clear();result.foam.clear();result.water_nodes.clear();result.foam_nodes.clear();result.initialized=true;return changed;
        }
        bool valid=std::isfinite(min_y)&&std::isfinite(max_y)&&min_y<=max_y;
        for(float value:matrix)valid=valid&&std::isfinite(value);
        if(!valid){
            result.water=m_indices;result.foam.clear();if(!horizon)result.foam=m_indices;
            // A sentinel distinguishes fallback indices from a valid empty
            // selection on the next call. Invalid inputs always rebuild.
            result.water_nodes.assign(1,std::numeric_limits<size_t>::max());result.foam_nodes.clear();result.initialized=true;return true;
        }
        std::array<std::array<double,4>,6> planes;
        for(int axis=0;axis<3;++axis)for(int sign=0;sign<2;++sign)for(int c=0;c<4;++c)
            planes[axis*2+sign][c]=double(matrix[c*4+3])+(sign?-1.0:1.0)*matrix[c*4+axis];
        // GL's near plane is conservative for both [-w,w] and [0,w] clip depth.
        const auto classify=[&](const bounds& box,bool water_pass){
            bool inside=true;
            // The water shader clamps depth for every vertex beyond extent 720,
            // including ordinary grid patches. Foam never applies that clamp.
            const bool clamped=water_pass&&(horizon||box.min_x < -720.f||box.max_x>720.f||box.min_z < -720.f||box.max_z>720.f);
            for(size_t i=0;i<(clamped?5u:6u);++i){const auto& p=planes[i];
                const double cx=(double(box.min_x)+box.max_x)*.5,cz=(double(box.min_z)+box.max_z)*.5,cy=(double(min_y)+max_y)*.5;
                const double distance=p[0]*cx+p[1]*cy+p[2]*cz+p[3];
                const double radius=std::abs(p[0])*(double(box.max_x)-box.min_x)*.5+std::abs(p[1])*(double(max_y)-min_y)*.5+std::abs(p[2])*(double(box.max_z)-box.min_z)*.5;
                const double epsilon=1.e-5*(1+std::abs(p[0]*cx)+std::abs(p[1]*cy)+std::abs(p[2]*cz)+std::abs(p[3])+radius);
                if(distance+radius < -epsilon)return -1;
                if(distance-radius<=epsilon)inside=false;
            }
            return inside?1:0;
        };
        struct rectangle {double min_x,min_y,max_x,max_y;bool bounded;};
        const auto project=[&](const bounds& box){
            rectangle r={std::numeric_limits<double>::infinity(),std::numeric_limits<double>::infinity(),-std::numeric_limits<double>::infinity(),-std::numeric_limits<double>::infinity(),true};
            for(float x:{box.min_x,box.max_x})for(float y:{min_y,max_y})for(float z:{box.min_z,box.max_z}){
                const double w=double(matrix[3])*x+double(matrix[7])*y+double(matrix[11])*z+matrix[15];
                const double ew=1.e-5*(1+std::abs(double(matrix[3])*x)+std::abs(double(matrix[7])*y)+std::abs(double(matrix[11])*z)+std::abs(matrix[15]));
                if(w<=ew){r.bounded=false;return r;}
                const double px=double(matrix[0])*x+double(matrix[4])*y+double(matrix[8])*z+matrix[12];
                const double py=double(matrix[1])*x+double(matrix[5])*y+double(matrix[9])*z+matrix[13];
                const double ex=1.e-5*(1+std::abs(double(matrix[0])*x)+std::abs(double(matrix[4])*y)+std::abs(double(matrix[8])*z)+std::abs(matrix[12]));
                const double ey=1.e-5*(1+std::abs(double(matrix[1])*x)+std::abs(double(matrix[5])*y)+std::abs(double(matrix[9])*z)+std::abs(matrix[13]));
                for(double denominator:{w-ew,w+ew}){
                    r.min_x=std::min(r.min_x,(px-ex)/denominator);r.max_x=std::max(r.max_x,(px+ex)/denominator);
                    r.min_y=std::min(r.min_y,(py-ey)/denominator);r.max_y=std::max(r.max_y,(py+ey)/denominator);
                }
            }
            return r;
        };
        const bool prune=std::isfinite(foam_extent)&&foam_extent>0;
        const auto support=project(bounds{-foam_extent,-foam_extent,foam_extent,foam_extent});
        const auto outside_support=[&](const bounds& b){
            const double epsilon=1.e-5*(1+foam_extent+std::abs(b.min_x)+std::abs(b.max_x)+std::abs(b.min_z)+std::abs(b.max_z));
            return b.max_x < -foam_extent-epsilon||b.min_x>foam_extent+epsilon||b.max_z < -foam_extent-epsilon||b.min_z>foam_extent+epsilon;
        };
        const auto inside_support=[&](const bounds& b){return b.min_x>=-foam_extent&&b.max_x<=foam_extent&&b.min_z>=-foam_extent&&b.max_z<=foam_extent;};
        const auto cannot_occlude=[&](const bounds& b){
            if(!outside_support(b)||!support.bounded)return false;
            const auto screen=project(b);if(!screen.bounded)return false;
            const double epsilon=1.e-5*(1+std::abs(screen.min_x)+std::abs(screen.max_x)+std::abs(screen.min_y)+std::abs(screen.max_y)+std::abs(support.min_x)+std::abs(support.max_x)+std::abs(support.min_y)+std::abs(support.max_y));
            return screen.max_x<support.min_x-epsilon||screen.min_x>support.max_x+epsilon||screen.max_y<support.min_y-epsilon||screen.min_y>support.max_y+epsilon;
        };
        std::vector<size_t> water_nodes,foam_nodes;
        water_nodes.reserve(result.water_nodes.size());foam_nodes.reserve(result.foam_nodes.size());
        std::function<void(int,bool,bool)> visit=[&](int index,bool contained,bool foam_only){
            ++result.tested_nodes;const auto& n=m_nodes[index];
            const int visibility=contained?1:classify(n.box,!foam_only);if(visibility<0)return;
            const bool all_inside=visibility==1;
            if(foam_only&&cannot_occlude(n.box))return;
            if(all_inside&&(!foam_only||inside_support(n.box))){
                (foam_only?foam_nodes:water_nodes).push_back(static_cast<size_t>(index));return;
            }
            bool leaf=true;for(int child:n.children)if(child>=0){leaf=false;visit(child,all_inside,foam_only);}
            // Intersecting leaves submit their original triangles together.
            // The GPU clips boundary triangles; retaining these small groups
            // avoids expensive per-triangle projection during camera motion.
            if(leaf)(foam_only?foam_nodes:water_nodes).push_back(static_cast<size_t>(index));
        };
        visit(0,false,false);
        if(!horizon&&prune)visit(0,false,true);
        else if(!horizon)foam_nodes=water_nodes;
        if(result.initialized&&water_nodes==result.water_nodes&&foam_nodes==result.foam_nodes)return false;
        result.initialized=true;
        result.water_nodes=std::move(water_nodes);result.foam_nodes=std::move(foam_nodes);
        std::vector<unsigned char> water(m_triangle_bounds.size(),0),foam(water.size(),0);
        for(auto index:result.water_nodes)for(auto t:m_nodes[index].triangles)water[t]=1;
        if(!horizon&&prune)for(auto index:result.foam_nodes)for(auto t:m_nodes[index].triangles)foam[t]=1;
        result.water.clear();result.foam.clear();
        result.water.reserve(m_indices.size());if(!horizon)result.foam.reserve(m_indices.size());
        for(size_t t=0;t<water.size();++t)if(water[t])for(size_t j=0;j<3;++j){
            result.water.push_back(m_indices[t*3+j]);if(!horizon&&(!prune||foam[t]))result.foam.push_back(m_indices[t*3+j]);
        }
        return true;
    }
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
        const int rings=std::min(40,65535/(4*(segments+1))-1);
        const float span=(outer_size-inner_size)*.5f;
        const float first=std::min(inner_size/segments,span/rings);
        // Keep detail close to the island when the final flat ring extends to
        // the visual horizon. Enlarging the background must not stretch waves.
        const float detailed_span=std::min(span,4096.f);
        const float curve=rings>1?(detailed_span-first*rings)/(rings*(rings-1)):0.f;
        mesh.vertices.reserve(static_cast<size_t>(segments+1)*(rings+1)*4);
        mesh.indices.reserve(static_cast<size_t>(segments)*rings*24);
        for(int side=0;side<4;++side){
            const uint16_t base=static_cast<uint16_t>(mesh.vertices.size());
            for(int ring=0;ring<=rings;++ring)for(int i=0;i<=segments;++i){
                const float distance=ring==rings?span:first*ring+curve*ring*(ring-1);
                const float size=inner_size+distance*2.f;
                float x=-size*.5f+i*size/segments,z=-size*.5f;
                for(int rotation=0;rotation<side;++rotation){float old=x;x=-z;z=old;}
                mesh.vertices.push_back({{{x,0,z}},{{0,1,0}},{{255,255,255,255}}});
            }
            for(int ring=0;ring<rings;++ring)for(int i=0;i<segments;++i){
                const uint16_t a=base+ring*(segments+1)+i,b=a+1,c=a+segments+1,d=c+1;
                for(auto index:{a,b,c,b,d,c})mesh.indices.push_back(index);
            }
        }
        return mesh;
    }
}
