//
//  ces_car_simulator_component.hpp
//  gbDemo
//
//  Created by serhii.s on 2/15/19.
//  Copyright © 2019 sergey.sergeev. All rights reserved.
//

#pragma once

#include "ces_base_component.h"
#include "ns_declarations.h"

namespace game
{
    class ces_car_simulator_component : public gb::ces_base_component
    {
    private:
        
        glm::vec2 m_velocity;
        glm::vec2 m_acceleration_wc;
        glm::vec2 m_flatf;
        glm::vec2 m_flatr;
        glm::vec2 m_ftraction;
        glm::vec2 m_resistance;
        glm::vec2 m_force;
        glm::vec2 m_acceleration;
        f32 m_rpm_wheel;
        f32 m_current_sign;
        f32 m_last_sign;
        f32 m_damping_throttle;
        
        f32 m_physics_timestep_reference_hz;
        f32 m_physics_dt;
        
        glm::vec2 m_lateral_force_front = glm::vec2(0.f);
        glm::vec2 m_lateral_force_rear = glm::vec2(0.f);
        
    protected:
        
    public:
        
        STTI_CLASS_GUID(ces_car_simulator_component, gb::ces_base_component::g_guids_container)
        ces_car_simulator_component();
        ~ces_car_simulator_component() = default;
        
        std::property_rw<glm::vec2> velocity;
        std::property_rw<glm::vec2> acceleration_wc;
        std::property_rw<glm::vec2> flatf;
        std::property_rw<glm::vec2> flatr;
        std::property_rw<glm::vec2> ftraction;
        std::property_rw<glm::vec2> resistance;
        std::property_rw<glm::vec2> force;
        std::property_rw<glm::vec2> acceleration;
        
        std::property_rw<f32> rpm_wheel;
        std::property_rw<f32> current_sign;
        std::property_rw<f32> last_sign;
        std::property_rw<f32> damping_throttle;
        
        std::property_rw<glm::vec2> lateral_force_front;
        std::property_rw<glm::vec2> lateral_force_rear;
    };
};
