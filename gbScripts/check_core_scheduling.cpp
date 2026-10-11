// Compile the real scheduler/loop/accessor without platform/render headers.
// Fake assets only replace disk/GPU work; thread scheduling and queue code are real.
#define main_headers_h
#define declarations_h
#define resource_h
#include <algorithm>
#include <array>
#include <atomic>
#include <cassert>
#include <chrono>
#include <condition_variable>
#include <cstdint>
#include <ctime>
#include <deque>
#include <functional>
#include <iostream>
#include <map>
#include <memory>
#include <mutex>
#include <queue>
#include <set>
#include <string>
#include <thread>
#include <tuple>
#include <unordered_map>
#include <vector>
#include <Windows.h>
using ui8=uint8_t; using ui32=uint32_t; using i32=int32_t; using f32=float;
namespace gb {
class thread_operation; using thread_operation_shared_ptr=std::shared_ptr<thread_operation>;
class resource_loading_operation; using resource_loading_operation_shared_ptr=std::shared_ptr<resource_loading_operation>;
class resource_serializer; using resource_serializer_shared_ptr=std::shared_ptr<resource_serializer>;
class resource_commiter; using resource_commiter_shared_ptr=std::shared_ptr<resource_commiter>;
struct resource_transfering_data; using resource_transfering_data_shared_ptr=std::shared_ptr<resource_transfering_data>;
class resource { public: std::function<void(bool)> callback; explicit resource(const std::string&) {} void on_resource_loaded(bool ok) { if(callback) callback(ok); } };
using resource_shared_ptr=std::shared_ptr<resource>;
}
#include "../gbCore/sources/thread_operation.cpp"
#include "../gbCore/sources/thread_operations_pool.cpp"
#include "../gbCore/sources/game_loop.cpp"
#include "../gbCore/sources/resource_loading_operation.cpp"
#include "../gbCore/sources/resource_accessor.cpp"
static double cpu_ms() {
 FILETIME c,e,k,u; GetProcessTimes(GetCurrentProcess(),&c,&e,&k,&u);
 ULARGE_INTEGER a,b; a.LowPart=k.dwLowDateTime;a.HighPart=k.dwHighDateTime;
 b.LowPart=u.dwLowDateTime;b.HighPart=u.dwHighDateTime;return (a.QuadPart+b.QuadPart)/10000.0;
}
static void require(bool ok,const char* message) {if(!ok)throw std::runtime_error(message);}
struct listener:gb::i_game_loop { double sum=0;void on_update(float dt) override{sum+=dt;} };
struct asset_operation:gb::resource_loading_operation {
 static std::atomic<int> serialized,committed; static std::thread::id callback_thread;
 asset_operation(const std::string& s,const gb::resource_shared_ptr& r):resource_loading_operation(s,r){}
 void serialize() override{std::this_thread::sleep_for(std::chrono::milliseconds(3));++serialized;m_status=gb::e_resource_loading_operation_status_waiting;}
 void commit() override{require(std::this_thread::get_id()==callback_thread,"GPU commit moved off game thread");++committed;m_status=gb::e_resource_loading_operation_status_success;}
};
std::atomic<int> asset_operation::serialized{0},asset_operation::committed{0};std::thread::id asset_operation::callback_thread;
struct accessor:gb::resource_accessor { using resource_accessor::on_update; };
int main() { try {
 const auto main_id=std::this_thread::get_id();
 auto pool=gb::thread_operations_pool::shared_instance();std::this_thread::sleep_for(std::chrono::milliseconds(120));
 std::atomic<bool> executed{false};auto bg=std::make_shared<gb::thread_operation>(gb::thread_operation::e_thread_operation_queue_background);
 bg->set_execution_callback([&]{require(std::this_thread::get_id()!=main_id,"background affinity lost");executed=true;});
 bg->add_to_execution_queue();auto until=std::chrono::steady_clock::now()+std::chrono::milliseconds(40);
 while(!executed && std::chrono::steady_clock::now()<until)std::this_thread::yield();
 require(executed,"background job wakeup still waits for 100ms polling");
 std::atomic<int> count{0};auto main=std::make_shared<gb::thread_operation>(gb::thread_operation::e_thread_operation_queue_main);
 main->set_execution_callback([&]{require(std::this_thread::get_id()==main_id,"main affinity lost");++count;});main->add_to_execution_queue();
 const auto start_cpu=cpu_ms();std::this_thread::sleep_for(std::chrono::milliseconds(180));
 const auto busy_cpu=cpu_ms()-start_cpu;std::cout<<"blocked-main worker CPU ms="<<busy_cpu<<"\n";
 require(busy_cpu<65,"workers spin on main-thread-only job");pool->update();require(count==1,"main callback not exactly once");
 auto root=std::make_shared<gb::thread_operation>(gb::thread_operation::e_thread_operation_queue_background);
 auto dep=std::make_shared<gb::thread_operation>(gb::thread_operation::e_thread_operation_queue_main);
 dep->set_execution_callback([&]{require(std::this_thread::get_id()==main_id,"dependency affinity lost");++count;});
 root->add_dependency(dep);root->set_execution_callback([&]{require(count==2,"dependency ordering lost");++count;});root->add_to_execution_queue();
 until=std::chrono::steady_clock::now()+std::chrono::seconds(2);while(count<3 && std::chrono::steady_clock::now()<until){pool->update();std::this_thread::yield();}
 require(count==3,"mixed queue chain stalled");root->execute();require(count==3,"job executed twice");
 auto canceled=std::make_shared<gb::thread_operation>(gb::thread_operation::e_thread_operation_queue_background);
 canceled->set_execution_callback([&]{++count;});canceled->cancel();canceled->add_to_execution_queue();
 std::this_thread::sleep_for(std::chrono::milliseconds(10));require(count==3,"canceled callback executed");
 auto shared=std::make_shared<gb::thread_operation>(gb::thread_operation::e_thread_operation_queue_background);
 std::atomic<int> shared_count{0},roots_done{0};std::atomic<bool> shared_finished{false};shared->set_execution_callback([&]{++shared_count;std::this_thread::sleep_for(std::chrono::milliseconds(15));shared_finished=true;});
 for(int n=0;n<100;++n){auto job=std::make_shared<gb::thread_operation>(gb::thread_operation::e_thread_operation_queue_background);job->add_dependency(shared);job->set_execution_callback([&]{require(shared_finished,"chain advanced before shared dependency completed");++roots_done;});job->add_to_execution_queue();}
 until=std::chrono::steady_clock::now()+std::chrono::seconds(2);while(roots_done<100&&std::chrono::steady_clock::now()<until)std::this_thread::yield();
 require(roots_done==100&&shared_count==1,"shared dependency callback repeated or roots stalled");
 // Two chains can capture the same nested leaf before either pops it.
 // Removing the stale leaf must never remove its unexecuted parent.
 {
  auto leaf=std::make_shared<gb::thread_operation>(gb::thread_operation::e_thread_operation_queue_background);
  auto parent=std::make_shared<gb::thread_operation>(gb::thread_operation::e_thread_operation_queue_background);
  auto a=std::make_shared<gb::thread_operation>(gb::thread_operation::e_thread_operation_queue_background),b=std::make_shared<gb::thread_operation>(gb::thread_operation::e_thread_operation_queue_background);
  leaf->set_execution_callback([]{});parent->set_execution_callback([]{});parent->add_dependency(leaf);a->add_dependency(parent);b->add_dependency(parent);
  const auto captured_a=a->next_operation(),captured_b=b->next_operation();leaf->execute();a->pop_operation(captured_a);b->pop_operation(captured_b);
  require(b->next_operation()==parent&&!parent->is_executed(),"stale captured leaf removed unexecuted shared parent");
 }
 for(int iteration=0;iteration<100;++iteration){
  std::atomic<int> leaf_calls{0},parent_calls{0},finished_roots{0};std::atomic<bool> parent_finished{false};
  auto leaf=std::make_shared<gb::thread_operation>(gb::thread_operation::e_thread_operation_queue_background);
  auto parent=std::make_shared<gb::thread_operation>(gb::thread_operation::e_thread_operation_queue_background);
  leaf->set_execution_callback([&]{++leaf_calls;std::this_thread::sleep_for(std::chrono::microseconds(100));});
  parent->add_dependency(leaf);parent->set_execution_callback([&]{require(leaf_calls==1,"nested leaf repeated");++parent_calls;parent_finished=true;});
  for(int n=0;n<10;++n){auto job=std::make_shared<gb::thread_operation>(gb::thread_operation::e_thread_operation_queue_background);job->add_dependency(parent);job->set_execution_callback([&]{require(parent_finished,"nested shared parent popped without execution");++finished_roots;});job->add_to_execution_queue();}
  until=std::chrono::steady_clock::now()+std::chrono::seconds(2);while(finished_roots<10&&std::chrono::steady_clock::now()<until)std::this_thread::yield();
  require(finished_roots==10&&parent_calls==1&&leaf_calls==1,"nested shared chain stalled/repeated");
 }
 const auto shutdown_start=std::chrono::steady_clock::now();{gb::thread_operations_pool local;std::this_thread::sleep_for(std::chrono::milliseconds(5));}
 require(std::chrono::steady_clock::now()-shutdown_start<std::chrono::milliseconds(60),"worker shutdown still waits for polling timeout");
 gb::game_loop loop;auto l=std::make_shared<listener>();loop.add_listener(l);loop.reset_run_loop_timestamp();
 const auto before=std::chrono::steady_clock::now();for(int n=0;n<120;++n){auto end=std::chrono::steady_clock::now()+std::chrono::microseconds(350);while(std::chrono::steady_clock::now()<end){}loop.on_update();}
 const auto elapsed=std::chrono::duration<double>(std::chrono::steady_clock::now()-before).count();
 require(std::abs(l->sum-elapsed)<.003,"dt loses sub-millisecond time");
 std::this_thread::sleep_for(std::chrono::milliseconds(1010));loop.on_update();
 require(gb::game_loop::get_fps()>90&&gb::game_loop::get_fps()<130,"FPS excludes time between updates");
 loop.pause_run_loop();const auto paused_sum=l->sum;loop.on_update();require(l->sum==paused_sum,"paused listener executed");
 std::this_thread::sleep_for(std::chrono::milliseconds(30));loop.resume_run_loop();loop.on_update();require(l->sum-paused_sum<.01,"resume incorporates paused interval");
 asset_operation::callback_thread=main_id;accessor assets;std::atomic<int> callbacks{0};
 auto r=assets.get_resource<gb::resource,asset_operation>("async",false);
 r->callback=[&](bool ok){require(ok,"load failed");require(std::this_thread::get_id()==main_id,"callback affinity lost");++callbacks;assets.get_resource<gb::resource,asset_operation>("reentrant",true);};
 until=std::chrono::steady_clock::now()+std::chrono::seconds(2);while(callbacks==0 && std::chrono::steady_clock::now()<until){assets.on_update(0);std::this_thread::yield();}
 require(callbacks==1 && asset_operation::committed==2,"async commit/reentrant callback failed");
 auto cached=assets.get_resource<gb::resource,asset_operation>("async",true);require(cached==r && asset_operation::serialized==2,"cache hit repeated load");
 std::cout<<"PASS real scheduler affinity/wakeup/exactly-once, resource callbacks/cache, fractional dt\n";return 0;
 }catch(const std::exception& e){std::cerr<<"FAIL "<<e.what()<<"\n";return 1;} }
