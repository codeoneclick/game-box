#include "sun_shadow_math.h"
#include <cassert>
#include <iostream>
int main(){
 using namespace game;
 const glm::vec3 light=glm::normalize(glm::vec3(.8f,.2f,.3f)),focus(0,14,0);
 auto m=make_sun_shadow_matrix(light,focus,96.f,2048);
 auto p=m*glm::vec4(focus,1),q=m*glm::vec4(focus+light*2.f,1);
 assert(glm::length(glm::vec2(p)-glm::vec2(q))<1.e-5f);
 assert(q.z<p.z); // Toward the sun is nearer the shadow camera.
 glm::vec3 ground=focus-light*(2.f/light.y);ground.y+=2.f;
 auto top=m*glm::vec4(focus+glm::vec3(0,2,0),1),shadow=m*glm::vec4(ground,1);
 assert(glm::length(glm::vec2(top)-glm::vec2(shadow))<1.e-5f);
 auto still=make_sun_shadow_matrix(light,focus+glm::vec3(.00001f,0,0),96.f,2048);
 for(int c=0;c<4;++c)for(int r=0;r<4;++r)assert(std::abs(m[c][r]-still[c][r])<1.e-6f);
 for(auto d:{glm::vec3(0,1,0),glm::vec3(1,.0001f,0),glm::vec3(-1,-.1f,0)}){
  auto v=make_sun_shadow_matrix(glm::normalize(d),focus,800.f,2048);
  for(int c=0;c<4;++c)for(int r=0;r<4;++r)assert(std::isfinite(v[c][r]));
 }
 assert(sun_shadow_bounds_visible(m,glm::mat4(1),focus-glm::vec3(1),focus+glm::vec3(1)));
 assert(!sun_shadow_bounds_visible(m,glm::mat4(1),glm::vec3(10000),glm::vec3(10001)));
 // Non-square XML maps must snap the two light-space axes independently.
 for(auto size:{glm::ivec2(512,1024),glm::ivec2(2048,512),glm::ivec2(1024)}){
  const auto rectangular=make_sun_shadow_matrix(light,focus,96.f,size);
  for(int axis=0;axis<2;++axis){
   const float pixel=rectangular[3][axis]*float(size[axis])*.5f;
   assert(std::abs(pixel-std::round(pixel))<.001f);
  }
 }
 // The enlarged island reaches roughly 900 m from its center. Check the
 // far map/depth range for low and high sun directions around the island.
 for(float elevation:{.02f,.2f,.8f,1.f})for(int azimuth=0;azimuth<8;++azimuth){
  float angle=azimuth*.785398163f;
  glm::vec3 sun=glm::normalize(glm::vec3(std::cos(angle),elevation,std::sin(angle)));
  auto wide=make_sun_shadow_matrix(sun,glm::vec3(0,8,0),2048.f,4096,2048.f);
  for(int point=0;point<32;++point)for(float height:{0.f,14.f}){
   float theta=point*.196349541f;
   auto clip=wide*glm::vec4(900.f*std::cos(theta),height,900.f*std::sin(theta),1);
   assert(std::abs(clip.x)<.996f&&std::abs(clip.y)<.996f&&std::abs(clip.z)<1.f);
  }
 }
 std::cout<<"Sun shadow CPU checks: direction, projection, stability, poles and light bounds passed\n";
}
