#pragma once
#include "resident_navigation.h"
#include "stairs.h"
#include <memory>
namespace game {
struct resident_stair_link {
    uint32_t id=0;
    size_t lower=0,upper=0;
    resident_point bottom,top;
    float bottom_y=0,top_y=0;
    float bottom_deck=.5f,top_deck=.5f;
};
struct resident_state {
    resident_point position;
    float yaw=0,idle_remaining=3;
    std::vector<resident_point> route,reachable;
    // Populations share immutable, grid-ordered reachability by connected region.
    std::shared_ptr<const std::vector<resident_point>> shared_reachable;
    size_t route_index=0;
    uint32_t random_state=1;
    bool available=false,walking=false;
    // A copied flight permits a continuous safe retreat if its editor object is removed.
    resident_stair_link stair;
    bool stair_pending=false,stair_transit=false,stair_returning=false,stair_up=true;
    float stair_progress=0,elevation=0;
};
struct woodcut_target {uint32_t id=0;resident_point position;float distance=1.5f;};
struct woodcutter_state {uint32_t tree_id=0;bool chopping=false;float retry=0;};
void step_woodcutter(const resident_navigation&,float dt,resident_state&,woodcutter_state&,const std::vector<woodcut_target>&);
resident_state create_resident_state(const resident_navigation&,uint32_t seed);
std::vector<resident_state> create_resident_population(const resident_navigation&,size_t count,uint32_t seed);
void step_resident(const resident_navigation&,float dt,resident_state&);
std::vector<resident_stair_link> build_resident_stair_links(const std::vector<EditorStairs>&,const terrain_config&,const std::vector<resident_navigation>&);
bool begin_resident_stairs(const std::vector<resident_navigation>&,const std::vector<resident_stair_link>&,size_t link,size_t terrace,resident_state&);
void rebind_resident_stairs(const std::vector<resident_navigation>&,const std::vector<resident_stair_link>&,size_t terrace,resident_state&);
void step_resident_with_stairs(const std::vector<resident_navigation>&,const std::vector<resident_stair_link>&,float dt,size_t& terrace,resident_state&);
}
