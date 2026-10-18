#include "terrain_lod.h"
#include "terrain_mesh_optimizer.h"
#include <algorithm>
#include <cmath>
#include <functional>
#include <limits>
#include <stdexcept>
#include <chrono>
namespace game {namespace {
terrain_mesh_data subset(const terrain_mesh_data& source,const std::vector<size_t>& triangles){
 terrain_mesh_data mesh;std::vector<int> indices(source.vertices.size(),-1);
 for(auto i:triangles)for(int j=0;j<3;++j){auto old=source.indices.at(i+j);int& next=indices[old];if(next<0){next=int(mesh.vertices.size());mesh.vertices.push_back(source.vertices[old]);}mesh.indices.push_back(uint16_t(next));}return mesh;
}
bool visible(const TerrainLodNode& node,const TerrainLodView& view){
 bool outside[6]={true,true,true,true,true,true};for(int i=0;i<8;++i){float p[4]={i&1?node.maximum[0]:node.minimum[0],i&2?node.maximum[1]:node.minimum[1],i&4?node.maximum[2]:node.minimum[2],1},c[4]={};for(int r=0;r<4;++r)for(int k=0;k<4;++k)c[r]+=view.view_projection[k*4+r]*p[k];
  outside[0]&=c[0]<-c[3];outside[1]&=c[0]>c[3];outside[2]&=c[1]<-c[3];outside[3]&=c[1]>c[3];outside[4]&=c[2]<-c[3];outside[5]&=c[2]>c[3];}
 for(bool p:outside)if(p)return false;return true;
}
float distance(const TerrainLodNode& node,const TerrainLodView& view){double d=0;for(int i=0;i<3;++i){double q=std::max(0.f,std::max(node.minimum[i]-view.camera[i],view.camera[i]-node.maximum[i]));d+=q*q;}return float(std::sqrt(d));}
}
TerrainLodGeneration build_terrain_lod_preview(terrain_mesh_data mesh,float cell_size,const TerrainLodVersion& version){
 TerrainLodGeneration result;result.version=version;result.cell_size=cell_size;result.source_triangles=mesh.indices.size()/3;
 if(mesh.indices.empty())return result;
 auto variant=std::make_shared<TerrainLodVariant>();variant->mesh=std::move(mesh);
 variant->morph_delta_xz.resize(variant->mesh.vertices.size(),{{0,0}});variant->morph_normal_xz.resize(variant->mesh.vertices.size(),{{0,0}});
 TerrainLodNode node;node.minimum={{1e20f,1e20f,1e20f}};node.maximum={{-1e20f,-1e20f,-1e20f}};
 for(const auto& v:variant->mesh.vertices)for(int axis=0;axis<3;++axis){node.minimum[axis]=std::min(node.minimum[axis],v.position[axis]);node.maximum[axis]=std::max(node.maximum[axis],v.position[axis]);}
 for(size_t i=0;i<variant->mesh.indices.size();i+=3)if(std::abs(variant->mesh.vertices[variant->mesh.indices[i]].normal[1])<.01f)++variant->side_triangles;else ++variant->top_triangles;
 result.source_sides=variant->side_triangles;result.source_top=variant->top_triangles;
 result.source=std::shared_ptr<const terrain_mesh_data>(variant,&variant->mesh);node.levels.push_back(variant);result.nodes.push_back(std::move(node));return result;
}
TerrainLodGeneration build_terrain_lod_generation(const terrain_mesh_data& source,const TerrainLodSettings& settings,const TerrainContourConstraints& constraints,float cell_size,const TerrainLodVersion& version){
 const auto started=std::chrono::steady_clock::now();
 if(cell_size<=0||!std::isfinite(cell_size)||!settings.valid())throw std::invalid_argument("Invalid Terrain CLOD generation");
 TerrainLodGeneration result;result.version=version;result.cell_size=cell_size;result.source_triangles=source.indices.size()/3;result.source=std::make_shared<const terrain_mesh_data>(source);if(source.indices.empty())return result;
 for(size_t i=0;i<source.indices.size();i+=3)if(std::abs(source.vertices[source.indices[i]].normal[1])<.01f)++result.source_sides;else ++result.source_top;
 std::function<int(const terrain_mesh_data&,std::array<float,4>,int,int)> build;
 build=[&](const terrain_mesh_data& input,std::array<float,4> footprint,int parent,int depth){
  int id=int(result.nodes.size());result.nodes.emplace_back();result.nodes[id].parent=parent;result.nodes[id].depth=depth;
  auto c=constraints;c.bounds=footprint;
  // Terrain materials use world position + outline terrace ID. Palette UV is
  // only a terrain marker in the vertex shader and is replaced by morph UV.
  // Keeping its obsolete per-vertex gradient falsely locks coplanar interiors.
  auto render_input=input;for(auto& v:render_input.vertices)if(v.editor_palette[0]>=0){v.editor_palette={{0,0}};
   // Repeated clipping of one seed edge creates float-roundoff duplicates.
   // Weld only sub-millimetre XZ noise, never heights or material identities.
   v.position[0]=std::round(v.position[0]*1024.f)/1024.f;v.position[2]=std::round(v.position[2]*1024.f)/1024.f;
  }
  auto compact=optimize_terrain_mesh(render_input,{});terrain_mesh_data surface=compact.parts.empty()?render_input:std::move(compact.parts.front());
  // Inputs are already bounded render parts; keep the original if retopology
  // unexpectedly requires splitting instead of silently dropping a part.
  if(compact.parts.size()>1)surface=input;
  auto variants=build_terrain_contour_lods(surface,settings,c);bool changed=false;for(auto& v:variants)changed|=v.actual_percent>0;
  std::shared_ptr<const TerrainLodVariant> exact;
  for(auto& v:variants){if(!changed){if(!exact)exact=std::make_shared<const TerrainLodVariant>(std::move(v));result.nodes[id].levels.push_back(exact);}else result.nodes[id].levels.push_back(std::make_shared<const TerrainLodVariant>(std::move(v)));}
  result.nodes[id].minimum={{1e20f,1e20f,1e20f}};result.nodes[id].maximum={{-1e20f,-1e20f,-1e20f}};
  for(const auto& v:input.vertices)for(int k=0;k<3;++k){result.nodes[id].minimum[k]=std::min(result.nodes[id].minimum[k],v.position[k]);result.nodes[id].maximum[k]=std::max(result.nodes[id].maximum[k],v.position[k]);}
  if(depth>=8||std::max(footprint[2]-footprint[0],footprint[3]-footprint[1])<=cell_size*16.001f||input.indices.size()<72)return id;
  const float mx=(footprint[0]+footprint[2])*.5f,mz=(footprint[1]+footprint[3])*.5f;std::array<std::vector<size_t>,4> groups;
  for(size_t i=0;i<input.indices.size();i+=3){float x=0,z=0;for(int j=0;j<3;++j){const auto& p=input.vertices[input.indices[i+j]].position;x+=p[0]/3;z+=p[2]/3;}groups[(x>=mx?1:0)+(z>=mz?2:0)].push_back(i);}
  for(int i=0;i<4;++i)if(!groups[i].empty()){auto mesh=subset(input,groups[i]);std::array<float,4> bounds={{i&1?mx:footprint[0],i&2?mz:footprint[1],i&1?footprint[2]:mx,i&2?footprint[3]:mz}};int child=build(mesh,bounds,id,depth+1);result.nodes[id].children[i]=child;}
  return id;
 };
 auto bounds=constraints.bounds;if(bounds[0]<-1e19f){bounds={{1e20f,1e20f,-1e20f,-1e20f}};for(const auto& v:source.vertices){bounds[0]=std::min(bounds[0],v.position[0]);bounds[1]=std::min(bounds[1],v.position[2]);bounds[2]=std::max(bounds[2],v.position[0]);bounds[3]=std::max(bounds[3],v.position[2]);}}
 build(source,bounds,-1,0);result.build_ms=std::chrono::duration<double,std::milli>(std::chrono::steady_clock::now()-started).count();return result;
}
float terrain_lod_allowed_error(const TerrainLodView& view,float d,float cell){
 const float t=std::max(0.f,d)/(std::max(.001f,cell)*128.f+std::max(0.f,d));
 float s=t<.5f?t*2.f:(t-.5f)*2.f;s=s*s*(3.f-2.f*s);
 return std::max(.01f,t<.5f?view.near_error_px+(view.normal_error_px-view.near_error_px)*s:view.normal_error_px+(view.far_error_px-view.normal_error_px)*s);
}
TerrainLodSelection select_terrain_lod(const TerrainLodGeneration& generation,const TerrainLodView& view,const TerrainLodSelection* previous){
 TerrainLodSelection result;if(generation.nodes.empty())return result;
 result.items.reserve(previous?std::max<size_t>(4,previous->items.size()):4);
 std::function<void(int,TerrainLodSelection&)> choose;
 choose=[&](int id,TerrainLodSelection& out){const auto& node=generation.nodes[id];++out.tested;if(!visible(node,view)){++out.culled;return;}
  float d=std::max(.001f,distance(node,view));TerrainLodItem item;item.node=id;
  const float budget=terrain_lod_allowed_error(view,d,generation.cell_size);
  float phase=0;
  const float target_error=.8f*d*budget/std::max(.001f,view.projection_scale);
  const float error_step=node.levels.size()>1?node.levels.back()->world_error*.02f/float(node.levels.size()-1):0;
  float previous_error=0;
  for(size_t i=1;i<node.levels.size();++i){
   // Invert one continuous error curve. Independent overlapping morph bands
   // jump across states, especially when several levels share the same error.
   // Bounded additive spacing resolves plateaus conservatively without making
   // the far threshold grow exponentially with the number of levels.
   const float error=node.levels[i]->world_error<=0?0:std::max(node.levels[i]->world_error,previous_error+error_step);
   if(target_error>=error){phase=float(i);previous_error=error;continue;}
   const float t=std::max(0.f,std::min(1.f,(target_error-previous_error)/std::max(1e-12f,error-previous_error)));
   phase=float(i-1)+t*t*(3.f-2.f*t);
   break;
  }
  // Continuous play hysteresis: entering/leaving a state uses opposite sides
  // of a small phase deadband. No discontinuous topology latch or morph reset.
  if(previous)for(const auto& old:previous->items)if(old.node==id){
   const float p=old.level+old.morph;
   const float fraction=phase-std::floor(phase),width=.06f*fraction*(1.f-fraction);
   phase=std::max(phase-width,std::min(phase+width,p));
   phase=std::max(0.f,std::min(float(node.levels.size()-1),phase));
   // Deadband vanishes continuously at endpoints, so completed transitions
   // remove indices without snapping the remaining morph distance.
   break;
  }
  item.level=std::min(int(node.levels.size()-1),int(phase));item.morph=phase-item.level;
  size_t count=node.levels[item.level]->mesh.indices.size()/3;
  const size_t before_items=out.items.size(),before_triangles=out.triangles;
  bool has_children=false;for(int child:node.children)if(child>=0){has_children=true;choose(child,out);}
  bool was_parent=false;if(previous)for(auto& p:previous->items)if(p.node==id)was_parent=true;
  double parent_cost=double(count)+64,child_cost=double(out.triangles-before_triangles)+(out.items.size()-before_items)*64.;
  if(has_children&&child_cost<parent_cost*(was_parent?.9:1.0)){
   // Children already occupy the one reusable traversal buffer.
  }else{out.items.resize(before_items);out.items.push_back(item);out.triangles=before_triangles+count;}
 };
 choose(0,result);return result;
}
}
