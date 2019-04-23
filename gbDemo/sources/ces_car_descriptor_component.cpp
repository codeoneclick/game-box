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
        position_wc.getter([=]() {
            return m_position_wc;
        });
        
        position_wc.setter([=](const glm::vec2& position_wc) {
            m_position_wc = position_wc;
        });
        
        velocity_wc.getter([=]() {
            return m_velocity_wc;
        });
        
        velocity_wc.setter([=](const glm::vec2& velocity_wc) {
            m_velocity_wc = velocity_wc;
        });
        
        angular_velocity.getter([=]() {
            return m_angular_velocity;
        });
        
        angular_velocity.setter([=](f32 angular_velocity) {
            m_angular_velocity = angular_velocity;
        });
        
        steer_angle.getter([=]() {
            return m_steer_angle;
        });
        
        steer_angle.setter([=](f32 steer_angle) {
            m_steer_angle = steer_angle;
        });
        
        throttle.getter([=]() {
            return m_throttle;
        });
        
        throttle.setter([=](f32 throttle) {
            m_throttle = throttle;
        });
        
        brake.getter([=]() {
            return m_brake;
        });
        
        brake.setter([=](f32 brake) {
            m_brake = brake;
        });
        
        side_angle.getter([=]() {
            return m_side_angle;
        });
        
        side_angle.setter([=](const glm::interpolated_f32& side_angle) {
            m_side_angle = side_angle;
        });
        
        body_angle.getter([=]() {
            return m_body_angle;
        });
        
        body_angle.setter([=](const glm::interpolated_f32& body_angle) {
            m_body_angle = body_angle;
        });
        
        last_collided_timestamp.getter([=]() {
            return m_last_collided_timestamp;
        });
        
        last_collided_timestamp.setter([=](f32 value) {
            m_last_collided_timestamp = value;
        });
        
        max_colission_interval.getter([=]() {
            return m_max_colission_interval;
        });
    }
}
