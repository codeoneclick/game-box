#pragma once
#include "ces_base_component.h"
#include "resident_behavior.h"
namespace game {
class ces_island_resident_component final:public gb::ces_base_component {
public:
    STTI_CLASS_GUID(ces_island_resident_component,gb::ces_base_component::g_guids_container)
    resident_state state;
    bool woodcutter=false;
    woodcutter_state woodcutting;
    gb::ces_entity_shared_ptr axe;
    std::array<std::pair<int,glm::vec3>,2> axe_hands;
    glm::vec3 axe_axis_bind=glm::vec3(0,0,1);
    glm::vec3 axe_edge_bind=glm::vec3(1,0,0);
    size_t terrace_index=0;
    int color_index=0;
    std::string animation;
    std::shared_ptr<const std::vector<std::pair<int,glm::vec3>>> foot_contacts;
    glm::vec4 clothing_color=glm::vec4(.055f,.23f,.43f,1);
};
}
