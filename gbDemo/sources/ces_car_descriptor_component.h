//
//  ces_car_descriptor_component.hpp
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
    class ces_car_descriptor_component : public gb::ces_base_component
    {
    private:
    
        glm::vec2 m_position_wc = glm::vec2(0.f);
        glm::vec2 m_velocity_wc = glm::vec2(0.f);
        
        f32 m_angular_velocity = 0.f;
        f32 m_steer_angle = 0.f;
        
        f32 m_throttle = 0.f;
        f32 m_brake = 0.f;
        
    protected:
        
    public:
        
        CTTI_CLASS_GUID(ces_car_descriptor_component, gb::ces_base_component::g_guids_container)
        ces_car_descriptor_component();
        ~ces_car_descriptor_component() = default;
        
        std::property_rw<glm::vec2> position_wc;
        std::property_rw<glm::vec2> velocity_wc;
        
        std::property_rw<f32> angular_velocity;
        std::property_rw<f32> steer_angle;
        
        std::property_rw<f32> throttle;
        std::property_rw<f32> brake;
    };
};

