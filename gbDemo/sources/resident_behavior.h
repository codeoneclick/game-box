#pragma once
#include "resident_navigation.h"
namespace game {
struct resident_state {
    resident_point position;
    float yaw=0,idle_remaining=3;
    std::vector<resident_point> route,reachable;
    size_t route_index=0;
    uint32_t random_state=1;
    bool available=false,walking=false;
};
resident_state create_resident_state(const resident_navigation&,uint32_t seed);
void step_resident(const resident_navigation&,float dt,resident_state&);
}
