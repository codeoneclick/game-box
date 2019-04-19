//
//  ces_car_drift_state_component.cpp
//  gbDemo
//
//  Created by serhii.s on 4/3/19.
//  Copyright © 2019 sergey.sergeev. All rights reserved.
//

#include "ces_car_drift_state_component.h"

namespace game
{
    ces_car_drift_state_component::ces_car_drift_state_component()
    {
        is_drifting.getter([=]() {
            return m_is_drifting;
        });
        
        is_drifting.setter([=](bool is_drifting) {
            m_is_drifting = is_drifting;
        });
        
        lateral_forces_front.getter([=]() {
            return m_lateral_forces_front;
        });
        
        lateral_forces_front.setter([=](f32 lateral_forces_front) {
            m_lateral_forces_front = lateral_forces_front;
        });
        
        lateral_forces_rear.getter([=]() {
            return m_lateral_forces_rear;
        });
        
        lateral_forces_rear.setter([=](f32 lateral_forces_rear) {
            m_lateral_forces_rear = lateral_forces_rear;
        });
        
        drift_strength.getter([=]() {
            return m_drift_strength;
        });
        
        drift_strength.setter([=](f32 drift_strength) {
            m_drift_strength = drift_strength;
        });
        
        is_collided.getter([=]() {
            return m_is_collided;
        });
        
        is_collided.setter([=](bool is_collided) {
            m_is_collided = is_collided;
        });
        
        last_rear.getter([=]() {
            return m_last_rear;
        });
        
        last_rear.setter([=](f32 last_rear) {
            m_last_rear = last_rear;
        });
        
        last_front.getter([=]() {
            return m_last_front;
        });
        
        last_front.setter([=](f32 last_front) {
            m_last_front = last_front;
        });
        
        current_time.getter([=]() {
            return m_current_time;
        });
        
        current_time.setter([=](f32 current_time) {
            m_current_time = current_time;
        });
        
        collision_time.getter([=]() {
            return m_collision_time;
        });
        
        collision_time.setter([=](f32 collision_time) {
            m_collision_time = collision_time;
        });
    }
}