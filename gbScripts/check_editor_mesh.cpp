#include "terrain_editor_mesh.h"
#include "terrain_editor_render_cache.h"
#include "outline_ids.h"
#include <cassert>
#include <cmath>
#include <iostream>
#include <cstdio>
#include <set>
#include <algorithm>
#include <vector>
static void same_mesh(const game::terrain_mesh_data&,const game::terrain_mesh_data&);
static void check_pine_connected(const game::terrain_mesh_data& mesh){
 using Cell=std::array<int,3>;
 std::map<std::array<int,2>,std::map<int,int>> crossings;
 for(size_t i=0;i<mesh.vertices.size();i+=4){
  const auto& a=mesh.vertices[i];if(std::abs(a.normal[0])<.5f)continue;
  int y0=100,y1=-100,z0=100,z1=-100;
  for(int k=0;k<4;++k){const auto& p=mesh.vertices[i+k].position;int y=int(std::round(p[1]/.35f)),z=int(std::round(p[2]/.35f));y0=std::min(y0,y);y1=std::max(y1,y);z0=std::min(z0,z);z1=std::max(z1,z);}
  for(int y=y0;y<y1;++y)for(int z=z0;z<z1;++z)crossings[{{y,z}}][int(std::round(a.position[0]/.35f))]=a.normal[0]>0?1:-1;
 }
 std::set<Cell> occupied,visited;
 for(const auto& row:crossings){int inside=0,previous=0;for(const auto& event:row.second){if(inside)for(int x=previous;x<event.first;++x)occupied.insert({{x,row.first[0],row.first[1]}});inside-=event.second;assert(inside==0||inside==1);previous=event.first;}assert(inside==0);}
 std::vector<Cell> pending(1,Cell{{-1,0,-1}});visited.insert(pending[0]);assert(occupied.count(pending[0]));
 for(size_t i=0;i<pending.size();++i)for(int axis=0;axis<3;++axis)for(int sign:{-1,1}){auto next=pending[i];next[axis]+=sign;if(occupied.count(next)&&visited.insert(next).second)pending.push_back(next);}
 assert(visited.size()==occupied.size());
}
static void check_voxel_trees(){
 size_t maximum_pine_triangles=0;
 for(uint32_t seed=0;seed<128;++seed){game::EditorTree pine;pine.type=game::TreeType::Pine;pine.id=171;
  auto mesh=game::build_editor_tree(pine,seed);maximum_pine_triangles=std::max(maximum_pine_triangles,mesh.indices.size()/3);assert(mesh.indices.size()/3<2000);check_pine_connected(mesh);
 }
 std::cout<<"Pine reference: 128 connected variants, max triangles="<<maximum_pine_triangles<<std::endl;
 for(int species=0;species<5;++species){
  game::EditorTree tree;tree.id=171;tree.type=static_cast<game::TreeType>(species);
  auto local=game::build_editor_tree(tree,42);same_mesh(local,game::build_editor_tree(tree,42));
  assert(!local.vertices.empty()&&local.vertices.size()<65535&&local.indices.size()%6==0);
  assert(local.indices.size()/3<2000);
  float low=100,high=-100;
  for(const auto&v:local.vertices){
   low=std::min(low,v.position[1]);high=std::max(high,v.position[1]);
   assert(v.position[1]>=0&&std::abs(v.position[0])<5&&std::abs(v.position[2])<5);
   for(float value:v.position)assert(std::isfinite(value));
   for(float value:v.normal)assert(std::isfinite(value));
   assert(v.outline_id==game::outline_tree_id(tree.id));
   assert((v.texcoord==std::array<float,2>{{0,0}}));
   assert(v.tree_bend[0]>=0&&v.tree_bend[0]<=1);
   if(v.position[1]==0)assert(v.tree_bend[0]==0);
  }
  assert(low==0&&high>=6.5f&&high<=8.f);
  if(tree.type==game::TreeType::Pine){
   // The reference has a bare lower trunk and irregular branch decks, not
   // mirrored solid plates. Inspect actual exposed leaf cells in the mesh.
   float lowest_leaf=100;std::set<std::array<int,3>> leaf_surface;
   for(size_t face=0;face<local.vertices.size();face+=4){
    const auto& a=local.vertices[face];if(a.color[1]<=a.color[0])continue;
    int axis=std::abs(a.normal[0])>.5f?0:std::abs(a.normal[1])>.5f?1:2;
    int u=(axis+1)%3,w=(axis+2)%3;std::array<int,3> lo={{100,100,100}},hi={{-100,-100,-100}};
    for(int k=0;k<4;++k)for(int j=0;j<3;++j){int p=int(std::round(local.vertices[face+k].position[j]/.35f));lo[j]=std::min(lo[j],p);hi[j]=std::max(hi[j],p);}
    lowest_leaf=std::min(lowest_leaf,lo[1]*.35f);
    for(int x=lo[u];x<hi[u];++x)for(int z=lo[w];z<hi[w];++z){std::array<int,3> cell=lo;cell[axis]-=a.normal[axis]>0?1:0;cell[u]=x;cell[w]=z;leaf_surface.insert(cell);}
   }
   assert(lowest_leaf>=1.39f&&lowest_leaf<=1.76f);
   bool irregular=false;for(auto cell:leaf_surface){cell[0]=-cell[0]-1;if(!leaf_surface.count(cell)){irregular=true;break;}}
   assert(irregular);
  }
  // Height-only nonlinear wind must not encounter long vertical edges at
  // material junctions. Horizontal merges are safe because their Y is constant.
  for(size_t face=0;face<local.vertices.size();face+=4){
   float min_y=100,max_y=-100;
   for(size_t corner=0;corner<4;++corner){const auto&v=local.vertices[face+corner];min_y=std::min(min_y,v.position[1]);max_y=std::max(max_y,v.position[1]);}
   if(std::abs(local.vertices[face].normal[1])<.5f)assert(max_y-min_y<=.35001f);
  }
  // Opposite exposed rectangle faces must never overlap: wood/leaf interfaces
  // are removed as well as interfaces between identical occupied voxels.
  for(size_t a=0;a<local.vertices.size();a+=4)for(size_t b=a+4;b<local.vertices.size();b+=4){
   const auto& av=local.vertices[a];const auto& bv=local.vertices[b];
   int axis=std::abs(av.normal[0])>.5f?0:std::abs(av.normal[1])>.5f?1:2;
   if(av.normal[axis]*bv.normal[axis]>-.5f||std::abs(av.position[axis]-bv.position[axis])>.00001f)continue;
   int u=(axis+1)%3,w=(axis+2)%3;
   float overlap_u=std::min(local.vertices[a+2].position[u],local.vertices[b+2].position[u])-std::max(av.position[u],bv.position[u]);
   float overlap_w=std::min(local.vertices[a+2].position[w],local.vertices[b+2].position[w])-std::max(av.position[w],bv.position[w]);
   assert(overlap_u<.00001f||overlap_w<.00001f);
  }
  tree.position={{2,3,4}};tree.yaw=.7f;tree.scale=1.3f;
  auto transformed=game::build_editor_tree(tree,42);
  assert(transformed.indices==local.indices);
  for(size_t i=0;i<transformed.vertices.size();++i){
   const auto&v=transformed.vertices[i];const auto&p=local.vertices[i].position;
   assert(std::abs(v.position[0]-(2+1.3f*(std::cos(.7f)*p[0]+std::sin(.7f)*p[2])))<.00001f);
   assert(std::abs(v.position[1]-(3+1.3f*p[1]))<.00001f);
   assert(std::abs(v.position[2]-(4+1.3f*(-std::sin(.7f)*p[0]+std::cos(.7f)*p[2])))<.00001f);
   assert(std::abs(v.normal[0]*v.normal[0]+v.normal[1]*v.normal[1]+v.normal[2]*v.normal[2]-1)<.00001f);
   assert((v.texcoord==std::array<float,2>{{2,4}}));
  }
  // Coincident edge endpoints retain identical root, phase and bend under the
  // quadratic height warp used by the shader, including white/black bark joins.
  std::map<std::array<int,3>,std::array<float,3>> bent;
  for(const auto&v:transformed.vertices){
   std::array<int,3> key={{int(std::round(v.position[0]*10000)),int(std::round(v.position[1]*10000)),int(std::round(v.position[2]*10000))}};
   float movement=.12f*std::sin(v.texcoord[0]*.13f+v.texcoord[1]*.19f+v.tree_bend[1])*v.tree_bend[0]*v.tree_bend[0];
   std::array<float,3> warped={{v.position[0]+movement,v.position[1],v.position[2]+movement*.4f}};
   auto found=bent.find(key);if(found!=bent.end())assert(found->second==warped);else bent.emplace(key,warped);
  }
  for(size_t i=0;i<transformed.indices.size();i+=3){
   const auto&a=transformed.vertices.at(transformed.indices[i]);const auto&b=transformed.vertices.at(transformed.indices[i+1]);const auto&c=transformed.vertices.at(transformed.indices[i+2]);
   float x=b.position[0]-a.position[0],y=b.position[1]-a.position[1],z=b.position[2]-a.position[2];
   float u=c.position[0]-a.position[0],v=c.position[1]-a.position[1],w=c.position[2]-a.position[2];
   assert((y*w-z*v)*a.normal[0]+(z*u-x*w)*a.normal[1]+(x*v-y*u)*a.normal[2]>.00001f);
  }
  std::cout<<"Voxel species="<<species<<" triangles="<<local.indices.size()/3<<" height="<<high<<std::endl;
 }
}
static void same_mesh(const game::terrain_mesh_data& a,const game::terrain_mesh_data& b){
 assert(a.indices==b.indices);assert(a.vertices.size()==b.vertices.size());
 for(size_t i=0;i<a.vertices.size();++i){const auto&x=a.vertices[i];const auto&y=b.vertices[i];
  assert(x.position==y.position&&x.normal==y.normal&&x.color==y.color&&x.texcoord==y.texcoord);
  assert(x.editor_palette==y.editor_palette&&x.grass_bend==y.grass_bend&&x.tree_bend==y.tree_bend);
  assert(x.outline_id==y.outline_id);
 }
}
static double area(const game::terrain_mesh_data& mesh) {
 double result=0; for(size_t i=0;i<mesh.indices.size();i+=3){
 const auto&a=mesh.vertices.at(mesh.indices[i]);const auto&b=mesh.vertices.at(mesh.indices[i+1]);const auto&c=mesh.vertices.at(mesh.indices[i+2]);
 if(a.normal[1]>.99f)result+=std::abs((b.position[0]-a.position[0])*(c.position[2]-a.position[2])-(b.position[2]-a.position[2])*(c.position[0]-a.position[0]))*.5;
 }return result;
}
int main(){
 check_voxel_trees();
 // Exercise all 22 transport bits, including IDs beyond signed 16-bit range.
 for(uint32_t id:{0u,1000u,32768u,65535u,1048576u,(1u<<22)-1}){
  const float normal_w=float((id>>7)&127u)/127.f;
  const float tangent_z=-float(id&127u)/127.f;
  const float alpha=float((id>>14)&255u)/255.f;
  const int8_t normal_byte=int8_t(std::round(normal_w*127.f));
  const int8_t tangent_byte=int8_t(std::round(tangent_z*127.f));
  const uint8_t color_byte=uint8_t(std::round(alpha*255.f));
  const uint32_t restored=uint32_t(-int(tangent_byte))|(uint32_t(normal_byte)<<7)|(uint32_t(color_byte)<<14);
  assert(restored==id);
 }
 assert(game::outline_tree_id(1)!=game::outline_tree_id(0xfffffff0u));
 assert(game::outline_tree_id(0xfffffff0u)==game::outline_tree_id(0xfffffff0u));
 assert(game::outline_tree_id(0xfffffff0u)<(1u<<22));
 auto constant_faces=[](const game::terrain_mesh_data& mesh){for(size_t i=0;i+2<mesh.indices.size();i+=3){const auto id=mesh.vertices[mesh.indices[i]].outline_id;assert(id>=2&&id<=40);assert(mesh.vertices[mesh.indices[i+1]].outline_id==id&&mesh.vertices[mesh.indices[i+2]].outline_id==id);}};
 game::TerrainEditorLevel level;level.terrain.config.resolution=32;level.terrain.config.world_size=32;
 {
  game::TerrainEditorLevel rocks_level;rocks_level.terrain.config.resolution=128;rocks_level.terrain.config.world_size=128;rocks_level.terrain.config.island_size=32;
  rocks_level.terrain.layers.assign(128*128,2);
  const auto rocks=game::build_editor_rocks(rocks_level,-64,-64,64,64);
  assert(!rocks.indices.empty()&&rocks.vertices.size()<60000);
  same_mesh(rocks,game::build_editor_rocks(rocks_level,-64,-64,64,64));
  for(const auto&v:rocks.vertices){assert(v.position[1]>=2&&v.position[1]<3.2f);float length=0;for(float c:v.normal)length+=c*c;assert(std::abs(length-1)<.00001f);}
  for(auto i:rocks.indices)assert(i<rocks.vertices.size());
  game::EditorBuilding house;house.position={{0,2,0}};house.terrace=2;rocks_level.buildings.push_back(house);
  for(const auto&v:game::build_editor_rocks(rocks_level,-64,-64,64,64).vertices)assert(!game::point_inside_building(house,v.position[0],v.position[2],0));
  rocks_level.terrain.layers.assign(128*128,-1);assert(game::build_editor_rocks(rocks_level,-64,-64,64,64).indices.empty());
  auto rock_state=std::make_shared<game::TerrainEditorState>();
  rock_state->level.terrain.config.resolution=256;rock_state->level.terrain.config.world_size=256;rock_state->level.terrain.config.seed=4;
  rock_state->level.terrain.layers.assign(256*256,2);
  game::TerrainEditorRenderCache rock_cache(rock_state);rock_cache.initialize({},{});
  size_t before=0;for(const auto&b:rock_cache.rebuild({}))if(b.key=="2_10_0")before=b.mesh.vertices.size();assert(before>0);
  // A cluster left of X=64 depends on terrain to its right at world X=68.
  game::TerrainEditorDirty rock_dirty;rock_dirty.x0=rock_dirty.x1=196;rock_dirty.z0=rock_dirty.z1=136;rock_dirty.terrain=true;
  rock_state->level.terrain.layers[136*256+196]=3;
  bool invalidated=false;for(const auto&b:rock_cache.rebuild(rock_dirty))if(b.key=="2_10_0"){assert(b.mesh.vertices.size()<before);invalidated=true;}assert(invalidated);
  rock_state->level.terrain.layers[136*256+196]=2;
  bool restored=false;for(const auto&b:rock_cache.rebuild(rock_dirty))if(b.key=="2_10_0"){assert(b.mesh.vertices.size()==before);restored=true;}assert(restored);
 }
 level.terrain.layers.assign(1024,2);level.climate.assign(1024,0);level.grass_density.assign(1024,0);level.grass_type.assign(1024,0);
 // A shallow diagonal border must not alternate between grid-axis and diagonal
 // wall normals. Keep the plateaus flat while filtering the contour's grid noise.
 auto diagonal=level;
 for(int z=0;z<32;++z)for(int x=0;x<32;++x)
  diagonal.terrain.layers[z*32+x]=x>=8+int(std::floor(.4f*z))?2:1;
 size_t contour_faces=0;
 for(int cz=0;cz<2;++cz)for(int cx=0;cx<2;++cx){
  auto mesh=game::build_editor_chunk(diagonal,cx,cz,false).terrain;
  constant_faces(mesh);
  assert(std::abs(area(mesh)-256)<.02);
  for(const auto&v:mesh.vertices){
   if(v.normal[1]>.99f){assert(v.position[1]==.5f||v.position[1]==2.f);continue;}
   if(v.position[2]<-10||v.position[2]>9)continue;
   const float alignment=std::abs(-v.normal[0]/std::sqrt(1.16f)+v.normal[2]*.4f/std::sqrt(1.16f));
   assert(alignment>.97f);++contour_faces;
  }
 }
 assert(contour_faces>20);
 // Independently built neighboring chunks must share precisely the same contour.
 std::set<std::pair<float,float>> left_edge,right_edge;
 auto left=game::build_editor_chunk(diagonal,0,1,false).terrain;
 auto right=game::build_editor_chunk(diagonal,1,1,false).terrain;
 for(const auto&v:left.vertices)if(std::abs(v.position[0])<.00001f)left_edge.insert({v.position[1],v.position[2]});
 for(const auto&v:right.vertices)if(std::abs(v.position[0])<.00001f)right_edge.insert({v.position[1],v.position[2]});
 assert(!left_edge.empty()&&left_edge==right_edge);
 // Remeshing an unchanged terrace must preserve its seeded material gradient,
 // including areas in the same chunk but outside the sculpted brush.
 auto colored=std::make_shared<game::TerrainEditorState>();colored->level=level;
 colored->level.terrain.layers.assign(1024,6);
 colored->level.terrain_remeshed.assign(4,1);colored->level.grass_remeshed.assign(4,1);
 game::terrain_mesh_data gradient;
 for(std::array<float,3> p:{std::array<float,3>{{-16,10,-16}},std::array<float,3>{{-16,10,16}},std::array<float,3>{{16,10,-16}},std::array<float,3>{{16,10,16}}}){
  gradient.vertices.push_back({p,{{0,1,0}},{{255,255,255,255}}});
  gradient.vertices.back().editor_palette={{(12.5f+(p[0]+16.f)/32.f)/24.f,.5f/7.f}};
 }
 gradient.indices={0,1,2,2,1,3};
 game::TerrainEditorRenderCache color_cache(colored);color_cache.initialize(gradient,{});color_cache.rebuild({});
 game::TerrainEditorDirty recolor;recolor.x0=recolor.z0=0;recolor.x1=recolor.z1=31;recolor.terrain=true;
 colored->level.terrain.layers[8*32+8]=16;color_cache.rebuild(recolor);
 size_t unchanged_vertices=0;
 for(const auto&mesh:color_cache.terrain_parts())for(const auto&v:mesh.vertices)if(v.normal[1]>.99f&&v.position[1]==10.f){
  assert(std::abs(v.editor_palette[0]-(12.5f+(v.position[0]+16.f)/32.f)/24.f)<.00001f);++unchanged_vertices;
 }
 assert(unchanged_vertices>100);
 auto band=colored->level;for(int z=0;z<32;++z)for(int x=0;x<32;++x)band.terrain.layers[z*32+x]=x<16?5:6;
 auto band_mesh=game::build_editor_chunk(band,0,0,false).terrain;size_t brown_wall=0;
 for(const auto&v:band_mesh.vertices)if(std::abs(v.normal[1])<.01f&&v.position[1]==10.f){assert(std::abs(v.editor_palette[0]-12.5f/24.f)<.00001f);++brown_wall;}
 assert(brown_wall>0);
 auto flat=game::build_editor_chunk(level,0,0);assert(std::abs(area(flat.terrain)-256)<.001);assert(flat.grass.empty());
 for(const auto&v:flat.terrain.vertices)assert(v.outline_id==game::outline_terrace_id(2));
 for(const auto&v:flat.terrain.vertices){assert(v.normal[1]>.99f);assert(std::abs(v.position[1]-2)<.001);}
 level.terrain.layers[8*32+8]=-9;auto hole=game::build_editor_chunk(level,0,0);assert(area(hole.terrain)<256);assert(area(hole.terrain)>250);
 level.terrain.layers.assign(1024,2);level.grass_density.assign(1024,255);
 auto grass=game::build_editor_chunk(level,0,0);auto repeated=game::build_editor_chunk(level,0,0);assert(!grass.grass.empty());
 assert(grass.grass[0].vertices.size()==repeated.grass[0].vertices.size());assert(grass.grass[0].vertices[0].position==repeated.grass[0].vertices[0].position);
 for(auto&m:grass.grass){assert(m.vertices.size()%4==0 && m.indices.size()==m.vertices.size()/4*6);assert(m.vertices.size()<=65535);for(auto index:m.indices)assert(index<m.vertices.size());for(size_t i=0;i<m.vertices.size();++i)assert(m.vertices[i].color[3]==(i%2?255:0));}
 for(const auto&m:grass.grass)for(const auto&v:m.vertices)assert(v.outline_id==0);
 game::EditorTree imported;imported.id=0xfffffff0u;auto imported_mesh=game::build_editor_tree(imported,42);for(const auto&v:imported_mesh.vertices)assert(v.outline_id==game::outline_tree_id(imported.id));
 for(int z=0;z<32;++z)for(int x=0;x<32;++x)level.terrain.layers[z*32+x]=((x+z)%2)?16:-8;
 auto worst=game::build_editor_chunk(level,0,0);std::cout<<"Extreme smoothed area="<<area(worst.terrain)<<std::endl;assert(std::abs(area(worst.terrain)-256)<.02);assert(worst.terrain.vertices.size()<=65535);
 for(auto index:worst.terrain.indices)assert(index<worst.terrain.vertices.size());
 auto state=std::make_shared<game::TerrainEditorState>();state->level=level;state->level.terrain.layers.assign(1024,2);state->level.grass_density.assign(1024,255);
 game::TerrainEditorRenderCache cache(state);cache.initialize(flat.terrain,grass.grass);auto initial=cache.rebuild({});assert(!initial.empty());assert(cache.rebuild({}).empty());assert(std::abs(area(cache.terrain_parts().at(0))-256)<.001);
 game::TerrainEditorDirty dirty;dirty.x0=0;dirty.z0=0;dirty.x1=15;dirty.z1=15;dirty.grass=true;state->level.grass_density.assign(1024,0);auto removed=cache.rebuild(dirty);assert(!removed.empty());for(auto&batch:removed){assert(batch.kind==game::EditorRenderBatch::Grass);assert(batch.mesh.indices.empty());}assert(std::abs(area(cache.terrain_parts().at(0))-256)<.001);
 dirty.grass=false;dirty.terrain=true;state->level.terrain.layers[8*32+8]=-9;auto changed=cache.rebuild(dirty);assert(!changed.empty());for(auto&batch:changed)assert(batch.mesh.vertices.size()<=60000);
 // A local conversion from the preserved seed to the editable grid must close
 // differences in contour height at its shared boundary with an old chunk.
 auto seamstate=std::make_shared<game::TerrainEditorState>();seamstate->level.terrain.config.resolution=64;seamstate->level.terrain.config.world_size=64;seamstate->level.terrain.layers.assign(4096,2);
 game::terrain_mesh_data legacy;for(std::array<float,3> p: {std::array<float,3>{{-32,3,-32}},std::array<float,3>{{-32,3,32}},std::array<float,3>{{32,3,-32}},std::array<float,3>{{32,3,32}}}){legacy.vertices.push_back({p,{{0,1,0}},{{255,255,255,255}}});legacy.vertices.back().editor_palette={{9.5f/24.f,.5f/7.f}};}legacy.indices={0,1,2,2,1,3};
 for(auto&v:legacy.vertices)v.outline_id=game::outline_terrace_id(3);
 game::TerrainEditorRenderCache seamcache(seamstate);seamcache.initialize(legacy,{});seamcache.rebuild({});dirty.x0=dirty.x1=2;dirty.z0=dirty.z1=2;auto seambatches=seamcache.rebuild(dirty);size_t seam_vertices=0;
 for(auto&batch:seambatches)if(batch.kind==game::EditorRenderBatch::Terrain)for(const auto&v:batch.mesh.vertices)if(std::abs(v.normal[1])<.01f){++seam_vertices;assert(std::abs(v.position[0]+16)<.001f||std::abs(v.position[2]+16)<.001f);assert(v.position[1]>=2&&v.position[1]<=3);}
 assert(seam_vertices>0);
 for(const auto&batch:seambatches)if(batch.kind==game::EditorRenderBatch::Terrain)constant_faces(batch.mesh);
 // Persisted local reconstruction policies preserve untouched seed shapes.
 seamcache.set_seed_builder([legacy](const game::terrain_config&){return std::make_pair(legacy,std::vector<game::terrain_mesh_data>{});});seamstate->level.terrain_remeshed.assign(16,0);seamstate->level.grass_remeshed.assign(16,0);++seamstate->load_generation;seamcache.rebuild({});for(const auto&mesh:seamcache.terrain_parts())for(const auto&v:mesh.vertices)assert(v.position[1]==3);
 seamstate->level.terrain_remeshed[0]=1;++seamstate->load_generation;seamcache.rebuild({});bool seed_top=false,editor_top=false;for(const auto&mesh:seamcache.terrain_parts())for(const auto&v:mesh.vertices){seed_top|=v.position[1]==3;editor_top|=v.position[1]==2;}assert(seed_top&&editor_top);seamcache.set_seed_builder({});
 // Successful same-config loads discard legacy geometry as well as all old
 // foliage. A changed grid layout emits old-key deletions before new uploads.
 auto before_load=seamcache.terrain_parts();assert(before_load.size()>1);seamstate->level.terrain.layers.assign(4096,4);++seamstate->load_generation;auto same_loaded=seamcache.rebuild({});assert(!same_loaded.empty());for(const auto&mesh:seamcache.terrain_parts())for(const auto&v:mesh.vertices)assert(std::abs(v.position[1]-6)<.001);
 seamstate->level.terrain.config.resolution=32;seamstate->level.terrain.config.world_size=128;seamstate->level.terrain.layers.assign(1024,2);++seamstate->load_generation;auto resized=seamcache.rebuild({});bool deletion=false;for(const auto&batch:resized)deletion|=batch.mesh.indices.empty();assert(deletion);assert(seamcache.terrain_parts().size()==4);double resized_area=0;for(const auto&mesh:seamcache.terrain_parts()){resized_area+=area(mesh);for(const auto&v:mesh.vertices)assert(std::abs(v.position[0])<=64&&std::abs(v.position[2])<=64);}assert(std::abs(resized_area-128*128)<.01);
 // Exact static roundtrip after sculpt, grass paint and climate paint.
 auto round=std::make_shared<game::TerrainEditorState>();game::terrain_data data;
 data.config.resolution=64;data.config.world_size=64;data.config.island_size=32;data.layers.assign(4096,1);round->initialize(data);round->consume_dirty();
 round->level.trees.push_back(imported);round->level.next_tree_id=imported.id+1;
 round->level.grass_density.assign(4096,255);auto seed_grass=game::build_editor_grass(round->level,legacy);
 game::TerrainEditorRenderCache rc(round);rc.set_seed_builder([legacy,seed_grass](const game::terrain_config&){return std::make_pair(legacy,seed_grass);});rc.initialize(legacy,seed_grass);
 std::map<std::string,game::terrain_mesh_data> rendered;
 auto accept=[&](std::vector<game::EditorRenderBatch> batches){for(auto&batch:batches)rendered[batch.key]=std::move(batch.mesh);};accept(rc.rebuild({}));
 round->level.terrain_remeshed[0]=round->level.grass_remeshed[0]=1;dirty={};dirty.x0=dirty.z0=dirty.x1=dirty.z1=2;dirty.terrain=true;accept(rc.rebuild(dirty));
 // A coalesced rectangle must preserve chunks with a seed policy.
 dirty.x1=dirty.z1=63;accept(rc.rebuild(dirty));bool preserved=false;
 for(const auto&mesh:rc.terrain_parts())for(const auto&v:mesh.vertices)preserved|=v.position[1]==3;assert(preserved);
 dirty.x1=dirty.z1=2;
 for(int z=0;z<16;++z)for(int x=0;x<16;++x){round->level.grass_density[z*64+x]=123;round->level.grass_type[z*64+x]=1;round->level.climate[z*64+x]=(x<8?0:5);}
 dirty.terrain=false;dirty.grass=true;dirty.climate=true;accept(rc.rebuild(dirty));const auto expected=rendered;
 assert(round->save("mesh-roundtrip.level"));assert(round->load("mesh-roundtrip.level"));accept(rc.rebuild(round->consume_dirty()));assert(rendered.size()==expected.size());
 for(const auto&entry:expected){const auto&actual=rendered.at(entry.first);std::cout<<"Roundtrip key="<<entry.first<<" vertices="<<entry.second.vertices.size()<<"/"<<actual.vertices.size()<<" indices="<<entry.second.indices.size()<<"/"<<actual.indices.size()<<std::endl;same_mesh(entry.second,actual);}std::remove("mesh-roundtrip.level");
 std::cout<<"Exact terrain/grass/climate render roundtrip passed\n";
 std::cout<<"Editor mesh CPU checks passed; extreme chunk vertices="<<worst.terrain.vertices.size()<<"\n";
}
