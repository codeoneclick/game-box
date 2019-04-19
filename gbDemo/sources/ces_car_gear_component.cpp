//
//  ces_car_gear_component.cpp
//  gbDemo
//
//  Created by serhii.s on 4/19/19.
//  Copyright © 2019 sergey.sergeev. All rights reserved.
//

#include "ces_car_gear_component.h"
#include "glm_extensions.h"

namespace game
{
    const i32 ces_car_gear_component::k_min_gear = 1;
    const i32 ces_car_gear_component::k_max_gear = 6;
    
    void ces_car_gear_component::select_gear(f32 speed_factor)
    {
        if (speed_factor > m_previous_speed_factor && m_current_gear < k_max_gear)
        {
            switch (m_current_gear)
            {
                default:
                    if (m_current_rpm > 7000)
                    {
                        shift_up();
                        return;
                    }
                    
                    break;
            }
            
        }
        else if (speed_factor < m_previous_speed_factor && m_current_gear > k_min_gear)
        {
            
            switch (m_current_gear)
            {
                default:
                    if (m_current_rpm < 5000)
                    {
                        shift_down();
                        return;
                    }
                    break;
                case 2:
                    if (m_current_rpm < 2000)
                    {
                        shift_down();
                        return;
                    }
                    break;
            }
        }
    }
    
    void ces_car_gear_component::shift_up()
    {
        if (m_current_gear > 0 && m_current_gear < k_max_gear)
        {
            m_current_gear++;
            
            f32 delta = 2000;
            if (m_current_rpm >= delta + 1000)
            {
                m_current_rpm -= delta;
            }
            else
            {
                m_current_rpm = 1000;
            }
        }
        
        if (m_current_gear == 0)
        {
            m_current_gear++;
        }
        m_current_rpm = glm::clamp(m_current_rpm, 1000, 10000);
    }
    
    void ces_car_gear_component::shift_down()
    {
        if (m_current_gear > k_min_gear && m_current_gear <= k_max_gear)
        {
            m_current_gear--;
            if (m_current_rpm != 1000)
            {
                m_current_rpm += 3000;
            }
        }
        m_current_rpm = glm::clamp(m_current_rpm, 1000, 10000);
    }
    
    f32 ces_car_gear_component::get_gear_ratio() const
    {
        f32 result = 0.f;
        switch(m_current_gear)
        {
            case 0:
                result = 3.f;
            case 1:
                result = 1.f;
                break;
            case 2:
                result = .7f;
                break;
            case 3:
                result = .5f;
                break;
            case 4:
                result = .3f;
                break;
            case 5:
                result = .2f;
                break;
            case 6:
                result = .1f;
                break;
        }
        
        return result;
    }
    
    f32 ces_car_gear_component::get_rpm(f32 speed_factor, f32 load)
    {
        select_gear(speed_factor);
        glm::interpolated_f32 ispeed;
        ispeed.set(speed_factor, .85f);
        f32 ispeed_factor = ispeed.get();
        
        f32 q = 11000;
        f32 factor = q * ispeed_factor * get_gear_ratio();
        
        glm::interpolated_f32 ifactor;
        if (speed_factor < m_previous_speed_factor)
        {
            ifactor.set(factor, .6f);
        }
        else
        {
            ifactor.set(factor, .85f);
        }
        
        m_current_rpm = 1000 + ifactor.get();
        
        m_current_rpm = glm::clamp(m_current_rpm, 1000, 10000);
        
        m_previous_speed_factor = speed_factor;
        m_previous_load = load;
        
        return m_current_rpm;
    }
    
    f32 ces_car_gear_component::get_previous_load() const
    {
        return m_previous_load;
    }
    
    void ces_car_gear_component::set_throttle(i32 throttle)
    {
        m_current_throttle = throttle;
    }
    
    i32 ces_car_gear_component::get_throttle() const
    {
        return m_current_throttle;
    }
}
