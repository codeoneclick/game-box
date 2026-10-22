#include "hand_surface_query.h"
#include <glm/gtc/matrix_transform.hpp>
#include <cassert>
#include <iostream>
using namespace game;
int main(){
 HandSurfaceMesh mesh;
 std::vector<glm::vec3> vertices={glm::vec3(-2,0,-2),glm::vec3(2,0,-2),glm::vec3(0,0,2),glm::vec3(-1,4,-1),glm::vec3(1,4,-1),glm::vec3(0,4,1)};
 mesh.reset(vertices,{0,1,2,3,4,5});float y=0;
 assert(mesh.height(0,0,glm::mat4(1),0,y)&&std::abs(y-4)<1.e-5f);
 assert(mesh.height(1.5f,-1.8f,glm::mat4(1),0,y)&&std::abs(y)<1.e-5f);
 assert(!mesh.height(4,4,glm::mat4(1),0,y));
 float path_y=0;
 assert(mesh.path_height(glm::vec2(-8,0),glm::vec2(8,0),glm::mat4(1),0,path_y)&&std::abs(path_y-4)<.0001f);
 assert(!mesh.path_height(glm::vec2(-8,8),glm::vec2(8,8),glm::mat4(1),0,path_y));
 // Yaw, pitch and nonuniform scaling must use world-space vertical rays.
 auto model=glm::translate(glm::mat4(1),glm::vec3(10,3,20));model=glm::rotate(model,25.f,glm::vec3(1,0,0));model=glm::scale(model,glm::vec3(2,3,1));
 const auto roof=model*glm::vec4(0,4,0,1);
 assert(mesh.height(roof.x,roof.z,model,0,y)&&std::abs(y-roof.y)<.0001f);
 // A swept curtain uses the triangle plane, not the maximum of its AABB.
 HandSurfaceMesh slope;slope.reset({glm::vec3(-2,0,-2),glm::vec3(2,0,-2),glm::vec3(0,4,2)},{0,1,2});
 assert(slope.path_height(glm::vec2(-10,0),glm::vec2(10,0),glm::mat4(1),0,path_y)&&std::abs(path_y-2)<.0001f);
 // Current animated geometry replaces the old posed BVH bounds.
 auto pose=vertices;for(auto& p:pose){p.x+=12;p.y+=2;}
 mesh.refit(pose);
 assert(!mesh.height(0,0,glm::mat4(1),0,y));
 assert(mesh.height(12,0,glm::mat4(1),0,y)&&std::abs(y-6)<.00001f);
 // A morph can move a triangle completely outside its original XZ footprint.
 std::vector<glm::vec3> delta(vertices.size(),glm::vec3(10,0,0));
 mesh.reset(vertices,{0,1,2,3,4,5},delta);
 assert(mesh.height(10,0,glm::mat4(1),1,y)&&std::abs(y-4)<.00001f);
 assert(!mesh.height(10,0,glm::mat4(1),0,y));
 assert(mesh.height(5,0,glm::mat4(1),.5f,y)&&std::abs(y-4)<.00001f);
 // A tiny moving canopy must be hit outside its rest footprint; its old
 // footprint must become empty. This also checks wind-expanded BVH bounds.
 std::vector<glm::vec3> leaf={glm::vec3(-.005f,3,-.005f),glm::vec3(.005f,3,-.005f),glm::vec3(0,3,.005f)};
 mesh.reset(leaf,{0,1,2},{},std::vector<glm::vec4>(3,glm::vec4(0,0,1,.3f)));
 for(float time:{0.f,1.f,10.f,100.f}){
  const auto wind=hand_tree_wind(glm::vec2(0),time,.3f);
  assert(mesh.height(wind.x,wind.y,glm::mat4(1),0,y,nullptr,time)&&std::abs(y-3)<.00001f);
  assert(!mesh.height(wind.x+1,wind.y+1,glm::mat4(1),0,y,nullptr,time));
 }
 for(int i=0;i<1000;i++){const auto w=hand_tree_wind(glm::vec2(i*.17f,-i*.43f),i*.31f,i*.001f);assert(std::abs(w.x)<=.301f&&std::abs(w.y)<=.301f);}
 // Vertical faces and degenerate triangles must not masquerade as roofs.
 mesh.reset({glm::vec3(0,0,0),glm::vec3(0,4,0),glm::vec3(0,0,2)},{0,1,2});
 assert(!mesh.height(0,1,glm::mat4(1),0,y));
 std::vector<glm::vec3> grid;std::vector<unsigned short> indices;
 for(int z=0;z<50;z++)for(int x=0;x<50;x++){const unsigned short n=static_cast<unsigned short>(grid.size());grid.push_back(glm::vec3(x*3,2,z*3));grid.push_back(glm::vec3(x*3+2,2,z*3));grid.push_back(glm::vec3(x*3,2,z*3+2));indices.insert(indices.end(),{n,static_cast<unsigned short>(n+1),static_cast<unsigned short>(n+2)});}
 mesh.reset(grid,indices);size_t tested=0;
 assert(mesh.height(.1f,.1f,glm::mat4(1),0,y,&tested)&&std::abs(y-2)<.00001f);
 assert(tested<100); // Spatial query, not a per-frame scan of all 2500 triangles.
 std::cout<<"Hand surface: topmost mesh hit, empty gaps, transformed ray, animated refit, LOD morph, moving canopy, exact swept path, spatial pruning PASS\n";
}
