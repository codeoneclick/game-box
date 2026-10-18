#include "stairs.h"
#include "building_placement.h"
#include <cassert>
#include <cmath>
#include <iostream>
#include <limits>
using namespace game;
namespace {
void rectangle(terrain_mesh_data& m,float x0,float z0,float x1,float z1,float y,
               float yaw=0,float edge_x=0,float edge_z=0) {
  const auto base=static_cast<uint16_t>(m.vertices.size());
  for(auto p:std::array<std::array<float,2>,4>{{{{x0,z0}},{{x0,z1}},{{x1,z1}},{{x1,z0}}}}) {
    const float c=std::cos(yaw),s=std::sin(yaw);
    m.vertices.emplace_back(std::array<float,3>{{edge_x+c*p[0]+s*p[1],y,edge_z-s*p[0]+c*p[1]}},
                            std::array<float,3>{{0,1,0}},std::array<uint8_t,4>{{255,255,255,255}});
  }
  for(int i:{0,1,2,0,2,3})m.indices.push_back(static_cast<uint16_t>(base+i));
}
terrain_mesh_data ledge(float lower=.5f,float upper=2.f,float width=20.f,
                       float upper_depth=20.f,float yaw=0,float edge_x=0,float edge_z=0) {
  terrain_mesh_data m;
  rectangle(m,-20,-20,20,0,lower,yaw,edge_x,edge_z);
  rectangle(m,-width*.5f,0,width*.5f,upper_depth,upper,yaw,edge_x,edge_z);
  return m;
}
void check_mesh(const EditorStairs& s,const terrain_config& c) {
  auto a=build_stairs_mesh(s,c),b=build_stairs_mesh(s,c);
  assert(a.vertices.size()>1000 && a.vertices.size()<2000 && a.indices.size()%3==0);
  assert(a.vertices.size()==b.vertices.size() && a.indices==b.indices);
  const float sn=std::sin(s.yaw),co=std::cos(s.yaw);
  for(size_t i=0;i<a.vertices.size();++i) {
    const auto& v=a.vertices[i];const auto& w=b.vertices[i];
    assert(v.position==w.position && v.normal==w.normal && v.color==w.color);
    assert(v.editor_palette[0]==-1 && v.grass_bend[0]==0 && v.tree_bend[0]==0 && v.outline_id==0 && v.color[3]==255 && v.stone_texture);
    for(float p:v.position)assert(std::isfinite(p));
    const float dx=v.position[0]-s.position[0],dz=v.position[2]-s.position[2];
    assert(std::abs(co*dx-sn*dz)<=stairs_width*.5f+1e-4f);
    assert(std::abs(sn*dx+co*dz)<=stairs_run*.5f+1e-4f);
    assert(v.position[1]>=s.position[1]-.026f && v.position[1]<=c.layer_height+.019f);
  }
  for(size_t i=0;i<a.indices.size();i+=3) {
    for(int k=0;k<3;++k)assert(a.indices[i+k]<a.vertices.size());
    const auto& p=a.vertices[a.indices[i]];const auto& q=a.vertices[a.indices[i+1]];const auto& r=a.vertices[a.indices[i+2]];
    const float ux=q.position[0]-p.position[0],uy=q.position[1]-p.position[1],uz=q.position[2]-p.position[2];
    const float vx=r.position[0]-p.position[0],vy=r.position[1]-p.position[1],vz=r.position[2]-p.position[2];
    const std::array<float,3> cross={{uy*vz-uz*vy,uz*vx-ux*vz,ux*vy-uy*vx}};
    float dot=0,norm=0;for(int k=0;k<3;++k){dot+=cross[k]*p.normal[k];norm+=p.normal[k]*p.normal[k];}
    assert(dot>1e-8f && std::abs(norm-1)<1e-4f);
  }
  auto changed=s;++changed.id;
  auto other=build_stairs_mesh(changed,c);bool differs=false;
  for(size_t i=0;i<a.vertices.size();++i)if(a.vertices[i].color!=other.vertices[i].color){differs=true;break;}
  assert(differs);
}
}
int main() {
  terrain_config c;EditorStairs s;s.id=42;
  BuildingSurfaceSupport surface({ledge()});
  float height=0;
  assert(surface.height_at(0,-1,height) && height==.5f);
  assert(surface.height_at(0,1,height) && height==2);
  assert(!surface.height_at(50,50,height));
  assert(find_stairs_site(surface,c,0,-2,s));
  assert(s.id==42 && s.lower_terrace==1 && std::abs(s.position[2]+1.65f)<1e-5f && s.yaw==0);
  assert(stairs_supported(surface,c,s));
  EditorStairs upper;
  assert(find_stairs_site(surface,c,0,2,upper));
  assert(upper.position==s.position && upper.yaw==s.yaw);
  assert(!find_stairs_site(surface,c,0,7,upper));
  assert(!find_stairs_site(BuildingSurfaceSupport({ledge(2,2)}),c,0,0,upper));
  assert(!find_stairs_site(BuildingSurfaceSupport({ledge(.5f,4)}),c,0,0,upper));
  assert(!find_stairs_site(BuildingSurfaceSupport({ledge(-2,.5f)}),c,0,0,upper));
  assert(!find_stairs_site(BuildingSurfaceSupport({ledge(.5f,2,3.7f)}),c,0,0,upper));
  assert(upper.lower_terrace==1 && std::abs(upper.position[2]+1.65f)<1e-5f); // Invalid preview still snaps.
  assert(!find_stairs_site(BuildingSurfaceSupport({ledge(.5f,2,20,1.f)}),c,0,0,upper));
  terrain_mesh_data hole=ledge();
  // Lower surface ends early: both landing and lower footprint are required.
  hole=terrain_mesh_data();rectangle(hole,-20,-1,20,0,.5f);rectangle(hole,-20,0,20,20,2);
  assert(!find_stairs_site(BuildingSurfaceSupport({hole}),c,0,0,upper));
  hole=terrain_mesh_data();
  rectangle(hole,-20,-20,.1f,0,.5f);rectangle(hole,.2f,-20,20,0,.5f);
  rectangle(hole,.1f,-20,.2f,-2,.5f);rectangle(hole,.1f,-1.7f,.2f,0,.5f);
  rectangle(hole,-20,0,20,20,2);
  assert(!stairs_supported(BuildingSurfaceSupport({hole}),c,s)); // Small hole inside the flight.
  auto moved=s;moved.position[2]+=.5f;assert(!stairs_supported(surface,c,moved));
  moved=s;moved.position[1]=0;assert(!stairs_supported(surface,c,moved));
  const float yaw=.63f,ex=3.173f,ez=-4.719f;
  BuildingSurfaceSupport rotated({ledge(.5f,2,20,20,yaw,ex,ez)});
  assert(find_stairs_site(rotated,c,ex+std::sin(yaw),ez+std::cos(yaw),upper));
  assert(stairs_supported(rotated,c,upper));
  assert(std::abs(upper.yaw-yaw)<1e-5f);
  assert(std::abs(upper.position[0]-(ex-std::sin(yaw)*1.65f))<1e-4f);
  BuildingSurfaceSupport inland({ledge(4,6)});
  EditorStairs inland_stairs;
  assert(find_stairs_site(inland,c,0,0,inland_stairs) && inland_stairs.lower_terrace==3);
  assert(stairs_supported(inland,c,inland_stairs));
  assert(point_inside_stairs(s,0,-1.65f));assert(!point_inside_stairs(s,4,-1.65f));
  assert(stairs_overlap(s,s));moved=s;moved.position[0]+=10;assert(!stairs_overlap(s,moved));
  moved=s;moved.yaw=1.1f;moved.position[0]+=2;assert(stairs_overlap(s,moved));
  moved=s;moved.position[0]+=stairs_width;assert(!stairs_overlap(s,moved,0));
  EditorBuilding building;building.position=s.position;assert(stairs_building_overlap(s,building));
  building.position[0]+=20;assert(!stairs_building_overlap(s,building));
  float distance=0;
  assert(stairs_ray_hit(s,c,{{0,10,-3.4f}},{{0,-1,0}},distance));
  assert(distance>9 && distance<9.5f); // First tread, rather than full-flight AABB top.
  assert(!stairs_ray_hit(s,c,{{10,10,0}},{{0,-1,0}},distance));
  assert(!stairs_ray_hit(s,c,{{0,10,0}},{{0,0,0}},distance));
  assert(!stairs_ray_hit(s,c,{{0,10,0}},{{0,1,0}},distance));
  assert(stairs_ray_hit(upper,c,{{upper.position[0],10,upper.position[2]}},{{0,-1,0}},distance));
  check_mesh(s,c);upper.id=42;check_mesh(upper,c);
  // Seeded hand-laid variants must keep winding and the reserved footprint.
  for(uint32_t id=1;id<=64;++id){auto variant=s;variant.id=id;variant.yaw=.37f;check_mesh(variant,c);}
  s.position[0]=std::numeric_limits<float>::quiet_NaN();assert(build_stairs_mesh(s,c).vertices.empty());
  std::cout<<"Stairs geometry CPU checks passed\n";
}
