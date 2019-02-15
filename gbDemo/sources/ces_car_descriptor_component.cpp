//
//  ces_car_descriptor_component.cpp
//  gbDemo
//
//  Created by serhii.s on 2/15/19.
//  Copyright © 2019 sergey.sergeev. All rights reserved.
//

#include "ces_car_descriptor_component.h"

namespace game
{
    ces_car_descriptor_component::ces_car_descriptor_component()
    {
        m_angular_velocity = 0.f;
        m_steer_angle = 0.f;
        m_throttle = 0;
        m_brake = 0;
        m_position_wc = glm::vec2(0.f);
        m_velocity_wc = glm::vec2(0.f);
    }
}
