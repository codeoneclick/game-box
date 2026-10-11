#pragma once
// Temporary Vulkan audit instrumentation. Installed/restored by core-probe.py.
// No extra waits, submissions or barriers. Results read after existing image fence.
#include "../../gbCore/sources/vk_device.h"
#include "../../gbCore/sources/render_statistics.h"
#include <chrono>
#include <map>
#include <vector>
#include <string>
#include <iostream>
#include <cstdlib>
#if USED_GRAPHICS_API == VULKAN_API
namespace gb_probe {
using clock = std::chrono::steady_clock;
inline bool enabled() { static bool v=std::getenv("GB_PROFILE_CORE")!=nullptr; return v; }
inline bool native_stress() { static bool v=std::getenv("GB_PROBE_NATIVE_STRESS")!=nullptr; return v; }
struct sample { double ms=0; unsigned calls=0; };
struct slot { VkQueryPool pool=VK_NULL_HANDLE; std::vector<std::string> labels; unsigned frame=0; };
struct data {
 unsigned frame=0, cpu_frames=0, gpu_frames=0; double period=0; uint64_t mask=~uint64_t(0);
 std::vector<slot> slots; std::map<std::string,sample> cpu,gpu;
 clock::time_point previous; bool previous_valid=false;
};
inline data& state() { static data v; return v; }
inline void cpu_add(const std::string& name,double ms) { auto& s=state(); if(s.frame>=60){s.cpu[name].ms+=ms; ++s.cpu[name].calls;} }
struct scope {
 std::string name; clock::time_point begin; bool active;
 scope(const std::string& n):name(n),active(enabled()) { if(active)begin=clock::now(); }
 ~scope() { if(active){double ms=std::chrono::duration<double,std::milli>(clock::now()-begin).count();cpu_add(name,ms);
  if(name=="upload_flush" && state().frame>=60)std::cout<<"PROBE_UPLOAD frame="<<state().frame<<" ms="<<ms<<"\n"; } }
};
inline void mark(const std::string& label) {
 if(!enabled())return;
 auto& s=state(); if(s.slots.empty())return;
 auto d=gb::vk_device::get_instance(); auto& slot=s.slots[d->get_current_image_index()];
 if(slot.labels.size()>=128)return;
 vkCmdWriteTimestamp(d->get_draw_cmd_buffer(d->get_current_image_index()),VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,slot.pool,(uint32_t)slot.labels.size());
 slot.labels.push_back(label);
}
inline void gpu_begin() {
 if(!enabled())return;
 auto& s=state(); auto d=gb::vk_device::get_instance();
 if(s.slots.empty()) {
  VkPhysicalDeviceProperties p;vkGetPhysicalDeviceProperties(d->get_physical_device(),&p);s.period=p.limits.timestampPeriod;
  uint32_t count=0;vkGetPhysicalDeviceQueueFamilyProperties(d->get_physical_device(),&count,nullptr);
  std::vector<VkQueueFamilyProperties> q(count);vkGetPhysicalDeviceQueueFamilyProperties(d->get_physical_device(),&count,q.data());
  unsigned bits=q[d->get_queue_family().m_graphics_family].timestampValidBits;
  if(!bits){std::cout<<"PROBE unsupported_timestamps"<<std::endl;return;}
  s.mask=bits==64?~uint64_t(0):((uint64_t(1)<<bits)-1);
  s.slots.resize(d->get_draw_cmd_buffers().size());
  for(auto& v:s.slots){VkQueryPoolCreateInfo ci={};ci.sType=VK_STRUCTURE_TYPE_QUERY_POOL_CREATE_INFO;ci.queryType=VK_QUERY_TYPE_TIMESTAMP;ci.queryCount=128;
   if(vkCreateQueryPool(d->get_logical_device(),&ci,nullptr,&v.pool)!=VK_SUCCESS)std::abort();}
  std::cout<<"PROBE timestamp_period_ns="<<s.period<<" valid_bits="<<bits<<" swapchain_images="<<s.slots.size()<<std::endl;
 }
 auto& v=s.slots[d->get_current_image_index()];
 if(!v.labels.empty()) {
  std::vector<uint64_t> values(v.labels.size());
  VkResult r=vkGetQueryPoolResults(d->get_logical_device(),v.pool,0,(uint32_t)values.size(),values.size()*sizeof(uint64_t),values.data(),sizeof(uint64_t),VK_QUERY_RESULT_64_BIT);
  if(r==VK_SUCCESS && v.frame>=60) {
   auto elapsed=[&](size_t a,size_t b){return double((values[b]-values[a])&s.mask)*s.period/1e6;};
   double total=elapsed(0,values.size()-1);s.gpu["frame"].ms+=total;++s.gpu["frame"].calls;
   for(size_t i=1;i+1<values.size()-1;i+=2){auto& a=s.gpu[v.labels[i]];a.ms+=elapsed(i,i+1);++a.calls;}
   std::cout<<"PROBE_GPU frame="<<v.frame<<" total_ms="<<total<<"\n";
   if(++s.gpu_frames%120==0){for(const auto& a:s.gpu)std::cout<<"PROBE_GPU_AVG name="<<a.first<<" ms="<<a.second.ms/a.second.calls<<" calls="<<a.second.calls<<"\n";s.gpu.clear();}
  }
 }
 v.labels.clear();v.frame=s.frame;
 vkCmdResetQueryPool(d->get_draw_cmd_buffer(d->get_current_image_index()),v.pool,0,128);
 mark("frame_begin");
}
inline void frame_begin() {
 if(!enabled())return;
 auto& s=state();auto now=clock::now();
 if(s.frame>=60&&s.previous_valid)std::cout<<"PROBE_FRAME frame="<<s.frame<<" wall_ms="<<std::chrono::duration<double,std::milli>(now-s.previous).count()<<"\n";
 s.previous=now;s.previous_valid=true;
}
inline void frame_end() {
 if(!enabled())return;
 auto& s=state();auto stats=gb::render_statistics::last_frame();
 if(s.frame>=60){std::cout<<"PROBE_DRAW frame="<<s.frame<<" triangles="<<stats.triangles<<" calls="<<stats.draw_calls<<"\n";
  if(++s.cpu_frames%120==0){for(const auto& a:s.cpu)std::cout<<"PROBE_CPU_AVG name="<<a.first<<" ms_per_frame="<<a.second.ms/120<<" calls_per_frame="<<double(a.second.calls)/120<<"\n";s.cpu.clear();}}
 ++s.frame;
}
inline void destroy() { if(!enabled())return;auto& s=state();auto d=gb::vk_device::get_instance();for(auto& v:s.slots)vkDestroyQueryPool(d->get_logical_device(),v.pool,nullptr);s.slots.clear(); }
}
#endif
