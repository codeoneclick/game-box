#include "terrain_lod_renderer.h"
#include "shape_3d.h"
#include "mesh_3d.h"
#include "ces_geometry_3d_component.h"
#include "ces_shader_uniforms_component.h"
#include "ces_material_component.h"
#include "material.h"
#include "ocean_shader_uniforms.h"
#include <map>
#include <set>
#include <sstream>
#include <chrono>
#include <iomanip>
#include <cmath>
namespace game {
struct TerrainLodRenderer::implementation {
 struct Cached {std::shared_ptr<const TerrainLodVariant> variant;gb::mesh_3d_shared_ptr mesh;size_t bytes=0;uint64_t used=0;};
 struct Root {std::shared_ptr<const TerrainLodGeneration> generation;gb::shape_3d_shared_ptr source,mask;TerrainLodSelection selected;};
 Upload upload;Create create;Destroy destroy;Interaction interaction;Retire retire;Casters casters;
 std::map<std::string,Root> active,pending;
 std::map<const TerrainLodVariant*,Cached> cache;
 std::map<const TerrainLodVariant*,std::shared_ptr<const TerrainLodVariant>> debug_variants;
 std::vector<gb::shape_3d_shared_ptr> pool;
 uint64_t tick=0;size_t bytes=0,source_triangles=0,triangles=0,culled=0,tested=0,uploads=0;
 size_t base_vertices=0,active_vertices=0,base_sides=0,sides=0,base_top=0,top=0,changed=0,draws=0;
 float max_error_px=0,max_budget_px=0;
 TerrainLodView last_view;bool selection_valid=false,last_wireframe=false;int last_debug=0;
 std::array<size_t,8> histogram={};float actual_percent=0,requested_percent=0;double selection_ms=0,upload_ms=0,build_ms=0;
 gb::mesh_3d_shared_ptr mesh(std::shared_ptr<const TerrainLodVariant> variant,const std::string& name,bool allow_upload){
  auto found=cache.find(variant.get());if(found!=cache.end()){found->second.used=tick;return found->second.mesh;}
  if(!allow_upload)return nullptr;
  const auto started=std::chrono::steady_clock::now();Cached item;item.variant=variant;item.mesh=upload(*variant,name);item.bytes=variant->mesh.vertices.size()*32+variant->mesh.indices.size()*2;item.used=tick;bytes+=item.bytes;cache.emplace(variant.get(),item);++uploads;upload_ms=std::chrono::duration<double,std::milli>(std::chrono::steady_clock::now()-started).count();return item.mesh;
 }
 std::shared_ptr<const TerrainLodVariant> display(std::shared_ptr<const TerrainLodVariant> variant,int mode,float cell){
  if(mode<4)return variant;
  auto it=debug_variants.find(variant.get());if(it!=debug_variants.end())return it->second;
  auto overlay=std::make_shared<const TerrainLodVariant>(build_terrain_contour_debug(*variant,mode,cell*.2f));debug_variants[variant.get()]=overlay;return overlay;
 }
 void collect(){
  const size_t budget=64u*1024u*1024u;
  while(bytes>budget){auto oldest=cache.end();for(auto it=cache.begin();it!=cache.end();++it)if(it->second.mesh.use_count()==1&&it->second.used+12<tick&&(oldest==cache.end()||it->second.used<oldest->second.used))oldest=it;
   if(oldest==cache.end())break;bytes-=oldest->second.bytes;retire(oldest->second.mesh);cache.erase(oldest);
  }
 }
};
TerrainLodRenderer::TerrainLodRenderer(Upload upload,Create create,Destroy destroy,Interaction interaction,Retire retire,Casters casters):impl_(std::make_shared<implementation>()){
 impl_->upload=std::move(upload);impl_->create=std::move(create);impl_->destroy=std::move(destroy);impl_->interaction=std::move(interaction);impl_->retire=std::move(retire);impl_->casters=std::move(casters);
}
void TerrainLodRenderer::stage(const std::string& key,std::shared_ptr<const TerrainLodGeneration> generation){
 auto& s=*impl_;implementation::Root root;root.generation=generation;
 bool stale=false;if(generation)for(auto& entry:s.pending)stale|=entry.second.generation&&!terrain_lod_version_current(entry.second.generation->version,generation->version);
 if(stale)s.pending.clear();
 if(generation&&!generation->nodes.empty())for(size_t i=0;i<generation->nodes[0].levels.size();++i)s.mesh(generation->nodes[0].levels[i],"island.terrain.lod."+key+".root."+std::to_string(i),true);
 s.pending[key]=std::move(root);
}
bool TerrainLodRenderer::publish_ready(const TerrainLodVersion& version){
 auto& s=*impl_;if(s.pending.empty())return false;
 for(auto& entry:s.pending)if(entry.second.generation&&!terrain_lod_version_current(entry.second.generation->version,version))return false;
 for(auto it=s.active.begin();it!=s.active.end();)if(it->second.generation->version.load_generation!=version.load_generation){s.destroy(it->second.source);s.destroy(it->second.mask);it=s.active.erase(it);}else ++it;
 for(auto& entry:s.pending){auto old=s.active.find(entry.first);
  if(old!=s.active.end()&&entry.second.generation&&!entry.second.generation->nodes.empty()&&old->second.generation->nodes[0].levels[0]==entry.second.generation->nodes[0].levels[0]){
   old->second.generation=entry.second.generation;continue;
  }
  if(old!=s.active.end()){s.destroy(old->second.source);s.destroy(old->second.mask);s.active.erase(old);}
  auto& root=entry.second;if(!root.generation||root.generation->nodes.empty())continue;
  root.source=s.create("island.terrain.lod.water.source."+entry.first,1);root.mask=s.create("island.terrain.lod.water.mask."+entry.first,2);
  auto mesh=s.mesh(root.generation->nodes[0].levels[0],entry.first,true);root.source->get_component<gb::ces_geometry_3d_component>()->set_mesh(mesh);root.mask->get_component<gb::ces_geometry_3d_component>()->set_mesh(mesh);
  s.interaction(root.source,root.mask);s.active[entry.first]=std::move(root);
 }
 s.pending.clear();s.selection_valid=false;s.debug_variants.clear();std::vector<TerrainShadowCaster> casters;
 for(auto& entry:s.active){TerrainShadowCaster caster;caster.id=entry.first;auto& levels=entry.second.generation->nodes[0].levels;for(size_t i=0;i<levels.size();++i)caster.levels.push_back({s.mesh(levels[i],entry.first,true),levels[i]->world_error});casters.push_back(std::move(caster));}
 s.build_ms=0;s.requested_percent=0;for(auto& entry:s.active){s.build_ms+=entry.second.generation->build_ms;s.requested_percent=std::max(s.requested_percent,entry.second.generation->nodes[0].levels.back()->requested_percent);}
 s.casters(casters);return true;
}
void TerrainLodRenderer::update_main(const TerrainLodView& view,bool wireframe,int debug_mode){
 const auto started=std::chrono::steady_clock::now();auto& state=*impl_;
 if(state.selection_valid&&state.last_wireframe==wireframe&&state.last_debug==debug_mode&&state.last_view.camera==view.camera&&state.last_view.view_projection==view.view_projection&&state.last_view.projection_scale==view.projection_scale&&state.last_view.near_error_px==view.near_error_px&&state.last_view.normal_error_px==view.normal_error_px&&state.last_view.far_error_px==view.far_error_px){
  ++state.tick;state.changed=0;state.selection_ms=std::chrono::duration<double,std::milli>(std::chrono::steady_clock::now()-started).count();return;
 }
 if(state.last_debug!=debug_mode)state.debug_variants.clear();
 state.last_view=view;state.last_wireframe=wireframe;state.last_debug=debug_mode;state.selection_valid=true;
 state.base_vertices=state.active_vertices=state.base_sides=state.sides=state.base_top=state.top=state.changed=state.draws=0;
 state.max_error_px=state.max_budget_px=0;
 auto& s=*impl_;++s.tick;s.source_triangles=s.triangles=s.culled=s.tested=0;s.histogram={};s.actual_percent=0;
 const auto start=std::chrono::steady_clock::now();size_t next_shape=0,new_uploads=0;
 for(auto& shape:s.pool){shape->visible=false;shape->visible_in_next_frame=false;}
 for(auto& entry:s.active){auto& root=entry.second;const auto& g=*root.generation;s.source_triangles+=g.source_triangles;
  s.base_vertices+=g.source->vertices.size();s.base_sides+=g.source_sides;s.base_top+=g.source_top;
  auto selection=select_terrain_lod(g,view,&root.selected);s.culled+=selection.culled;s.tested+=selection.tested;bool ready=true;
  for(auto item:selection.items){auto variant=s.display(g.nodes[item.node].levels[item.level],debug_mode,g.cell_size);const bool absent=s.cache.find(variant.get())==s.cache.end();auto mesh=s.mesh(variant,entry.first+"."+std::to_string(item.node)+"."+std::to_string(item.level),new_uploads<1&&s.bytes<64u*1024u*1024u);if(!mesh){ready=false;break;}if(absent)++new_uploads;}
  if(!ready){s.selection_valid=false;selection.items.clear();TerrainLodItem item;selection.items.push_back(item);selection.triangles=g.nodes[0].levels[0]->mesh.indices.size()/3;}
  if(selection.items!=root.selected.items)++s.changed;
  root.selected=selection;
  for(auto item:selection.items){auto variant=s.display(g.nodes[item.node].levels[item.level],debug_mode,g.cell_size);auto mesh=s.mesh(variant,entry.first,false);
   if(!mesh){variant=g.nodes[0].levels[0];mesh=s.mesh(variant,entry.first,false);}if(!mesh)continue;
   if(next_shape==s.pool.size())s.pool.push_back(s.create("island.terrain.lod.main."+std::to_string(next_shape),0));auto shape=s.pool[next_shape++];auto geometry=shape->get_component<gb::ces_geometry_3d_component>();if(geometry->get_mesh()!=mesh){if(auto old=geometry->get_mesh())s.retire(old->as_3d());geometry->set_mesh(mesh);}
   float distance2=0;const auto& node=g.nodes[item.node];for(int k=0;k<3;++k){float d=std::max(0.f,std::max(node.minimum[k]-view.camera[k],view.camera[k]-node.maximum[k]));distance2+=d*d;}
   const float distance=std::max(.001f,std::sqrt(distance2)),budget=terrain_lod_allowed_error(view,distance,g.cell_size);
   const float error=(variant->world_error+(variant->next_world_error-variant->world_error)*item.morph)*view.projection_scale/distance;
   s.max_error_px=std::max(s.max_error_px,error);s.max_budget_px=std::max(s.max_budget_px,budget);
   for(auto mode:{gb::ces_shader_uniforms_component::e_shader_uniform_mode::e_vertex,gb::ces_shader_uniforms_component::e_shader_uniform_mode::e_fragment}){
    auto u=std::static_pointer_cast<ocean_shader_uniforms>(shape->get_component<gb::ces_shader_uniforms_component>()->get_uniforms(mode,"ocean",mode==gb::ces_shader_uniforms_component::e_shader_uniform_mode::e_vertex?2:0));
    if(u)u->set_terrain_lod(item.morph,item.morph>0,debug_mode,debug_mode==3?error/budget:variant->actual_percent/100.f);
   }
   shape->get_component<gb::ces_material_component>()->get_material("ws.base")->set_wireframe(wireframe||debug_mode==2);
   s.active_vertices+=variant->mesh.vertices.size();s.sides+=variant->side_triangles;s.top+=variant->top_triangles;++s.draws;
   shape->visible=true;shape->visible_in_next_frame=true;s.triangles+=variant->mesh.indices.size()/3;++s.histogram[std::min(7,item.level)];s.actual_percent=std::max(s.actual_percent,variant->actual_percent);
  }
 }
 for(size_t i=next_shape;i<s.pool.size();++i){auto geometry=s.pool[i]->get_component<gb::ces_geometry_3d_component>();if(auto old=geometry->get_mesh()){s.retire(old->as_3d());geometry->set_mesh(nullptr);}}
 s.collect();s.selection_ms=std::chrono::duration<double,std::milli>(std::chrono::steady_clock::now()-start).count();
}
std::string TerrainLodRenderer::diagnostics()const{
 const auto& s=*impl_;std::ostringstream text;text<<std::fixed<<std::setprecision(2)<<"Terrain "<<s.source_triangles<<" -> "<<s.triangles<<" tris ("<<(s.source_triangles?100.-100.*s.triangles/s.source_triangles:0)<<"%)\nNodes "<<s.tested<<" | culled "<<s.culled<<" | LOD:";for(size_t i=0;i<s.histogram.size();++i)if(s.histogram[i])text<<" "<<i<<":"<<s.histogram[i];text<<"\nSide "<<s.base_sides<<" -> "<<s.sides<<" | Top "<<s.base_top<<" -> "<<s.top<<"\nVertices "<<s.base_vertices<<" -> "<<s.active_vertices<<" | Draws "<<s.draws<<" | changed "<<s.changed<<"\nSelect "<<s.selection_ms<<" ms | GPU cache "<<s.bytes/1048576<<" MiB | uploads "<<s.uploads<<"\nError "<<s.max_error_px<<" px | budget <= "<<s.max_budget_px<<" px | GPU ms: probe";return text.str();
}
size_t TerrainLodRenderer::selected_triangles()const{return impl_->triangles;}
}
