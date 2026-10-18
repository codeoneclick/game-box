#include "stairs.h"
#include "building_placement.h"
#include <algorithm>
#include <cmath>
#include <limits>

namespace game { namespace {
using Point = std::array<float,3>;
using Color = std::array<uint8_t,4>;
constexpr float edge_z = stairs_run*.5f-.35f;
float terrace_height(const terrain_config& c,int terrace) {
  return terrace==1?c.shore_height:(terrace-1)*c.layer_height;
}
bool valid_pose(const EditorStairs& s,const terrain_config& c) {
  return s.lower_terrace>=1 && s.lower_terrace<16 &&
    std::isfinite(s.yaw) && std::isfinite(s.position[0]) &&
    std::isfinite(s.position[1]) && std::isfinite(s.position[2]) &&
    std::isfinite(c.layer_height) && c.layer_height>0 &&
    std::isfinite(c.shore_height) && c.shore_height>=0 &&
    c.shore_height<c.layer_height;
}
Point world(const EditorStairs& s,float x,float y,float z) {
  const float c=std::cos(s.yaw),sn=std::sin(s.yaw);
  return {{s.position[0]+c*x+sn*z,s.position[1]+y,s.position[2]-sn*x+c*z}};
}
Point local(const EditorStairs& s,Point p,bool direction=false) {
  if(!direction) for(int i=0;i<3;++i)p[i]-=s.position[i];
  const float c=std::cos(s.yaw),sn=std::sin(s.yaw);
  return {{c*p[0]-sn*p[2],p[1],sn*p[0]+c*p[2]}};
}
bool obb_overlap(Point a,float ay,float aw,float ad,
                 Point b,float by,float bw,float bd,float margin) {
  if(!std::isfinite(margin))return false;
  aw+=margin;ad+=margin;bw+=margin;bd+=margin;
  if(aw<=0||ad<=0||bw<=0||bd<=0)return false;
  for(float yaw:{ay,by})for(int axis=0;axis<2;++axis) {
    const float nx=axis?std::sin(yaw):std::cos(yaw),
                nz=axis?std::cos(yaw):-std::sin(yaw);
    auto extent=[&](float y,float w,float d){
      return w*std::abs(nx*std::cos(y)-nz*std::sin(y))+
             d*std::abs(nx*std::sin(y)+nz*std::cos(y));
    };
    if(std::abs((a[0]-b[0])*nx+(a[2]-b[2])*nz)>=
       extent(ay,aw,ad)+extent(by,bw,bd)-1e-6f)return false;
  }
  return true;
}
bool box_hit(Point o,Point d,Point lo,Point hi,float& distance) {
  float enter=0,exit=std::numeric_limits<float>::max();
  for(int i=0;i<3;++i) {
    if(std::abs(d[i])<1e-8f) {if(o[i]<lo[i]||o[i]>hi[i])return false;}
    else {
      float a=(lo[i]-o[i])/d[i],b=(hi[i]-o[i])/d[i];
      if(a>b)std::swap(a,b);
      enter=std::max(enter,a);exit=std::min(exit,b);
      if(enter>exit)return false;
    }
  }
  distance=enter;return true;
}
uint32_t hash(uint32_t v) {
  v=(v^(v>>16))*0x7feb352du;v=(v^(v>>15))*0x846ca68bu;return v^(v>>16);
}
Point cross(Point a,Point b,Point c) {
  Point u={{b[0]-a[0],b[1]-a[1],b[2]-a[2]}},v={{c[0]-a[0],c[1]-a[1],c[2]-a[2]}};
  return {{u[1]*v[2]-u[2]*v[1],u[2]*v[0]-u[0]*v[2],u[0]*v[1]-u[1]*v[0]}};
}
void triangle(terrain_mesh_data& mesh,Point a,Point b,Point c,Color color) {
  Point n=cross(a,b,c);
  float length=std::sqrt(n[0]*n[0]+n[1]*n[1]+n[2]*n[2]);
  if(length<1e-9f)return;
  for(auto& v:n)v/=length;
  const auto base=static_cast<uint16_t>(mesh.vertices.size());
  for(auto p:{a,b,c})mesh.vertices.emplace_back(p,n,color);
  for(int i=0;i<3;++i)mesh.indices.push_back(static_cast<uint16_t>(base+i));
}
void slab(terrain_mesh_data& mesh,const EditorStairs& stairs,
          float x0,float x1,float z0,float z1,float top,Color color,uint32_t seed) {
  // Independent skew/offset and chipped corners give each block a hand-laid
  // silhouette. Keep the center walking strip aligned to the nominal treads.
  const float bevel=.025f,bottom=-.025f,cx=(x0+x1)*.5f,cz=(z0+z1)*.5f;
  auto random=[&](uint32_t salt){return (hash(seed^salt)&65535)/65535.f;};
  const float angle=(random(17)-.5f)*.15f,cs=std::cos(angle),sn=std::sin(angle);
  const float offset_x=(random(31)-.5f)*.08f,offset_z=(random(47)-.5f)*.12f;
  auto point=[&](float x,float y,float z){
    float px=cx+cs*(x-cx)+sn*(z-cz)+offset_x;
    float pz=cz-sn*(x-cx)+cs*(z-cz)+offset_z;
    const float side_weight=std::max(0.f,std::min(1.f,(std::abs(px)-.3f)/.5f));
    px=x+(px-x)*side_weight;
    pz=z+(pz-z)*side_weight;
    px=std::max(-stairs_width*.5f,std::min(stairs_width*.5f,px));
    pz=std::max(-stairs_run*.5f,std::min(stairs_run*.5f,pz));
    return world(stairs,px,y,pz);
  };
  const float corners[4]={.045f+.075f*random(59),.045f+.075f*random(71),
                          .045f+.075f*random(83),.045f+.075f*random(97)};
  auto ring=[&](float inset,float y) {
    const float l=x0+inset,r=x1-inset,f=z0+inset,b=z1-inset;
    return std::array<Point,8>{{point(l+corners[0],y,f),point(l,y,f+corners[0]),
      point(l,y,b-corners[1]),point(l+corners[1],y,b),
      point(r-corners[2],y,b),point(r,y,b-corners[2]),
      point(r,y,f+corners[3]),point(r-corners[3],y,f)}};
  };
  auto low=ring(0,bottom),side=ring(0,top-bevel),high=ring(bevel,top);
  Point center=point(cx,top,cz);
  Point base=point(cx,bottom,cz);
  for(int i=0;i<8;++i) {
    const int j=(i+1)%8;
    triangle(mesh,center,high[i],high[j],color);
    Color dark=color;for(int c=0;c<3;++c)dark[c]=static_cast<uint8_t>(color[c]*.87f);
    triangle(mesh,low[i],low[j],side[i],dark);
    triangle(mesh,side[i],low[j],side[j],dark);
    triangle(mesh,side[i],side[j],high[i],color);
    triangle(mesh,high[i],side[j],high[j],color);
    triangle(mesh,base,low[j],low[i],dark);
  }
}
} // namespace

bool point_inside_stairs(const EditorStairs& s,float x,float z,float margin) {
  auto p=local(s,{{x,s.position[1],z}});
  return std::abs(p[0])<=stairs_width*.5f+margin && std::abs(p[2])<=stairs_run*.5f+margin;
}
bool stairs_overlap(const EditorStairs& a,const EditorStairs& b,float margin) {
  return obb_overlap(a.position,a.yaw,stairs_width*.5f,stairs_run*.5f,
                     b.position,b.yaw,stairs_width*.5f,stairs_run*.5f,margin);
}
bool stairs_building_overlap(const EditorStairs& s,const EditorBuilding& b,float margin) {
  const auto& d=building_definition(b.type);
  return obb_overlap(s.position,s.yaw,stairs_width*.5f,stairs_run*.5f,
                     b.position,b.yaw,d.half_width,d.half_depth,margin);
}
bool stairs_supported(const BuildingSurfaceSupport& surface,const terrain_config& c,
                      const EditorStairs& s) {
  if(!valid_pose(s,c))return false;
  const float lower=terrace_height(c,s.lower_terrace),upper=terrace_height(c,s.lower_terrace+1);
  if(std::abs(s.position[1]-lower)>1e-4f || lower<=c.water_y)return false;
  auto cover=[&](float start,float end,float half_width,float height) {
    auto p=world(s,0,0,(start+end)*.5f);
    return surface.covers(p[0],p[2],s.yaw,half_width,(end-start)*.5f,height);
  };
  return cover(-stairs_run*.5f,edge_z-.10f,stairs_width*.5f,lower) &&
         cover(-3.f,-1.9f,stairs_width*.5f+.1f,lower) &&
         cover(edge_z+.10f,3.f,stairs_width*.5f+.1f,upper);
}
bool find_stairs_site(const BuildingSurfaceSupport& surface,const terrain_config& c,
                      float x,float z,EditorStairs& result,float snap_distance) {
  if(!std::isfinite(x)||!std::isfinite(z)||!std::isfinite(snap_distance)||snap_distance<=0 ||
     !std::isfinite(c.layer_height)||c.layer_height<=0)return false;
  struct Candidate {EditorStairs stairs;float distance;};
  std::vector<Candidate> candidates;
  for(const auto& edge:surface.nearby_edges(x,z,snap_distance)) {
    float dx=edge.b[0]-edge.a[0],dz=edge.b[1]-edge.a[1],len2=dx*dx+dz*dz;
    if(len2<1e-8f)continue;
    float t=std::max(.001f,std::min(.999f,((x-edge.a[0])*dx+(z-edge.a[1])*dz)/len2));
    float px=edge.a[0]+dx*t,pz=edge.a[1]+dz*t;
    float distance=(px-x)*(px-x)+(pz-z)*(pz-z);
    if(distance>snap_distance*snap_distance)continue;
    float nx=-dz/std::sqrt(len2),nz=dx/std::sqrt(len2),a=0,b=0;
    // An internal triangulation edge has equal top heights on both sides.
    if(!surface.height_at(px+nx*.04f,pz+nz*.04f,a) ||
       !surface.height_at(px-nx*.04f,pz-nz*.04f,b) || std::abs(a-b)<1e-4f)continue;
    if(a<b){std::swap(a,b);nx=-nx;nz=-nz;}
    int lower=std::abs(b-c.shore_height)<1e-4f?1:static_cast<int>(std::lround(b/c.layer_height))+1;
    if(lower<1||lower>=16||b<=c.water_y ||
       std::abs(b-terrace_height(c,lower))>1e-4f ||
       std::abs(a-terrace_height(c,lower+1))>1e-4f)continue;
    EditorStairs s=result;
    s.position={{px-nx*edge_z,b,pz-nz*edge_z}};
    s.yaw=normalized_building_yaw(std::atan2(nx,nz));s.lower_terrace=lower;
    candidates.push_back({s,distance});
  }
  std::stable_sort(candidates.begin(),candidates.end(),[](const Candidate& a,const Candidate& b){return a.distance<b.distance;});
  if(candidates.empty())return false;
  result=candidates.front().stairs;
  for(const auto& candidate:candidates)if(stairs_supported(surface,c,candidate.stairs)) {
    result=candidate.stairs;return true;
  }
  return false;
}
bool stairs_ray_hit(const EditorStairs& s,const terrain_config& c,const Point& origin,
                    const Point& direction,float& distance) {
  if(!valid_pose(s,c))return false;
  for(int i=0;i<3;++i)if(!std::isfinite(origin[i])||!std::isfinite(direction[i]))return false;
  if(std::abs(direction[0])+std::abs(direction[1])+std::abs(direction[2])<1e-8f)return false;
  Point o=local(s,origin),d=local(s,direction,true);
  const float rise=terrace_height(c,s.lower_terrace+1)-terrace_height(c,s.lower_terrace);
  float best=std::numeric_limits<float>::max();
  for(int step=0;step<stairs_step_count;++step) {
    float t=0,z0=-stairs_run*.5f+step*stairs_run/float(stairs_step_count);
    if(box_hit(o,d,{{-stairs_width*.5f,-.025f,z0}},
               {{stairs_width*.5f,rise*(step+1)/float(stairs_step_count)+.018f,z0+stairs_run/float(stairs_step_count)}},t))best=std::min(best,t);
  }
  if(best==std::numeric_limits<float>::max())return false;
  distance=best;return true;
}
terrain_mesh_data build_stairs_mesh(const EditorStairs& s,const terrain_config& c) {
  terrain_mesh_data mesh;
  if(!valid_pose(s,c))return mesh;
  const float rise=terrace_height(c,s.lower_terrace+1)-terrace_height(c,s.lower_terrace);
  mesh.vertices.reserve(stairs_step_count*2*144);mesh.indices.reserve(stairs_step_count*2*144);
  for(int row=0;row<stairs_step_count;++row) {
    const int count=2;
    float left=-stairs_width*.5f;
    for(int column=0;column<count;++column) {
      const uint32_t random=hash(s.id^hash(static_cast<uint32_t>(row*7+column+1)));
      float cut=column==count-1?stairs_width*.5f:
        -stairs_width*.5f+(column+1)*stairs_width/count+((random&255)/255.f-.5f)*.85f;
      if(column<count-1&&std::abs(cut)<.18f)cut=row%2?-.18f:.18f;
      const uint8_t sand=static_cast<uint8_t>(204+(random>>8)%27);
      const Color color={{sand,static_cast<uint8_t>(sand-35),static_cast<uint8_t>(sand-100),255}};
      const float z0=-stairs_run*.5f+row*stairs_run/float(stairs_step_count);
      slab(mesh,s,left+(column?.006f:0),cut-(column+1<count?.006f:0),
           z0+(row?.004f:0),z0+stairs_run/float(stairs_step_count)-.004f,rise*(row+1)/float(stairs_step_count)+.018f,color,random);
      left=cut;
    }
  }
  for(auto& vertex:mesh.vertices){
    const auto p=local(s,vertex.position),n=local(s,vertex.normal,true);
    vertex.texcoord=std::abs(n[1])>.5f?std::array<float,2>{{p[0],p[2]}}:
      (std::abs(n[0])>std::abs(n[2])?std::array<float,2>{{p[2],p[1]}}:std::array<float,2>{{p[0],p[1]}});
    vertex.stone_texture=true;
  }
  return mesh;
}
} // namespace game
