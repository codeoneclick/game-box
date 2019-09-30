//
//  ces_track_route_component.h
//  gbDemo
//
//  Created by serhii.s on 4/11/19.
//  Copyright © 2019 sergey.sergeev. All rights reserved.
//

#pragma once

#include "ces_base_component.h"
#include "ns_declarations.h"

namespace game
{
    class ces_level_route_component : public gb::ces_base_component
    {
    private:
        
        std::vector<glm::vec2> m_route;
        std::vector<glm::vec2> m_spawners;
        std::vector<glm::vec2> m_slow_motion_triggers;
        std::vector<glm::vec2> m_speed_up_triggers;
        
    protected:
        
    public:
        
        STTI_CLASS_GUID(ces_level_route_component, gb::ces_base_component::g_guids_container)
        ces_level_route_component();
        ~ces_level_route_component() = default;
        
        void add_route_point(const glm::vec2& point);
        void add_spawner_point(const glm::vec2& point);
        void add_slow_motion_trigger(const glm::vec2& point);
        void add_speed_up_trigger(const glm::vec2& point);
        
        std::property_ro<std::vector<glm::vec2>> route;
        std::property_ro<std::vector<glm::vec2>> spawners;
        std::property_ro<std::vector<glm::vec2>> slow_motion_triggers;
        std::property_ro<std::vector<glm::vec2>> speed_up_triggers;
    };
};

