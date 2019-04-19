//
//  ces_car_gear_component.h
//  gbDemo
//
//  Created by serhii.s on 4/19/19.
//  Copyright © 2019 sergey.sergeev. All rights reserved.
//

#pragma once

#include "ces_base_component.h"
#include "ns_declarations.h"

namespace game
{
    class ces_car_gear_component : public gb::ces_base_component
    {
    private:
        
        static const i32 k_min_gear;
        static const i32 k_max_gear;
        
        f32 m_previous_speed_factor = 0.f;
        i32 m_current_gear = k_min_gear;
        i32 m_current_rpm = 0;
        f32 m_previous_load = 0.f;
        i32 m_current_throttle = 0;
        
    protected:
        
        void select_gear(f32 speed_factor);
        void shift_up();
        void shift_down();
        f32 get_gear_ratio() const;
        
    public:
        
        CTTI_CLASS_GUID(ces_car_gear_component, gb::ces_base_component::g_guids_container)
        ces_car_gear_component() = default;
        ~ces_car_gear_component() = default;
        
        f32 get_rpm(f32 speed_factor, f32 load);
        
        f32 get_previous_load() const;
        
        void set_throttle(i32 throttle);
        i32 get_throttle() const;
       
    };
};


