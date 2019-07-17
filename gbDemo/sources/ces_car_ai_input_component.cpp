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
        
        throttle.setter([=](f32 value) {
            m_throttle = value;
        });
        
        follow_side.getter([=]() {
            return m_follow_side;
        });
        
        follow_side.setter([=](e_follow_side value) {
            m_follow_side = value;
        });
        
        steer_angle.getter([=]() {
            return m_steer_angle;
        });
        
        steer_angle.setter([=](f32 value) {
            m_steer_angle = value;
        });
        
        brake.getter([=]() {
            return m_brake;
        });
        
        brake.setter([=](f32 value) {
            m_brake = value;
        });
        
        speed_multiplier.getter([=]() {
            return m_speed_multiplier;
        });
        
        speed_multiplier.setter([=](f32 value) {
            m_speed_multiplier = value;
        });
        
        updated.getter([=]() {
            return m_updated;
        });
        
        updated.setter([=](bool updated) {
            m_updated = updated;
        });
    }
}
