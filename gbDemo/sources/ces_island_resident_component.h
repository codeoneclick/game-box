#pragma once
#include "ces_base_component.h"
#include "resident_behavior.h"
namespace game {
class ces_island_resident_component final:public gb::ces_base_component {
public:
    STTI_CLASS_GUID(ces_island_resident_component,gb::ces_base_component::g_guids_container)
    resident_state state;
    int color_index=0;
    std::string animation;
    glm::vec4 clothing_color=glm::vec4(.055f,.23f,.43f,1);
};
}
