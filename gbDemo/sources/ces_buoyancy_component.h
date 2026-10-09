#pragma once
#include "ces_base_component.h"
#include "buoyancy_simulation.h"
namespace game {
    class ces_buoyancy_component final : public gb::ces_base_component {
    public:
        STTI_CLASS_GUID(ces_buoyancy_component,gb::ces_base_component::g_guids_container)
        buoyancy_settings settings;
        buoyancy_state state;
        std::vector<buoyancy_probe> probes;
    };
}
