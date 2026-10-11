#include "resident_behavior.h"
#include <cassert>
#include <iostream>
#include <cmath>
#include <algorithm>

int main() {
    game::terrain_mesh_data mesh;
    for (auto p : {game::resident_point{-20,-20},game::resident_point{-20,20},game::resident_point{20,20},game::resident_point{20,-20}})
        mesh.vertices.push_back({{{p.x,.5f,p.z}},{{0,1,0}},{{255,255,255,255}}});
    mesh.indices={0,1,2,0,2,3};
    auto nav=game::build_resident_navigation(mesh,.5f);
    auto crowd=game::create_resident_population(nav,25,1337);
    size_t stored=0;
    for (const auto& resident:crowd) stored+=resident.reachable.capacity();
    // Population reachability must not allocate a full connected grid per resident.
    assert(stored<=nav.walkable.size());
    for(int tick=0;tick<600;++tick) for(auto& resident:crowd) {
        game::step_resident(nav,.1f,resident);
        assert(resident.available&&nav.is_safe(resident.position));
    }
    // Stable, ordered paths and exact movement must survive reuse of BFS scratch.
    auto expected=nav.find_path({-10,-10},{10,10});
    for(int i=0;i<10;++i) {
        auto unrelated=nav.find_path({10,-10},{-10,10});
        auto actual=nav.find_path({-10,-10},{10,10});
        assert(actual.size()==expected.size());
        for(size_t j=0;j<actual.size();++j) assert(actual[j].x==expected[j].x&&actual[j].z==expected[j].z);
    }
    std::cout<<"ECS navigation checks passed: crowd=25, ticks=600, stable BFS paths\n";
}
