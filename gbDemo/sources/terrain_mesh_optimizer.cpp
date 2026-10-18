#include "terrain_mesh_optimizer.h"
#include <algorithm>
#include <array>
#include <cmath>
#include <cstring>
#include <map>
#include <queue>
#include <set>
#include <stdexcept>

namespace game {namespace {
using Key=std::array<uint32_t,17>;
Key key(const terrain_vertex& v){
 Key k{};size_t i=0;auto f=[&](float a){std::memcpy(&k[i++],&a,sizeof(a));};
 for(float a:v.position)f(a);for(float a:v.normal)f(a);
 for(int c=0;c<4;++c)k[i]|=uint32_t(v.color[c])<<(c*8);++i;
 for(float a:v.texcoord)f(a);for(float a:v.grass_bend)f(a);
 for(float a:v.tree_bend)f(a);for(float a:v.editor_palette)f(a);
 k[i++]=v.outline_id;k[i++]=v.stone_texture?1:0;return k;
}
struct Face {std::array<uint16_t,3> v;bool alive=true;};
double up_area(const terrain_vertex& a,const terrain_vertex& b,const terrain_vertex& c){
 return double(b.position[2]-a.position[2])*(c.position[0]-a.position[0])
       -double(b.position[0]-a.position[0])*(c.position[2]-a.position[2]);
}
bool triangulate(std::vector<uint16_t> loop,const std::vector<terrain_vertex>& vertices,std::vector<std::array<uint16_t,3>>& output){
 // Local one-ring polygon. Inclusive containment keeps collinear boundary
 // vertices on their original segments, rather than creating T-junctions.
 while(loop.size()>3){bool clipped=false;
  for(size_t i=0;i<loop.size();++i){const size_t a=(i+loop.size()-1)%loop.size(),c=(i+1)%loop.size();
   const auto& va=vertices[loop[a]];const auto& vb=vertices[loop[i]];const auto& vc=vertices[loop[c]];
   if(up_area(va,vb,vc)<=1e-10)continue;
   bool contains=false;for(size_t j=0;j<loop.size();++j){if(j==a||j==i||j==c)continue;const auto& p=vertices[loop[j]];
    if(up_area(va,vb,p)>=-1e-10&&up_area(vb,vc,p)>=-1e-10&&up_area(vc,va,p)>=-1e-10){contains=true;break;}}
   if(contains)continue;output.push_back({{loop[a],loop[i],loop[c]}});loop.erase(loop.begin()+i);clipped=true;break;
  }
  if(!clipped)return false;
 }
 if(loop.size()!=3||up_area(vertices[loop[0]],vertices[loop[1]],vertices[loop[2]])<=1e-10)return false;
 output.push_back({{loop[0],loop[1],loop[2]}});return true;
}
}
TerrainOptimizeResult optimize_terrain_mesh(const terrain_mesh_data& source,const TerrainOptimizeConstraints& constraints){
 TerrainOptimizeResult result;result.input_triangles=source.indices.size()/3;
 if(source.indices.size()%3||constraints.vertex_limit<3||constraints.vertex_limit>65535)
  throw std::invalid_argument("Invalid Terrain optimizer indices or vertex limit");
 std::vector<terrain_vertex> vertices;std::vector<Key> attributes;
 std::vector<uint16_t> remap;remap.reserve(source.vertices.size());std::map<Key,uint16_t> unique;
 for(const auto& v:source.vertices){for(float p:v.position)if(!std::isfinite(p))throw std::invalid_argument("Nonfinite terrain position");
  const auto k=key(v);auto found=unique.find(k);
  if(found!=unique.end()){remap.push_back(found->second);continue;}
  if(vertices.size()>=65535)throw std::length_error("Terrain input exceeds 16-bit capacity");
  uint16_t index=static_cast<uint16_t>(vertices.size());unique.emplace(k,index);vertices.push_back(v);remap.push_back(index);
  auto attr=k;attr[0]=attr[2]=0;attributes.push_back(attr);
 }
 std::vector<Face> faces;faces.reserve(source.indices.size()/3);
 std::vector<std::vector<size_t>> incident(vertices.size());std::vector<bool> locked(vertices.size(),false);
 std::map<std::pair<uint16_t,uint16_t>,unsigned> edges;
 auto add=[&](const std::array<uint16_t,3>& v){size_t id=faces.size();Face face;face.v=v;faces.push_back(face);for(auto j:v)incident[j].push_back(id);};
 for(size_t i=0;i<source.indices.size();i+=3){std::array<uint16_t,3> v={{remap.at(source.indices[i]),remap.at(source.indices[i+1]),remap.at(source.indices[i+2])}};add(v);
  const bool flat=vertices[v[0]].normal==std::array<float,3>{{0,1,0}}
   &&attributes[v[0]]==attributes[v[1]]&&attributes[v[0]]==attributes[v[2]]
   &&up_area(vertices[v[0]],vertices[v[1]],vertices[v[2]])>1e-10;
  for(int j=0;j<3;++j){if(!flat)locked[v[j]]=true;++edges[std::minmax(v[j],v[(j+1)%3])];}
 }
 for(auto& e:edges)if(e.second!=2){locked[e.first.first]=locked[e.first.second]=true;}
 for(auto old:constraints.locked_vertices)locked.at(remap.at(old))=true;
 using Candidate=std::pair<size_t,uint16_t>;
 std::priority_queue<Candidate,std::vector<Candidate>,std::greater<Candidate>> queue;
 auto degree=[&](uint16_t v){size_t n=0;for(size_t i:incident[v])if(faces[i].alive)++n;return n;};
 for(size_t i=0;i<vertices.size();++i)if(!locked[i])queue.push({degree(static_cast<uint16_t>(i)),static_cast<uint16_t>(i)});
 while(!queue.empty()){
  auto candidate=queue.top();queue.pop();const uint16_t v=candidate.second;if(locked[v])continue;
  const size_t n=degree(v);if(!n)continue;if(n!=candidate.first){queue.push({n,v});continue;}
  // A bounded local cavity avoids pathological quadratic ear clipping in a
  // heavily edited chunk; a skipped vertex preserves the original coverage.
  if(n<3||n>256){locked[v]=true;++result.fallback_regions;continue;}
  std::map<uint16_t,uint16_t> next;std::vector<size_t> old_faces;bool valid=true;double before=0;
  for(size_t id:incident[v])if(faces[id].alive){const auto f=faces[id].v;int center=0;while(center<3&&f[center]!=v)++center;
   if(center==3){valid=false;break;}uint16_t a=f[(center+1)%3],b=f[(center+2)%3];
   if(a==b||!next.emplace(a,b).second){valid=false;break;}
   old_faces.push_back(id);before+=up_area(vertices[f[0]],vertices[f[1]],vertices[f[2]]);
  }
  std::vector<uint16_t> loop;
  if(valid&&!next.empty()){uint16_t a=next.begin()->first;for(size_t i=0;i<n;++i){loop.push_back(a);auto it=next.find(a);if(it==next.end()){valid=false;break;}a=it->second;}
   if(loop.empty()||a!=loop.front()||std::set<uint16_t>(loop.begin(),loop.end()).size()!=n)valid=false;}
  std::vector<std::array<uint16_t,3>> replacement;
  if(valid)valid=triangulate(loop,vertices,replacement);
  double after=0;for(const auto& f:replacement)after+=up_area(vertices[f[0]],vertices[f[1]],vertices[f[2]]);
  if(!valid||replacement.size()+2!=old_faces.size()||std::abs(before-after)>std::max(1e-8,std::abs(before)*1e-8)){
   locked[v]=true;++result.fallback_regions;continue;
  }
  for(size_t id:old_faces)faces[id].alive=false;
  for(auto& f:replacement)add(f);locked[v]=true;
  for(uint16_t a:loop)if(!locked[a])queue.push({degree(a),a});
 }
 std::vector<int> mapped(vertices.size(),-1);
 for(const auto& face:faces)if(face.alive){size_t needed=0;for(auto v:face.v)needed+=mapped[v]<0;
  if(result.parts.empty()||result.parts.back().vertices.size()+needed>constraints.vertex_limit){result.parts.emplace_back();std::fill(mapped.begin(),mapped.end(),-1);}
  auto& part=result.parts.back();for(auto v:face.v){int& index=mapped[v];if(index<0){index=static_cast<int>(part.vertices.size());part.vertices.push_back(vertices[v]);}part.indices.push_back(static_cast<uint16_t>(index));}++result.output_triangles;
 }
 return result;
}
}
