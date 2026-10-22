#include "ces_sun_shadow_system.h"
#include "sun_shadow_math.h"
#include "stylized_lighting_settings.h"
#include "scene_fabricator.h"
#include "shape_3d.h"
#include "mesh_3d.h"
#include "camera_3d.h"
#include "ces_geometry_3d_component.h"
#include "ces_material_component.h"
#include "ces_transformation_3d_component.h"
#include "ces_shader_uniforms_component.h"
#include "ces_render_technique_uniforms_component.h"
#include <array>
#include <map>
#include <deque>
#include <cstdlib>
#include <iostream>
#include <cstring>
#include <stdexcept>
namespace game { namespace {
using Mode=gb::ces_shader_uniforms_component::e_shader_uniform_mode;
class caster_uniforms final:public gb::ces_shader_uniforms_component::shader_uniforms {
 struct values {glm::mat4 matrix;glm::vec4 cast;} data_;
 struct skin_values {glm::mat4 bones[64];values caster;} skin_;
 std::shared_ptr<shader_uniforms> bones_;
public:
 caster_uniforms(Mode mode,const std::string& name):shader_uniforms(mode,name){
  m_uniforms["shadow_mvp"]=std::make_shared<gb::shader_uniform>(gb::e_uniform_type_mat4);
  m_uniforms["shadow_cast"]=std::make_shared<gb::shader_uniform>(gb::e_uniform_type_vec4);
 }
 void update(const glm::mat4& m,float time,float building){data_={m,glm::vec4(time,building,0,0)};set(m,"shadow_mvp");set(data_.cast,"shadow_cast");}
 void bind_bones(const std::shared_ptr<shader_uniforms>& source){bones_=source;m_uniforms["u_bones"]=source->get_uniforms_ref().at("u_bones");}
 void* get_values()override{if(!bones_)return &data_;std::memcpy(skin_.bones,bones_->get_values(),sizeof(skin_.bones));skin_.caster=data_;return &skin_;}
 ui32 get_values_size()override{return bones_?sizeof(skin_):sizeof(data_);}
};
class resolve_uniforms final:public gb::ces_render_technique_uniforms_component::shader_uniforms {
 struct values {glm::mat4 near_matrix,far_matrix;glm::vec4 sun,params;} data_;
public:
 explicit resolve_uniforms(gb::ces_render_technique_uniforms_component::e_shader_uniform_type type):shader_uniforms(type){
  for(auto key:{"shadow_near","shadow_far"})m_uniforms[key]=std::make_shared<gb::shader_uniform>(gb::e_uniform_type_mat4);
  for(auto key:{"shadow_sun","shadow_params"})m_uniforms[key]=std::make_shared<gb::shader_uniform>(gb::e_uniform_type_vec4);
 }
 void update(const std::array<glm::mat4,2>& m,glm::vec3 sun,bool enabled,int debug,float far_width,float depth_half_extent,const std::array<glm::ivec2,2>& sizes){data_={m[0],m[1],glm::vec4(sun,enabled?1.f:0.f),glm::vec4(96.f/std::min(sizes[0].x,sizes[0].y),far_width/std::min(sizes[1].x,sizes[1].y),depth_half_extent*2.f,float(debug))};set(m[0],"shadow_near");set(m[1],"shadow_far");set(data_.sun,"shadow_sun");set(data_.params,"shadow_params");}
 void* get_values()override{return &data_;}ui32 get_values_size()override{return sizeof(data_);}
};
}
struct ces_sun_shadow_system::implementation {
 struct entry {gb::ces_entity_weak_ptr source;std::array<gb::shape_3d_shared_ptr,2> proxy;std::array<std::shared_ptr<caster_uniforms>,2> uniforms;bool seen=false;};
 gb::scene_fabricator_shared_ptr fabricator;
 std::map<const gb::ces_entity*,entry> entries;
 std::map<std::string,entry> terrain_entries;
 std::vector<TerrainShadowCaster> terrain_sources;
 std::shared_ptr<resolve_uniforms> resolve;
 std::array<glm::mat4,2> matrices;
 std::array<glm::ivec2,2> map_sizes;
 bool enabled=false;
 float far_width=800.f,depth_half_extent=800.f;
 std::deque<std::pair<int,gb::mesh_3d_shared_ptr>> retired;
 explicit implementation(const gb::scene_fabricator_shared_ptr& f):fabricator(f){}
 void retire(const gb::shape_3d_shared_ptr& p){auto g=p->get_component<gb::ces_geometry_3d_component>();if(auto mesh=g->get_mesh())retired.push_back({12,mesh->as_3d()});g->set_mesh(nullptr);}
};
ces_sun_shadow_system::ces_sun_shadow_system(const gb::scene_fabricator_shared_ptr& f,const std::array<glm::ivec2,2>& sizes):impl_(std::make_shared<implementation>(f)){
 for(const auto& size:sizes)if(size.x<=0||size.y<=0)throw std::invalid_argument("Missing sun shadow map texture");
 impl_->map_sizes=sizes;
 set_order(6); // All simulation feeds finish before this phase; render ends at 7.
 add_required_component_guid(mask_,gb::ces_transformation_component::class_guid());
 add_required_component_guid(mask_,gb::ces_geometry_component::class_guid());
 add_required_component_guid(mask_,gb::ces_material_component::class_guid());add_required_components_mask(mask_);
}
ces_sun_shadow_system::~ces_sun_shadow_system()=default;
void ces_sun_shadow_system::set_terrain_casters(const std::vector<TerrainShadowCaster>& sources){impl_->terrain_sources=sources;}
void ces_sun_shadow_system::set_world_dimensions(float world_size){
 if(!std::isfinite(world_size)||world_size<=0||!std::isfinite(world_size*2.f))throw std::invalid_argument("Invalid shadow world size");
 impl_->far_width=world_size;impl_->depth_half_extent=std::max(800.f,world_size);
}
void ces_sun_shadow_system::initialize(const gb::ces_entity_shared_ptr& root){
 root_=root;
 impl_->resolve=root->get_component<gb::ces_render_technique_uniforms_component>()->construct_uniforms<resolve_uniforms>(gb::ces_render_technique_uniforms_component::e_shader_uniform_type_fragment,"ss.sunshadow");
 const auto d=stylized_lighting_settings().sun_direction();glm::vec3 sun(d[0],d[1],d[2]);
 const std::array<glm::mat4,2> m={{make_sun_shadow_matrix(sun,glm::vec3(0,6,0),96,impl_->map_sizes[0],impl_->depth_half_extent),make_sun_shadow_matrix(sun,glm::vec3(0,8,0),impl_->far_width,impl_->map_sizes[1],impl_->depth_half_extent)}};
 impl_->resolve->update(m,sun,false,0,impl_->far_width,impl_->depth_half_extent,impl_->map_sizes);
}
void ces_sun_shadow_system::on_feed(const gb::ces_entity_shared_ptr& root,f32){
 root_=root;auto camera=get_current_camera_3d();if(!camera)return;
 auto& s=*impl_;
 if(!s.resolve)initialize(root);
 const auto d=stylized_lighting_settings().sun_direction();const glm::vec3 sun(d[0],d[1],d[2]);
 const char* setting=std::getenv("GB_SUN_SHADOWS");bool enabled=(!setting||std::string(setting)!="0")&&sun.y>.015f;
 s.matrices={{make_sun_shadow_matrix(sun,camera->get_look_at(),96,s.map_sizes[0],impl_->depth_half_extent),make_sun_shadow_matrix(sun,glm::vec3(0,8,0),impl_->far_width,s.map_sizes[1],impl_->depth_half_extent)}};s.enabled=enabled;
 int debug=std::getenv("GB_SUN_SHADOW_DEBUG")?std::atoi(std::getenv("GB_SUN_SHADOW_DEBUG")):0;
 // The renderer COPIES scalar/matrix SS uniforms during its feed at order 7.
 // Publish here, not in end: maps and receivers must use this same-frame matrix.
 s.resolve->update(s.matrices,sun,enabled,debug,s.far_width,s.depth_half_extent,s.map_sizes);
}
void ces_sun_shadow_system::on_feed_end(f32){
 auto root=root_.lock();if(!root)return;auto& s=*impl_;
 const auto& matrices=s.matrices;const bool enabled=s.enabled;
 for(auto& r:s.retired)--r.first;while(!s.retired.empty()&&s.retired.front().first<=0)s.retired.pop_front();
 for(auto& item:s.entries)item.second.seen=false;
 for(auto& item:s.terrain_entries)item.second.seen=false;
 for(const auto& source:s.terrain_sources){
  if(source.levels.empty())continue;auto found=s.terrain_entries.find(source.id);
  if(found==s.terrain_entries.end()){
   implementation::entry entry;for(int i=0;i<2;++i){auto proxy=s.fabricator->create_shape_3d(i==0?"sun_shadow.rigid.near.xml":"sun_shadow.rigid.far.xml","plane");proxy->tag="sun.terrain.caster";proxy->get_component<gb::ces_geometry_3d_component>()->bounding_mode=gb::ces_geometry_component::e_none;
    entry.uniforms[i]=proxy->add_component<gb::ces_shader_uniforms_component>()->construct_uniforms_buffer<caster_uniforms>(Mode::e_vertex,"sun.caster",3);entry.proxy[i]=proxy;root->add_child(proxy);}
   found=s.terrain_entries.emplace(source.id,std::move(entry)).first;
  }
  auto& entry=found->second;entry.seen=true;
  for(int i=0;i<2;++i){const float texel=(i==0?96.f:s.far_width)/std::max(s.map_sizes[i].x,s.map_sizes[i].y);size_t level=0;for(size_t j=0;j<source.levels.size();++j)if(source.levels[j].second<=texel)level=j;
   auto mesh=source.levels[level].first;auto proxy=entry.proxy[i];auto geometry=proxy->get_component<gb::ces_geometry_3d_component>();if(geometry->get_mesh()!=mesh){s.retire(proxy);geometry->set_mesh(mesh);}
   entry.uniforms[i]->update(matrices[i],0,0);const bool draw=enabled&&mesh&&sun_shadow_bounds_visible(matrices[i],glm::mat4(1),mesh->get_min_bound(),mesh->get_max_bound());proxy->visible=draw;proxy->visible_in_next_frame=draw;
  }
 }
 for(auto it=s.terrain_entries.begin();it!=s.terrain_entries.end();)if(!it->second.seen){for(auto p:it->second.proxy){s.retire(p);root->remove_child(p);}it=s.terrain_entries.erase(it);}else ++it;
 // Collect first: attaching proxies must not mutate the feeder query being iterated.
 std::vector<gb::ces_entity_shared_ptr> sources;
 enumerate_entities_with_components(mask_,[&](const gb::ces_entity_shared_ptr& e){if(e->get_component<gb::ces_material_component>()->get_material("ws.base"))sources.push_back(e);});
 for(auto& source:sources){
  const std::string managed_tag=source->tag;if(managed_tag=="editor.hand_cursor"||managed_tag.find("island.terrain.lod.main.")==0)continue;
  auto material=source->get_component<gb::ces_material_component>()->get_material("ws.base");auto shader=material->get_shader();if(!shader)continue;
  const auto guid=shader->get_guid();const bool skin=guid.find("shader_island_resident")!=std::string::npos;
  const bool building=guid.find("shader_editor_building")!=std::string::npos;
  if(!skin&&!building&&guid.find("shader_island_terrain")==std::string::npos)continue;
  const std::string tag=source->tag;if(tag.find("1_")==0||tag.find("island.grass")==0)continue;
  auto source_uniforms=source->get_component<gb::ces_shader_uniforms_component>();
  auto bones=source_uniforms?source_uniforms->get_uniforms(Mode::e_vertex,"resident.bones",2):nullptr;
  if(skin&&!bones)continue;
  auto found=s.entries.find(source.get());
  if(found!=s.entries.end()&&found->second.source.lock()!=source){for(auto p:found->second.proxy){s.retire(p);root->remove_child(p);}s.entries.erase(found);found=s.entries.end();}
  if(found==s.entries.end()){
   implementation::entry item;item.source=source;
   for(int i=0;i<2;++i){
    const std::string cfg=std::string("sun_shadow.")+(skin?"skin.":"rigid.")+(i==0?"near.xml":"far.xml");
    auto p=s.fabricator->create_shape_3d(cfg,"plane");p->tag="sun.caster";p->visible=false;p->visible_in_next_frame=false;
    p->get_component<gb::ces_geometry_3d_component>()->bounding_mode=gb::ces_geometry_component::e_none;
    auto u=p->add_component<gb::ces_shader_uniforms_component>();item.uniforms[i]=u->construct_uniforms_buffer<caster_uniforms>(Mode::e_vertex,"sun.caster",skin?2:3);
    if(skin)item.uniforms[i]->bind_bones(bones);
    if(building)p->get_component<gb::ces_material_component>()->set_texture(material->get_texture(gb::e_shader_sampler_01),gb::e_shader_sampler_01);
    item.proxy[i]=p;root->add_child(p);
   }
   found=s.entries.emplace(source.get(),std::move(item)).first;
  }
  auto& item=found->second;item.seen=true;
  auto source_mesh=source->get_component<gb::ces_geometry_3d_component>()->get_mesh();
  auto mesh=source_mesh?source_mesh->as_3d():nullptr;
  const auto model=source->get_component<gb::ces_transformation_3d_component>()->get_absolute_transformation();float time=0;
  if(source_uniforms)if(auto ocean=source_uniforms->get_uniforms(Mode::e_vertex,"ocean",2)){const auto& uniforms=ocean->get_uniforms_ref();time=uniforms.at("time_level")->get_vec4().x;}
  bool visible=enabled&&source->get_is_visible()&&source->get_is_visible_in_next_frame()&&mesh;
  if(building&&source_uniforms)for(const auto& b:source_uniforms->get_fragment_buffers()){auto tint=b.second->get_uniforms_ref().find("building_tint");if(tint!=b.second->get_uniforms_ref().end()&&tint->second->get_vec4().a>.01f)visible=false;}
  for(int i=0;i<2;++i){auto p=item.proxy[i];auto geometry=p->get_component<gb::ces_geometry_3d_component>();
   if(geometry->get_mesh()!=mesh){s.retire(p);geometry->set_mesh(mesh?mesh->as_3d():nullptr);}
   p->get_component<gb::ces_transformation_3d_component>()->update_absolute_transformation(model);
   item.uniforms[i]->update(matrices[i],time,building?1.f:0.f);
   // Bulk custom-uniform binding skips arrays on Vulkan/GL, as in the visible
   // resident path. Upload the same current pose explicitly to each material.
   if(skin)p->get_component<gb::ces_material_component>()->set_custom_shader_uniform_array(static_cast<glm::mat4*>(bones->get_values()),64,"u_bones");
   if(building)if(auto texture=material->get_texture(gb::e_shader_sampler_01))p->get_component<gb::ces_material_component>()->set_texture(texture,gb::e_shader_sampler_01);
   const bool draw=visible&&(sun_shadow_bounds_visible(matrices[i],model,mesh->get_min_bound()-glm::vec3(1),mesh->get_max_bound()+glm::vec3(1)));
   p->visible=draw;p->visible_in_next_frame=draw;
  }
 }
 for(auto it=s.entries.begin();it!=s.entries.end();)if(!it->second.seen){for(auto p:it->second.proxy){s.retire(p);root->remove_child(p);}it=s.entries.erase(it);}else ++it;
}
}
