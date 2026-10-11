#include "resident_navigation.h"
#include <algorithm>
#include <cmath>
#include <queue>
#include <stdexcept>
#include <map>
#include <tuple>
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
        static const auto offsets=[]{
            std::array<resident_point,16> result;
            for(int i=0;i<16;++i){float angle=i*6.283185307f/16;
                result[i]={1.2f*std::cos(angle),1.2f*std::sin(angle)};}
            return result;
        }();
        for(const auto& offset:offsets){
            if(!n.surface_supports({p.x+offset.x,p.z+offset.z}))return false;
        }return true;
    }
    struct search_scratch {
        std::vector<int> parent,queue;
        std::vector<uint32_t> visited;
        uint32_t generation=0;
        bool contains(size_t cell)const{return visited[cell]==generation;}
    };
    const search_scratch& search(const resident_navigation& n,int start,int target){
        thread_local search_scratch result;
        result.parent.resize(n.walkable.size());result.visited.resize(n.walkable.size(),0);
        result.queue.clear();
        if(++result.generation==0){std::fill(result.visited.begin(),result.visited.end(),0);result.generation=1;}
        if(start<0||!n.walkable[start])return result;
        result.parent[start]=start;result.visited[start]=result.generation;result.queue.push_back(start);
        for(size_t head=0;head<result.queue.size();++head){
            int at=result.queue[head];if(at==target)break;
            int x=at%n.width,z=at/n.width;
            for(auto offset:{resident_point{1,0},resident_point{-1,0},resident_point{0,1},resident_point{0,-1}}){
                int nx=x+static_cast<int>(offset.x),nz=z+static_cast<int>(offset.z);
                if(nx<0||nz<0||nx>=n.width||nz>=n.height)continue;
                int next=nz*n.width+nx;
                if(!result.contains(next)&&n.walkable[next]){
                    result.parent[next]=at;result.visited[next]=result.generation;result.queue.push_back(next);
                }
            }
        }return result;
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
    if(!solid_cells.empty()&&solid_cells[cell])return true;
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
    const auto& found=search(*this,cell_at(a),-1);for(size_t i=0;i<found.parent.size();++i)if(found.contains(i))result.push_back(center(static_cast<int>(i)));return result;
}
std::vector<resident_point> resident_navigation::find_path(resident_point a,resident_point b)const{
    if(!is_safe(a)||!is_safe(b))return {};
    int start=cell_at(a),end=cell_at(b);const auto& found=search(*this,start,end);if(!found.contains(end))return {};
    std::vector<resident_point> route={b};for(int at=end;at!=start;at=found.parent[at])route.push_back(center(at));
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
    // Certify complete cells inside the triangle union. An exterior contour
    // cannot cross a cell whose bounding box touches no boundary edge. Points
    // near contours still use the original exact test and 1.2m footprint.
    using edge_key=std::tuple<float,float,float,float>;
    std::map<edge_key,unsigned> sides;
    for(const auto& t:n.triangles)for(int j=0;j<3;++j){
        auto a=t[j],b=t[(j+1)%3];const auto c=t[(j+2)%3];
        if(std::tie(a.x,a.z)>std::tie(b.x,b.z))std::swap(a,b);
        const float cross=(b.x-a.x)*(c.z-a.z)-(b.z-a.z)*(c.x-a.x);
        sides[{a.x,a.z,b.x,b.z}]|=cross>0?1u:cross<0?2u:4u;
    }
    std::vector<uint8_t> boundary(n.walkable.size(),0);
    for(const auto& edge:sides){
        if(edge.second==3)continue; // triangles cover both sides of this edge
        const auto& e=edge.first;
        const int x0=std::max(0,static_cast<int>(std::floor((std::min(std::get<0>(e),std::get<2>(e))-n.origin_x)/cell-.0001f)));
        const int x1=std::min(n.width-1,static_cast<int>(std::floor((std::max(std::get<0>(e),std::get<2>(e))-n.origin_x)/cell+.0001f)));
        const int z0=std::max(0,static_cast<int>(std::floor((std::min(std::get<1>(e),std::get<3>(e))-n.origin_z)/cell-.0001f)));
        const int z1=std::min(n.height-1,static_cast<int>(std::floor((std::max(std::get<1>(e),std::get<3>(e))-n.origin_z)/cell+.0001f)));
        for(int z=z0;z<=z1;++z)for(int x=x0;x<=x1;++x)boundary[z*n.width+x]=1;
    }
    n.solid_cells.resize(n.walkable.size());
    for(size_t i=0;i<n.walkable.size();++i)n.solid_cells[i]=n.walkable[i]&&!boundary[i];
    return n;
}
}
