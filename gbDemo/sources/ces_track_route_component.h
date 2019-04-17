//
//  ces_track_route_component.hpp
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
    class ces_track_route_component : public gb::ces_base_component
    {
    private:
        
        std::vector<glm::vec2> m_route;
        
    protected:
        
    public:
        
        CTTI_CLASS_GUID(ces_track_route_component, gb::ces_base_component::g_guids_container)
        ces_track_route_component();
        ~ces_track_route_component() = default;
        
        void add_route_point(const glm::vec2& point);
        
        std::property_ro<std::vector<glm::vec2>> route;
        std::property_ro<glm::vec2> start_point;
    };
};

