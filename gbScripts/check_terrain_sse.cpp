#include "terrain_lod.h"
#include <cmath>
#include <iostream>
#include <stdexcept>
using namespace game;
void require(bool b,const char*m){if(!b)throw std::runtime_error(m);}
int main(){try{
 {
  terrain_mesh_data mesh;for(auto p:{std::array<float,3>{{0,4,0}},std::array<float,3>{{0,4,2}},std::array<float,3>{{2,4,0}}})mesh.vertices.emplace_back(p,std::array<float,3>{{0,1,0}},std::array<uint8_t,4>{{255,255,255,255}});
  mesh.indices={0,1,2};TerrainLodVersion version;version.terrain_revision=42;
  auto preview=build_terrain_lod_preview(mesh,2,version);
  require(preview.nodes.size()==1&&preview.nodes[0].levels.size()==1,"preview must not build offline LODs");
  require(preview.nodes[0].levels[0]->mesh.indices==mesh.indices&&preview.version.terrain_revision==42,"preview lost current geometry/version");
  for(size_t i=0;i<mesh.vertices.size();++i)require(preview.nodes[0].levels[0]->mesh.vertices[i].position==mesh.vertices[i].position,"preview changed the edited surface");
 }
 TerrainLodGeneration g;g.cell_size=2;g.nodes.resize(1);auto& n=g.nodes[0];n.minimum={{-1,-1,-1}};n.maximum={{1,1,1}};
 for(int i=0;i<6;++i){auto v=std::make_shared<TerrainLodVariant>();v->world_error=i*.2f;v->mesh.indices.resize((120-i*12)*3);n.levels.push_back(v);}
 TerrainLodView v;v.view_projection={{.0001f,0,0,0,0,.0001f,0,0,0,0,.0001f,0,0,0,0,1}};v.projection_scale=500;
 v.near_error_px=v.normal_error_px=v.far_error_px=1;
 v.camera={{301,0,0}};auto a=select_terrain_lod(g,v,nullptr);
 auto detailed=g;for(auto& p:detailed.nodes[0].levels){auto copy=std::make_shared<TerrainLodVariant>(*p);copy->world_error*=10;p=copy;}
 auto b=select_terrain_lod(detailed,v,nullptr);
 require(a.items[0].level>b.items[0].level,"geometric importance does not control selection at the same distance");
 v.projection_scale=1000;auto zoom=select_terrain_lod(g,v,nullptr);require(zoom.items[0].level<a.items[0].level,"FOV/resolution does not control screen-error selection");
 v.projection_scale=500;int last=-1;size_t last_count=10000;bool morphed=false;
 for(int distance=2;distance<2000;++distance){v.camera={{1.f+distance,0,0}};auto s=select_terrain_lod(g,v,nullptr);auto item=s.items[0];require(item.level>=last&&s.triangles<=last_count,"index reduction not monotonic");require(n.levels[item.level]->world_error*v.projection_scale/distance<=1.001f,"selected contour exceeds pixel budget");last=item.level;last_count=s.triangles;morphed|=item.morph>0&&item.morph<1;}
 require(morphed&&last==5,"missing continuous morph or far index reduction");
 for(auto errors:std::vector<std::vector<float>>{{0,1,1,1,1.1f,1.15f},{0,1,1.1f,1.2f,1.3f,1.4f}}){
  auto plateau=g;for(size_t i=0;i<errors.size();++i){auto copy=std::make_shared<TerrainLodVariant>(*n.levels[i]);copy->world_error=errors[i];plateau.nodes[0].levels[i]=copy;}
  float last_phase=-1;
  for(int step=50000;step<90000;++step){v.camera={{1+step*.01f,0,0}};auto s=select_terrain_lod(plateau,v,nullptr);float phase=s.items[0].level+s.items[0].morph;
   if(last_phase>=0)require(phase-last_phase<.02f,"equal or overlapping error thresholds skip geometry morph states");last_phase=phase;
  }
 }
 // Hysteresis must preserve a small change around a completed morph without
 // holding the entire terrain on one global level.
 v.camera={{501,0,0}};auto stable=select_terrain_lod(g,v,nullptr);v.camera[0]+=.001f;auto jitter=select_terrain_lod(g,v,&stable);require(jitter.items==stable.items,"threshold jitter changes active contour");
 std::cout<<"SSE checks passed: local importance, FOV, pixel budget, compact counts, morph and hysteresis\n";return 0;
}catch(const std::exception&e){std::cerr<<e.what()<<'\n';return 1;}}
