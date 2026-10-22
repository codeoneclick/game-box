#pragma once
#include "ces_base_component.h"
namespace game
{
class ces_dance_character_component : public gb::ces_base_component
{
  public:
    STTI_CLASS_GUID(ces_dance_character_component, gb::ces_base_component::g_guids_container)
    glm::vec3 base_position = glm::vec3(0);
    float phase = 0, amplitude = .15f, speed = 1;
    int index = 0;
    bool local_player = false, dj = false;
};
} // namespace game
