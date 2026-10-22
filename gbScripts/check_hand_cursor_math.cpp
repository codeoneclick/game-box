#include "hand_cursor_math.h"
#include <cassert>
#include <iostream>
int main(){
 using namespace game;
 const glm::vec3 toward(0,0,1),right(1,0,0);
 const auto facing=hand_cursor_facing(toward,right);
 const auto dorsal=glm::vec3(facing*glm::vec4(0,0,-1,0));
 const auto finger=glm::vec3(facing[1]);
 assert(dorsal.y>.9f&&dorsal.z>0); // Back of hand visible, palm toward ground.
 assert(finger.x<0&&finger.z<0&&finger.y>0); // Fingers away/up and slightly left.
 assert(glm::dot(glm::cross(glm::vec3(facing[0]),finger),glm::vec3(facing[2]))>.999f);
 const auto palm=hand_cursor_palm_bind_point();
 assert(std::abs(palm.x+3.2f*2.f/30.4f)<1.e-6f);
 assert(std::abs(palm.y+19.8f*2.f/30.4f)<1.e-6f);
 glm::vec3 p;
 assert(hand_cursor_water_hit({0,10,0},{0,-1,0},0,100,p)&&p.y==0);
 assert(!hand_cursor_water_hit({0,10,0},{0,1,0},0,100,p));
 assert(!hand_cursor_water_hit({0,10,0},{1,0,0},0,100,p));
 assert(!hand_cursor_water_hit({0,10,0},{0,-.01f,1},0,100,p));
 // Ocean hit remains usable beyond the terrain grid and covers submerged terrain.
 assert(hand_cursor_water_hit({5000,100,5000},{0,-1,0},2,1000,p)&&p.y==2);
 assert(hand_cursor_surface_height(-5,2)==2);
 assert(hand_cursor_surface_height(12,2)==12);
 // Follow a raised obstacle without jumping Y or cutting through its side.
 for(float dt:{1.f/30,1.f/60,1.f/144}){
  HandCursorMotion motion;auto floor=[](float x,float){return x>=2&&x<=4?4.f:0.f;};
  motion.reset(glm::vec3(0,.5f,0));glm::vec3 previous=motion.position();bool rose=false,descended=false;
  for(int frame=0;frame<int(3/dt);frame++){
   const auto q=motion.update(glm::vec2(6,0),dt,floor,[](glm::vec2 a,glm::vec2 b){return std::max(a.x,b.x)>=2&&std::min(a.x,b.x)<=4?4.f:0.f;});
   assert(q.y>=floor(q.x,q.z)+.4999f);
   assert(std::abs(q.y-previous.y)<100*dt); // no four-metre cliff teleport.
   rose|=q.y>4;descended|=rose&&q.x>4&&q.y<2;previous=q;
  }
  assert(rose&&descended&&glm::length(motion.position()-glm::vec3(6,.5f,0))<.05f);
 }
 // Sub-millimetre movement can still cross a discontinuous cliff edge.
 {HandCursorMotion tiny;tiny.reset(glm::vec3(0,.5f,0));
  auto edge=[](float x,float){return x>=.000001f?4.f:0.f;};
  auto sweep=[](glm::vec2 a,glm::vec2 b){return std::max(a.x,b.x)>=.000001f?4.f:0.f;};
  const auto q=tiny.update(glm::vec2(.001f,0),1.f/60,edge,sweep);assert(q.y>=edge(q.x,q.z)+.4999f);
 }
 // At 0.05 seconds, match the previous tuning at 0.1 seconds.
 {HandCursorMotion fast;fast.reset(glm::vec3(0,10.5f,0));
  for(int i=0;i<6;i++)fast.update(glm::vec2(10,0),1.f/120,[](float,float){return 0.f;},[](glm::vec2,glm::vec2){return 0.f;});
  assert(std::abs(fast.position().x-10.f*(1.f-(1.f+48.f*.1f)*std::exp(-48.f*.1f)))<.0001f);
  assert(std::abs(fast.position().y-(.5f+10.f*(1.f+32.f*.1f)*std::exp(-32.f*.1f)))<.0001f);
 }
 // Rapid waypoint changes retain continuity and a bounded backlog.
 HandCursorMotion motion;motion.reset(glm::vec3(0,.5f,0));auto flat=[](float,float){return 0.f;};
 const auto first=motion.update(glm::vec2(10,0),1.f/60,flat,[](glm::vec2,glm::vec2){return 0.f;});assert(first.x>0&&first.x<6);
 for(int i=0;i<120;i++)motion.update(glm::vec2(i<60?10.f:-10.f,0),1.f/60,flat,[](glm::vec2,glm::vec2){return 0.f;});
 for(int i=0;i<60;i++)motion.update(glm::vec2(-10,0),1.f/60,flat,[](glm::vec2,glm::vec2){return 0.f;});
 assert(glm::length(motion.position()-glm::vec3(-10,.5f,0))<.05f);
 std::cout<<"Hand cursor: dorsal facing, spring/waypoints, obstacle ascent/descent at 30/60/144 Hz, hover PASS\n";
}
