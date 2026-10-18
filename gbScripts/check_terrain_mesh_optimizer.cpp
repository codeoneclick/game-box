#include "terrain_mesh_optimizer.h"
#include <algorithm>
#include <cmath>
#include <iostream>
#include <map>
#include <set>
#include <stdexcept>
using namespace game;
void require(bool value,const char* message){if(!value)throw std::runtime_error(message);}
terrain_mesh_data grid(bool hole=false,bool gradient=false){
 terrain_mesh_data mesh;
 for(int z=0;z<=16;++z)for(int x=0;x<=16;++x){terrain_vertex v({{x*2.f,4,z*2.f}},{{0,1,0}},{{255,255,255,255}});v.editor_palette={{gradient?x*.01f:0.f,0.f}};v.outline_id=13;mesh.vertices.push_back(v);}
 for(int z=0;z<16;++z)for(int x=0;x<16;++x){if(hole&&x>=6&&x<10&&z>=6&&z<10)continue;uint16_t a=z*17+x;for(auto i:{a,uint16_t(a+17),uint16_t(a+1),uint16_t(a+1),uint16_t(a+17),uint16_t(a+18)})mesh.indices.push_back(i);}
 return mesh;
}
double area(const terrain_mesh_data& m){double result=0;for(size_t i=0;i<m.indices.size();i+=3){auto a=m.vertices.at(m.indices[i]).position,b=m.vertices.at(m.indices[i+1]).position,c=m.vertices.at(m.indices[i+2]).position;double y=double(b[2]-a[2])*(c[0]-a[0])-double(b[0]-a[0])*(c[2]-a[2]);require(y>0,"reversed or degenerate top");result+=y*.5;}return result;}
using Point=std::array<float,3>;using Edge=std::pair<Point,Point>;
std::set<Edge> boundary(const std::vector<terrain_mesh_data>& parts){std::map<Edge,int> edges;for(auto& m:parts)for(size_t i=0;i<m.indices.size();i+=3)for(int j=0;j<3;++j){auto a=m.vertices.at(m.indices[i+j]).position,b=m.vertices.at(m.indices[i+(j+1)%3]).position;if(b<a)std::swap(a,b);++edges[{a,b}];}std::set<Edge> result;for(auto& edge:edges)if(edge.second==1)result.insert(edge.first);return result;}
int main(){try{
 auto flat=grid();auto result=optimize_terrain_mesh(flat,{});require(result.output_triangles<=62,"flat grid must drop from 512 to <=62 triangles");require(boundary({flat})==boundary(result.parts),"every perimeter segment must remain exact");double sum=0;for(auto& p:result.parts){sum+=area(p);for(auto& v:p.vertices){require(v.position[1]==4&&v.normal==Point{{0,1,0}},"height/normal changed");require(v.outline_id==13,"outline identity changed");}}require(std::abs(sum-1024)<1e-6,"coverage area changed");
 auto hole=grid(true);auto h=optimize_terrain_mesh(hole,{});require(boundary({hole})==boundary(h.parts),"hole boundary changed");sum=0;for(auto& p:h.parts)sum+=area(p);require(std::abs(sum-960)<1e-6,"hole was filled");require(h.output_triangles<hole.indices.size()/3,"hole fixture was not simplified");
 auto gradient=grid(false,true);auto g=optimize_terrain_mesh(gradient,{});require(g.output_triangles==512,"gradient interpolation changed");
 TerrainOptimizeConstraints constraints;constraints.vertex_limit=40;auto split=optimize_terrain_mesh(flat,constraints);sum=0;for(auto& p:split.parts){require(p.vertices.size()<=40,"vertex part limit");sum+=area(p);}require(std::abs(sum-1024)<1e-6&&boundary({flat})==boundary(split.parts),"split coverage/seams changed");
 constraints={};constraints.locked_vertices.push_back(8*17+8);auto locked=optimize_terrain_mesh(flat,constraints);bool kept=false;for(auto& p:locked.parts)for(auto& v:p.vertices)kept|=v.position==Point{{16,4,16}};require(kept,"locked interior anchor removed");
 auto walls=flat;for(Point p:{Point{{0,4,0}},Point{{0,2,0}},Point{{2,4,0}}}){terrain_vertex v(p,{{0,0,-1}},{{1,2,3,255}});v.outline_id=27;walls.indices.push_back(uint16_t(walls.vertices.size()));walls.vertices.push_back(v);}auto w=optimize_terrain_mesh(walls,{});int wall_count=0;for(auto& p:w.parts)for(size_t i=0;i<p.indices.size();i+=3){auto a=p.vertices[p.indices[i]];if(a.normal[1]<.99f){++wall_count;require(a.outline_id==27&&a.color[0]==1,"wall attributes changed");for(int j=0;j<3;++j)require(p.vertices[p.indices[i+j]].position==walls.vertices[289+j].position,"wall geometry changed");}}require(wall_count==1,"wall removed or duplicated");
 std::cout<<"Terrain optimizer CPU checks passed: flat "<<512<<" -> "<<result.output_triangles<<", holes, gradients, fixed anchors, walls and 16-bit parts.\n";return 0;
}catch(const std::exception& e){std::cerr<<e.what()<<'\n';return 1;}}
