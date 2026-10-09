#include "resident_navigation.h"
#include <algorithm>
#include <cmath>
#include <queue>
#include <stdexcept>
namespace game {
namespace {
    bool inside(resident_point p,const std::array<resident_point,3>& t){
        float a=(t[1].x-t[0].x)*(p.z-t[0].z)-(t[1].z-t[0].z)*(p.x-t[0].x);
        float b=(t[2].x-t[1].x)*(p.z-t[1].z)-(t[2].z-t[1].z)*(p.x-t[1].x);
        float c=(t[0].x-t[2].x)*(p.z-t[2].z)-(t[0].z-t[2].z)*(p.x-t[2].x);
        return (a>=-.00001f&&b>=-.00001f&&c>=-.00001f)||(a<=.00001f&&b<=.00001f&&c<=.00001f);
    }
    bool footprint(const resident_navigation& n,resident_point p){
        // 0.4m body footprint plus 0.8m clearance from the actual shore contours.
        if(!n.surface_supports(p))return false;
        for(int i=0;i<16;++i){float angle=i*6.283185307f/16;
            if(!n.surface_supports({p.x+1.2f*std::cos(angle),p.z+1.2f*std::sin(angle)}))return false;
        }return true;
    }
    std::vector<int> search(const resident_navigation& n,int start,int target){
        std::vector<int> parent(n.walkable.size(),-1);if(start<0||!n.walkable[start])return parent;
        std::queue<int> queue;parent[start]=start;queue.push(start);
        while(!queue.empty()){
            int at=queue.front();queue.pop();if(at==target)break;
            int x=at%n.width,z=at/n.width;
            for(auto offset:{resident_point{1,0},resident_point{-1,0},resident_point{0,1},resident_point{0,-1}}){
                int nx=x+static_cast<int>(offset.x),nz=z+static_cast<int>(offset.z);
                if(nx<0||nz<0||nx>=n.width||nz>=n.height)continue;
                int next=nz*n.width+nx;
                if(parent[next]<0&&n.walkable[next]){parent[next]=at;queue.push(next);}
            }
        }return parent;
    }
}
int resident_navigation::cell_at(resident_point p)const{
    if(!std::isfinite(p.x)||!std::isfinite(p.z)||width==0||height==0)return -1;
    float x=(p.x-origin_x)/cell_size,z=(p.z-origin_z)/cell_size;
    if(x<0||z<0||x>=width||z>=height)return -1;
    return static_cast<int>(z)*width+static_cast<int>(x);
}
resident_point resident_navigation::center(int i)const{return {origin_x+(i%width+.5f)*cell_size,origin_z+(i/width+.5f)*cell_size};}
bool resident_navigation::surface_supports(resident_point p)const{
    if(!std::isfinite(p.x)||!std::isfinite(p.z))return false;
    int cell=cell_at(p);if(cell<0)return false;
    for(auto index:cell_triangles[cell]){const auto& t=triangles[index];
        if(p.x<std::min({t[0].x,t[1].x,t[2].x})||p.x>std::max({t[0].x,t[1].x,t[2].x})||
           p.z<std::min({t[0].z,t[1].z,t[2].z})||p.z>std::max({t[0].z,t[1].z,t[2].z}))continue;
        if(inside(p,t))return true;
    }return false;
}
bool resident_navigation::is_safe(resident_point p)const{int i=cell_at(p);return i>=0&&walkable[i]&&footprint(*this,p);}
bool resident_navigation::segment_is_safe(resident_point a,resident_point b)const{
    if(!is_safe(a)||!is_safe(b))return false;
    float dx=b.x-a.x,dz=b.z-a.z;int steps=std::max(1,static_cast<int>(std::ceil(std::sqrt(dx*dx+dz*dz)/.25f)));
    for(int i=1;i<steps;++i)if(!is_safe({a.x+dx*i/steps,a.z+dz*i/steps}))return false;return true;
}
std::vector<resident_point> resident_navigation::reachable_points(resident_point a)const{
    std::vector<resident_point> result;if(!is_safe(a))return result;
    auto parent=search(*this,cell_at(a),-1);for(size_t i=0;i<parent.size();++i)if(parent[i]>=0)result.push_back(center(static_cast<int>(i)));return result;
}
std::vector<resident_point> resident_navigation::find_path(resident_point a,resident_point b)const{
    if(!is_safe(a)||!is_safe(b))return {};
    int start=cell_at(a),end=cell_at(b);auto parent=search(*this,start,end);if(parent[end]<0)return {};
    std::vector<resident_point> route={b};for(int at=end;at!=start;at=parent[at])route.push_back(center(at));
    route.push_back(center(start));route.push_back(a);std::reverse(route.begin(),route.end());
    for(size_t i=1;i<route.size();++i)if(!segment_is_safe(route[i-1],route[i]))return {};return route;
}
resident_navigation build_resident_navigation(const terrain_mesh_data& mesh,float y,float cell){
    if(!std::isfinite(y)||!std::isfinite(cell)||cell<.25f||cell>4.f)throw std::invalid_argument("Invalid resident navigation parameters");
    resident_navigation n;n.shore_y=y;n.cell_size=cell;
    if(mesh.indices.size()%3)throw std::invalid_argument("Malformed terrain indices");
    float minx=1e9f,minz=1e9f,maxx=-1e9f,maxz=-1e9f;
    for(size_t i=0;i<mesh.indices.size();i+=3){std::array<resident_point,3> t;bool shore=true;
        for(int j=0;j<3;++j){if(mesh.indices[i+j]>=mesh.vertices.size())throw std::invalid_argument("Terrain index out of range");
            const auto& v=mesh.vertices[mesh.indices[i+j]];shore&=v.normal[1]>.99f&&std::abs(v.position[1]-y)<.001f;
            t[j]={v.position[0],v.position[2]};
            if(!std::isfinite(t[j].x)||!std::isfinite(t[j].z))throw std::invalid_argument("Nonfinite terrain");
        }if(!shore)continue;n.triangles.push_back(t);
        for(auto p:t){minx=std::min(minx,p.x);maxx=std::max(maxx,p.x);minz=std::min(minz,p.z);maxz=std::max(maxz,p.z);}
    }
    if(n.triangles.empty())return n;
    if(maxx-minx>4096||maxz-minz>4096)throw std::invalid_argument("Resident navigation domain too large");
    n.origin_x=std::floor(minx/cell)*cell;n.origin_z=std::floor(minz/cell)*cell;
    n.width=static_cast<int>(std::ceil((maxx-n.origin_x)/cell))+1;n.height=static_cast<int>(std::ceil((maxz-n.origin_z)/cell))+1;
    n.walkable.resize(n.width*n.height,0);
    n.cell_triangles.resize(n.walkable.size());
    for(size_t i=0;i<n.triangles.size();++i){const auto& t=n.triangles[i];
        int x0=static_cast<int>((std::min({t[0].x,t[1].x,t[2].x})-n.origin_x)/cell);
        int x1=static_cast<int>((std::max({t[0].x,t[1].x,t[2].x})-n.origin_x)/cell);
        int z0=static_cast<int>((std::min({t[0].z,t[1].z,t[2].z})-n.origin_z)/cell);
        int z1=static_cast<int>((std::max({t[0].z,t[1].z,t[2].z})-n.origin_z)/cell);
        for(int z=z0;z<=z1;++z)for(int x=x0;x<=x1;++x)n.cell_triangles[z*n.width+x].push_back(i);
    }
    for(int i=0;i<n.width*n.height;++i)if(footprint(n,n.center(i)))n.walkable[i]=1;
    return n;
}
}
