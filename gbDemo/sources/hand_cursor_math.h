#pragma once
#include <glm/glm.hpp>
#include <algorithm>
#include <cmath>
#include <vector>
#include <deque>
#include <limits>
namespace game {
// Critically damped palm motion with a short, bounded cursor trail.
class HandCursorMotion {
 struct Waypoint {glm::vec2 p;float time;};
 std::deque<Waypoint> trail_;
 glm::vec3 position_{0},velocity_{0};float clock_=0;
 static float spring(float x,float goal,float& speed,float dt,float omega=96.f){
  const float delta=x-goal,impulse=speed+omega*delta,decay=std::exp(-omega*dt);
  speed=(speed-omega*impulse*dt)*decay;return goal+(delta+impulse*dt)*decay;
 }
public:
 void reset(glm::vec3 p){position_=p;velocity_=glm::vec3(0);trail_.clear();clock_=0;}
 glm::vec3 position()const{return position_;}
 template<class Height,class Path> glm::vec3 update(glm::vec2 cursor,float dt,const Height& height,const Path& path){
  dt=std::max(0.f,std::min(.1f,dt));clock_+=dt;
  if(trail_.empty()||glm::length(trail_.back().p-cursor)>.01f)trail_.push_back({cursor,clock_});
  while(trail_.size()>1&&(trail_.size()>8||clock_-trail_.front().time>.025f||glm::length(trail_.front().p-glm::vec2(position_.x,position_.z))<.05f))trail_.pop_front();
  const auto goal=trail_.front().p;
  const glm::vec2 start(position_.x,position_.z);
  auto next_speed=velocity_;
  const glm::vec2 next(spring(position_.x,goal.x,next_speed.x,dt),spring(position_.z,goal.y,next_speed.z,dt));
  float floor=height(start.x,start.y)+.5f,ceiling=floor;
  // Exact swept queries include thin objects between cursor waypoints.
  const float next_floor=(next.x==start.x&&next.y==start.y)?floor:path(start,next)+.5f;
  const float ahead=std::max((goal.x==start.x&&goal.y==start.y)?floor:path(start,goal)+.5f,next_floor);
  if(ahead>floor+.01f)ceiling=std::max(ceiling,ahead+.15f);
  float y=spring(position_.y,ceiling,velocity_.y,dt,64.f);
  y=std::max(y,floor); // Moving geometry can enter the palm's old position.
  float safe=1;
  if((next.x!=start.x||next.y!=start.y)&&next_floor>y+.00001f){
   float low=0,high=1;for(int j=0;j<8;j++){const float mid=(low+high)*.5f;const auto v=start+(next-start)*mid;if(path(start,v)+.5f<=y+.00001f)low=mid;else high=mid;}safe=low;
  }
  const auto xz=start+(next-start)*safe;position_=glm::vec3(xz.x,y,xz.y);
  velocity_.x=next_speed.x*safe;velocity_.z=next_speed.z*safe;return position_;
 }
};
inline glm::vec3 hand_cursor_palm_bind_point(){return glm::vec3(-3.2f,-19.8f,-1.f)*(2.f/30.4f);}
inline glm::mat4 hand_cursor_facing(glm::vec3 toward,glm::vec3 right){
 toward.y=right.y=0;
 if(glm::length(toward)<.0001f)toward=glm::vec3(0,0,1);
 if(glm::length(right)<.0001f)right=glm::vec3(1,0,0);
 const auto away=glm::normalize(-glm::normalize(toward)*.985f-glm::normalize(right)*.17f);
 const float tilt=.34906585f;
 const auto finger=away*std::cos(tilt)+glm::vec3(0,std::sin(tilt),0);
 const auto back=glm::vec3(0,std::cos(tilt),0)-away*std::sin(tilt);
 const auto palm_normal=-back;
 glm::mat4 result(1);result[0]=glm::vec4(glm::cross(finger,palm_normal),0);result[1]=glm::vec4(finger,0);result[2]=glm::vec4(palm_normal,0);return result;
}
inline float hand_cursor_surface_height(float terrain,float water){return std::max(terrain,water);}
inline bool hand_cursor_water_hit(const glm::vec3& origin,const glm::vec3& ray,float water,float far_plane,glm::vec3& hit){
 if(std::abs(ray.y)<.000001f)return false;
 const float t=(water-origin.y)/ray.y;
 if(!std::isfinite(t)||t<0||t>far_plane)return false;
 hit=origin+ray*t;return true;
}
}
