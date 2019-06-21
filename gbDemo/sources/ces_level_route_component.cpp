//
//  ces_track_route_component.cpp
//  gbDemo
//
//  Created by serhii.s on 4/11/19.
//  Copyright © 2019 sergey.sergeev. All rights reserved.
//

#include "ces_level_route_component.h"

namespace game
{
    ces_level_route_component::ces_level_route_component()
    {
        route.getter([=]() {
            return m_route;
        });
        
        spawners.getter([=]() {
            return m_spawners;
        });
        
        slow_motion_triggers.getter([=]() {
            return m_slow_motion_triggers;
        });
    }
    
    void ces_level_route_component::add_route_point(const glm::vec2& point)
    {
        m_route.push_back(point);
    }
    
    void ces_level_route_component::add_spawner_point(const glm::vec2& point)
    {
        m_spawners.push_back(point);
    }
    
     void ces_level_route_component::add_slow_motion_trigger(const glm::vec2& point)
    {
        m_slow_motion_triggers.push_back(point);
    }
}
