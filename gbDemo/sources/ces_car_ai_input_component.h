//
//  ces_car_ai_input_component.h
//  gbDemo
//
//  Created by serhii.s on 5/3/19.
//  Copyright © 2019 sergey.sergeev. All rights reserved.
//

#pragma once

#include "ces_base_component.h"
#include "ns_declarations.h"

namespace game
{
    class ces_car_ai_input_component : public gb::ces_base_component
    {
    public:
        
        enum e_follow_side
        {
            e_undefined = 0,
            e_right,
            e_left
        };
        
    private:
        
        f32 m_throttle = 0.f;
        f32 m_steer_angle = 0.f;
        f32 m_brake = 0.f;
        f32 m_speed_multiplier = 1.f;
        bool m_updated = false;
        
        e_follow_side m_follow_side = e_follow_side::e_undefined;
        
    protected:
        
    public:
        
        STTI_CLASS_GUID(ces_car_ai_input_component, gb::ces_base_component::g_guids_container)
        ces_car_ai_input_component();
        ~ces_car_ai_input_component() = default;
        
        std::property_rw<f32> throttle;
        std::property_rw<f32> steer_angle;
        std::property_rw<f32> brake;
        std::property_rw<f32> speed_multiplier;
        
        std::property_rw<bool> updated;
        
        std::property_rw<e_follow_side> follow_side;
    };
};


