#pragma once
#include <glm/glm.hpp>
#include <cmath>
#include <algorithm>
namespace game {
// Explicit RH orthographic projection in [-1,1]. The caster converts Z to
// [0,1] on Vulkan/Metal; receivers use the same conversion before comparison.
inline glm::mat4 make_sun_shadow_matrix(glm::vec3 light,glm::vec3 focus,float width,int pixels){
 light=glm::normalize(light);
 glm::vec3 right=glm::normalize(glm::cross(std::abs(light.y)>.99f?glm::vec3(0,0,1):glm::vec3(0,1,0),light));
 glm::vec3 up=glm::cross(light,right);
 const float texel=width/pixels;
 const float x=std::floor(glm::dot(right,focus)/texel+.5f)*texel;
 const float y=std::floor(glm::dot(up,focus)/texel+.5f)*texel;
 // Fixed depth origin avoids camera-driven changes to stored depth.
 glm::mat4 m(1.f);
 for(int c=0;c<3;++c){m[c][0]=right[c]*2.f/width;m[c][1]=up[c]*2.f/width;m[c][2]=-light[c]/800.f;}
 m[3][0]=-x*2.f/width;m[3][1]=-y*2.f/width;m[3][2]=0.f;
 return m;
}
inline bool sun_shadow_bounds_visible(const glm::mat4& light,const glm::mat4& model,glm::vec3 lo,glm::vec3 hi){
 const glm::mat4 transform=light*model;glm::vec3 a(1.e20f),b(-1.e20f);
 for(int i=0;i<8;++i){glm::vec3 p(transform*glm::vec4(i&1?hi.x:lo.x,i&2?hi.y:lo.y,i&4?hi.z:lo.z,1));a=glm::min(a,p);b=glm::max(b,p);}
 return a.x<1.02f&&b.x> -1.02f&&a.y<1.02f&&b.y> -1.02f&&a.z<1.f&&b.z> -1.f;
}
}
