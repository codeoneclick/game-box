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
 std::cout<<"Sun shadow CPU checks: direction, projection, stability, poles and light bounds passed\n";
}
