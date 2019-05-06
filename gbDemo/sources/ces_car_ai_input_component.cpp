//
//  ces_car_ai_input_component.cpp
//  gbDemo
//
//  Created by serhii.s on 5/3/19.
//  Copyright © 2019 sergey.sergeev. All rights reserved.
//

#include "ces_car_ai_input_component.h"

namespace game
{
    ces_car_ai_input_component::ces_car_ai_input_component()
    {
        throttle.getter([=]() {
            return m_throttle;
        });
        
        throttle.setter([=](f32 throttle) {
            m_throttle = throttle;
        });
        
        steer_angle.getter([=]() {
            return m_steer_angle;
        });
        
        steer_angle.setter([=](f32 steer_angle) {
            m_steer_angle = steer_angle;
        });
        
        brake.getter([=]() {
            return m_brake;
        });
        
        brake.setter([=](f32 brake) {
            m_brake = brake;
        });
        
        updated.getter([=]() {
            return m_updated;
        });
        
        updated.setter([=](bool updated) {
            m_updated = updated;
        });
    }
}
