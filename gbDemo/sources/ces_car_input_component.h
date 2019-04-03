//
//  ces_car_imput_component.h
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
    class ces_car_input_component : public gb::ces_base_component
    {
    private:
        
        f32 m_throttle = 0.f;
        f32 m_steer_angle = 0.f;
        f32 m_brake = 0.f;
        
        bool m_updated = false;
        
    protected:
        
    public:
        
        CTTI_CLASS_GUID(ces_car_input_component, gb::ces_base_component::g_guids_container)
        ces_car_input_component();
        ~ces_car_input_component() = default;
        
        std::property_rw<f32> throttle;
        std::property_rw<f32> steer_angle;
        std::property_rw<f32> brake;
        
        std::property_rw<bool> updated;
    };
};

