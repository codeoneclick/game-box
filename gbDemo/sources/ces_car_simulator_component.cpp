//
//  ces_car_simulator_component.cpp
//  gbDemo
//
//  Created by serhii.s on 2/15/19.
//  Copyright © 2019 sergey.sergeev. All rights reserved.
//

#include "ces_car_simulator_component.h"


namespace game
{
    ces_car_simulator_component::ces_car_simulator_component()
    {
        m_physics_timestep_reference_hz = 60.f;
        m_physics_dt = .016f;
        
        m_damping_throttle = std::pow(.98f, m_physics_timestep_reference_hz * m_physics_dt);
        
        m_current_sign = 1.f;
        m_last_sign = 1.f;
        
        velocity.getter([=]() {
            return m_velocity;
        });
        
        velocity.setter([=](const glm::vec2& velocity) {
            m_velocity = velocity;
        });
        
        acceleration_wc.getter([=]() {
            return m_acceleration_wc;
        });
        
        acceleration_wc.setter([=](const glm::vec2& acceleration_wc) {
            m_acceleration_wc = acceleration_wc;
        });
        
        flatf.getter([=]() {
            return m_flatf;
        });
        
        flatf.setter([=](const glm::vec2& flatf) {
            m_flatf = flatf;
        });
        
        flatr.getter([=]() {
            return m_flatr;
        });
        
        flatr.setter([=](const glm::vec2& flatr) {
            m_flatr = flatr;
        });
        
        ftraction.getter([=]() {
            return m_ftraction;
        });
        
        ftraction.setter([=](const glm::vec2& ftraction) {
            m_ftraction = ftraction;
        });
        
        resistance.getter([=]() {
            return m_resistance;
        });
        
        resistance.setter([=](const glm::vec2& resistance) {
            m_resistance = resistance;
        });
        
        force.getter([=]() {
            return m_force;
        });
        
        force.setter([=](const glm::vec2& force) {
            m_force = force;
        });
        
        acceleration.getter([=]() {
            return m_acceleration;
        });
        
        acceleration.setter([=](const glm::vec2& acceleration) {
            m_acceleration = acceleration;
        });
        
        rpm_wheel.getter([=]() {
            return m_rpm_wheel;
        });
        
        rpm_wheel.setter([=](f32 rpm_wheel) {
            m_rpm_wheel = rpm_wheel;
        });
        
        current_sign.getter([=]() {
            return m_current_sign;
        });
        
        current_sign.setter([=](f32 current_sign) {
            m_current_sign = current_sign;
        });
        
        last_sign.getter([=]() {
            return m_last_sign;
        });
        
        last_sign.setter([=](f32 last_sign) {
            m_last_sign = last_sign;
        });
        
        damping_throttle.getter([=]() {
            return m_damping_throttle;
        });
        
        damping_throttle.setter([=](f32 damping_throttle) {
            m_damping_throttle = damping_throttle;
        });
        
        lateral_force_front.getter([=]() {
            return m_lateral_force_front;
        });
        
        lateral_force_front.setter([=](const glm::vec2& lateral_force_front) {
            m_lateral_force_front = lateral_force_front;
        });
        
        lateral_force_rear.getter([=]() {
            return m_lateral_force_rear;
        });
        
        lateral_force_rear.setter([=](const glm::vec2& lateral_force_rear) {
            m_lateral_force_rear = lateral_force_rear;
        });
    }
}
