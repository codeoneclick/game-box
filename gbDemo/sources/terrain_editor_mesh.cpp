#include "terrain_editor_mesh.h"
#include "outline_ids.h"
#include "voxel_tree_builder.h"
#include <algorithm>
#include <cmath>
#include <map>
#include <stdexcept>
#include <tuple>
#include <unordered_map>

namespace game { namespace {
using Point=std::array<float,3>;
using Color=std::array<uint8_t,4>;
const Color white={{255,255,255,255}};
Point sub(Point a,Point b){return {{a[0]-b[0],a[1]-b[1],a[2]-b[2]}};}
Point cross(Point a,Point b){return {{a[1]*b[2]-a[2]*b[1],a[2]*b[0]-a[0]*b[2],a[0]*b[1]-a[1]*b[0]}};}
Point unit(Point p){float l=std::sqrt(p[0]*p[0]+p[1]*p[1]+p[2]*p[2]);return l>1e-10f?Point{{p[0]/l,p[1]/l,p[2]/l}}:Point{{0,1,0}};}
uint32_t hash(uint32_t v){v=(v^(v>>16))*0x7feb352du;v=(v^(v>>15))*0x846ca68bu;return v^(v>>16);}
float random(uint32_t&state){state=hash(state+0x9e3779b9u);return (state&0xffffffu)/16777216.f;}
std::array<float,2> palette(float level,int climate){float column=level<0?level+8:(level>0?level+7:7);return {{(column+.5f)/24.f,(climate+.5f)/7.f}};}
// Match the seed mesh: level 4 transitions green->brown, level 6 brown->gray.
float material_level(int layer,bool inner=false){
 if(layer==6)return inner?6.f:5.f;
 if(layer==4&&inner)return 5.f;
 return static_cast<float>(layer);
}
float surface_material_level(int layer,float field){
 if(layer==4||layer==6)return material_level(layer)+std::max(0.f,std::min(1.f,field-(layer-.5f)));
 return material_level(layer);
}
struct Sample {float x,z,value;};
using Polygon=std::vector<Sample>;
// Exact global endpoints and a fixed diagonal make adjacent chunk contours agree.
Polygon clip(const Polygon&source,float threshold,bool above){
 Polygon result;if(source.empty())return result;Sample a=source.back();bool ia=above?a.value>=threshold:a.value<=threshold;
 for(const Sample&b:source){bool ib=above?b.value>=threshold:b.value<=threshold;
  if(ia!=ib){float t=(threshold-a.value)/(b.value-a.value);result.push_back({a.x+(b.x-a.x)*t,a.z+(b.z-a.z)*t,threshold});}
  if(ib)result.push_back(b);a=b;ia=ib;
 }return result;
}
struct TerrainWriter {
 terrain_mesh_data&mesh;
 using Key=std::array<int64_t,9>;
 struct KeyHash{size_t operator()(const Key&key)const{size_t result=1469598103934665603ull;for(auto value:key){result^=static_cast<size_t>(value);result*=1099511628211ull;}return result;}};
 std::unordered_map<Key,uint16_t,KeyHash> vertices;
 const TerrainEditorLevel& logical;
 uint16_t vertex(Point p,Point n,float level,int climate,uint32_t outline_id){
  const auto& config=logical.terrain.config;const int count=config.resolution;
  const int x=std::max(0,std::min(count-1,static_cast<int>((p[0]/config.world_size+.5f)*count))),z=std::max(0,std::min(count-1,static_cast<int>((p[2]/config.world_size+.5f)*count)));
  const size_t cell=static_cast<size_t>(z)*count+x;climate=cell<logical.climate.size()?std::min(6,static_cast<int>(logical.climate[cell])):0;
  const Key key={{static_cast<int64_t>(std::llround(p[0]*100000)),static_cast<int64_t>(std::llround(p[1]*100000)),static_cast<int64_t>(std::llround(p[2]*100000)),static_cast<int64_t>(std::llround(n[0]*100000)),static_cast<int64_t>(std::llround(n[1]*100000)),static_cast<int64_t>(std::llround(n[2]*100000)),static_cast<int64_t>(std::llround(level*100000)),climate,outline_id}};
  auto found=vertices.find(key);if(found!=vertices.end())return found->second;
  if(mesh.vertices.size()>=65535)throw std::length_error("Editor terrain chunk exceeds 16-bit mesh limit");
  uint16_t index=static_cast<uint16_t>(mesh.vertices.size());terrain_vertex v(p,n,white);v.outline_id=outline_id;v.editor_palette=palette(level,climate);mesh.vertices.push_back(v);vertices.emplace(key,index);return index;
 }
 void triangle(Point a,Point b,Point c,Point n,float la,float lb,float lc,int climate,int terrace){
  const auto face=cross(sub(b,a),sub(c,a));if(std::abs(face[0])+std::abs(face[1])+std::abs(face[2])<1e-7f)return;
  mesh.indices.push_back(vertex(a,n,la,climate,outline_terrace_id(terrace)));mesh.indices.push_back(vertex(b,n,lb,climate,outline_terrace_id(terrace)));mesh.indices.push_back(vertex(c,n,lc,climate,outline_terrace_id(terrace)));
 }
};
uint8_t map_value(const std::vector<uint8_t>&map,size_t cell,uint8_t fallback=0){return cell<map.size()?map[cell]:fallback;}
void grass_triangle(EditorChunkMesh&result,const TerrainEditorLevel&level,Point a,Point b,Point c,uint32_t&state){
 const int resolution=level.terrain.config.resolution;const float size=level.terrain.config.world_size;
 float area=std::abs((b[0]-a[0])*(c[2]-a[2])-(b[2]-a[2])*(c[0]-a[0]))*.5f;
 // Sample a fixed maximum density, then thin at each actual root using its map cell.
 int count=static_cast<int>(std::floor(area*8.3f+random(state)));
 for(int i=0;i<count;++i){float r=std::sqrt(random(state)),s=random(state),wb=.97f*r*(1-s)+.01f,wc=.97f*r*s+.01f;
  float x=a[0]+wb*(b[0]-a[0])+wc*(c[0]-a[0]),z=a[2]+wb*(b[2]-a[2])+wc*(c[2]-a[2]);
  bool under_stairs=false;for(const auto&stairs:level.stairs)if(point_inside_stairs(stairs,x,z,.2f)){under_stairs=true;break;}if(under_stairs)continue;
  int cx=std::max(0,std::min(resolution-1,static_cast<int>((x/size+.5f)*resolution))),cz=std::max(0,std::min(resolution-1,static_cast<int>((z/size+.5f)*resolution)));size_t cell=static_cast<size_t>(cz)*resolution+cx;
  const uint8_t density=map_value(level.grass_density,cell);if(density==0||random(state)>=density/255.f)continue;
  const float patch=.5f+.5f*std::sin(x*.24f+std::sin(z*.13f))*std::cos(z*.21f);
  if(random(state)>.12f+.88f*patch*patch)continue;
  uint8_t type=map_value(level.grass_type,cell);float height=(.28f+random(state)*.27f)*1.33f,width=.12f+random(state)*.12f,angle=random(state)*6.283185307f,dx=std::cos(angle),dz=std::sin(angle),lean=(random(state)-.5f)*height*.65f,y=a[1]+.002f;
  Color root={{35,58,7,255}},tip={{107,139,21,255}};
  if(type==1){root={{36,20,6,255}};tip={{146,111,43,255}};}
  if(type==2){root={{92,116,111,255}};tip={{216,230,225,255}};}
  if(result.grass.empty()||result.grass.back().vertices.size()+4>60000)result.grass.emplace_back();auto&mesh=result.grass.back();uint16_t base=static_cast<uint16_t>(mesh.vertices.size());Point n={{-dz*.6f,.8f,dx*.6f}};
  mesh.vertices.push_back({{{x-dx*width*.5f,y,z-dz*width*.5f}},n,root});mesh.vertices.push_back({{{x+dx*width*.5f,y,z+dz*width*.5f}},n,root});
  mesh.vertices.push_back({{{x-dx*width*.5f-dz*lean,y+height,z-dz*width*.5f+dx*lean}},n,tip});mesh.vertices.push_back({{{x+dx*width*.5f-dz*lean,y+height,z+dz*width*.5f+dx*lean}},n,tip});
  for(size_t j=0;j<4;++j){mesh.vertices[base+j].texcoord={{x,z}};mesh.vertices[base+j].grass_bend={{j<2?0.f:1.f,height}};mesh.vertices[base+j].color[3]=j%2?255:0;}
  for(uint16_t j:{0,1,2,1,3,2})mesh.indices.push_back(static_cast<uint16_t>(base+j));
 }
}
} // namespace

EditorChunkMesh build_editor_chunk(const TerrainEditorLevel&level,int chunk_x,int chunk_z,bool with_grass){
 EditorChunkMesh result;const auto&data=level.terrain;const auto&config=data.config;const int n=config.resolution;
 if(n<=0||data.layers.size()!=static_cast<size_t>(n)*n||!std::isfinite(config.world_size)||config.world_size<=0)throw std::invalid_argument("Invalid editor terrain grid");
 if(chunk_x<0||chunk_z<0||chunk_x>=(n+15)/16||chunk_z>=(n+15)/16)return result;
 const float cell=config.world_size/n,half=config.world_size*.5f;TerrainWriter writer{result.terrain,{},level};
 writer.vertices.reserve(2048);
 // Filter grid quantization before extracting terrace bands. Heights remain
 // discrete; only the world-space positions of their contours are smoothed.
 // Cache the 17x17 corner field once per chunk, using identical global samples
 // on shared boundaries. Empty cells remain holes rather than being filled.
 std::array<float,17*17> field{};
 const int weights[5]={1,4,6,4,1};
 auto raw=[&](int x,int z){x=std::max(0,std::min(n-1,x));z=std::max(0,std::min(n-1,z));return data.layers[static_cast<size_t>(z)*n+x];};
 for(int z=0;z<17;++z)for(int x=0;x<17;++x){
  const int gx=chunk_x*16+x,gz=chunk_z*16+z;
  const int center=raw(gx,gz);float sum=0,weight=0;
  if(center!=terrain_data::empty_layer){
   for(int dz=-2;dz<=2;++dz)for(int dx=-2;dx<=2;++dx){
    const int value=raw(gx+dx,gz+dz);if(value==terrain_data::empty_layer)continue;
    const float w=static_cast<float>(weights[dx+2]*weights[dz+2]);sum+=value*w;weight+=w;
   }
  }
  field[z*17+x]=weight>0?sum/weight:static_cast<float>(center);
 }
 auto sample=[&](int x,int z){return Sample{x*cell-half,z*cell-half,field[(z-chunk_z*16)*17+x-chunk_x*16]};};
 for(int z=chunk_z*16;z<std::min(n,chunk_z*16+16);++z)for(int x=chunk_x*16;x<std::min(n,chunk_x*16+16);++x){
  const Sample p00=sample(x,z),p01=sample(x,z+1),p10=sample(x+1,z),p11=sample(x+1,z+1);
  const Polygon triangles[2]={{p00,p01,p10},{p10,p01,p11}};
  for(int tri=0;tri<2;++tri){const auto&source=triangles[tri];float minvalue=std::min({source[0].value,source[1].value,source[2].value}),maxvalue=std::max({source[0].value,source[1].value,source[2].value});
   int climate=std::min(6,static_cast<int>(map_value(level.climate,static_cast<size_t>(z)*n+x)));uint32_t state=hash(config.seed^hash(static_cast<uint32_t>(z*n+x))^static_cast<uint32_t>(tri*713));
   for(int layer=std::max(-8,static_cast<int>(std::floor(minvalue+.5f)));layer<=std::min(16,static_cast<int>(std::floor(maxvalue+.5f)));++layer){
    Polygon polygon=clip(clip(source,layer-.5f,true),layer+.5f,false);float y=editor_level_height(config,layer);
    if(polygon.size()>=3){Point a={{polygon[0].x,y,polygon[0].z}};for(size_t i=1;i+1<polygon.size();++i){Point b={{polygon[i].x,y,polygon[i].z}},c={{polygon[i+1].x,y,polygon[i+1].z}};writer.triangle(a,b,c,{{0,1,0}},surface_material_level(layer,polygon[0].value),surface_material_level(layer,polygon[i].value),surface_material_level(layer,polygon[i+1].value),climate,layer);if(with_grass&&layer>0)grass_triangle(result,level,a,b,c,state);}}
    // A single contour wall at each threshold; no triangle/chunk perimeter walls.
    const float threshold=layer-.5f;if(minvalue>=threshold||maxvalue<=threshold)continue;
    Polygon high=clip(source,threshold,true);for(size_t i=0;i<high.size();++i){auto a=high[i],b=high[(i+1)%high.size()];if(std::abs(a.value-threshold)>1e-5f||std::abs(b.value-threshold)>1e-5f)continue;
     float lower=editor_level_height(config,layer-1);Point at={{a.x,y,a.z}},bt={{b.x,y,b.z}},ab={{a.x,lower,a.z}},bb={{b.x,lower,b.z}};Point normal=unit(Point{{-(b.z-a.z),0,b.x-a.x}});
     const float upper_material=material_level(layer),lower_material=material_level(layer== -8?-8:layer-1,true);
     writer.triangle(at,ab,bt,normal,upper_material,lower_material,upper_material,climate,layer);writer.triangle(bt,ab,bb,normal,upper_material,lower_material,lower_material,climate,layer);
    }
   }
  }
 }return result;
}

EditorChunkMesh build_editor_seed_chunk(const TerrainEditorLevel&level,int chunk_x,int chunk_z,const std::vector<EditorSeedTriangle>& domain,const terrain_data& seed){
 EditorChunkMesh result;const auto& config=level.terrain.config;const int n=config.resolution;
 if(seed.layers.size()!=level.terrain.layers.size())throw std::invalid_argument("Seed terrain field size mismatch");
 const float cell=config.world_size/n,half=config.world_size*.5f;TerrainWriter writer{result.terrain,{},level};writer.vertices.reserve(4096);
 std::array<float,17*17> current{},original{};std::array<std::array<float,17*17>,26> occupancy_change{};const int weights[5]={1,4,6,4,1};
 auto filtered=[&](const terrain_data& data,int gx,int gz){
  auto raw=[&](int x,int z){return int(data.layers[size_t(std::max(0,std::min(n-1,z)))*n+std::max(0,std::min(n-1,x))]);};
  const int center=raw(gx,gz);if(center==terrain_data::empty_layer)return float(center);float sum=0,weight=0;
  for(int dz=-2;dz<=2;++dz)for(int dx=-2;dx<=2;++dx){const int value=raw(gx+dx,gz+dz);if(value==terrain_data::empty_layer)continue;float w=float(weights[dx+2]*weights[dz+2]);sum+=w*value;weight+=w;}
  return weight>0?sum/weight:float(center);
 };
 for(int z=0;z<17;++z)for(int x=0;x<17;++x){int gx=chunk_x*16+x,gz=chunk_z*16+z;current[z*17+x]=filtered(level.terrain,gx,gz);original[z*17+x]=filtered(seed,gx,gz);
  for(int dz=-2;dz<=2;++dz)for(int dx=-2;dx<=2;++dx){size_t i=size_t(std::max(0,std::min(n-1,gz+dz)))*n+std::max(0,std::min(n-1,gx+dx));int a=seed.layers[i],b=level.terrain.layers[i];if(a==b)continue;
   for(int l=std::max(-8,std::min(a,b)+1);l<=std::min(17,std::max(a,b));++l)occupancy_change[l+8][z*17+x]+=float(weights[dx+2]*weights[dz+2])/256.f;
  }
 }
 auto field=[&](const std::array<float,17*17>& values,float x,float z){
  float fx=std::max(0.f,std::min(16.f,(x+half)/cell-chunk_x*16)),fz=std::max(0.f,std::min(16.f,(z+half)/cell-chunk_z*16));
  int ix=std::min(15,int(fx)),iz=std::min(15,int(fz));fx-=ix;fz-=iz;
  return (values[iz*17+ix]*(1-fx)+values[iz*17+ix+1]*fx)*(1-fz)+(values[(iz+1)*17+ix]*(1-fx)+values[(iz+1)*17+ix+1]*fx)*fz;
 };
 // The original coast has no zero shelf. Preserve its submerged floor and
 // introduce an authored zero shelf continuously, using the same world-space
 // field on both sides of every source triangle and chunk edge.
 auto zero_height=[&](float x,float z){float t=std::min(1.f,std::max(field(occupancy_change[8],x,z),field(occupancy_change[9],x,z))*4.f);t=t*t*(3-2*t);return editor_level_height(config,-1)+(editor_level_height(config,0)-editor_level_height(config,-1))*t;};
 auto cut=[](const Polygon& source,int axis,float limit,bool greater){Polygon out;if(source.empty())return out;auto a=source.back();auto coordinate=[&](const Sample& p){return axis==0?p.x:p.z;};bool ia=greater?coordinate(a)>=limit:coordinate(a)<=limit;
  for(auto b:source){bool ib=greater?coordinate(b)>=limit:coordinate(b)<=limit;if(ia!=ib){float t=(limit-coordinate(a))/(coordinate(b)-coordinate(a));Sample p={a.x+(b.x-a.x)*t,a.z+(b.z-a.z)*t,a.value+(b.value-a.value)*t};if(axis==0)p.x=limit;else p.z=limit;out.push_back(p);}if(ib)out.push_back(b);a=b;ia=ib;}return out;};
 for(const auto& triangle:domain){
  const float minx=std::min({triangle[0].x,triangle[1].x,triangle[2].x}),maxx=std::max({triangle[0].x,triangle[1].x,triangle[2].x});
  const float minz=std::min({triangle[0].z,triangle[1].z,triangle[2].z}),maxz=std::max({triangle[0].z,triangle[1].z,triangle[2].z});
  const int x0=std::max(chunk_x*16,int(std::floor((minx+half)/cell))),x1=std::min(std::min(n,chunk_x*16+16)-1,int(std::floor((maxx+half)/cell)));
  const int z0=std::max(chunk_z*16,int(std::floor((minz+half)/cell))),z1=std::min(std::min(n,chunk_z*16+16)-1,int(std::floor((maxz+half)/cell)));
  for(int z=z0;z<=z1;++z)for(int x=x0;x<=x1;++x){
   Polygon polygon;for(const auto& p:triangle)polygon.push_back({p.x,p.z,p.value});
   polygon=cut(cut(cut(cut(polygon,0,x*cell-half,true),0,(x+1)*cell-half,false),2,z*cell-half,true),2,(z+1)*cell-half,false);
   if(polygon.size()<3)continue;
   for(auto& p:polygon){float now=field(current,p.x,p.z),before=field(original,p.x,p.z),t=std::min(1.f,std::abs(now-before)*2.f);t=t*t*(3-2*t);const float baseline=p.value;
    float value=now+(baseline-before)*(1-t);
    // Scalar height smoothing must not spill a tall edit across another
    // terrace boundary whose binary occupancy the command never changed.
    if(value>baseline)for(int l=std::max(-8,int(std::ceil(baseline+.5f-1e-5f)));l<=std::min(17,int(std::floor(value+.5f)));++l){
     float influence=std::min(1.f,field(occupancy_change[l+8],p.x,p.z)*4.f);if(influence>=1)continue;influence=influence*influence*(3-2*influence);float threshold=l-.5f;
     const float protected_value=std::abs(baseline-threshold)<1e-5f?threshold:std::min(value,threshold-std::min(.001f,threshold-baseline));
     value=protected_value+(value-protected_value)*influence;
    }
    else if(value<baseline)for(int l=std::min(17,int(std::floor(baseline+.5f+1e-5f)));l>=std::max(-8,int(std::ceil(value+.5f)));--l){
     float influence=std::min(1.f,field(occupancy_change[l+8],p.x,p.z)*4.f);if(influence>=1)continue;influence=influence*influence*(3-2*influence);float threshold=l-.5f;
     const float protected_value=std::abs(baseline-threshold)<1e-5f?threshold:std::max(value,threshold+std::min(.001f,baseline-threshold));
     value=protected_value+(value-protected_value)*influence;
    }
    p.value=value;
   }
   for(size_t fan=1;fan+1<polygon.size();++fan){Polygon source={polygon[0],polygon[fan],polygon[fan+1]};
    const float low=std::min({source[0].value,source[1].value,source[2].value}),high=std::max({source[0].value,source[1].value,source[2].value});
    const int climate=std::min(6,int(map_value(level.climate,size_t(z)*n+x)));
    for(int layer=std::max(-8,int(std::floor(low+.5f)));layer<=std::min(16,int(std::floor(high+.5f)));++layer){
     Polygon band=clip(clip(source,layer-.5f,true),layer+.5f,false);
     const float y=editor_level_height(config,layer);
     auto at_height=[&](const Sample& p){return Point{{p.x,layer==0?zero_height(p.x,p.z):y,p.z}};};
     for(size_t i=1;i+1<band.size();++i){auto a=at_height(band[0]),b=at_height(band[i]),c=at_height(band[i+1]);const int rendered_layer=layer==0&&a[1]==editor_level_height(config,-1)&&b[1]==a[1]&&c[1]==a[1]?-1:layer;
      writer.triangle(a,b,c,layer==0?unit(cross(sub(b,a),sub(c,a))):Point{{0,1,0}},surface_material_level(rendered_layer,band[0].value),surface_material_level(rendered_layer,band[i].value),surface_material_level(rendered_layer,band[i+1].value),climate,rendered_layer);
     }
     const float threshold=layer-.5f;if(low>threshold+1e-6f||high<=threshold+1e-6f)continue;
     auto upper=clip(source,threshold,true);
     for(size_t i=0;i<upper.size();++i){auto a=upper[i],b=upper[(i+1)%upper.size()];if(std::abs(a.value-threshold)>1e-5f||std::abs(b.value-threshold)>1e-5f)continue;
      float lower_a=layer==1?zero_height(a.x,a.z):editor_level_height(config,layer-1),lower_b=layer==1?zero_height(b.x,b.z):editor_level_height(config,layer-1);
      if(layer==1&&config.underwater_layers_count==0){lower_a=lower_b=std::min(config.water_y-config.layer_height,config.shore_height-config.layer_height);}
      Point at=at_height(a),bt=at_height(b),ab={{a.x,lower_a,a.z}},bb={{b.x,lower_b,b.z}},normal=unit({{-(b.z-a.z),0,b.x-a.x}});
      const float top=material_level(layer),bottom=material_level(layer== -8?-8:layer-1,true);
      writer.triangle(at,ab,bt,normal,top,bottom,top,climate,layer);writer.triangle(bt,ab,bb,normal,top,bottom,bottom,climate,layer);
     }
    }
   }
  }
 }
 return result;
}

std::vector<terrain_mesh_data> build_editor_grass(const TerrainEditorLevel&level,const terrain_mesh_data&surface){
 EditorChunkMesh result;
 for(size_t i=0;i+2<surface.indices.size();i+=3){const auto&a=surface.vertices.at(surface.indices[i]);const auto&b=surface.vertices.at(surface.indices[i+1]);const auto&c=surface.vertices.at(surface.indices[i+2]);if(a.normal[1]<.99f||b.normal[1]<.99f||c.normal[1]<.99f||a.position[1]<=level.terrain.config.water_y)continue;
  uint32_t state=hash(level.terrain.config.seed^hash(static_cast<uint32_t>(std::llround(a.position[0]*1024)))^hash(static_cast<uint32_t>(std::llround(a.position[2]*1024)))^hash(static_cast<uint32_t>(i)));
  grass_triangle(result,level,a.position,b.position,c.position,state);
 }return std::move(result.grass);
}

terrain_mesh_data build_editor_tree(const EditorTree&tree,uint32_t seed){
 return build_voxel_tree(tree,seed);
}
terrain_mesh_data build_editor_rocks(const TerrainEditorLevel&level,float x0,float z0,float x1,float z1){
 terrain_mesh_data mesh;const auto&config=level.terrain.config;
 const int n=config.resolution;const float cell=config.world_size/n,half=config.world_size*.5f,pitch=std::max(10.f,8.f*cell);
 auto at=[&](float x,float z){return level.terrain.layer_at(static_cast<int>(std::floor((x+half)/cell)),static_cast<int>(std::floor((z+half)/cell)));};
 auto triangle=[&](Point a,Point b,Point c,Color color){
  const Point normal=unit(cross(sub(b,a),sub(c,a)));
  for(auto p:{a,b,c}){mesh.indices.push_back(static_cast<uint16_t>(mesh.vertices.size()));mesh.vertices.push_back({p,normal,color});}
 };
 auto quad=[&](Point a,Point b,Point c,Point d,Color color){triangle(a,b,c,color);triangle(b,d,c,color);};
 auto box=[&](float x,float y,float z,float rx,float h,float rz){
  const Point a={{x-rx,y,z-rz}},b={{x+rx,y,z-rz}},c={{x-rx,y,z+rz}},d={{x+rx,y,z+rz}};
  const Point e={{x-rx,y+h,z-rz}},f={{x+rx,y+h,z-rz}},g={{x-rx,y+h,z+rz}},j={{x+rx,y+h,z+rz}};
  quad(e,g,f,j,{{119,127,114,255}});quad(a,b,c,d,{{72,83,82,255}});
  quad(a,e,b,f,{{93,102,99,255}});quad(c,d,g,j,{{86,96,95,255}});
  quad(a,c,e,g,{{101,109,103,255}});quad(b,f,d,j,{{91,102,100,255}});
 };
 for(int tz=static_cast<int>(std::floor(z0/pitch));tz<=static_cast<int>(std::floor(z1/pitch));++tz)
 for(int tx=static_cast<int>(std::floor(x0/pitch));tx<=static_cast<int>(std::floor(x1/pitch));++tx){
  uint32_t state=hash(config.seed^hash(static_cast<uint32_t>(tx))^hash(static_cast<uint32_t>(tz)+1741u));
  if(random(state)>.16f)continue;
  const float x=(tx+.3f+random(state)*.4f)*pitch,z=(tz+.3f+random(state)*.4f)*pitch;
  if(x<x0||x>=x1||z<z0||z>=z1)continue;
  const int terrace=at(x,z);if(terrace<=0)continue;
  const float margin=1.5f+3.f*cell;
  bool flat=true;
  const int cx0=static_cast<int>(std::floor((x+half-margin)/cell)),cx1=static_cast<int>(std::floor((x+half+margin)/cell));
  const int cz0=static_cast<int>(std::floor((z+half-margin)/cell)),cz1=static_cast<int>(std::floor((z+half+margin)/cell));
  for(int cz=cz0;cz<=cz1&&flat;++cz)for(int cx=cx0;cx<=cx1;++cx)if(level.terrain.layer_at(cx,cz)!=terrace){flat=false;break;}
  if(!flat)continue;
  bool occupied=false;for(const auto&building:level.buildings)occupied|=point_inside_building(building,x,z,2.f);
  for(const auto&stairs:level.stairs)occupied|=point_inside_stairs(stairs,x,z,2.f);
  if(occupied)continue;
  if(mesh.vertices.size()+108>60000)throw std::length_error("Rock decoration region exceeds mesh index capacity");
  const float y=editor_level_height(config,terrace)+.002f;
  const float rx=.7f+random(state)*.35f,h=.5f+random(state)*.65f,rz=.6f+random(state)*.3f;
  box(x,y,z,rx,h,rz);
  box(x+1.05f,y,z-.5f,.45f,.35f,.45f);
  box(x-.65f,y,z+.9f,.35f,.28f,.4f);
 }
 return mesh;
}
} // namespace game
