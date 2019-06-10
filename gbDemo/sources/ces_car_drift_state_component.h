//
//  ces_car_drift_state_component.hpp
//  gbDemo
//
//  Created by serhii.s on 4/3/19.
//  Copyright © 2019 sergey.sergeev. All rights reserved.
//

#pragma once

#include "ces_base_component.h"
#include "ns_declarations.h"

namespace game
{
    class ces_car_drift_state_component : public gb::ces_base_component
    {
    private:
        
        bool m_is_drifting = false;
        bool m_is_interrupted = false;
        f32 m_lateral_forces_front = 0.f;
        f32 m_lateral_forces_rear = 0.f;
        f32 m_drift_strength = 0.f;
        
        bool m_is_collided = false;
        
        f32 m_last_rear = 0.f;
        f32 m_last_front = 0.f;
        
        f32 m_current_time = 0.f;
        f32 m_collision_time = 0.f;
        
        f32 m_last_drifting_time = 0.f;
        
        f32 m_session_drifting_time = 0.f;
        f32 m_total_drifting_time = 0.f;
        
    protected:
        
    public:
        
        CTTI_CLASS_GUID(ces_car_drift_state_component, gb::ces_base_component::g_guids_container)
        ces_car_drift_state_component();
        ~ces_car_drift_state_component() = default;
        
        std::property_rw<bool> is_drifting;
        std::property_rw<bool> is_interrupted;
        std::property_rw<f32> lateral_forces_front;
        std::property_rw<f32> lateral_forces_rear;
        std::property_rw<f32> drift_strength;
        
        std::property_rw<bool> is_collided;
        
        std::property_rw<f32> last_rear;
        std::property_rw<f32> last_front;
        
        std::property_rw<f32> current_time;
        std::property_rw<f32> collision_time;
        
        std::property_rw<f32> last_drifting_time;
        
        std::property_rw<f32> session_drifting_time;
        std::property_rw<f32> total_drifting_time;
        
        void apply_session_drifting_time_to_total();
    };
};
