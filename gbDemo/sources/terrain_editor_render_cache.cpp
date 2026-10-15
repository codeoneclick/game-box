#include "terrain_editor_render_cache.h"
#include <algorithm>
#include <cmath>
#include <stdexcept>
#include <chrono>

namespace game {namespace {
terrain_vertex interpolate(const terrain_vertex&a,const terrain_vertex&b,float t){
 terrain_vertex v=a;for(int i=0;i<3;++i){v.position[i]+=t*(b.position[i]-a.position[i]);v.normal[i]+=t*(b.normal[i]-a.normal[i]);}
 for(int i=0;i<4;++i)v.color[i]=static_cast<uint8_t>(a.color[i]+t*(b.color[i]-a.color[i]));
 for(int i=0;i<2;++i){v.texcoord[i]+=t*(b.texcoord[i]-a.texcoord[i]);v.editor_palette[i]+=t*(b.editor_palette[i]-a.editor_palette[i]);v.grass_bend[i]+=t*(b.grass_bend[i]-a.grass_bend[i]);v.tree_bend[i]+=t*(b.tree_bend[i]-a.tree_bend[i]);}return v;
}
std::vector<terrain_vertex> clip(const std::vector<terrain_vertex>&source,int axis,float limit,bool greater){
 std::vector<terrain_vertex> result;if(source.empty())return result;auto a=source.back();bool ia=greater?a.position[axis]>=limit:a.position[axis]<=limit;
 for(const auto&b:source){bool ib=greater?b.position[axis]>=limit:b.position[axis]<=limit;if(ia!=ib)result.push_back(interpolate(a,b,(limit-a.position[axis])/(b.position[axis]-a.position[axis])));if(ib)result.push_back(b);a=b;ia=ib;}return result;
}
bool nondegenerate(const terrain_vertex&a,const terrain_vertex&b,const terrain_vertex&c){float u[3],v[3];for(int i=0;i<3;++i){u[i]=b.position[i]-a.position[i];v[i]=c.position[i]-a.position[i];}return std::abs(u[1]*v[2]-u[2]*v[1])+std::abs(u[2]*v[0]-u[0]*v[2])+std::abs(u[0]*v[1]-u[1]*v[0])>1e-7f;}
void append_triangle(terrain_mesh_data&mesh,const terrain_vertex&a,const terrain_vertex&b,const terrain_vertex&c){
 if(!nondegenerate(a,b,c))return;if(mesh.vertices.size()+3>65535)throw std::length_error("Seed terrain chunk exceeds mesh index capacity");uint16_t base=static_cast<uint16_t>(mesh.vertices.size());mesh.vertices.push_back(a);mesh.vertices.push_back(b);mesh.vertices.push_back(c);for(int i=0;i<3;++i)mesh.indices.push_back(static_cast<uint16_t>(base+i));
}
// Merge by triangles, preserving vertex sharing within each input mesh. A part
// boundary resets the remap, so no index can address an earlier GPU buffer.
std::vector<terrain_mesh_data> merge(const std::vector<const terrain_mesh_data*>&sources){
 std::vector<terrain_mesh_data> parts;size_t remaining_vertices=0,remaining_indices=0;for(const auto*source:sources){remaining_vertices+=source->vertices.size();remaining_indices+=source->indices.size();}parts.reserve((remaining_vertices+59999)/60000);
 for(const auto*source:sources){std::vector<int32_t> remap(source->vertices.size(),-1);
  for(size_t i=0;i+2<source->indices.size();i+=3){int needed=0;for(int j=0;j<3;++j)needed+=remap.at(source->indices[i+j])<0;
   if(parts.empty()||parts.back().vertices.size()+needed>60000){parts.emplace_back();parts.back().vertices.reserve(std::min<size_t>(remaining_vertices,60000));parts.back().indices.reserve(std::min<size_t>(remaining_indices,180000));std::fill(remap.begin(),remap.end(),-1);}
   auto&part=parts.back();for(int j=0;j<3;++j){uint16_t old=source->indices[i+j];int32_t&index=remap.at(old);if(index<0){index=static_cast<int32_t>(part.vertices.size());part.vertices.push_back(source->vertices.at(old));}part.indices.push_back(static_cast<uint16_t>(index));}
  }
 }return parts;
}
struct EdgeSegment {float first,last;terrain_vertex vertex;};
std::vector<EdgeSegment> edge_segments(const terrain_mesh_data&mesh,int axis,float boundary){
 std::vector<EdgeSegment> result;const int along=axis==0?2:0;
 for(size_t i=0;i+2<mesh.indices.size();i+=3){std::vector<terrain_vertex> edge;
  for(int j=0;j<3;++j){const auto&v=mesh.vertices.at(mesh.indices[i+j]);if(v.normal[1]>.99f&&std::abs(v.position[axis]-boundary)<.0001f)edge.push_back(v);}
  if(edge.size()<2)continue;float first=edge[0].position[along],last=edge[1].position[along];if(first>last)std::swap(first,last);if(last-first>1e-5f)result.push_back({first,last,edge[0]});
 }return result;
}
terrain_mesh_data stitch(const terrain_mesh_data&a,const terrain_mesh_data&b,int axis,float boundary,float floor){
 terrain_mesh_data result;const int along=axis==0?2:0;auto ae=edge_segments(a,axis,boundary),be=edge_segments(b,axis,boundary);std::vector<float> breaks;
 for(const auto&segments:{ae,be})for(const auto&s:segments){breaks.push_back(s.first);breaks.push_back(s.last);}std::sort(breaks.begin(),breaks.end());breaks.erase(std::unique(breaks.begin(),breaks.end(),[](float x,float y){return std::abs(x-y)<1e-5f;}),breaks.end());
 auto at=[](const std::vector<EdgeSegment>&segments,float p)->const terrain_vertex*{for(const auto&s:segments)if(p>s.first-1e-5f&&p<s.last+1e-5f)return &s.vertex;return nullptr;};
 for(size_t i=1;i<breaks.size();++i){float middle=(breaks[i-1]+breaks[i])*.5f;const auto*av=at(ae,middle);const auto*bv=at(be,middle);if(!av&&!bv)continue;
  float ay=av?av->position[1]:floor,by=bv?bv->position[1]:floor;if(std::abs(ay-by)<1e-4f)continue;
  terrain_vertex va=av?*av:*bv,vb=bv?*bv:*av;if(!av){va.position[1]=floor;va.editor_palette[0]=.5f/24.f;}if(!bv){vb.position[1]=floor;vb.editor_palette[0]=.5f/24.f;}
  // All endpoints lie on the shared chunk plane.
  va.position[axis]=boundary;vb.position[axis]=boundary;va.position[along]=breaks[i-1];vb.position[along]=breaks[i-1];terrain_vertex vc=va,vd=vb;vc.position[along]=breaks[i];vd.position[along]=breaks[i];
  // The skirt is the upper terrace's cliff. Both edges of each face must use
  // its identity even when the adjacent preserved mesh belongs to another top.
  const uint32_t outline=ay>by?va.outline_id:vb.outline_id;
  va.outline_id=vb.outline_id=vc.outline_id=vd.outline_id=outline;
  std::array<float,3> normal={{0,0,0}};normal[axis]=ay>by?1.f:-1.f;va.normal=vb.normal=vc.normal=vd.normal=normal;
  // Winding depends on plane axis and which neighbor has the higher surface.
  bool flip=(axis==0?ay>by:ay<by);if(flip){append_triangle(result,va,vc,vb);append_triangle(result,vc,vd,vb);}else{append_triangle(result,va,vb,vc);append_triangle(result,vc,vb,vd);}
 }return result;
}
}
int TerrainEditorRenderCache::chunk_count()const{return (state_->level.terrain.config.resolution+15)/16;}
void TerrainEditorRenderCache::preserve_seed_palette(terrain_mesh_data& mesh,int chunk)const{
 auto found=seed_top_triangles_.find(chunk);if(found==seed_top_triangles_.end())return;
 for(auto&v:mesh.vertices){if(v.normal[1]<.99f)continue;
  for(size_t i:found->second){const auto&a=seed_terrain_.vertices[seed_terrain_.indices[i]];const auto&b=seed_terrain_.vertices[seed_terrain_.indices[i+1]];const auto&c=seed_terrain_.vertices[seed_terrain_.indices[i+2]];
   if(std::abs(a.position[1]-v.position[1])>.0001f)continue;
   const float bx=b.position[0]-a.position[0],bz=b.position[2]-a.position[2],cx=c.position[0]-a.position[0],cz=c.position[2]-a.position[2],px=v.position[0]-a.position[0],pz=v.position[2]-a.position[2];
   const float determinant=bx*cz-bz*cx;if(std::abs(determinant)<1e-8f)continue;
   const float wb=(px*cz-pz*cx)/determinant,wc=(bx*pz-bz*px)/determinant;
   if(wb<-.00001f||wc<-.00001f||wb+wc>1.00001f)continue;
   v.editor_palette[0]=a.editor_palette[0]+wb*(b.editor_palette[0]-a.editor_palette[0])+wc*(c.editor_palette[0]-a.editor_palette[0]);break;
  }
 }
}
void TerrainEditorRenderCache::initialize(const terrain_mesh_data&terrain,const std::vector<terrain_mesh_data>&grass){
 if(!state_)throw std::invalid_argument("Missing editor state");chunks_.clear();canonical_.clear();skirts_.clear();part_counts_.clear();initial_flush_=true;load_generation_=state_->load_generation;
 const auto&config=state_->level.terrain.config;const int count=chunk_count();const float width=16.f*config.world_size/config.resolution,half=config.world_size*.5f;
 seed_terrain_=terrain;seed_top_triangles_.clear();
 auto coordinate=[&](float p){return std::max(0,std::min(count-1,static_cast<int>(std::floor((p+half)/width))));};
 for(size_t i=0;i+2<terrain.indices.size();i+=3){const auto&a=terrain.vertices.at(terrain.indices[i]);const auto&b=terrain.vertices.at(terrain.indices[i+1]);const auto&c=terrain.vertices.at(terrain.indices[i+2]);
  const int x0=coordinate(std::min({a.position[0],b.position[0],c.position[0]})),x1=coordinate(std::max({a.position[0],b.position[0],c.position[0]})),z0=coordinate(std::min({a.position[2],b.position[2],c.position[2]})),z1=coordinate(std::max({a.position[2],b.position[2],c.position[2]}));
  for(int z=z0;z<=z1;++z)for(int x=x0;x<=x1;++x){auto polygon=clip(clip(clip(clip({a,b,c},0,x*width-half,true),0,std::min(config.world_size,(x+1)*width)-half,false),2,z*width-half,true),2,std::min(config.world_size,(z+1)*width)-half,false);
   if(a.normal[1]>.99f&&b.normal[1]>.99f&&c.normal[1]>.99f&&polygon.size()>=3)seed_top_triangles_[z*count+x].push_back(i);
   auto&mesh=chunks_[z*count+x].terrain;for(size_t j=1;j+1<polygon.size();++j)append_triangle(mesh,polygon[0],polygon[j],polygon[j+1]);
  }
 }
 // Existing pixel grass cards are four consecutive vertices and six indices.
 for(const auto&source:grass){for(size_t base=0;base+3<source.vertices.size();base+=4){const auto&root=source.vertices[base].texcoord;int x=coordinate(root[0]),z=coordinate(root[1]);auto&batches=chunks_[z*count+x].grass;if(batches.empty()||batches.back().vertices.size()+4>60000)batches.emplace_back();auto&mesh=batches.back();uint16_t target=static_cast<uint16_t>(mesh.vertices.size());for(size_t j=0;j<4;++j)mesh.vertices.push_back(source.vertices[base+j]);for(uint16_t j:{0,1,2,1,3,2})mesh.indices.push_back(static_cast<uint16_t>(target+j));}}
}
std::vector<EditorRenderBatch> TerrainEditorRenderCache::rebuild(TerrainEditorDirty dirty){
 using Clock=std::chrono::steady_clock;auto start=Clock::now();profile_={};std::vector<EditorRenderBatch> result;if(!state_)return result;bool fallback=false;
 if(load_generation_!=state_->load_generation){
  for(const auto&entry:part_counts_)for(size_t i=0;i<entry.second;++i){EditorRenderBatch batch;batch.kind=static_cast<EditorRenderBatch::Kind>(entry.first.first);batch.key=std::to_string(entry.first.first)+"_"+std::to_string(entry.first.second)+"_"+std::to_string(i);result.push_back(std::move(batch));}
  std::pair<terrain_mesh_data,std::vector<terrain_mesh_data>> seed;if(seed_builder_)seed=seed_builder_(state_->level.terrain.config);fallback=seed.first.indices.empty();initialize(seed.first,seed.second);dirty.x0=dirty.z0=0;dirty.x1=dirty.z1=state_->level.terrain.config.resolution-1;dirty.terrain=dirty.grass=dirty.trees=dirty.climate=true;
 }
 auto mesh_start=Clock::now();profile_.seed_ms=std::chrono::duration<double,std::milli>(mesh_start-start).count();
 const int count=chunk_count(),groups=(count+3)/4;std::set<int> terrain_groups,grass_groups,tree_groups;
 if(initial_flush_){for(int i=0;i<groups*groups;++i){terrain_groups.insert(i);grass_groups.insert(i);tree_groups.insert(i);}initial_flush_=false;}
 if(!dirty.empty()){
  const int n=state_->level.terrain.config.resolution;
  // Include the contour filter and neighboring triangles on chunk boundaries.
  // Building removal must also restore decorations just outside its footprint,
  // including across a render-group boundary. Placement already clears cells.
  const float edit_cell=state_->level.terrain.config.world_size/n;
  const int rock_halo=dirty.terrain?static_cast<int>(std::ceil((1.5f+3.f*edit_cell)/edit_cell)):0;
  const int halo=std::max({terrain_editor_rebuild_halo,rock_halo,dirty.buildings?static_cast<int>(std::ceil(12.f/edit_cell)):0});
  int x0=std::max(0,dirty.x0-halo)/16,z0=std::max(0,dirty.z0-halo)/16,x1=std::min(n-1,dirty.x1+halo)/16,z1=std::min(n-1,dirty.z1+halo)/16;
  for(int z=z0;z<=z1;++z)for(int x=x0;x<=x1;++x){int group=(z/4)*groups+x/4;
   const size_t id=static_cast<size_t>(z)*count+x;
   const bool terrain_now=dirty.terrain&&(fallback||state_->level.terrain_remeshed.empty()||(id<state_->level.terrain_remeshed.size()&&state_->level.terrain_remeshed[id]));
   const bool grass_now=dirty.grass&&(fallback||state_->level.grass_remeshed.empty()||(id<state_->level.grass_remeshed.size()&&state_->level.grass_remeshed[id]));
   if(terrain_now||dirty.climate||grass_now){
    auto&old=chunks_[z*count+x];
    if(terrain_now){++profile_.chunks;old.terrain=build_editor_chunk(state_->level,x,z,false).terrain;preserve_seed_palette(old.terrain,z*count+x);canonical_.insert(z*count+x);terrain_groups.insert(group);}
    if(dirty.climate||terrain_now){const auto&config=state_->level.terrain.config;for(auto&v:old.terrain.vertices){int cx=std::max(0,std::min(n-1,static_cast<int>((v.position[0]/config.world_size+.5f)*n))),cz=std::max(0,std::min(n-1,static_cast<int>((v.position[2]/config.world_size+.5f)*n)));size_t cell=static_cast<size_t>(cz)*n+cx;uint8_t climate=cell<state_->level.climate.size()?state_->level.climate[cell]:0;v.editor_palette[1]=(std::min(6,static_cast<int>(climate))+.5f)/7.f;}terrain_groups.insert(group);}
    if(grass_now||terrain_now){if(!terrain_now)++profile_.chunks;old.grass=build_editor_grass(state_->level,old.terrain);grass_groups.insert(group);}
   }
   if(dirty.trees||dirty.terrain||dirty.buildings)tree_groups.insert(group);
  }
  if(dirty.terrain||dirty.climate){std::set<std::pair<int,int>> edges;for(int z=z0;z<=z1;++z)for(int x=x0;x<=x1;++x){int id=z*count+x;for(int neighbor:{x>0?id-1:-1,x+1<count?id+1:-1,z>0?id-count:-1,z+1<count?id+count:-1})if(neighbor>=0)edges.insert(std::minmax(id,neighbor));}
   const auto&config=state_->level.terrain.config;float width=16.f*config.world_size/config.resolution;
   for(const auto&edge:edges){int a=edge.first,b=edge.second,axis=b-a==1?0:2;float boundary=((axis==0?b%count:b/count)*width)-config.world_size*.5f;
    if(canonical_.count(a)!=canonical_.count(b)){auto mesh=stitch(chunks_[a].terrain,chunks_[b].terrain,axis,boundary,editor_level_height(config,-8));if(mesh.indices.empty())skirts_.erase(edge);else skirts_[edge]=std::move(mesh);}else skirts_.erase(edge);
    terrain_groups.insert(((a/count)/4)*groups+(a%count)/4);
   }
  }
 }
 auto batch_start=Clock::now();profile_.mesh_ms=std::chrono::duration<double,std::milli>(batch_start-mesh_start).count();
 auto publish=[&](EditorRenderBatch::Kind kind,int group,const std::vector<const terrain_mesh_data*>&sources){
  auto parts=merge(sources);auto key=std::make_pair(static_cast<int>(kind),group);size_t old=part_counts_[key],total=std::max(old,parts.size());part_counts_[key]=parts.size();
  for(size_t i=0;i<total;++i){EditorRenderBatch batch;batch.kind=kind;batch.key=std::to_string(static_cast<int>(kind))+"_"+std::to_string(group)+"_"+std::to_string(i);if(i<parts.size())batch.mesh=std::move(parts[i]);result.push_back(std::move(batch));}
 };
 for(int group:terrain_groups){std::vector<const terrain_mesh_data*>sources;int gx=group%groups,gz=group/groups;for(int z=gz*4;z<std::min(count,gz*4+4);++z)for(int x=gx*4;x<std::min(count,gx*4+4);++x){auto found=chunks_.find(z*count+x);if(found!=chunks_.end())sources.push_back(&found->second.terrain);}for(const auto&entry:skirts_){int a=entry.first.first;if(((a/count)/4)*groups+(a%count)/4==group)sources.push_back(&entry.second);}publish(EditorRenderBatch::Terrain,group,sources);}
 for(int group:grass_groups){std::vector<const terrain_mesh_data*>sources;int gx=group%groups,gz=group/groups;for(int z=gz*4;z<std::min(count,gz*4+4);++z)for(int x=gx*4;x<std::min(count,gx*4+4);++x){auto found=chunks_.find(z*count+x);if(found!=chunks_.end())for(auto&mesh:found->second.grass)sources.push_back(&mesh);}publish(EditorRenderBatch::Grass,group,sources);}
 const auto&config=state_->level.terrain.config;float group_width=64.f*config.world_size/config.resolution;
 for(int group:tree_groups){std::vector<terrain_mesh_data>trees;const float x0=(group%groups)*group_width-config.world_size*.5f,z0=(group/groups)*group_width-config.world_size*.5f;trees.push_back(build_editor_rocks(state_->level,x0,z0,x0+group_width,z0+group_width));for(const auto&tree:state_->level.trees){int gx=std::max(0,std::min(groups-1,static_cast<int>((tree.position[0]+config.world_size*.5f)/group_width))),gz=std::max(0,std::min(groups-1,static_cast<int>((tree.position[2]+config.world_size*.5f)/group_width)));if(gz*groups+gx==group)trees.push_back(build_editor_tree(tree,config.seed));}std::vector<const terrain_mesh_data*>sources;for(auto&mesh:trees)sources.push_back(&mesh);publish(EditorRenderBatch::Trees,group,sources);}
 profile_.batch_ms=std::chrono::duration<double,std::milli>(Clock::now()-batch_start).count();profile_.batches=result.size();for(const auto&batch:result)profile_.vertices+=batch.mesh.vertices.size();return result;
}
std::vector<terrain_mesh_data> TerrainEditorRenderCache::terrain_parts()const{std::vector<terrain_mesh_data> result;result.reserve(chunks_.size());for(const auto&entry:chunks_)if(!entry.second.terrain.indices.empty())result.push_back(entry.second.terrain);return result;}
std::vector<std::array<float,4>> TerrainEditorRenderCache::active_tree_blobs()const{std::vector<std::array<float,4>> result;for(const auto&tree:state_->level.trees){float radius=tree.type==TreeType::Oak?3.f:tree.type==TreeType::Palm?3.3f:tree.type==TreeType::Birch?1.5f:2.f;result.push_back({{tree.position[0],tree.position[1]+.01f,tree.position[2],radius*tree.scale}});}return result;}
}
