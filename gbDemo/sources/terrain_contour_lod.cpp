#include "terrain_contour_lod.h"
#include "terrain_mesh_optimizer.h"
#include <algorithm>
#include <cmath>
#include <limits>
#include <map>
#include <set>
#include <stdexcept>
namespace game {
bool terrain_morph_triangle_valid(const std::array<std::array<float,2>,3>& start,const std::array<std::array<float,2>,3>& target){
 const double ax=double(start[1][0])-start[0][0],az=double(start[1][1])-start[0][1];
 const double bx=double(start[2][0])-start[0][0],bz=double(start[2][1])-start[0][1];
 const double dax=(double(target[1][0])-target[0][0])-ax,daz=(double(target[1][1])-target[0][1])-az;
 const double dbx=(double(target[2][0])-target[0][0])-bx,dbz=(double(target[2][1])-target[0][1])-bz;
 const double q0=ax*bz-az*bx,q1=dax*bz+ax*dbz-daz*bx-az*dbx,q2=dax*dbz-daz*dbx;
 double maximum=std::max(q0,q0+q1+q2);
 if(q2<0){const double t=-q1/(2*q2);if(t>0&&t<1)maximum=std::max(maximum,q0+t*(q1+t*q2));}
 return std::isfinite(maximum)&&maximum<=1e-8;
}
namespace {
using P=std::array<float,2>;
double cross(P a,P b,P c){return double(b[0]-a[0])*(c[1]-a[1])-double(b[1]-a[1])*(c[0]-a[0]);}
double distance2(P a,P b){return double(a[0]-b[0])*(a[0]-b[0])+double(a[1]-b[1])*(a[1]-b[1]);}
double segment_distance2(P p,P a,P b){double d=distance2(a,b);if(d<1e-20)return distance2(p,a);double t=((p[0]-a[0])*(b[0]-a[0])+(p[1]-a[1])*(b[1]-a[1]))/d;t=std::max(0.,std::min(1.,t));return distance2(p,{{float(a[0]+t*(b[0]-a[0])),float(a[1]+t*(b[1]-a[1]))}});}
bool proper_intersection(P a,P b,P c,P d){return cross(a,b,c)*cross(a,b,d)<-1e-15&&cross(c,d,a)*cross(c,d,b)<-1e-15;}
bool inside(P p,P a,P b,P c){double x=cross(a,b,p),y=cross(b,c,p),z=cross(c,a,p);return (x>1e-9&&y>1e-9&&z>1e-9)||(x< -1e-9&&y< -1e-9&&z< -1e-9);}
struct Column {P p;int parent=0;std::set<int> neighbors;bool pinned=false;uint32_t terrace=0;};
struct Build {
 const terrain_mesh_data& source;std::vector<Column> columns;std::vector<int> vertex_column;std::vector<P> transition_start;
 int root(int a)const{while(columns[a].parent!=a)a=columns[a].parent;return a;}
 P position(int a)const{return columns[root(a)].p;}
 explicit Build(const terrain_mesh_data& mesh,const TerrainContourConstraints& constraints):source(mesh){
  std::map<P,int> ids;std::map<uint32_t,float> top;
  for(auto& v:mesh.vertices)if(std::abs(v.normal[1])<.01f)top[v.outline_id]=std::max(top.count(v.outline_id)?top[v.outline_id]:-1e20f,v.position[1]);
  for(auto& v:mesh.vertices){P p={{v.position[0],v.position[2]}};auto it=ids.find(p);if(it==ids.end()){Column c;c.p=p;c.parent=static_cast<int>(columns.size());
    const auto& b=constraints.bounds;c.pinned=std::abs(p[0]-b[0])<1e-4f||std::abs(p[1]-b[1])<1e-4f||std::abs(p[0]-b[2])<1e-4f||std::abs(p[1]-b[3])<1e-4f;
    for(auto disk:constraints.protected_disks)if(distance2(p,{{disk[0],disk[1]}})<=double(disk[2])*disk[2])c.pinned=true;
    it=ids.emplace(p,c.parent).first;columns.push_back(c);
   }vertex_column.push_back(it->second);}
  for(size_t i=0;i<mesh.indices.size();i+=3){const auto& va=mesh.vertices.at(mesh.indices[i]);if(std::abs(va.normal[1])>.01f)continue;
   const float y=top[va.outline_id];for(int j=0;j<3;++j){auto ia=mesh.indices[i+j],ib=mesh.indices[i+(j+1)%3];const auto& a=mesh.vertices[ia];const auto& b=mesh.vertices[ib];if(a.position[1]!=y||b.position[1]!=y)continue;
    int ca=vertex_column[ia],cb=vertex_column[ib];if(ca==cb)continue;
    for(int c:{ca,cb}){if(columns[c].terrace&&columns[c].terrace!=va.outline_id)columns[c].pinned=true;columns[c].terrace=va.outline_id;}
    columns[ca].neighbors.insert(cb);columns[cb].neighbors.insert(ca);
   }
  }
  for(auto& c:columns)if(c.neighbors.size()!=2)c.pinned=true;
  // A split node may end on a radial/top edge rather than an axis-aligned
  // plane. Pin artificial top seams; actual contour edges may simplify.
  using V=std::array<float,3>;std::map<std::pair<V,V>,unsigned> top_edges;
  for(size_t i=0;i<mesh.indices.size();i+=3)if(mesh.vertices[mesh.indices[i]].normal[1]>.99f)for(int j=0;j<3;++j){V a=mesh.vertices[mesh.indices[i+j]].position,b=mesh.vertices[mesh.indices[i+(j+1)%3]].position;++top_edges[std::minmax(a,b)];}
  for(auto& e:top_edges)if(e.second==1){int a=ids.at({{e.first.first[0],e.first.first[2]}}),b=ids.at({{e.first.second[0],e.first.second[2]}});if(!columns[a].neighbors.count(b))columns[a].pinned=columns[b].pinned=true;}
 }
 bool valid_collapse(int v,int a,int b,const TerrainContourConstraints& constraints)const{
  const P p=columns[v].p,pa=columns[a].p,pb=columns[b].p;
  for(auto disk:constraints.protected_disks){P center={{disk[0],disk[1]}};double r2=double(disk[2])*disk[2];if(inside(center,p,pa,pb)||segment_distance2(center,pa,pb)<r2||segment_distance2(center,p,pa)<r2||segment_distance2(center,p,pb)<r2)return false;}
  // The swept ear cannot cross or enclose any nonadjacent contour. Testing
  // the full swept region is conservative for every intermediate position.
  for(size_t i=0;i<columns.size();++i)if(root(int(i))==int(i)&&!columns[i].neighbors.empty()){
   if(int(i)!=v&&int(i)!=a&&int(i)!=b&&(inside(columns[i].p,p,pa,pb)||segment_distance2(columns[i].p,pa,pb)<1e-14))return false;
   for(int j:columns[i].neighbors)if(int(i)<j&&int(i)!=v&&j!=v){P c=columns[i].p,d=columns[j].p;
    if(proper_intersection(pa,pb,c,d)||proper_intersection(p,pa,c,d)||proper_intersection(p,pb,c,d))return false;
    if(int(i)!=a&&j!=a&&segment_distance2(pa,c,d)<1e-14)return false;
    if(int(i)!=b&&j!=b&&segment_distance2(pb,c,d)<1e-14)return false;}
  }
  // Validate the complete simultaneous transition from the preceding LOD.
  // Signed top area is quadratic when multiple columns move, so checking
  // only the endpoints can miss an inversion in the middle of a morph.
  for(size_t i=0;i<source.indices.size();i+=3){auto ia=source.indices[i],ib=source.indices[i+1],ic=source.indices[i+2];const auto& va=source.vertices[ia];if(va.normal[1]<.99f)continue;
   int ca=root(vertex_column[ia]),cb=root(vertex_column[ib]),cc=root(vertex_column[ic]);if(ca!=v&&cb!=v&&cc!=v)continue;
   P x=columns[ca].p,y=columns[cb].p,z=columns[cc].p;
   if(ca==v)x=pa;if(cb==v)y=pa;if(cc==v)z=pa;
   if(!terrain_morph_triangle_valid({{transition_start[vertex_column[ia]],transition_start[vertex_column[ib]],transition_start[vertex_column[ic]]}},{{x,y,z}}))return false;
  }return true;
 }
 terrain_mesh_data snapshot()const{
  terrain_mesh_data out;out.vertices=source.vertices;
  for(size_t i=0;i<out.vertices.size();++i){auto p=position(vertex_column[i]);out.vertices[i].position[0]=p[0];out.vertices[i].position[2]=p[1];}
  // Keep wall vertices independent where changed faces need different normals.
  for(size_t i=0;i<source.indices.size();i+=3){auto a=source.indices[i],b=source.indices[i+1],c=source.indices[i+2];const auto& x=out.vertices[a].position;const auto& y=out.vertices[b].position;const auto& z=out.vertices[c].position;
   double nx=double(y[1]-x[1])*(z[2]-x[2])-double(y[2]-x[2])*(z[1]-x[1]),ny=double(y[2]-x[2])*(z[0]-x[0])-double(y[0]-x[0])*(z[2]-x[2]),nz=double(y[0]-x[0])*(z[1]-x[1])-double(y[1]-x[1])*(z[0]-x[0]);
   double length=std::sqrt(nx*nx+ny*ny+nz*nz);if(length<1e-9)continue;
   if(std::abs(source.vertices[a].normal[1])<.01f)for(auto index:{a,b,c})out.vertices[index].normal={{float(nx/length),float(ny/length),float(nz/length)}};
   for(auto index:{a,b,c})out.indices.push_back(index);
  }return out;
 }
 float error()const{double maximum=0;for(size_t i=0;i<columns.size();++i)if(columns[i].terrace){double nearest=1e30;
   for(size_t a=0;a<columns.size();++a)if(root(int(a))==int(a)&&columns[a].terrace==columns[i].terrace)for(int b:columns[a].neighbors)if(int(a)<b)nearest=std::min(nearest,segment_distance2(columns[i].p,columns[a].p,columns[b].p));
   if(nearest<1e29)maximum=std::max(maximum,nearest);
  }return float(std::sqrt(maximum));}
};
}
std::vector<TerrainLodVariant> build_terrain_contour_lods(const terrain_mesh_data& input,const TerrainLodSettings& settings,const TerrainContourConstraints& constraints){
 if(!settings.valid())throw std::invalid_argument("Invalid terrain LOD preferences");
 terrain_mesh_data source=input;size_t wall_vertices=0;
 for(size_t i=0;i<input.indices.size();i+=3)if(std::abs(input.vertices.at(input.indices[i]).normal[1])<.01f)wall_vertices+=3;
 // Independent wall faces carry exact face normals throughout the morph.
 // If this part would exceed the index format, retain the exact source.
 const bool can_split=source.vertices.size()+wall_vertices<=60000;
 if(settings.contourSimplificationPercent>0&&settings.lodCount>1&&can_split){
  for(size_t i=0;i<input.indices.size();i+=3)if(std::abs(input.vertices.at(input.indices[i]).normal[1])<.01f)for(int j=0;j<3;++j){source.indices[i+j]=uint16_t(source.vertices.size());source.vertices.push_back(input.vertices[input.indices[i+j]]);}
 }
 Build build(source,constraints);size_t original=0;for(auto& c:build.columns)original+=!c.neighbors.empty();
 if(!can_split)for(auto& c:build.columns)c.pinned=true;
 size_t removed=0;std::vector<TerrainLodVariant> levels;
 for(int level=0;level<settings.lodCount;++level){const float percent=settings.contour_percent(level);const size_t target=static_cast<size_t>(std::floor(original*double(percent)/100.+1e-8));
  build.transition_start.resize(build.columns.size());for(size_t i=0;i<build.columns.size();++i)build.transition_start[i]=build.position(int(i));
  std::vector<std::array<P,3>> swept;
  std::vector<TerrainLodVariant::Collapse> collapses;
  while(removed<target){int best=-1,ba=-1,bb=-1;double error=1e30;
   for(size_t i=0;i<build.columns.size();++i){const auto& col=build.columns[i];if(col.pinned||build.root(int(i))!=int(i)||col.neighbors.size()!=2)continue;
    auto next=col.neighbors.begin();int a=*next++,b=*next;
    double e=segment_distance2(col.p,build.columns[a].p,build.columns[b].p);if(e>=error)continue;
    if(!build.valid_collapse(int(i),a,b,constraints))continue;
    const P p=col.p,pa=build.columns[a].p,pb=build.columns[b].p;bool overlaps=false;
    for(auto& old:swept){for(auto q:old)overlaps|=inside(q,p,pa,pb);for(P q:{p,pa,pb})overlaps|=inside(q,old[0],old[1],old[2]);const P current[3]={p,pa,pb};for(int x=0;x<3;++x)for(int y=0;y<3;++y)overlaps|=proper_intersection(current[x],current[(x+1)%3],old[y],old[(y+1)%3]);}
    if(overlaps)continue;error=e;best=int(i);ba=a;bb=b;
   }
   if(best<0)break;
   TerrainLodVariant::Collapse record;record.position=build.columns[best].p;record.target=build.columns[ba].p;
   record.vertex=uint32_t(best);record.target_vertex=uint32_t(ba);record.other_neighbor=uint32_t(bb);record.geometric_error=float(std::sqrt(error));collapses.push_back(record);
   float height=-1e20f;for(size_t i=0;i<source.vertices.size();++i)if(build.root(build.vertex_column[i])==best)height=std::max(height,source.vertices[i].position[1]);collapses.back().height=height;
   swept.push_back({{build.columns[best].p,build.columns[ba].p,build.columns[bb].p}});
   build.columns[best].parent=ba;build.columns[best].neighbors.clear();build.columns[ba].neighbors.erase(best);build.columns[bb].neighbors.erase(best);build.columns[ba].neighbors.insert(bb);build.columns[bb].neighbors.insert(ba);
   // Targets may collapse again in this transition. root() resolves that
   // dependency; the analytic simultaneous-morph guard validates the full path.
   ++removed;
  }
  TerrainLodVariant variant;variant.mesh=level==0?source:build.snapshot();variant.requested_percent=percent;variant.actual_percent=original?float(removed*100./original):0;variant.world_error=removed?build.error():0;
  variant.collapses=std::move(collapses);
  variant.morph_delta_xz.resize(variant.mesh.vertices.size(),{{0,0}});variant.morph_normal_xz.resize(variant.mesh.vertices.size(),{{0,0}});
  if(!levels.empty()){auto& previous=levels.back();variant.world_error=std::max(variant.world_error,previous.world_error);previous.next_world_error=variant.world_error;for(size_t i=0;i<previous.mesh.vertices.size();++i){previous.morph_delta_xz[i]={{variant.mesh.vertices[i].position[0]-previous.mesh.vertices[i].position[0],variant.mesh.vertices[i].position[2]-previous.mesh.vertices[i].position[2]}};previous.morph_normal_xz[i]={{variant.mesh.vertices[i].normal[0],variant.mesh.vertices[i].normal[2]}};}}
  levels.push_back(std::move(variant));
 }
 // Morph correspondence is needed only while building adjacent snapshots.
 // Once deltas are stored, discard every unreferenced vertex independently.
 for(auto& level:levels){
  if(&level==&levels.back()&&level.actual_percent>0){
   auto optimized=optimize_terrain_mesh(level.mesh,{});
   if(optimized.parts.size()==1){level.mesh=std::move(optimized.parts[0]);level.morph_delta_xz.assign(level.mesh.vertices.size(),{{0,0}});level.morph_normal_xz.assign(level.mesh.vertices.size(),{{0,0}});}
  }
  terrain_mesh_data compact;std::vector<std::array<float,2>> deltas,normals;
  std::vector<int> remap(level.mesh.vertices.size(),-1);
  for(size_t i=0;i<level.mesh.indices.size();i+=3){
   if(std::abs(level.mesh.vertices[level.mesh.indices[i]].normal[1])<.01f)++level.side_triangles;else ++level.top_triangles;
   for(int j=0;j<3;++j){auto old=level.mesh.indices[i+j];auto& index=remap[old];if(index<0){index=int(compact.vertices.size());compact.vertices.push_back(level.mesh.vertices[old]);deltas.push_back(level.morph_delta_xz[old]);normals.push_back(level.morph_normal_xz[old]);}compact.indices.push_back(uint16_t(index));}
  }
  level.mesh=std::move(compact);level.morph_delta_xz=std::move(deltas);level.morph_normal_xz=std::move(normals);
 }
 return levels;
}
TerrainLodVariant build_terrain_contour_debug(const TerrainLodVariant& source,int mode,float size){
 auto result=source;
 const auto tri=[&](std::array<float,3> a,std::array<float,3> b,std::array<float,3> c,std::array<uint8_t,4> color){
  if(result.mesh.vertices.size()+3>60000)return;
  if((b[0]-a[0])*(c[2]-a[2])-(b[2]-a[2])*(c[0]-a[0])>0)std::swap(b,c);
  for(auto p:{a,b,c}){result.mesh.indices.push_back(uint16_t(result.mesh.vertices.size()));result.mesh.vertices.emplace_back(p,std::array<float,3>{{0,1,0}},color);result.morph_delta_xz.push_back({{0,0}});result.morph_normal_xz.push_back({{0,0}});}++result.top_triangles;
 };
 const auto marker=[&](std::array<float,3> p,std::array<uint8_t,4> color){p[1]+=.08f;auto a=p,b=p,c=p;a[0]-=size;a[2]-=size;b[2]+=size;c[0]+=size;c[2]-=size;tri(a,b,c,color);};
 if(mode==4)for(const auto& collapse:source.collapses){
  std::array<float,3> a={{collapse.position[0],collapse.height+.06f,collapse.position[1]}},b={{collapse.target[0],collapse.height+.06f,collapse.target[1]}};
  marker(a,{{255,60,20,255}});marker(b,{{20,255,80,255}});
  const float dx=b[0]-a[0],dz=b[2]-a[2],length=std::sqrt(dx*dx+dz*dz);if(length<1e-6f)continue;
  auto c=a,d=b;const float x=-dz/length*size*.18f,z=dx/length*size*.18f;a[0]-=x;a[2]-=z;b[0]-=x;b[2]-=z;c[0]+=x;c[2]+=z;d[0]+=x;d[2]+=z;
  tri(a,b,c,{{255,210,20,255}});tri(c,b,d,{{255,210,20,255}});
 }
 if(mode==5){std::set<std::array<float,3>> points;for(auto index:source.mesh.indices){const auto& v=source.mesh.vertices[index];if(std::abs(v.normal[1])<.01f)points.insert(v.position);}for(auto p:points)marker(p,{{20,255,80,255}});}
 return result;
}
}
