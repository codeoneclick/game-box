//
//  ces_track_route_component.cpp
//  gbDemo
//
//  Created by serhii.s on 4/11/19.
//  Copyright © 2019 sergey.sergeev. All rights reserved.
//

#include "ces_track_route_component.h"

namespace game
{
    ces_track_route_component::ces_track_route_component()
    {
        route.getter([=]() {
            return m_route;
        });
        
        spawners.getter([=]() {
            return m_spawners;
        });
    }
    
    void ces_track_route_component::add_route_point(const glm::vec2& point)
    {
        m_route.push_back(point);
    }
    
    void ces_track_route_component::add_spawner_point(const glm::vec2& point)
    {
        m_spawners.push_back(point);
    }
}
