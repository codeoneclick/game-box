#pragma once
#include "resident_navigation.h"
#include <memory>
namespace game {
struct resident_state {
    resident_point position;
    float yaw=0,idle_remaining=3;
    std::vector<resident_point> route,reachable;
    // Populations share immutable, grid-ordered reachability by connected region.
    std::shared_ptr<const std::vector<resident_point>> shared_reachable;
    size_t route_index=0;
    uint32_t random_state=1;
    bool available=false,walking=false;
};
resident_state create_resident_state(const resident_navigation&,uint32_t seed);
std::vector<resident_state> create_resident_population(const resident_navigation&,size_t count,uint32_t seed);
void step_resident(const resident_navigation&,float dt,resident_state&);
}
