#pragma once
#include <glm/glm.hpp>
#include <algorithm>
#include <vector>
#include <limits>
#include <cmath>
namespace game {
// Match shader_island_terrain.vert tree wind using the rendered ocean time.
inline glm::vec2 hand_tree_wind(glm::vec2 p,float t,float phase){
 const float slow=t*.4f;
 const float gust=.3f+.25f*(.5f+.5f*std::sin(glm::dot(p,glm::vec2(.063f,.041f))-slow*.71f))+.2f*(.5f+.5f*std::sin(glm::dot(p,glm::vec2(-.037f,.079f))+slow*.43f));
 const float seed=phase*91.7f;
 const glm::vec2 broad(std::sin(slow*.83f+glm::dot(p,glm::vec2(.091f,-.053f))),std::cos(slow*.67f+glm::dot(p,glm::vec2(.047f,.083f))));
 const glm::vec2 local(std::sin(slow*(1.61f+.17f*std::sin(seed))+seed),std::cos(slow*(1.37f+.19f*std::cos(seed))+seed*1.73f));
 auto wind=(broad*.72f+local*.28f)*gust;wind*=std::min(1.f,.7f/std::max(glm::length(wind),.0001f));
 return wind*.4f+.02f*glm::vec2(std::sin(t*1.37f+phase*31.f+p.x*.17f),std::cos(t*1.12f+phase*47.f+p.y*.19f));
}
// CPU-only triangle BVH. Bounds include both endpoints of terrain geomorphs.
// Animated meshes refit the same topology with the current rendered skin pose.
class HandSurfaceMesh {
 struct Node {glm::vec3 low,high;int begin=0,end=0,left=-1,right=-1;};
 std::vector<glm::vec3> positions_,delta_;
 std::vector<glm::vec4> wind_; // root XZ, squared bend weight, phase.
 std::vector<unsigned short> indices_;
 std::vector<int> order_;
 std::vector<Node> nodes_;
 void bounds(int n){
  auto& node=nodes_[n];node.low=glm::vec3(std::numeric_limits<float>::max());node.high=-node.low;
  if(node.left>=0){bounds(node.left);bounds(node.right);node.low=glm::min(nodes_[node.left].low,nodes_[node.right].low);node.high=glm::max(nodes_[node.left].high,nodes_[node.right].high);return;}
  for(int t=node.begin;t<node.end;t++)for(int k=0;k<3;k++){const auto i=indices_[order_[t]*3+k];const auto a=positions_[i],b=a+delta_[i];const glm::vec3 margin(.301f*wind_[i].z,0,.301f*wind_[i].z);node.low=glm::min(node.low,glm::min(a,b)-margin);node.high=glm::max(node.high,glm::max(a,b)+margin);}
 }
 int build(int begin,int end){
  const int n=static_cast<int>(nodes_.size());nodes_.emplace_back();nodes_[n].begin=begin;nodes_[n].end=end;bounds(n);
  if(end-begin>12){const auto size=nodes_[n].high-nodes_[n].low;const int axis=size.x>size.y?(size.x>size.z?0:2):(size.y>size.z?1:2);const int mid=(begin+end)/2;
   std::nth_element(order_.begin()+begin,order_.begin()+mid,order_.begin()+end,[&](int a,int b){float ca=0,cb=0;for(int k=0;k<3;k++){const int ia=indices_[a*3+k],ib=indices_[b*3+k];ca+=positions_[ia][axis]+delta_[ia][axis]*.5f;cb+=positions_[ib][axis]+delta_[ib][axis]*.5f;}return ca<cb;});
   const int left=build(begin,mid),right=build(mid,end);nodes_[n].left=left;nodes_[n].right=right;
  }return n;
 }
 static bool box(const Node& n,const glm::vec3& o,const glm::vec3& d,float limit){
  float lo=0,hi=limit;for(int a=0;a<3;a++){if(std::abs(d[a])<1.e-9f){if(o[a]<n.low[a]-.00001f||o[a]>n.high[a]+.00001f)return false;continue;}float x=(n.low[a]-o[a])/d[a],y=(n.high[a]-o[a])/d[a];if(x>y)std::swap(x,y);lo=std::max(lo,x);hi=std::min(hi,y);if(lo>hi+.00001f)return false;}return true;
 }
 void trace(int n,const glm::vec3& o,const glm::vec3& d,float morph,float time,float& nearest,size_t& tested)const{
  const auto& node=nodes_[n];if(!box(node,o,d,nearest))return;
  if(node.left>=0){trace(node.left,o,d,morph,time,nearest,tested);trace(node.right,o,d,morph,time,nearest,tested);return;}
  for(int t=node.begin;t<node.end;t++){
   ++tested;glm::vec3 p[3];for(int k=0;k<3;k++){const auto i=indices_[order_[t]*3+k];p[k]=positions_[i]+delta_[i]*morph;if(wind_[i].z>0){const auto w=hand_tree_wind(glm::vec2(wind_[i]),time,wind_[i].w)*wind_[i].z;p[k].x+=w.x;p[k].z+=w.y;}}
   const auto e1=p[1]-p[0],e2=p[2]-p[0],cross=glm::cross(d,e2);const float det=glm::dot(e1,cross);if(std::abs(det)<1.e-9f)continue;
   const auto s=o-p[0];const float u=glm::dot(s,cross)/det;if(u<-.00001f||u>1.00001f)continue;
   const auto q=glm::cross(s,e1);const float v=glm::dot(d,q)/det;if(v<-.00001f||u+v>1.00001f)continue;
   const float distance=glm::dot(e2,q)/det;if(distance>=0&&distance<nearest)nearest=distance;
  }
 }
 // Sweep a vertical curtain along the palm path. Clip its XZ segment against
 // each projected triangle; Y is linear over the resulting interval.
 void path_trace(int n,glm::vec2 from,glm::vec2 to,const glm::mat4& model,float morph,float time,float& highest,bool& hit,size_t& tested)const{
  const auto& node=nodes_[n];glm::vec3 low(std::numeric_limits<float>::max()),high=-low;
  for(int k=0;k<8;k++){const auto p=glm::vec3(model*glm::vec4(k&1?node.high.x:node.low.x,k&2?node.high.y:node.low.y,k&4?node.high.z:node.low.z,1));low=glm::min(low,p);high=glm::max(high,p);}
  if(high.y<highest)return;float enter=0,leave=1;const auto direction=to-from;
  for(int axis=0;axis<2;axis++){const int a=axis==0?0:2;const float d=direction[axis];if(std::abs(d)<1.e-9f){if(from[axis]<low[a]-.00001f||from[axis]>high[a]+.00001f)return;}else{float x=(low[a]-from[axis])/d,y=(high[a]-from[axis])/d;if(x>y)std::swap(x,y);enter=std::max(enter,x);leave=std::min(leave,y);if(enter>leave+.00001f)return;}}
  if(node.left>=0){path_trace(node.left,from,to,model,morph,time,highest,hit,tested);path_trace(node.right,from,to,model,morph,time,highest,hit,tested);return;}
  const auto cross=[](glm::vec2 a,glm::vec2 b){return a.x*b.y-a.y*b.x;};
  for(int t=node.begin;t<node.end;t++){
   ++tested;glm::vec3 p[3];for(int k=0;k<3;k++){const auto i=indices_[order_[t]*3+k];auto v=positions_[i]+delta_[i]*morph;if(wind_[i].z>0){const auto w=hand_tree_wind(glm::vec2(wind_[i]),time,wind_[i].w)*wind_[i].z;v.x+=w.x;v.z+=w.y;}p[k]=glm::vec3(model*glm::vec4(v,1));}
   const glm::vec2 a(p[0].x,p[0].z),b(p[1].x-p[0].x,p[1].z-p[0].z),c(p[2].x-p[0].x,p[2].z-p[0].z);
   const float det=cross(b,c);if(std::abs(det)<1.e-9f)continue;
   const float u=cross(from-a,c)/det,v=cross(b,from-a)/det,du=cross(direction,c)/det,dv=cross(b,direction)/det;
   float lo=0,hi=1;bool inside=true;const float start[3]={u,v,1-u-v},slope[3]={du,dv,-du-dv};
   for(int i=0;i<3;i++){if(std::abs(slope[i])<1.e-9f){if(start[i]<-.000001f)inside=false;}else if(slope[i]>0)lo=std::max(lo,-start[i]/slope[i]);else hi=std::min(hi,-start[i]/slope[i]);}
   if(!inside||lo>hi+.000001f)continue;
   const float base=p[0].y+u*(p[1].y-p[0].y)+v*(p[2].y-p[0].y),rise=du*(p[1].y-p[0].y)+dv*(p[2].y-p[0].y);
   highest=std::max(highest,std::max(base+rise*lo,base+rise*hi));hit=true;
  }
 }
public:
 bool path_height(glm::vec2 from,glm::vec2 to,const glm::mat4& model,float morph,float& y,size_t* triangle_tests=nullptr,float time=0)const{
  if(nodes_.empty())return false;float highest=-std::numeric_limits<float>::infinity();bool hit=false;size_t tested=0;path_trace(0,from,to,model,glm::clamp(morph,0.f,1.f),time,highest,hit,tested);if(triangle_tests)*triangle_tests+=tested;if(hit)y=highest;return hit;
 }
 void reset(std::vector<glm::vec3> vertices,std::vector<unsigned short> indices,std::vector<glm::vec3> delta={},std::vector<glm::vec4> wind={}){
  wind_=std::move(wind);if(wind_.empty())wind_.resize(vertices.size(),glm::vec4(0));
  positions_=std::move(vertices);indices_=std::move(indices);delta_=std::move(delta);if(delta_.empty())delta_.resize(positions_.size(),glm::vec3(0));
  order_.resize(indices_.size()/3);for(size_t i=0;i<order_.size();i++)order_[i]=static_cast<int>(i);nodes_.clear();if(!order_.empty())build(0,static_cast<int>(order_.size()));
 }
 void refit(const std::vector<glm::vec3>& pose){positions_=pose;std::fill(delta_.begin(),delta_.end(),glm::vec3(0));if(!nodes_.empty())bounds(0);}
 bool height(float x,float z,const glm::mat4& model,float morph,float& y,size_t* triangle_tests=nullptr,float time=0)const{
  if(nodes_.empty())return false;
  glm::vec3 low(std::numeric_limits<float>::max()),high=-low;
  for(int k=0;k<8;k++){const auto p=glm::vec3(model*glm::vec4(k&1?nodes_[0].high.x:nodes_[0].low.x,k&2?nodes_[0].high.y:nodes_[0].low.y,k&4?nodes_[0].high.z:nodes_[0].low.z,1));low=glm::min(low,p);high=glm::max(high,p);}
  if(x<low.x-.0001f||x>high.x+.0001f||z<low.z-.0001f||z>high.z+.0001f)return false;
  const glm::vec3 origin(x,high.y+1,z);const auto inverse=glm::inverse(model);
  const auto o=glm::vec3(inverse*glm::vec4(origin,1)),d=glm::vec3(inverse*glm::vec4(0,-1,0,0));
  float distance=std::numeric_limits<float>::infinity();size_t tested=0;trace(0,o,d,std::max(0.f,std::min(1.f,morph)),time,distance,tested);if(triangle_tests)*triangle_tests+=tested;
  if(!std::isfinite(distance))return false;y=origin.y-distance;return true;
 }
};
}
